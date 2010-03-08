#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "RTL/ProcessGroup.h"
#include <iostream>

using namespace std;
using namespace RTL;

static std::string command() {
  string cmd = ::getenv("ONLINEKERNELROOT");
  cmd += "/";
  cmd += ::getenv("CMTCONFIG");
  cmd += "/test.exe";
  return cmd;
}

extern "C" int mep_test(int argc, char** /* argv */)  {
  Process* p[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ProcessGroup pg1, pg2;
  const char* output = argc>1 ? "" : "/dev/null";
  const char *a1[] ={"mbm_install","-s=8096","-e=5",  "-u=4", "-i=MEP",0};
  const char *a2[] ={"mbm_install","-s=4096","-e=30", "-u=10","-i=EVENT",0};
  const char *a3[] ={"mbm_install","-s=4096","-e=30", "-u=10","-i=RESULT",0};
  const char *a4[] ={"mep_reform_a","-name=Moore_0",  "-i=EVENT","-o=RESULT",0};
  const char *a5[] ={"mep_reform_a","-name=Moore_1",  "-i=EVENT","-o=RESULT",0};
  const char *a6[] ={"mep_reform_a","-name=Moore_2",  "-i=EVENT","-o=RESULT",0};
  const char *a7[] ={"mep_reform_a","-name=Moore_3",  "-i=EVENT","-o=RESULT",0};
  const char *a8[] ={"mep_reform_a","-name=Moore_4",  "-i=EVENT","-o=RESULT",0};
  const char *a9[] ={"mep_cons_a",  "-name=Diskwr_0", "-i=RESULT",0};
  const char *a10[]={"mep_cons_a",  "-name=Monit_0",  "-i=RESULT",0};
  const char *a11[]={"mep_cons_a",  "-name=Monit_1",  "-i=RESULT",0};
  const char *a12[]={"mep_holder_a","-name=MepHold_0","-i=MEP",0};
  const char *a13[]={"mep_prod_a",  "-name=Mep_0",    "-count=1000",0};
  const char *a14[]={"mbm_summary",0};

  ::lib_rtl_signal_log(0);
  Process::setDebug(true);  
  pg1.add(p[0]=new Process("Install_0", command(),a1, output));
  pg1.add(p[1]=new Process("Install_1", command(),a2, output));
  pg1.add(p[2]=new Process("Install_2", command(),a3, output));
  pg1.start();
  ::lib_rtl_sleep(3500);
  pg2.add(p[3] =new Process("Moore_0",  command(),a4, output));
  pg2.add(p[4] =new Process("Moore_1",  command(),a5, output));
  pg2.add(p[5] =new Process("Moore_2",  command(),a6, output));
  pg2.add(p[6] =new Process("Moore_3",  command(),a7, output));
  pg2.add(p[7] =new Process("Moore_4",  command(),a8, output));
  pg2.add(p[8] =new Process("DiskWr_0", command(),a9, output));
  pg2.add(p[9] =new Process("Monit_0",  command(),a10,output));
  pg2.add(p[10]=new Process("Monit_1",  command(),a11,output));
  pg2.add(p[11]=new Process("MepHold_0",command(),a12,output));
  pg2.start();

  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("Mep_0",command(),a13,output);
  prod->start();
  ::lib_rtl_sleep(8000);
  prod->wait(Process::WAIT_BLOCK);
  delete prod;
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(4000);
  Process* summary=new Process("Summary_0",command(),a14);
  summary->start();
  summary->wait(Process::WAIT_BLOCK);
  delete summary;
  cout << "Summary task finished work.. " << endl;

  ::lib_rtl_sleep(3000);
  for(int i=11; i>=3; --i) p[i]->stop();
  ::lib_rtl_sleep(1000);
  for(int i=2; i>=0; --i) p[i]->stop();
  ::lib_rtl_sleep(1000);
  for(int i=11; i>=0; --i) p[i]->wait(Process::WAIT_BLOCK);
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);

  return 0;
}
