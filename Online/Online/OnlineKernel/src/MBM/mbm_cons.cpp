#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "MBM/Consumer.h"

#define PATTERN 0xFEEDBABE

namespace {
  static void help()  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"mbm_cons -opt [-opt]");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -n=<name>              buffer member name");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s=<number>            sleep interval between events [milli seconds]");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=<name>              Buffer identifier ");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p(artition)=<number>  Partition ID");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -q                     Quiet mode (do not print trigger number mismatch)");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -a(synchronous)        Asynchonous mode (default is synchronous)");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -o(ne)                 ONE consumer (REQ type ONE)");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -u(sermode)            USER mode (SPY) consumer");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -f(fifo)               Use fifos connections for communication");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -t(cp)                 Use boost::asio tcp connections for communication");
  }
  struct Cons : public MBM::Consumer  {
    int m_nbad, m_trnumber, m_quiet, m_sleep, m_check;
    Cons(const std::string& b,const std::string& n,int pid,int sleep,bool q, bool chk, int com) 
      : MBM::Consumer(b,n,pid,com), m_nbad(0), m_trnumber(-1), m_quiet(q), 
	m_sleep(sleep), m_check(chk?1:0)
    {
    }
    int eventAction() {
      const MBM::EventDesc& e = event();
      if ( m_trnumber == -1 )  {
        m_trnumber = *e.data;
      }
      //      else if( m_trnumber != *e.data ) {
      else if ( PATTERN != *(unsigned int*)e.data ) {
	if ( !m_quiet && m_check )  {
	  //          ::lib_rtl_output(LIB_RTL_ERROR,"======= Mismatch [%d] found %d %d [%p]",
	  //	   ++m_nbad, m_trnumber, *e.data, (void*)e.data);
	  ::lib_rtl_output(LIB_RTL_ERROR,"++++++Mismatch [%d] found %08X [%p]",
			   ++m_nbad,*e.data,e.data);
        }
        //m_trnumber = *e.data;
      }
      //m_trnumber++;
      if ( m_sleep ) lib_rtl_sleep(m_sleep);
      return Consumer::eventAction();
    }
  };
}

extern "C" int mbm_cons(int argc,char **argv) {  
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", buffer="0";
  int partID = 0x103, sleep = 0, inc_opt=0;
  bool one   = cli.getopt("one",1) != 0;
  bool quiet = cli.getopt("quiet",1) != 0;
  bool async = cli.getopt("asynchronous",1) != 0;
  bool user  = cli.getopt("usermode",1) != 0;
  unsigned int vetomask[4] = {0u,0u,0u,0u};
  unsigned int trmask[4]   = {~0x0u,~0x0u,~0x0u,~0x0u};
  int comm_type = BM_COM_FIFO;

  cli.getopt("name",1,name);
  cli.getopt("buffer",1,buffer);
  cli.getopt("sleep",1,sleep);
  cli.getopt("inc_opt",1,inc_opt);
  cli.getopt("partitionid",1,partID);
  if ( cli.getopt("fifo",1) )
    comm_type = BM_COM_FIFO;
  else if ( cli.getopt("tcp",1) )
    comm_type = BM_COM_ASIO;

  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Consumer \"%s\" (pid:%d) running in buffer:\"%s\"",
		   name.c_str(),Cons::pid(),buffer.c_str());

  Cons c(buffer,name,partID,sleep,quiet,!(one||user), comm_type);
  if ( async )   {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"  Running in asynchronous mode.");
    c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  }
  if ( one ) {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,(inc_opt<<8)+BM_REQ_ONE,BM_FREQ_PERC,100.);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"  Event request type is BM_REQ_ONE [%d].",inc_opt);
  }
  else if ( user ) {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_USER,BM_FREQ_PERC,100.);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"  Event request type is BM_REQ_USER.");
  }
  else {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"  Event request type is BM_REQ_ALL.");
  }
  return c.run();
}
