#include "RTL/rtl.h"
#include "MBM/Producer.h"

#define PATTERN 0xFEEDBABE

static void help()  {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"mbm_prod -opt [-opt]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -n=<name>      buffer member name\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -m=<number>    number of events  \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s=<number>    event size [bytes]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=<name>      Buffer identifier \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -l=<name>      Sweep lower limit \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -u=<name>      Sweep upper limit \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -f(fifo)       Use fifos connections for communication");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -t(cp)         Use boost::asio tcp connections for communication");
}

extern "C" int mbm_prod(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer", buffer="0";
  int trnumber = -1, len = 1792, nevt = 1000000, lower=4, upper=1024*1024;
  int comm_type = BM_COM_FIFO;
  cli.getopt("name",1,name);
  cli.getopt("m",1,nevt);
  cli.getopt("size",1,len);
  cli.getopt("buffer",1,buffer);
  cli.getopt("lower",1,lower);
  cli.getopt("upper",1,upper);
  if ( cli.getopt("fifo",1) )
    comm_type = BM_COM_FIFO;
  else if ( cli.getopt("tcp",1) )
    comm_type = BM_COM_ASIO;
  if ( lower > 4 ) len = lower;
  if ( lower < 4 ) len = lower;
  {
    MBM::Producer p(buffer,name,0x103,comm_type);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Producer \"%s\" (pid:%d) included in buffer:\"%s\" len=%d [%d,%d] nevt=%d\n",
		     name.c_str(), MBM::Producer::pid(), buffer.c_str(), len, lower, upper, nevt);
    while(nevt--)  {
      if ( p.getSpace(lower!=4 ? upper : len) == MBM_NORMAL ) {
	MBM::EventDesc& e = p.event();
	//*e.data   = trnumber;
	*e.data=PATTERN;
	*(int*)((char*)e.data+len-sizeof(int)) = trnumber;
	trnumber++;
	e.type    = 1;
	e.mask[0] = 0x103;
	e.mask[1] = 0;
	e.mask[2] = 0;
	e.mask[3] = 0;
	e.len     = len;
	if ( lower != 4 ) {
	  ++len;
	  if ( len > upper )  {
	    len = lower;
	    ::lib_rtl_output(LIB_RTL_ALWAYS,"Max. event size of %d bytes reached. wrapping back to %d bytes.\n", upper, lower);
	  }
	}
	p.sendEvent();
	// lib_rtl_sleep(1);
      }
    }
  }
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Max. event size of %d bytes reached. wrapping back to %d bytes.\n", upper, lower);
  ::lib_rtl_sleep(1000);
  //::exit(0);
  return 1;
}
