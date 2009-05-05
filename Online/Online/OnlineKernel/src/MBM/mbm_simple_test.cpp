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

extern "C" int mbm_simple_test(int /* ac  */, char** /* av */)  {
  ProcessGroup pg;
  Process* p[10] = {0,0,0,0,0,0,0,0,0,0};
  const char *a1[]={"mbm_install","-s=8096","-e=64","-u=64","-i=0","-f",0};
  const char *a2[]={"mbm_cons","-name=cons_s_0",0};
  const char *a4[]={"mbm_cons","-name=cons_o_0","-one",0};
  const char *a8[]={"mbm_prod","-name=prod_0","-m=50000","-s=3500",0};
  const char *a9[]={"mbm_summary",0};

  Process::setDebug(true);
  pg.add(p[0]=new Process("Installer",command(),a1,"/dev/null"));
  pg.start();
  ::lib_rtl_sleep(1500);
  pg.add(p[1]=new Process("Cons_s_0",command(),a2,"/dev/null"));
  pg.add(p[2]=new Process("Cons_o_0",command(),a4,"/dev/null"));
  pg.start();
  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  p[3]=new Process("Prod_0",command(),a8,"/dev/null");
  p[3]->start();
  p[3]->wait(Process::WAIT_BLOCK);
  delete p[3];
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(4000);
  p[3]=new Process("Summary_0",command(),a9);
  p[3]->start();
  p[3]->wait(Process::WAIT_BLOCK);
  delete p[3];
  cout << "Summary task finished work.. " << endl;
  ::lib_rtl_sleep(2000);
  for(int i=2; i>0; --i) p[i]->stop();
  ::lib_rtl_sleep(1000);
  p[0]->stop();
  for(int i=2; i>=0; --i) p[i]->wait();
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);

  return 0;
}

extern "C" int mbm_full_test(int /* ac  */, char** /* av */)  {
  ProcessGroup pg;
  Process* p[10] = {0,0,0,0,0,0,0,0,0,0};
  const char *a1[]={"mbm_install","-s=8096","-e=64","-u=64","-i=0",0};
  const char *a2[]={"mbm_cons","-name=cons_s_0",0};
  const char *a3[]={"mbm_cons","-name=cons_s_1",0};
  const char *a4[]={"mbm_cons","-name=cons_o_0","-one",0};
  const char *a5[]={"mbm_cons","-name=cons_o_1","-one",0};
  const char *a6[]={"mbm_cons","-name=cons_u_0","-s=1",0};
  const char *a7[]={"mbm_cons","-name=cons_u_1","-s=1",0};
  const char *a8[]={"mbm_prod","-name=prod_0","-m=10000","-s=3500",0};
  const char *a9[]={"mbm_summary",0};

  Process::setDebug(true);
  pg.add(p[0]=new Process("Install", command(),a1,"/dev/null"));
  pg.start();
  ::lib_rtl_sleep(1500);
  pg.add(p[1]=new Process("Cons_s_0",command(),a2,"/dev/null"));
  pg.add(p[2]=new Process("Cons_s_1",command(),a3,"/dev/null"));
  pg.add(p[3]=new Process("Cons_o_0",command(),a4,"/dev/null"));
  pg.add(p[4]=new Process("Cons_o_1",command(),a5,"/dev/null"));
  pg.add(p[5]=new Process("Cons_u_0",command(),a6,"/dev/null"));
  pg.add(p[6]=new Process("Cons_u_1",command(),a7,"/dev/null"));
  pg.start();

  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("Prod_0",command(),a8,"/dev/null");
  prod->start();
  ::lib_rtl_sleep(4000);
  prod->wait(Process::WAIT_BLOCK);
  delete prod;
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(4000);
  Process* summary=new Process("Summary_0",command(),a9);
  summary->start();
  summary->wait(Process::WAIT_BLOCK);
  delete summary;
  cout << "Summary task finished work.. " << endl;

  ::lib_rtl_sleep(3000);
  for(int i=6; i>0; --i) p[i]->stop();
  ::lib_rtl_sleep(1000);
  p[0]->stop();
  for(int i=6; i>=0; --i) p[i]->wait();
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);

  return 0;
}
