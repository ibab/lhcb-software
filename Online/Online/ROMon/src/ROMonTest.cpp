// $Id: ROMonTest.cpp,v 1.2 2008-11-11 15:09:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonTest.cpp,v 1.2 2008-11-11 15:09:26 frankb Exp $

// Framework includes
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMonServer.h"
#include "ROMon/ROMonOstream.h"

using namespace ROMon;
using namespace std;

static FSMTask& fill(FSMTask* t, const MBMClient& c) {
  ::strcpy(t->name,c.name);
  t->processID   = c.processID;
  t->state       = 'I';
  t->targetState = 'R';
  t->metaState   = 'E';
  t->lastCmd  = 0;
  t->doneCmd = 1;
  return *t;
}

static MBMBuffer& fill(MBMBuffer* mbm, int partitionID, const char* name) {
  static int task_no = 0;
  const char types[] = {'C','P','U'};
  ::memset(mbm,0,sizeof(MBMBuffer));
  MBMBuffer::Clients::iterator it = mbm->clients.reset();
  ::strcpy(mbm->name,name);
  for(int i=0; i<10; ++i, it=mbm->clients.add(it)) {
    ::sprintf((*it).name,"Task_%s_%02d",name,++task_no);
    (*it).partitionID = partitionID;
    (*it).processID   = 1024+10*i+task_no;
    (*it).events      = 1000*i;
    (*it).type        = types[i%3];
    (*it).state       = 'a'+i;
  }
  return *mbm;
}

static Node& fill(Node* n, const char* name) {
  char txt[64];
  static int buff_no = 0;
  n->reset();
  ::strcpy(n->name,name);
  Node::Buffers* b = n->buffers();
  Node::Buffers::iterator ib = b->reset();
  for(int i=0; i<10; ++i, ib = b->add(ib) ) {
    ::sprintf(txt,"Events_%02d",++buff_no);
    fill(ib, 0x111*i, txt);
  }
  Node::Tasks* t = n->tasks();
  Node::Tasks::iterator it = t->reset();
  for(ib=b->begin(); ib != b->end(); ib = b->next(ib) ) {
    MBMBuffer::Clients::iterator ic=(*ib).clients.begin();
    for(; ic != (*ib).clients.end(); ++ic, it=t->add(it))
      fill(it,*ic);
  }
  n->fixup();
  return *n;
}


extern "C" int romon_test_structs(int, char**) {
  size_t len = 1024*64;
  char* buff = new char[len];
  typedef FixItems<int> IntItems;
  IntItems* i_items = (IntItems*)buff;
  i_items->reset();
  log() << *i_items << endl;
  for(int i=0, *i_it=i_items->begin(); i<10; ++i) {
    *i_it = i;
    i_it=i_items->add(i_it);
  }
  log() << *i_items << endl;
  log() << fill((MBMBuffer*)buff, 0x14d, "Events") << endl;
  log() << fill((Node*)buff, "storectl01") << endl;
  delete [] buff;
  return 1;
}
	
extern "C" int romon_test_serv(int, char**) {
  size_t len = 32*1024;
  char* buff = new char[len];
  ROMonServer mon(2000);
  int sc = mon.monitor(buff,len);
  ::delete [] buff;
  return sc;
}
