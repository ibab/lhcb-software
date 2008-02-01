// $Id: ROMonDimNodeListener.cpp,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonDimNodeListener.cpp,v 1.1 2008-02-01 17:41:45 frankm Exp $

// C++ include files
#include <cstdlib>

// Framework includes
#include "dic.hxx"
#include "RTL/strdef.h"
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimNodeListener.h"

using namespace ROMon;

/// Standard destructor
RODimNodeListener::~RODimNodeListener() {
  delete m_dns;
  m_dns = 0;
  dim_lock();
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    Item* it = (*i).second;
    ::dic_release_service(it->id);
    it->data<Descriptor>()->release();
    it->release();
  }
  dim_unlock();
}

/// Add handler for a given message source
void RODimNodeListener::addHandler(const std::string& node,const std::string& svc) {
  dim_lock();
  Clients::iterator i=m_clients.find(svc);
  if ( i == m_clients.end() )  {
    if ( ::str_match_wild(svc.c_str(), m_match.c_str()) )  {
      Item* itm = Item::create<Descriptor>(this);
      m_clients[svc] = itm;
      itm->id = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,infoHandler,(long)itm,0,0);
      if ( m_verbose ) log() << "[RODimNodeListener] Create DimInfo:" 
			     << svc << "@" << node << " id=" << itm->id << std::endl;
    }
  }
  dim_unlock();
}

/// Remove handler for a given message source
void RODimNodeListener::removeHandler(const std::string& node, const std::string& svc)   {
  dim_lock();
  Clients::iterator i=m_clients.find(svc);
  if ( i != m_clients.end() ) {
    Item* it = (*i).second;
    it->data<Descriptor>()->release();
    ::dic_release_service(it->id);
    if ( m_verbose )   {
      log() << "[RODimNodeListener] Delete DimInfo:" 
	    << svc << "@" << node << " " << it->id << std::endl;
    }
    it->release();
    m_clients.erase(i);
  }
  dim_unlock();
}

/// DimInfo overload to process messages
void RODimNodeListener::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    int len = *size;
    Item* it = *(Item**)tag;
    Descriptor* d = it->data<Descriptor>();
    if ( d->len < len ) {
      d->len = len;
      ::free(d->data);
      d->data = (char*)::malloc(d->len);
    }
    d->actual = len;
    ::memcpy(d->data,address,d->actual);
  }
}
