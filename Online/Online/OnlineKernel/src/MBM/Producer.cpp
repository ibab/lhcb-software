#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include "bm_struct.h"
#include "RTL/rtl.h"

using namespace MBM;

namespace {
  int space_ast(void* param)    {
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
    return 1;
  }

  int prod_exit(int c)  {
    exit(c);
  }
}

extern "C" int mbm_prod(int argc,char **argv) {
  int trnumber = 0, icode;
  char *name = argv[1];  
  BMID bmid = mbm_include("0", name,0x103);
  if ( int(bmid) == -1 ) exit(errno);

  ::printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,lib_rtl_pid(),"0");
  while(1)  {
    BMDESCRIPT* bm = (BMDESCRIPT*)bmid;
    int len = 1792, flen;
    int* add = 0;
    void *fadd;
    icode = mbm_get_space_a(bmid, len, &add, space_ast);
    if ( icode != MBM_NORMAL )  {
      prod_exit(icode);
    }
    icode = mbm_wait_space(bmid);
    if ( icode != MBM_NORMAL ) {
      prod_exit(icode);
    }
    *add = trnumber;
    unsigned int mask[4] = {0x103, 0, 0, 0,};
    icode = mbm_declare_event(bmid, len, 1, mask, 0, &fadd, &flen, 0x103);
    if ( icode != MBM_NORMAL ) {
      prod_exit(icode);
    }
    icode = mbm_send_space(bmid);
    if ( icode != MBM_NORMAL ) {
      prod_exit(icode);
    }
    if ( trnumber % 10 )   {
      trnumber++;
    }
    else  {
      trnumber++;
    }
  }
  icode = mbm_exclude(bmid);
  if(icode) exit(icode);
}
