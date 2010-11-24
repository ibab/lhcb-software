// $Id: RODimFSMListener.cpp,v 1.9 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RODimFSMListener.cpp,v 1.9 2010-10-14 13:30:09 frankb Exp $

// Framework includes
#include "dic.hxx"
#include "RTL/rtl.h"
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimFSMListener.h"
#include "GaudiKernel/strcasecmp.h"

namespace {
  struct FSMMonitoring {
    unsigned long lastCmd, doneCmd;
    int pid;
    char targetState, state, metaState, pad;
    int partitionID;
  };
}
using namespace ROMon;
using namespace std;


/// Standard destructor
RODimFSMListener::~RODimFSMListener() {
  DimLock lock;
  delete m_dns;
  m_dns = 0;
  for (Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i) {
    ::dic_release_service((*i).second->id);
    (*i).second->release();
  }
}

/// Add handler for a given message source
void RODimFSMListener::addHandler(const string& node,const string& svc)    {
  static string myNode = RTL::nodeNameShort();
  static string myUtgid = RTL::processName();
  if ( ::strncasecmp(node.c_str(),myNode.c_str(),myNode.length()) == 0 ) {
    if ( ::strncasecmp(svc.c_str(),myUtgid.c_str(),myUtgid.length()) != 0 ) {
      if ( ::strncasecmp(svc.c_str(),"DIS_DNS",7) != 0 ) {
        DimLock lock;
        string nam = svc+"/fsm_status";
        Clients::iterator i=m_clients.find(nam);
        if ( i == m_clients.end() )  {
          Item*    itm = Item::create<FSMTask>(this);
          FSMTask* t   = itm->data<FSMTask>();
          ::strncpy(t->name,svc.substr(0,svc.find("/")).c_str(),sizeof(t->name));
          t->name[sizeof(t->name)-1] = 0;
          t->processID   = -1;
          t->partitionID = -1;
          m_clients[nam] = itm;
          itm->id = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,infoHandler,(long)itm,0,0);
          if ( m_verbose ) {
            log() << "Create DimInfo:" << nam << " id:" << itm->id << endl;
          }
        }
      }
    }
  }
}

/// Remove handler for a given message source
void RODimFSMListener::removeHandler(const string& /* node */, const string& svc)   {
  DimLock lock;
  string nam = svc+"/fsm_status";
  Clients::iterator i=m_clients.find(nam);
  if ( i != m_clients.end() ) {
    Item* it = (Item*)(*i).second;
    ::dic_release_service(it->id);
    if ( m_verbose )  {
      log() << "Delete DimInfo:" << nam << " id:" << it->id << endl;
    }
    it->release();
    m_clients.erase(i);
  }
}

/// DimInfo overload to process messages
void RODimFSMListener::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    static int count = 0;
    FSMMonitoring* mon = (FSMMonitoring*)address;
    ++count;
    if ( mon && mon->pid != -1 )   {
      Item* itm      = *(Item**)tag;
      FSMTask* t     = itm->data<FSMTask>();
      t->processID   = mon->pid;
      t->partitionID = mon->partitionID;
      t->state       = mon->state;
      t->targetState = mon->targetState;
      t->metaState   = mon->metaState;
      t->lastCmd     = mon->lastCmd;
      t->doneCmd     = mon->lastCmd;
      //RODimFSMListener* l = (RODimFSMListener*)itm->object;
      //if ( l->m_verbose && ((++count)%1000)==0 ) {
      //log() << "Processed " << count << " FSM task updates." << endl;
      //}
    }
  }
}

extern "C" int romon_test_fsm_listener(int, char**) {
  bool do_run = true;
  RODimFSMListener listener;
  log() << "Going asleep" << endl;
  while(do_run) {
    ::lib_rtl_sleep(1000);
  }
  return 1;
}

