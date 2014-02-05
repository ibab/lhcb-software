#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/ProcessGroup.h"
#include <iostream>

using namespace std;
using namespace RTL;

static string command() {
  string cmd = ::getenv("GaudiOnlineExe");
  return cmd;
}
#if 0
static string gentest() {
  string cmd = ::getenv("gentest_exe");
  return cmd;
}

static void i_putenv(const string& n, const string& v) {
  string val = n+"="+v;
  ::putenv((char*)val.c_str());
}


static string makeOpts(const string& n) {
  string v = "-opt=";
  v += ::getenv("GAUDIONLINE_OPTS");
  v += ("/"+n);
  return v;
}
#endif

static string pyOpts(const string& n) {
  string v = "-opt=command="+n;
  ::printf("Python options:%s\n",v.c_str());
  return v;
}

static string gaudionlineOpts()  {
  string v = ::getenv("GAUDIONLINE_OPTS");
  return v;
}


#define CLASS1(XXXXX)    {"libGaudiOnline.so","OnlineTask","-tasktype=LHCb::Class1Task","-msgsvc=MessageSvc","-auto",main_opts.c_str(),(new string(makeOpts( XXXXX )))->c_str(),0}
#define CLASS1_PY(XXXXX) {"libGaudiOnline.so","OnlineTask","-tasktype=LHCb::Class1Task","-msgsvc=LHCb::FmcMessageSvc","-auto",main_opts.c_str(),(new string(pyOpts( XXXXX )))->c_str(),0}

static int collect_summary(size_t len,Process* p[], bool run_summary=true) {
  const char *asum[] ={"libOnlineKernel.so", "mbm_summary","-nostates",0};
  ::lib_rtl_sleep(4000);
  if ( run_summary ) {
    Process* summary=new Process("MBMsummary_0",command(),asum);
    summary->start();
    summary->wait(Process::WAIT_BLOCK);
    delete summary;
    cout << "Summary task finished work.. " << endl;
  }
  ::lib_rtl_sleep(3000);
  for(int i=len-1; i>=0; --i) if ( p[i] )   {
    p[i]->stop();
    ::lib_rtl_sleep(500);
    p[i]->wait(Process::WAIT_BLOCK);
  }
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);
  return 0;
}

struct MooreTest {
  int m_fifo;
  std::string m_fifoName;
  lib_rtl_thread_t m_messagePump;
  MooreTest(int argc, char** argv);
  int startLogger(const std::string&  fifoName);
  static int readMessages(void* param);
};

static void help()  {
  ::printf("mep_prod_a -opt [-opt]\n");
  ::printf("    -n(ame)=<name>         buffer member name\n");
  ::printf("    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
  ::printf("    -i(nput)=<name>        Buffer name to place data.\n");
  ::printf("    -s(pace)=<number>      Default space allocation in kBytes\n");
  ::printf("    -c(ount)=<number>      Number of events to be sent. default: unlimited\n");
  ::printf("    -p(artition)=<number>  Partition ID\n");
  ::printf("    -r(efcount)=<number>   Initial MEP reference count\n");
  ::printf("    -m(apunused)           Map unused MEP buffers\n");
  ::printf("    -d(ebug)               Invoke debugger\n");
}

MooreTest::MooreTest(int argc, char** argv)
  : m_fifo(-1)
{
  RTL::CLI cli(argc, argv, help);
  m_fifoName = "/tmp/LogTest.fifo";
  m_fifo = startLogger(m_fifoName);
  int sc = ::lib_rtl_start_thread(readMessages,this,&m_messagePump);
  if ( !lib_rtl_is_success(sc) ) {
    throw runtime_error("Failed to start message pump....");
  }
}

int MooreTest::startLogger(const string&  fifoName) {
  int fifo = -1;
  if( ::mkfifo(fifoName.c_str(),0666)  ) {
    if ( errno != EEXIST ) { // It is not an error if the file already exists....
      ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to create the fifo: %s.\n",fifoName.c_str());
      return -1;
    }
  }
  ::chmod(fifoName.c_str(),0666);
  if( -1 == (fifo=::open(fifoName.c_str(),O_RDWR|O_NONBLOCK)) ) {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to open the fifo: %s\n",fifoName.c_str());
    return -1;
  }
  if ( -1 == (fcntl(fifo,F_SETFL,fcntl(fifo,F_GETFL)|O_NONBLOCK)) ) {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to set fifo: %s non-blocking\n",fifoName.c_str());
    return -1;
  }
  return fifo;
}

int MooreTest::readMessages(void* param) {
  MooreTest* test = (MooreTest*)param;
  char buffer[1024];
  int fd = test->m_fifo;
  while(1) {
    int sc = ::read(fd,buffer,sizeof(buffer));
    if ( sc >  0 ) {
      buffer[sc] = 0;
      ::puts(buffer);
    }
    else if ( sc <= 0 && errno == EINTR  ) continue;
    else if ( sc <= 0 && errno == EAGAIN ) continue;
    else {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Recv message FAIL");
      return -1;
    }
  }
}


//
//   Test of HLT event processing
//
extern "C" int moore_test(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string host = RTL::nodeNameShort();



  string out = argc>1 ? argv[1] : "/dev/null";
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer(partitionBuffers=True)");
  const char *a3[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(percent=100.0,partitionBuffers=True)");
  const char *a4[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runSender(target=None,partitionBuffers=True)");
  const char *aprod[]={"libGaudiOnline.so", "OnlineStart", "libGaudiOnline.so", "mep2mbm_producer",
		       "-n=prod_0","-p=333","-s=500","-r=2","-c=5000",
		       "-input=Events_LHCb","-file=mepData_0.dat",0};

  string m_fifoName="/tmp/LogTest.fifo";


  Process::setDebug(true);  
  


  pg.add(p[0]=new Process("MEPInit_0",  command(),a1,out.c_str()));

  pg.start();
  ::lib_rtl_sleep(8000);
  pg.add(p[2] =new Process("Moore_1",   command(),a3,out.c_str()));
  pg.add(p[3] =new Process("Moore_2",   command(),a3,out.c_str()));
  pg.add(p[4] =new Process("Moore_3",   command(),a3,out.c_str()));
  pg.add(p[5] =new Process("Moore_4",   command(),a3,out.c_str()));
  pg.add(p[6] =new Process("Cons_0",    command(),a4,out.c_str()));

  cout << "Starting processes ..... " << endl;
  pg.start();
  ::lib_rtl_sleep(20000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("Prod_0",command(),aprod,out.c_str());
  prod->start();
  prod->wait(Process::WAIT_BLOCK);
  ::lib_rtl_sleep(10000);
  delete prod;
  cout << "Producer finished work.. " << endl;
  return collect_summary(sizeof(p)/sizeof(p[0]),p);
}
