// $Id: FMCMonListener.cpp,v 1.8 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FMCMonListener.cpp,v 1.8 2010-10-14 13:30:09 frankb Exp $

// C++ include files
#include <cstdlib>
#include "sys/timeb.h"

// Framework includes
#include "dic.hxx"
#include "RTL/strdef.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/FMCMonListener.h"
#include "ROMonDefs.h"

using namespace ROMon;

/// Copy data to descriptor
void FMCMonListener::Descriptor::copy(const void* address, size_t siz) {
  if ( size_t(len) < siz ) {
    len = siz+256;
    ::free(data);
    data = (char*)::malloc(len+256);
  }
  actual = siz;
  int tm=0;
  ro_gettime(&tm,(unsigned int*)&millitm);
  time = tm;
  ::memcpy(data,address,actual);
}

/// Standard constructor
FMCMonListener::FMCMonListener(bool verbose) 
  : RODimListener(verbose), m_infoTMO(10)
{
}

/// Standard destructor
FMCMonListener::~FMCMonListener() {
  delete m_dns;
  m_dns = 0;
  DimLock lock;
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    Item* it = (*i).second;
    ::dic_release_service(it->id);
    it->data<Descriptor>()->release();
    it->release();
  }
}

/// Add handler for a given message source
void FMCMonListener::addHandler(const std::string& node,const std::string& svc) {
  DimLock lock;
  Clients::iterator i=m_clients.find(node);
  if ( i == m_clients.end() )  {
    if ( ::str_match_wild(svc.c_str(),m_match.c_str()) )  {
      Item* itm = Item::create<Descriptor>(this);
      std::string nam = svc;
      if ( !m_item.empty() ) nam += "/" + m_item;
      m_clients[node] = itm;
      itm->id = ::dic_info_service(nam.c_str(),MONITORED,m_infoTMO,0,0,infoHandler,(long)itm,0,0);
      if ( m_verbose ) log() << "[FMCMonListener] Create DimInfo:" 
                             << nam << "@" << node << " id=" << itm->id << std::endl;
    }
    else if ( m_verbose ) log() << "[FMCMonListener] IGNORE DimInfo:" 
                                << svc << "@" << node << std::endl;
  }
}

/// Remove handler for a given message source
void FMCMonListener::removeHandler(const std::string& node, const std::string& svc)   {
  DimLock lock;
  Clients::iterator i=m_clients.find(node);
  if ( i != m_clients.end() ) {
    if ( ::str_match_wild(svc.c_str(),m_match.c_str()) )  {
      Item* it = (*i).second;
      it->data<Descriptor>()->release();
      ::dic_release_service(it->id);
      if ( m_verbose )   {
        log() << "[FMCMonListener] Delete DimInfo:" 
              << svc << "@" << node << " " << it->id << std::endl;
      }
      it->release();
      m_clients.erase(i);
    }
  }
}

/// DimInfo overload to process messages
void FMCMonListener::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    Item* it = *(Item**)tag;
    Descriptor* d = it->data<Descriptor>();
    RODimListener* l = (RODimListener*)it->object;
    d->copy(address,*size);
    if ( l && l->updateHandler() ) l->updateHandler()->update(it);
  }
}
