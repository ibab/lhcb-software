// $Id: PartitionListener.cpp,v 1.11 2008-09-10 10:08:11 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PartitionListener.cpp,v 1.11 2008-09-10 10:08:11 frankb Exp $

// Framework include files
#include "ROLogger/PartitionListener.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "UPI/upidef.h"
#include "ROLoggerDefs.h"

#include <vector>

extern "C" {
#include "dic.h"
}
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

typedef std::vector<std::string> _SV;
using namespace ROLogger;

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(Interactor* parent,const std::string& nam) : m_parent(parent), m_name(nam)
{
  std::string name;
  name = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  ::upic_write_message2("Subfarm content for %s_RunInfo from:%s",m_name.c_str(),name.c_str());
  name = "RunInfo/" + m_name + "/HLTnodeList";
  m_nodesDP   = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,nodeHandler,(long)this,0,0);
  ::upic_write_message2("Nodelist content for %s_RunInfo from:%s",m_name.c_str(),name.c_str());
  name = "RunInfo/" + m_name + "/CALIBnodeList";
  m_calibNodesDP   = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,calibNodeHandler,(long)this,0,0);
  ::upic_write_message2("Calibration nodelist content for %s_RunInfo from:%s",m_name.c_str(),name.c_str());
  name = "RunInfo/" + m_name + "/MonitoringSlice";
  m_monSliceDP     = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,monSliceHandler,(long)this,0,0);
  ::upic_write_message2("Monitoring slice info for %s_RunInfo from:%s",m_name.c_str(),name.c_str());
  name = "RunInfo/" + m_name + "/StorageSlice";
  m_storSliceDP    = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,storSliceHandler,(long)this,0,0);
  ::upic_write_message2("Storage slice info for %s_RunInfo from:%s",m_name.c_str(),name.c_str());
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  ::dic_release_service(m_calibNodesDP);
  ::dic_release_service(m_subFarmDP);
  ::dic_release_service(m_nodesDP);
  ::dic_release_service(m_storSliceDP);
  ::dic_release_service(m_monSliceDP);
}

/// DIM command service callback
void PartitionListener::storSliceHandler(void* tag, void* address, int* size) {
  PartitionListener* h = *(PartitionListener**)tag;
  if(*size > 0)  {
    std::string slice = (char*)address;
    if ( slice.empty() ) {
      IocSensor::instance().send(h->m_parent,CMD_DISCONNECT_STORAGE,(void*)0);
    }
    else {
      std::string svc = "/STORECTL01/";
      svc += (char*)address;
      svc += "/log";
      IocSensor::instance().send(h->m_parent,CMD_CONNECT_STORAGE,new std::string(svc));
    }
  }
}

/// DIM command service callback
void PartitionListener::monSliceHandler(void* tag, void* address, int* size) {
  PartitionListener* h = *(PartitionListener**)tag;
  if(*size > 0)  {
    std::string slice = (char*)address;
    if ( slice.empty() ) {
      IocSensor::instance().send(h->m_parent,CMD_DISCONNECT_MONITORING,(void*)0);
    }
    else {
      std::string svc = "/MONA08/";
      svc += (char*)address;
      svc += "/log";
      IocSensor::instance().send(h->m_parent,CMD_CONNECT_MONITORING,new std::string(svc));
    }
  }
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  std::string svc;
  std::auto_ptr<_SV> f(new _SV());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1) {
    svc = "/";
    svc += data;
    svc += "/gaudi/log";
    f->push_back(svc);
  }
  if ( h->name() == "LHCb" ) f->push_back("/CALD07/gaudi/log");
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_FARMS,f.release());
}

static void get_nodes(void* address, int* size, _SV* n) {
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1) {
    in_addr addr;
    addr.s_addr = inet_addr(data);
#ifdef _WIN32
    hostent* he = gethostbyaddr((const char*)&addr,sizeof(addr),AF_INET);
#else
    hostent* he = gethostbyaddr(&addr,sizeof(addr),AF_INET);
#endif
    if ( he ) {
      std::string node = he->h_name;
      size_t idx = node.find("-d1"); 
      if ( idx != std::string::npos ) node = node.substr(0,idx); 
      idx = node.find(".");
      if ( idx != std::string::npos ) node = node.substr(0,idx); 
      for(size_t i=0; i<node.length();++i) node[i] = ::toupper(node[i]);
      n->push_back(node);
    }
    else {
      ::upic_write_message2("Failed to resolve network address:%s",data);
    }
  }
}

/// DIM command service callback
void PartitionListener::nodeHandler(void* tag, void* address, int* size) {
  std::auto_ptr<_SV> n(new _SV());
  PartitionListener* h = *(PartitionListener**)tag;
  get_nodes(address,size,n.get());
  n->push_back("STORECTL01");
  n->push_back("STORERECV01");
  n->push_back("STORERECV02");
  n->push_back("STORESTRM01");
  n->push_back("STORESTRM02");
  n->push_back("MONA08");
  n->push_back("MONA0801");
  n->push_back("MONA0802");
  n->push_back("MONA0803");
  n->push_back("MONA0804");
  n->push_back("MONA0805");
  n->push_back("MONA0806");
  n->push_back("MONA0807");
  n->push_back("MONA0808");
  n->push_back("MONA0809");
  n->push_back("MONA0810");
  if ( h->name() == "LHCb" ) n->push_back("CALD0701");
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_NODES,n.release());
}

/// DIM command service callback
void PartitionListener::calibNodeHandler(void* tag, void* address, int* size) {
  std::auto_ptr<_SV> n(new _SV());
  PartitionListener* h = *(PartitionListener**)tag;
  get_nodes(address,size,n.get());
  if ( h ) {
    //IocSensor::instance().send(h->m_parent,CMD_ADD_NODES,n.release());
  }
}
