// $Id: ROMonInfo.cpp,v 1.6 2010-09-03 14:47:46 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonInfo.cpp,v 1.6 2010-09-03 14:47:46 frankb Exp $

// Framework include files
#include "dic.hxx"
#include "dis.hxx"
#include "RTL/rtl.h"
#include "ROMonDefs.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimListener.h"

using namespace ROMon;

/// Standard constructor
ROMonInfo::ROMonInfo(RODimListener* s) {
  if ( s ) m_servers.push_back(s);
  m_info = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Standard constructor
ROMonInfo::ROMonInfo(const Servers& s)  {
  m_servers = s;
  m_info = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Standard destructor
ROMonInfo::~ROMonInfo()  {
  if ( m_info ) {
    ::dic_release_service(m_info);
    m_info = 0;
  }
}

/// Add handler for a given message source
void ROMonInfo::addHandler(const std::string& node, const std::string& svc) {
  for(Servers::iterator i=m_servers.begin(); i!=m_servers.end();++i)
    (*i)->addHandler(node,svc);
}

/// Remove handler for a given message source
void ROMonInfo::removeHandler(const std::string& node, const std::string& svc) {
  for(Servers::iterator i=m_servers.begin(); i!=m_servers.end();++i)
    (*i)->removeHandler(node,svc);
}

/// DimInfo overload to process messages
void ROMonInfo::infoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    ROMonInfo* h = *(ROMonInfo**)tag;
    char *msg = (char*)address;
    std::string svc, node;
    switch(msg[0]) {
    case '+':
      getServiceNode(++msg,svc,node);
      h->addHandler(node,svc);
      break;
    case '-':
      getServiceNode(++msg,svc,node);
      h->removeHandler(node,svc);
      break;
    case '!':
      getServiceNode(++msg,svc,node);
      log() << "Service " << msg << " in ERROR." << std::endl;
      break;
    default:
      if ( *(int*)msg != *(int*)"DEAD" )  {
        char *at, *p = msg, *last = msg;
        while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
          last = strchr(at,'|');
          if ( last ) *last = 0;
          getServiceNode(p,svc,node);
          h->addHandler(node,svc);
          p = last+1;
        }
      }
      else  {
        log() << "DIM nameserver died...." << std::endl;
      }
      break;
    }
  }
  else  {
    log() << "DIM nameserver died...." << std::endl;
  }
}

extern "C" int romon_test_dim_info(int, char**) {
  bool run = true;
  ROMonInfo info(0);
  DimServer::start(RTL::processName().c_str());
  log() << "Going asleep" << std::endl;
  while(run==true) ::lib_rtl_sleep(1000);
  return 1;
}
