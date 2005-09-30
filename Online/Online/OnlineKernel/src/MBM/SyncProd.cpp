#include <cstdio>
#include <climits>
#include "MBM/Producer.h"

extern "C" int mbm_prod(int argc,char **argv) {
  const char* name = argc>1 ? argv[1] : "producer";
  int trnumber = -1, len = 1792;
  MBM::Producer p("0",name,0x103);
  ::printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,MBM::Producer::pid(),"0");
  int cnt = 1000;//INT_MAX;

  while(--cnt)  {
    if ( p.getSpace(len) == MBM_NORMAL ) {
      MBM::EventDesc& e = p.event();
      *e.data   = ++trnumber;
      e.type    = 1;
      e.mask[0] = 0x103;
      e.mask[1] = 0;
      e.mask[2] = 0;
      e.mask[3] = 0;
      e.len     = len;
      p.sendEvent();
    }
  }
  exit(0);
  return 1;
}
