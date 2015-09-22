#define MBM_IMPLEMENTATION
#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "RTL/ProcessGroup.h"
#include "MBM/bmstruct.h"
#include "MBM/bmdef.h"
#include <iostream>
#include <stdexcept>

#include "RTL/DllAccess.h"
using OnlineBase::currentCommand;

using namespace std;
using namespace RTL;
extern "C" int mbm_summary(int argc, char** argv);


extern "C" int mbm_install_test_bm(int argc , char** argv) {
  try  {
    std::vector<char*> opts;
    std::vector<ServerBMID> bmids;
    char type[64] = "mbm_install";
    ServerBMID bmid;
    for(size_t i=0; i<size_t(argc); ++i)  {
      char c0 = argv[i][0];
      char c1 = ::toupper(argv[i][1]);
      opts.push_back(argv[i]);
      if ( (c0 == '-' || c0 == '/') && (c1 == 'C' || c1 == 'A') ) {
	bmid = mbm_install_server(opts.size(), &opts[0]);
	if ( !bmid )  {
	  ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
	  throw std::runtime_error("Unable to install MBM buffers...");
	}
	bmids.push_back(bmid);
	opts.clear();
	opts.push_back(type);
      }
    }
    if ( !opts.empty() )  {
      //opts.push_front("dummy");
      RTL::CLI cli(opts.size(),&opts[0],0);
      int sleep = 10000;
      bool summary = cli.getopt("summary",3);
      cli.getopt("sleep",3,sleep);
      ::lib_rtl_sleep(sleep*1000);
      if ( summary )   {
	return mbm_summary(opts.size(),&opts[0]);
      }
    }
  }
  catch (std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"++mbm_install++ MBM initialization failed: %s\n",e.what());
  }
  return MBM_ERROR;
}

extern "C" int mbm_simple_test(int argc, char** /* argv */)  {
  ProcessGroup pg;
  Process* p[10] = {0,0,0,0,0,0,0,0,0,0};
  const char* output = argc>1 ? "" : "/dev/null";
  const char *a1[]={"mbm_install_test_bm","-s=8096","-e=128","-u=12","-i=0","-f","-c","-sleep=60",0};
  const char *a2[]={"mbm_cons","-name=cons_s_0",0};
  const char *a4[]={"mbm_cons","-name=cons_o_0","-one",0};
  const char *a8[]={"mbm_prod","-name=prod_0","-m=50000","-s=3500",0};
  const char *a9[]={"mbm_summary",0};

  std::string cmd(currentCommand());

  ::mbm_qmtest_check_start();
  Process::setDebug(true);
  pg.add(p[0]=new Process("Installer",cmd,a1,output));
  pg.start();
  ::lib_rtl_sleep(1500);
  pg.add(p[1]=new Process("Cons_s_0",cmd,a2,output));
  pg.add(p[2]=new Process("Cons_o_0",cmd,a4,output));
  pg.start();
  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  p[3]=new Process("Prod_0",cmd,a8,output);
  p[3]->start();
  p[3]->wait(Process::WAIT_BLOCK);
  delete p[3];
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(10000);
  p[3]=new Process("Summary_0",cmd,a9);
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

extern "C" int mbm_full_test(int argc, char** /* argv */)  {
  ProcessGroup pg;
  Process* p[10] = {0,0,0,0,0,0,0,0,0,0};
  const char* output = argc>1 ? "" : "/dev/null";
  const char *a1[]={"mbm_install_test_bm","-s=8096","-e=128","-u=12","-i=0","-c","-sleep=60",0};
  const char *a2[]={"mbm_cons","-name=cons_s_0",0};
  const char *a3[]={"mbm_cons","-name=cons_s_1",0};
  const char *a4[]={"mbm_cons","-name=cons_o_0","-one",0};
  const char *a5[]={"mbm_cons","-name=cons_o_1","-one",0};
  const char *a6[]={"mbm_cons","-name=cons_u_0","-s=1",0};
  const char *a7[]={"mbm_cons","-name=cons_u_1","-s=1",0};
  const char *a8[]={"mbm_prod","-name=prod_0","-m=5000","-s=3500",0};
  const char *a9[]={"mbm_summary",0};

  std::string cmd(currentCommand());

  ::mbm_qmtest_check_start();
  Process::setDebug(true);
  pg.add(p[0]=new Process("Install", cmd,a1,output));
  pg.start();
  ::lib_rtl_sleep(1500);
  pg.add(p[1]=new Process("Cons_s_0",cmd,a2,output));
  pg.add(p[2]=new Process("Cons_s_1",cmd,a3,output));
  pg.add(p[3]=new Process("Cons_o_0",cmd,a4,output));
  pg.add(p[4]=new Process("Cons_o_1",cmd,a5,output));
  pg.add(p[5]=new Process("Cons_u_0",cmd,a6,output));
  pg.add(p[6]=new Process("Cons_u_1",cmd,a7,output));
  pg.start();

  cout << "Starting processes ..... " << endl;
  ::lib_rtl_sleep(3000);
  cout << "Starting producer ...... " << endl;
  Process* prod=new Process("Prod_0",cmd,a8,output);
  prod->start();
  ::lib_rtl_sleep(4000);
  prod->wait(Process::WAIT_BLOCK);
  delete prod;
  cout << "Producer finished work.. " << endl;
  ::lib_rtl_sleep(15000);
  Process* summary=new Process("Summary_0",cmd,a9);
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

int mbm_qmtest_check_no_active_buffers(int, char**)   {
  lib_rtl_gbl_t handle = 0;
  int res = ::mbmsrv_map_global_buffer_info(&handle,true);
  if ( lib_rtl_is_success(res) ) {
    BUFFERS* buffers = (BUFFERS*)handle->address;
    time_t start = ::time(0), now;
    int bad = 0;
  rescan:
    bad = 0;
    now = ::time(0);
    for (int i = 0; i < buffers->p_bmax; ++i)  {
      if ( buffers->buffers[i].used == 1 )  {
	if ( now - start > 400 ) {
	  _mbm_printf("MBM: Buffer %s is still in use!\n",buffers->buffers[i].name);
	  ++bad;
	}
      }
    }
    if ( (bad > 0) && (now - start > 400) ) {
      _mbm_printf("MBM: Buffers are still not released after 400 seconds!\n");
      ::mbmsrv_unmap_global_buffer_info(handle,false);
      ::exit(0);
    }
    else if ( bad > 0 )  {
      ::lib_rtl_sleep(1000);
      goto rescan;
    }
    res = 1;
  }
  ::mbmsrv_unmap_global_buffer_info(handle,false);
  return res;
}

extern "C" int mbm_qmtest_check_start() {
  string cmd = currentCommand() + " mbm_qmtest_check_no_active_buffers";
  int ret = system(cmd.c_str());
  if ( ret != 1 ) {
    _mbm_printf("MBM: Failed to wait for other test to finish....\n");
    return 0;
  }
  return ret;
}

