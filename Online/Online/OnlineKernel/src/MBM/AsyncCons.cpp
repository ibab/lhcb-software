#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
//#include "bmdef.h"
#include "bm_struct.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

using namespace MBM;

namespace {

  struct Consumer;
  Consumer* s_cons;
  struct Consumer  {
    BMID bmid;
    int nmiss_match;
    int  *prt;
    int  len,evtype, trnumber, triggermask[4];
    Consumer(const char* name)  {
      bmid = mbm_include("0",name,0x103);
      if ( int(bmid) == -1 ) exit(errno);
      int vetomask[4] = {0,0,0,0};
      int trmask[4]   = {-1,-1,-1,-1};
      int icode = mbm_add_req(bmid,1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
      if ( icode != MBM_NORMAL ) exit(icode);
      nmiss_match = 0;
      trnumber = 0;
      s_cons = this;
    }
    static int get_event_ast(void* par) {
      BMDESCRIPT* bm = (BMDESCRIPT*)par;
      USER*       us = bm->user+bm->owner;
      EVENT*      ev = bm->event+us->held_eid;
      mbm_get_event_ast(par);  // Call default implementation
      //printf("EVENT: id=%d uid:%d count=%d %08X data=%d\n",ev->eid, us->held_eid, ev->count, ev->ev_add, **(int**)us->we_ptr_add);
      int status = wtc_insert(WT_FACILITY_DAQ_EVENT, (int)s_cons);
      if( status != WT_SUCCESS ) exit(status);
      return 1;
    }

    static int event_rearm(int fac, int par)  {
      memset(s_cons->triggermask,0xff,sizeof(s_cons->triggermask));
      s_cons->prt = 0;
      int icode = mbm_get_event_a(s_cons->bmid,&s_cons->prt,&s_cons->len,&s_cons->evtype,s_cons->triggermask, 0x103, get_event_ast);
      if( icode != MBM_NORMAL && icode != MBM_NO_EVENT ) ::printf("Error in mbm_get_event_a:%d\n",icode);
      icode = mbm_wait_event_a(s_cons->bmid);
      if( icode != MBM_NORMAL ) ::printf("Error in mbm_get_event_a:%d\n",icode);
      return WT_SUCCESS;
    }

    static int event_action(int fac, int par) {
      static bool first = true;
      Consumer* c = (Consumer*)par;
      if ( first )  {
        first = false;
        c->trnumber = *(c->prt);
      }
      else if( c->trnumber != *(c->prt) ) {
        c->nmiss_match++;
        ::printf("======= Mismatch [%d] found %d %d [0x%08X]\n",
          c->nmiss_match, c->trnumber, *(c->prt), c->prt);
        c->trnumber = *(c->prt);
      }
      else  {
        //::printf("========= Data found %d %d [0x%08X] flag=%d\n",trnumber,*prt, prt, flag);
      }
      c->trnumber++;
      mbm_free_event(c->bmid);
      return WT_SUCCESS;
    }
  };
}

extern "C" int mbm_cons_a(int argc,char **argv) {
  char* name = argv[1];
  Consumer c(name);
  ::printf("Asynchronous Consumer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,lib_rtl_pid(),"0");

  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);

  status = wtc_subscribe(WT_FACILITY_DAQ_EVENT, Consumer::event_rearm, Consumer::event_action);
  if( status != WT_SUCCESS ) exit(status);

  while(1)  {
    int sub_status, userpar, facility;
    status = wtc_wait(&facility, &userpar, &sub_status);
    ::printf("Exited WAIT>>>> Facility = %d Status=%d Sub-Status = %d\n", 
      facility, status, sub_status);
  }
  status = mbm_exclude(c.bmid);
  if(status) exit(status);
}
