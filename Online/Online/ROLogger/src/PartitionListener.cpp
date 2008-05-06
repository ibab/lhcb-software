// $Id: PartitionListener.cpp,v 1.4 2008-05-06 16:29:38 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PartitionListener.cpp,v 1.4 2008-05-06 16:29:38 frankm Exp $

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
PartitionListener::PartitionListener(Interactor* parent,const std::string& nam) : m_parent(parent)
{
  std::string name;
  name = "RunInfo/" + nam + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  ::upic_write_message2("Subfarm content for %s_RunInfo from:%s",nam.c_str(),name.c_str());
  name = "RunInfo/" + nam + "/HLTnodeList";
  m_nodesDP   = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,nodeHandler,(long)this,0,0);
  ::upic_write_message2("Nodelist content for %s_RunInfo from:%s",nam.c_str(),name.c_str());
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  ::dic_release_service(m_subFarmDP);
  ::dic_release_service(m_nodesDP);
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  std::auto_ptr<_SV> f(new _SV());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(data);
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_FARMS,f.release());
}

/// DIM command service callback
void PartitionListener::nodeHandler(void* tag, void* address, int* size) {
  std::auto_ptr<_SV> n(new _SV());
  PartitionListener* h = *(PartitionListener**)tag;
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
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_NODES,n.release());
}
