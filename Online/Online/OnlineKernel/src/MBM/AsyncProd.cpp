#include "MBM/Producer.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"

namespace {

  /// Function to display help text
  static void help()  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"mbm_prod_a -opt [-opt]\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -n=<name>              Buffer member name\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=<name>              Buffer identifier \n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p(artition)=<number>  Partition ID\n");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -f(fifo)               Use fifos connections for communication");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -t(cp)                 Use boost::asio tcp connections for communication");
  }

  /** @class AsyncProd
   *
   *  Test class to test MBM functionality
   * 
   *  @author  M.Frank
   *  @version 1.0
   */
  struct AsyncProd  : public MBM::Producer  {
    int trnumber;
    AsyncProd(const std::string& buff, const std::string& nam, int pid, int com) 
      : MBM::Producer(buff,nam,pid,com), trnumber(0)
    {
      setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
    }
    int spaceRearm(int) {
      return Producer::spaceRearm(1792);
    }
    int spaceAction() {
      *m_event.data  = trnumber++;
      m_event.mask[0] = partitionID();
      m_event.mask[1] = 0;
      m_event.mask[2] = 0;
      m_event.mask[3] = 0;
      m_event.type    = 1;
      return Producer::spaceAction();
    }
  };
}

/// Main invokation routine to be called by test program
extern "C" int mbm_prod_a(int argc,char **argv) {
  RTL::CLI cli(argc, argv, help);
  std::string name = "producer", buffer="0";
  int comm_type = BM_COM_FIFO;
  int partID = 0x103;
  cli.getopt("name",1,name);
  cli.getopt("buffer",1,buffer);
  cli.getopt("partitionid",1,partID);
  if ( cli.getopt("fifo",1) )
    comm_type = BM_COM_FIFO;
  else if ( cli.getopt("tcp",1) )
    comm_type = BM_COM_ASIO;
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::lib_rtl_output(LIB_RTL_INFO,"Asynchronous Producer \"%s\" (pid:%d) included in buffer:\"%s\"\n",
		   name.c_str(),AsyncProd::pid(),buffer.c_str());
  return AsyncProd(buffer,name,partID,comm_type).run();
}
