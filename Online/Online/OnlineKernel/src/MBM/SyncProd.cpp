#include <cstdio>
#include "MBM/Producer.h"

extern "C" int mbm_prod(int argc,char **argv) {
  const char* name = argc>1 ? argv[1] : "producer";
  int trnumber = -1, len = 1792;
  Producer p(name);
  ::printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",name,Producer::pid(),"0");
  while(1)  {
    if ( p.getSpace(len) == MBM_NORMAL ) {
      *p.event.data   = ++trnumber;
      p.event.type    = 1;
      p.event.mask[0] = 0x103;
      p.event.mask[1] = 0;
      p.event.mask[2] = 0;
      p.event.mask[3] = 0;
      p.event.len     = len;
      p.sendEvent();
    }
  }
  return 1;
}
