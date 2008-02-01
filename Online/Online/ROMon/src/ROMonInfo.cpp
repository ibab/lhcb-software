// $Id: ROMonInfo.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonInfo.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $

// Framework include files
#include "dic.hxx"
#include "dis.hxx"
#include "RTL/rtl.h"
#include "ROMonDefs.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimListener.h"

using namespace ROMon;

/// Standard constructor
ROMonInfo::ROMonInfo(RODimListener* s) : m_server(s) {
  m_info = ::dic_info_service("DIS_DNS/SERVER_LIST",MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Standard destructor
ROMonInfo::~ROMonInfo()  {
  if ( m_info ) {
    ::dic_release_service(m_info);
  }
}

/// Extract node/service name from DNS info
void ROMonInfo::getServiceNode(char* s, std::string& svc, std::string& node) const {
  char* at = strchr(s,'@');
  *at = 0;
  svc = s;
  node = at+1;
}

/// Add handler for a given message source
void ROMonInfo::addHandler(const std::string& node, const std::string& svc) {
  if ( m_server ) m_server->addHandler(node,svc);
}

/// Remove handler for a given message source
void ROMonInfo::removeHandler(const std::string& node, const std::string& svc) {
  if ( m_server ) m_server->removeHandler(node,svc);
}

/// DimInfo overload to process messages
void ROMonInfo::infoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    ROMonInfo* h = *(ROMonInfo**)tag;
    char c, *msg = (char*)address;
    std::string svc, node;
    switch(c=msg[0]) {
    case '+':
      h->getServiceNode(++msg,svc,node);
      h->addHandler(node,svc);
      break;
    case '-':
      h->getServiceNode(++msg,svc,node);
      h->removeHandler(node,svc);
      break;
    case '!':
      h->getServiceNode(++msg,svc,node);
      log() << "Service " << msg << " in ERROR." << std::endl;
      break;
    default:
      if ( *(int*)msg != *(int*)"DEAD" )  {
	char *at, *p = msg, *last = msg;
	while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
	  last = strchr(at,'|');
	  if ( last ) *last = 0;
	  h->getServiceNode(p,svc,node);
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
  ROMonInfo info(0);
  DimServer::start(RTL::processName().c_str());
  log() << "Going asleep" << std::endl;
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}
