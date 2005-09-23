#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include "bm_struct.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
using namespace MBM;

namespace {
  struct Producer;
  Producer* s_prod = 0;

  struct Producer {
    BMID bmid;
    int *add;
    int len, trnumber;
    Producer(const char* name) {
      s_prod = this;
      add = 0;
      trnumber = 0;
      len = 1792;
      bmid = mbm_include("0", name,0x103);
      if ( int(bmid) == -1 ) exit(errno);
    }

    static int prod_exit(int c)  {
      exit(c);
    }

    static int space_ast(void* param)    {
      BMDESCRIPT *bm = (BMDESCRIPT*)param;
      USER *us = bm->_user();
      if ( us == (void*)0 )  {
        return 1;
      }
      if (us->p_state != S_wspace_ast_queued)   {
        printf("us->p_state Not S_wspace_ast_queued, but is %d\n",us->p_state);
        us->p_state = S_wspace_ast_handled;
        us->reason = -1;
        return 1;
      }
      us->p_state = S_active;
      if ( !(us->reason&BM_K_INT_SPACE) )   {
        printf("space_ast spurious wakeup reason = %d\n",us->reason);
        us->p_state = S_wspace_ast_handled;
        us->reason = -1;
        return 1;
      }
      *us->ws_ptr_add = (int*)(us->space_add+bm->buffer_add);
      *us->ws_ptr_add += sizeof(int);
      us->p_state = S_wspace_ast_handled;
      us->reason = -1;
      int status = wtc_insert(WT_FACILITY_DAQ_SPACE, (int)s_prod);
      if( status != WT_SUCCESS ) exit(status);
      return 1;
    }

    static int space_rearm(int fac, int par)  {
      int icode = mbm_get_space_a(s_prod->bmid, s_prod->len, &s_prod->add, space_ast);
      if ( icode != MBM_NORMAL )  {
        prod_exit(icode);
      }
      icode = mbm_wait_space_a(s_prod->bmid);
      if ( icode != MBM_NORMAL )  {
        prod_exit(icode);
      }
      return WT_SUCCESS;
    }

    static int space_action(int fac, int par) {
      int flen;
      void *fadd;
      Producer* p = (Producer*)par;
      *(p->add) = s_prod->trnumber;
      unsigned int mask[4] = {0x103, 0, 0, 0,};
      int icode = mbm_declare_event(p->bmid, p->len, 1, mask, 0, &fadd, &flen, 0x103);
      if ( icode != MBM_NORMAL ) {
        prod_exit(icode);
      }
      icode = mbm_send_space(p->bmid);
      if ( icode != MBM_NORMAL ) {
        prod_exit(icode);
      }
      if ( p->trnumber % 10 ) {
        p->trnumber++;
      }
      else  {
        p->trnumber++;
      }
      return WT_SUCCESS;
    }
  };
}

extern "C" int mbm_prod_a(int argc,char **argv) {
  int trnumber = 0, icode;
  char *name = argv[1];  
  Producer p(name);

  ::printf("Asynchronous Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,lib_rtl_pid(),"0");
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);

  status = wtc_subscribe( WT_FACILITY_DAQ_SPACE, Producer::space_rearm, Producer::space_action);
  if( status != WT_SUCCESS ) exit(status);

  while(1)  {
    int sub_status, userpar, facility;
    status = wtc_wait(&facility, &userpar, &sub_status);
    ::printf("Exited WAIT>>>> Facility = %d Status=%d Sub-Status = %d\n", 
      facility, status, sub_status);
  }
  icode = mbm_exclude(p.bmid);
  if(icode) exit(icode);
}
