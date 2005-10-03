#include <cstdio>
#include <cctype>
#include "MBM/Producer.h"

extern "C" int mbm_prod(int argc,char **argv) {
  const char* name = "producer";
  int trnumber = -1, len = 1792, nevt = 1000000;
  while ( --argc > 0 ) {                                  /* process options  */
    const char* cptr = argv[argc];
    if ( *cptr == '-' || *cptr == '/' ) {
      switch( ::tolower(cptr[1]) ) {
      case 'n':
	name = cptr+3;
	break;
      case 'm':
	::sscanf(cptr+3,"%d",&nevt);
	break;
      case 's':
	::sscanf(cptr+3,"%d",&len);
	break;
      case 'h':
      case '?':
      default:
	::printf("mbm_prod -opt [-opt]\n");
        ::printf("    -n=%-8s    buffer member name\n",name);
        ::printf("    -m=%-8d    number of events\n",nevt);
        ::printf("    -s=%-8d    event size [bytes]\n",len);
        return 0;
      }
    }
  }
  MBM::Producer p("0",name,0x103);
  ::printf("Producer \"%s\" (pid:%d) included in buffer:\"%s\" len=%d nevt=%d\n",
	   name, MBM::Producer::pid(), "0", len, nevt);
  while(nevt--)  {
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
  //exit(0);
  return 1;
}
