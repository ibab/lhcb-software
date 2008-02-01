// $Id: ROMonServer.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonServer.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $

// C++ include files
#include "sys/timeb.h"

// Framework include files
#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "ROMon/ROMonServer.h"
#include "ROMon/ROMonOstream.h"

using namespace ROMon;

/// Start monitoring activity
int ROMonServer::monitor(void* buff, size_t len) {
  Node* n = (Node*)buff;
  while(1)    {
    handle(n, len);
    log() << *n << std::endl;
    ::lib_rtl_sleep(2000);
  }
  return 1;
}

/// Handle update
int ROMonServer::handle(void* buff, size_t /* len */)  {
  Node* n = (Node*)buff;
  lib_rtl_gbl_t bm_all = 0;
  timeb tm;
  ::ftime(&tm);
  n->reset();
  n->time = tm.time;
  n->millitm = tm.millitm;
  ::strncpy(n->name,RTL::nodeName().c_str(),sizeof(n->name));
  n->name[sizeof(n->name)-1] = 0;
  Node::Buffers* b = n->buffers();
  Node::Buffers::iterator mbm = b->reset();

  int status = ::mbm_map_global_buffer_info(&bm_all,false);
  if( lib_rtl_is_success(status) )   {   
    try {
      BUFFERS* buffers = (BUFFERS*)bm_all->address;
      for (int i = 0; i < buffers->p_bmax; ++i)  {
	if ( buffers->buffers[i].used == 1 )  {
	  const char* bm_name = buffers->buffers[i].name;
	  BMID dsc = ::mbm_map_mon_memory(bm_name);
	  if ( dsc ) {
	    try {
	      dumpBufferInfo(bm_name,dsc,mbm);
	    }
	    catch(...)    {
	    }
	    ::mbm_unmap_memory(dsc);
	    mbm = b->add(mbm);
	  }
	}
      }
    }
    catch(...) {
    }
    ::mbm_unmap_global_buffer_info(bm_all,false);
    return 1;
  }
  //::lib_rtl_printf("Cannot map global MBM buffer information....\n");
  return 0;
}

/// Dump buffer information to memory
void ROMonServer::dumpBufferInfo(const char* bm_name, BMID dsc, MBMBuffer* mbm)   {
  CONTROL  *ctr = mbm_get_control_table(dsc);
  USER     *us  = mbm_get_user_table(dsc), *utst=(USER*)~0x0;
  MBMBuffer::Clients::iterator t = mbm->clients.reset();
  ::memset(mbm,0,sizeof(MBMBuffer));
  ::memcpy(&mbm->ctrl,ctr,sizeof(CONTROL));
  ::strncpy(mbm->name,bm_name,sizeof(mbm->name));
  mbm->name[sizeof(mbm->name)-1] = 0;
  for (int j=0; j < ctr->p_umax; j++,us++)    {
    if ( us == utst || us == 0              ) break;
    if ( us->block_id != int(MBM::BID_USER) ) continue;
    if ( us->busy     == 0                  ) continue;
    t->reqs[0] = t->reqs[1] = t->reqs[2] = t->reqs[3] = ' ';
    for (int k=0; k<us->n_req; ++k )  {
      if      ( us->req[k].user_type == BM_REQ_ONE  ) t->reqs[1] = '1';
      else if ( us->req[k].user_type == BM_REQ_USER ) t->reqs[2] = 'U';
      else if ( us->req[k].user_type == BM_REQ_ALL  ) t->reqs[3] = 'A';
      else if ( us->req[k].freq < 100.0             ) t->reqs[0] = 'S';
    }
    strncpy(t->name,us->name,sizeof(t->name));
    t->name[sizeof(t->name)-1] = 0;
    t->partitionID = us->partid;
    t->processID   = us->pid;
    t->type        = 'U';
    t->state       = 0;
    t->events      = 0;
    if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
      t->type      = 'P';
      t->state     = us->p_state+1;
      t->events    = us->ev_produced;
    }
    else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
      t->type      = 'C';
      t->state     = us->c_state+1;
      t->events    = us->ev_seen;
    }
    t = mbm->clients.add(t);
  }
}
