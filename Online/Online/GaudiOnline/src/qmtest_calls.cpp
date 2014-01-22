#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "RTL/ProcessGroup.h"
#include <iostream>

using namespace std;
using namespace RTL;

static string command() {
  string cmd = ::getenv("GaudiOnlineExe");
  return cmd;
}

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
#define CLASS1_PY(XXXXX) {"libGaudiOnline.so","OnlineTask","-tasktype=LHCb::Class1Task","-msgsvc=MessageSvc","-auto",main_opts.c_str(),(new string(pyOpts( XXXXX )))->c_str(),0}

extern "C" int pyhlt_test(int /* ac  */, char** /* av */)  {
  i_putenv("TAN_PORT","YES");
  i_putenv("TAN_NODE",RTL::nodeNameShort());
  i_putenv("NODENAME",RTL::nodeNameShort());
  i_putenv("DATAINTERFACE",RTL::nodeNameShort());
  i_putenv("DIM_DNS_NODE",RTL::nodeNameShort());
  string cmd = gentest()+" libGaudiOnline.so pyhlt_test_run";
  system(cmd.c_str());
  return 0;
}

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
//
//   Test of HLT event processing
//
extern "C" int qmtest_hlt(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = argc>1 ? argv[1] : "/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer(partitionBuffers=True)");
  const char *a3[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(percent=100.0,partitionBuffers=True)");
  const char *a4[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runSender(target=None,partitionBuffers=True)");
  const char *aprod[]={"libGaudiOnline.so", "OnlineStart", "libGaudiOnline.so", "mep2mbm_producer",
		       "-n=prod_0","-p=333","-s=500","-r=2","-c=5000",
		       "-input=Events_LHCb","-file=mepData_0.dat",0};

  Process::setDebug(true);  
  pg.add(p[0]=new Process("MEPInit_0",  command(),a1,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(8000);
  pg.add(p[1] =new Process("Moore_0",   command(),a3,out.c_str()));
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
//
//   Test of TAE event processing
//
extern "C" int qmtest_tae(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = argc>1 ? argv[1] : "/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer(partitionBuffers=True)");
  const char *a2[] =CLASS1("ReadTAE.opts");
  const char *aprod[]={"libGaudiOnline.so", "OnlineStart", "libGaudiOnline.so", "mep2mbm_producer",
		       "-n=TAEprod_0","-p=333","-s=500","-r=2","-c=5000","-file=taeData_0.dat",
		       "-input=Events_LHCb",0};

  Process::setDebug(true);  
  pg.add(p[0]=new Process("TAEBuffers_0",command(),a1,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(5000);
  pg.add(p[1] =new Process("TAE_0",   command(),a2,out.c_str()));
  pg.add(p[2] =new Process("TAE_1",   command(),a2,out.c_str()));
  pg.add(p[3] =new Process("TAE_2",   command(),a2,out.c_str()));
  pg.add(p[4] =new Process("TAE_3",   command(),a2,out.c_str()));
  pg.add(p[5] =new Process("TAE_4",   command(),a2,out.c_str()));
  cout << "Starting processes ..... " << endl;
  pg.start();
  ::lib_rtl_sleep(10000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("TAEProd_0",command(),aprod,out.c_str());
  prod->start();
  prod->wait(Process::WAIT_BLOCK);
  ::lib_rtl_sleep(10000);
  delete prod;
  cout << "Producer finished work.. " << endl;
  return collect_summary(sizeof(p)/sizeof(p[0]),p);
}
//
//   Test of data sending from MBM buffer to receiver connected to MBM buffer
//
extern "C" int qmtest_evacuate(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = argc>1 ? argv[1] : "/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a0[] ={"libGaudiOnline.so", "OnlineStart", "libOnlineKernel.so", "tan_nameserver","-a","-tcp","-d",0};
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Node1')");
  const char *a2[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Node2')");
  const char *a3[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Node3')");
  const char *a4[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Storage')");
  const char *a5[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runReceiver(buffer='Storage')");
  const char *a6[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runDataSender(target='Receiver_0',buffer='Node1',req='ALL')");
  const char *a7[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runDataSender(target='Receiver_0',buffer='Node2',req='ALL')");
  const char *a8[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runDataSender(target='Receiver_0',buffer='Node3',req='ALL')");

  const char *aprod1[]=CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(buffers=['Node1'],fname='mdfData_0.dat')");
  const char *aprod2[]=CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(buffers=['Node2'],fname='mdfData_0.dat')");
  const char *aprod3[]=CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(buffers=['Node3'],fname='mdfData_0.dat')");

  Process::setDebug(true);  
  pg.add(p[0]=new Process("TanServ_0",command(),a0,out.c_str()));
  pg.add(p[1]=new Process("Node1_0",  command(),a1,out.c_str()));
  pg.add(p[2]=new Process("Node2_0",  command(),a2,out.c_str()));
  pg.add(p[3]=new Process("Node3_0",  command(),a3,out.c_str()));
  pg.add(p[4]=new Process("Storage_0",command(),a4,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(5000);
  pg.add(p[5] =new Process("Receiver_0",command(),a5,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(15000);
  pg.add(p[6]=new Process("Sender1_0",command(),a6,out.c_str()));
  pg.add(p[7]=new Process("Sender2_0",command(),a7,out.c_str()));
  pg.add(p[8]=new Process("Sender3_0",command(),a8,out.c_str()));

  cout << "Starting processes ..... " << endl;
  pg.start();
  ::lib_rtl_sleep(5500);
  cout << "Starting producer ...... " << endl;
  pg.add(p[9] =new Process("Prod1_0",command(),aprod1,out.c_str()));
  pg.add(p[10]=new Process("Prod2_0",command(),aprod2,out.c_str()));
  pg.add(p[11]=new Process("Prod3_0",command(),aprod3,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(30000);
  cout << "Producer finished work.. " << endl;
  return collect_summary(sizeof(p)/sizeof(p[0]),p);
}
//
//   Test of disk writer attached to MBM buffer
//
extern "C" int qmtest_write_buffer(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = argc>1 ? argv[1] : "/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a0[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Events')");
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('Events',True,False,'./mdfOutput.dat')");
  const char *aprod[]=CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(buffers=['Events'],fname='mdfData_0.dat')");

  Process::setDebug(true);  
  pg.add(p[0]=new Process("Events_0", command(),a0,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(3000);
  pg.add(p[1]=new Process("DiskWr_0", command(),a1,out.c_str()));
  cout << "Starting processes ..... " << endl;
  pg.start();
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  pg.add(p[2] =new Process("Prod_0",command(),aprod,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(15000);
  cout << "Producer finished work.. " << endl;
  return collect_summary(sizeof(p)/sizeof(p[0]),p);
}
//
//   Test of event server application for event displays
//
extern "C" int qmtest_event_server(int argc, char** argv)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = argc>1 ? argv[1] : "/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a0[] ={"libGaudiOnline.so", "OnlineStart", "libOnlineKernel.so", "tan_nameserver","-a","-tcp","-d",0};
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Events')");
  const char *a2[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runEvtServer('Events',True,'ALL')");
  const char *a3[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runNetCons('EvtServ_0',1,0.0005)");
  const char *aprod[]=CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(buffers=['Events'],fname='mdfData_0.dat')");

  Process::setDebug(true);  
  pg.add(p[0]=new Process("TanServ_0",command(),a0,out.c_str()));
  pg.add(p[1]=new Process("Events_0", command(),a1,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(3000);
  pg.add(p[2]=new Process("EvtServ_0", command(),a2,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(2000);
  pg.add(p[3]=new Process("NetCons_0", command(),a3,""));
  pg.add(p[4]=new Process("NetCons_1", command(),a3,out.c_str()));
  cout << "Starting processes ..... " << endl;
  pg.start();
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  pg.add(p[5]=new Process("Prod_0",command(),aprod,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(15000);
  cout << "Producer finished work.. " << endl;
  return collect_summary(sizeof(p)/sizeof(p[0]),p,false);
}

extern "C" int pyhlt_test_run(int /* ac  */, char** /* av */)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = "";//"/dev/null";
  string host = RTL::nodeNameShort();
  Process* p[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg;
  const char *a0[] ={"libGaudiOnline.so", "OnlineStart", "libOnlineKernel.so", "tan_nameserver","-a","-tcp","-d",0};
  const char *a1[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer()");
  const char *a2[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runOutBuffer()");
  //const char *a3[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runEvtProd()");
  const char *a3[] =CLASS1("MEPConverter.opts");
  const char *a4[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runEvtHolder()");
  const char *a5[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()");
  const char *a6[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runReceiver()");
  const char *a7[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runEvtServer('OUT',True)");
  const char *a8[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runSender('Receiver')");
  const char *a9[] =CLASS1_PY("import GaudiOnlineTests;GaudiOnlineTests.runNetCons('EvtServ',1,0.0005)");
  const char *aprod[]={"libGaudiOnline.so", "OnlineStart", "libGaudiOnline.so", 
		       "mep2mbm_producer","-n=prod_0","-p=333","-s=500","-r=2","-c=1000",
		       "-input=Events","-file=mepData_0.dat",0};
  const char *asum[] ={"libOnlineKernel.so", "mbm_summary",0};

  Process::setDebug(true);  
  pg.add(p[0]=new Process("TanServ_0",  command(),a0,out.c_str()));
  pg.add(p[1]=new Process("MEPInit_0",  command(),a1,out.c_str()));
  pg.add(p[2]=new Process("MBMOut_0",   command(),a2,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(3500);
  pg.add(p[3] =new Process("EvtProd_0", command(),a3,out.c_str()));
  pg.add(p[4] =new Process("EvtHold_0", command(),a4,out.c_str()));
  pg.add(p[5] =new Process("Moore_0",   command(),a5,out.c_str()));
  pg.add(p[6] =new Process("Moore_1",   command(),a5,out.c_str()));
  pg.add(p[7] =new Process("Moore_2",   command(),a5,out.c_str()));
  pg.add(p[8] =new Process("Moore_3",   command(),a5,out.c_str()));
  pg.add(p[9] =new Process("Moore_4",   command(),a5,out.c_str()));
  pg.add(p[10]=new Process("Receiver",  command(),a6,out.c_str()));
  pg.add(p[11]=new Process("EvtServ",   command(),a7,out.c_str()));
  pg.start();
  ::lib_rtl_sleep(5500);
  pg.add(p[12] =new Process("Sender1",  command(),a8,out.c_str()));
  pg.add(p[13] =new Process("Sender2",  command(),a8,out.c_str()));
  pg.add(p[14] =new Process("EvtCons1", command(),a9,out.c_str()));
  pg.add(p[15] =new Process("EvtCons2", command(),a9,out.c_str()));

  //pg.add(p[16] =new Process("DiskWr_0", command(),a9,out.c_str()));
  pg.start();

  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("Prod_0",command(),aprod,out.c_str());
  prod->start();
  ::lib_rtl_sleep(4000);
  prod->wait(Process::WAIT_BLOCK);
  delete prod;
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(4000);
  Process* summary=new Process("Summary_0",command(),asum);
  summary->start();
  summary->wait(Process::WAIT_BLOCK);
  delete summary;
  cout << "Summary task finished work.. " << endl;

  ::lib_rtl_sleep(3000);
  for(int i=20; i>=0; --i) if ( p[i] ) p[i]->stop();
  ::lib_rtl_sleep(1000);
  for(int i=20; i>=0; --i) if ( p[i] ) p[i]->wait(Process::WAIT_BLOCK);
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);

  return 0;
}

extern "C" int qmtest_mepinj(int /* ac  */, char** /* av */)  {
  string main_opts = "-main="+gaudionlineOpts()+"/Main.opts";
  string out = "";//"/dev/null";
  string host = RTL::nodeNameShort();
  const char *injclass[] = CLASS1("MEPInjectorQMTest.opts");
  const char *readclass[] = CLASS1("ReaderSvcQMTest.opts");
  const char *meprxclass[] = CLASS1("MEPRxQMTest.opts");
  const char *mepinitclass[] = CLASS1("MEPInit.opts");
  std::vector<std::string> args;

  Process::setDebug(true);
  Process* mepinitproc=new Process("MEPInit_0",  command(),mepinitclass,out.c_str()); 
  Process* meprxproc=new Process("MEPRx_0", command(), meprxclass, out.c_str());
  Process* injproc=new Process("MEPInj_0",  command(),injclass,out.c_str());
  Process* readproc=new Process("Reader_0",  command(),readclass,out.c_str());
  cout << "Starting processes ..... " << endl;

  mepinitproc->start();
  ::lib_rtl_sleep(3500);
  meprxproc->start();
  injproc->start();
  ::lib_rtl_sleep(3500);
  readproc->start();
  ::lib_rtl_sleep(3500);

  cout << "Stopping processes ..... " << endl;
  ::lib_rtl_sleep(20000);
  collect_summary(0,0);

  readproc->stop();
  ::lib_rtl_sleep(1000);
  injproc->stop();
  ::lib_rtl_sleep(1000);
  meprxproc->stop();
  ::lib_rtl_sleep(1000); 
  mepinitproc->stop();
  ::lib_rtl_sleep(1000); 

  readproc->wait(Process::WAIT_BLOCK);
  injproc->wait(Process::WAIT_BLOCK); 
  meprxproc->wait(Process::WAIT_BLOCK);
  mepinitproc->wait(Process::WAIT_BLOCK);

  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);

  return 0;
}
