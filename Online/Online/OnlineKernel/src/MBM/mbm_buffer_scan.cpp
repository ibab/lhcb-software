#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "MBM/bmdef.h"
#include "MBM/Producer.h"

#include <cstring>
using namespace std;
using namespace RTL;

static void help()  {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"mbm_buffer_scan -opt [-opt]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=[Events]  buffer name\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s=[200]     buffer size in kB\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -l=[1023]    Last scan point\n");
}
#define KByte 1024

extern "C" int mbm_buffer_scan(int argc , char ** argv) {
  int tot, last;
  string buffer;
  char txt[32], buff[32];
  const char* args[] = {"mbm_buffer_scan","-s=60001","-e=16","-u=4","-i=0","-f","-c"};
  CLI cli(argc, argv, help);
  cli.getopt("buffer",1,buffer="Events");
  cli.getopt("size",1,tot=200);
  cli.getopt("last",1,last=1024);
  int diff = KByte;
  int stot = tot*KByte;
  int s1   = (tot-32)*KByte;
  ::snprintf(txt, sizeof(txt), "-s=%d",stot/KByte);
  ::snprintf(buff,sizeof(txt), "-i=%s",buffer.c_str());
  args[1] = txt;
  args[4] = buff;
  mbm_install(7,(char**)args);
  MBM::Producer p1(buffer,"p1",0x103);
  MBM::Producer p2(buffer,"p2",0x103);
  
  if ( MBM_NORMAL == p1.getSpace(s1) ) {
    MBM::EventDesc& e1 = p1.event();
    int cnt = 0;
    int s2 = 0;
    ::memset(e1.data,0xCD,s1);
    while( (s2+=diff) + s1 <= stot )   {
      bool end = false;
      if ( s1 + s2 + diff > stot )   {
	s2 = stot-s1-diff;
	end = true;
      }
      else {
	continue;
      }
      for(int scan = 0; scan<last; ++scan) {
	s2 = s2 + 1;
	if ( ((++cnt)%1000) == 0 || end ) {
	  int t1 = ((s1+KByte-1)/KByte)*KByte;
	  int t2 = ((s2+KByte-1)/KByte)*KByte;
	  const char* flag = end ? "LAST" : "NORMAL";
	  ::lib_rtl_output(LIB_RTL_ALWAYS,"%-6s %6d Buffer sizes: %8d/%8d [%4d] and %8d/%8d [%4d] bytes: %8d/%8d [%4d] bytes total\n", 
			   flag, cnt, s1, t1, int(t1/KByte), s2, t2, int(t2/KByte), s1+s2, t1+t2, int((t1+t2)/KByte));
	}
	if ( scan == 1022 ) {
	  ++s1;
	  --s1;
	}
	if ( MBM_NORMAL == p2.getSpace(s2) ) {
	  MBM::EventDesc& e2 = p2.event();
	  ::memset(e2.data,0xEF,s2);
	  e2.type    = 1;
	  e2.mask[0] = 0x103;
	  e2.mask[1] = 0;
	  e2.mask[2] = 0;
	  e2.mask[3] = 0;
	  e2.len     = s2;
	  p2.sendEvent();
	}
      }
      if ( end ) break;
    }
    e1.type    = 1;
    e1.mask[0] = 0x103;
    e1.mask[1] = 0;
    e1.mask[2] = 0;
    e1.mask[3] = 0;
    e1.len     = s1;
    p1.sendEvent();
  }
  return 0;
}
