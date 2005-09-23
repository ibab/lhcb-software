#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
//#include "bmdef.h"
#include "RTL/rtl.h"
#include "bm_struct.h"
using namespace MBM;

int get_event_ast(void* par) {
  mbm_get_event_ast(par);  // Call default implementation
  BMDESCRIPT* bm = (BMDESCRIPT*)par;
  USER*       us = bm->user+bm->owner;
  EVENT*      ev = bm->event+us->held_eid;
  //printf("EVENT: id=%d uid:%d count=%d %08X data=%d\n",ev->eid, us->held_eid, ev->count, ev->ev_add, **(int**)us->we_ptr_add);
  return 1;
}

extern "C" int mbm_cons(int argc,char **argv) {
  int  trnumber = 0;
  char* name = argv[1];
  BMID bmid = mbm_include("0",name,0x103);
  if ( int(bmid) == -1 ) exit(errno);
  //int mbm_add_req (BMDESCRIPT *bm, int evtype, int trmask[4], int veto[4], int masktype, 
  //               int usertype, int freqmode, float freq);
  ::printf("Consumer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,lib_rtl_pid(),"0");

  int trmask[4] = {-1,-1,-1,-1};
  int vetomask[4] = {0,0,0,0};
  int icode = mbm_add_req(bmid,1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
  if ( icode != MBM_NORMAL ) exit(icode);
  int nmiss_match = 0;
  bool first = true;
  while(1)  {
    int  *prt = 0;
    int  len,evtype, flag=0;
    int triggermask[4] = {-1,-1,-1,-1};
    // int mbm_get_event_a (BMDESCRIPT *bm, int* ptr, int* size, int* evtype, int* trmask, int part_id, int astadd) {

    icode = mbm_get_event_a(bmid,&prt,&len,&evtype,triggermask, 0x103, get_event_ast);
    if( icode != MBM_NORMAL && icode != MBM_NO_EVENT ) ::printf("Error in mbm_get_event_a:%d\n",icode);
    icode = mbm_wait_event(bmid);
    if( icode != MBM_NORMAL ) ::printf("Error in mbm_get_event_a:%d\n",icode);
    if ( first )  {
      first = false;
      trnumber = *prt;
    }
    else if( trnumber != *prt ) {
      nmiss_match++;
      ::printf("======= Mismatch [%d] found %d %d [0x%08X] flag=%d\n",
        nmiss_match, trnumber, *prt, prt, flag);
      trnumber = *prt;
    }
    else  {
      //::printf("========= Data found %d %d [0x%08X] flag=%d\n",trnumber,*prt, prt, flag);
    }
    trnumber++;
    mbm_free_event(bmid);
  }
  icode = mbm_exclude(bmid);
  if(icode) exit(icode);
}
