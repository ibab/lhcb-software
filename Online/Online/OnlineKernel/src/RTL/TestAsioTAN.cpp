// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
// C/C++ include files
#include <iostream>

// Framework include files
#include "RTL/rtl.h"
#include "RTL/ProcessGroup.h"
#include "RTL/DllAccess.h"

using OnlineBase::currentCommand;
using namespace std;
using namespace RTL;

extern "C" int boost_asio_tan_qmtest(int argc, char** /* argv */)  {
  ProcessGroup pg;
  Process* p[10] = {0,0,0,0,0,0,0,0,0,0};
  const char* output = argc>1 ? "" : "/dev/null";
  const char *a1[]={"boost_asio_tan_server","-address=0.0.0.0","-threads=10","-delete",0};
  const char *a2[]={"boost_asio_tan_alloc_test","-task=AAAAAA","-maxtask=25","-inquire","-alias",0};
  const char *a3[]={"boost_asio_tan_alloc_test","-task=BBBBBB","-maxtask=25","-inquire","-alias",0};
  const char *a4[]={"tanmon","-simple",0};
  std::string cmd(currentCommand());
  Process::setDebug(true);
  pg.add(p[0]=new Process("TANServer",cmd,a1,output));
  pg.start();
  ::lib_rtl_sleep(3000);
  pg.add(p[1]=new Process("AAAAAA",cmd,a2,output));
  pg.start();
  ::lib_rtl_sleep(5000);
  pg.add(p[2]=new Process("BBBBBB",cmd,a3,output));
  pg.start();
  ::lib_rtl_sleep(5000);
  pg.add(p[3]=new Process("MONITOR",cmd,a4,output));
  pg.start();
  cout << "Wait for processes ..... " << endl;
  ::lib_rtl_sleep(10000);
  p[0]->stop();
  for(int i=3; i>=0; --i) p[i]->wait();
  cout << "All processes finished work.. " << endl;
  ::lib_rtl_sleep(1000);
  return 0;
}
