#include <cstdio>
#include "MBM/Consumer.h"

extern "C" int mbm_cons(int argc,char **argv) {
  int  trnumber = -1, nbad = 0;
  int  trmask[4] = {-1,-1,-1,-1};
  int  vetomask[4] = {0,0,0,0};
  const char* name = argc>1 ? argv[1] : "consumer";

  MBM::Consumer c("0",name,0x103);
  ::printf("Consumer \"%s\" (pid:%d) including buffer:\"%s\"\n",name,c.pid(),"0");
  c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_VIP,BM_FREQ_PERC,100.);
  while(1)  {
    c.getEvent();
    const MBM::EventDesc& e = c.event();
    if ( -1 == trnumber )  {
      trnumber = *e.data;
    }
    else if( trnumber != *e.data ) {
      ::printf("======= Mismatch [%d] found %d %d [0x%p]\n", nbad++, trnumber, *e.data, e.data);
      trnumber = *e.data;
    }
    trnumber++;
    c.freeEvent();
  }
}
