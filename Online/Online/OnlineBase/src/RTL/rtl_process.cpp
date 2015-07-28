// $Id:$
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include "rtl_internal.h"
#include <iostream>
#include <cerrno>

using namespace std;
using namespace RTL;

extern "C" int rtl_start_process2(int, char**) {
#ifndef _WIN32
  int pID = ::fork();
  if (pID == 0)  {       // child
    cout << "PG:" << getpgid(0) << " PID:" << pID << "  getPID:" << getpid() << " 'Child Process'" << endl;
    ::execl("/bin/sleep","SLEEPER","10",0);
    cout << "PG:" << getpgid(0) << " PID:" << pID << "  getPID:" << getpid() << " 'Child Process'" << endl;
  }
  else if (pID < 0)   {  // failed to fork
    cerr << "Failed to fork" << endl;
    exit(1);
    // Throw exception 
  }
  else   {              // parent
    cout << "PG:" << getpgid(0) << " PID:" << pID << "  getPID:" << getpid() << " 'Parent Process'" << endl;
    ::lib_rtl_sleep(1000);
    system("ps -ef | grep frank");
    ::lib_rtl_sleep(1000);
    int  killReturn = ::killpg(getpid(), SIGTERM);  // Kill child process group
    if( killReturn == ESRCH)  {   // pid does not exist
      cout << "Group does not exist!" << endl;
    }
    else if( killReturn == EPERM)   {       // No permission to send signal
      cout << "No permission to send signal!" << endl;
    }
    else  {
      cout << "Signal sent. All Ok!" << endl;
    }
    ::lib_rtl_sleep(1000);
    ::system("ps -ef | grep frank");
    ::lib_rtl_sleep(3000);
  }
#endif
  return 0;
}

