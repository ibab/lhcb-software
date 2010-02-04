// $Id: AlarmLogger.cpp,v 1.1 2010-02-04 10:32:51 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ErrorSrv/src/AlarmLogger.cpp,v 1.1 2010-02-04 10:32:51 frankb Exp $
// Framework include files
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "RTL/rtl.h"
#include "RTL/Base64.h"
#include "RTL/strdef.h"
#include "RTL/graphics.h"
#include "CPP/IocSensor.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"
#include "ErrorSrv/ErrorSrv.h"

extern "C" {
#include "dic.h"
#include "dis.h"
}
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

namespace ROLogger {
  class AlarmLogger : public Interactor {
    int m_id;
    bool m_debug;
  public:
    static void help_fun();
    AlarmLogger(int argc, char** argv);
    virtual ~AlarmLogger();
    static void messageInfoHandler(void* tag, void* address, int* size);
    void handle(const Event& ev);
    void handleMessage(const char*, int size);
  };
}


using namespace ROLogger;
using namespace graphics;
using namespace LHCb;
using namespace std;

void AlarmLogger::help_fun() {
  using namespace std;
  cout 
    <<"Usage: romon_logger -option [-option]                                 "<<endl
    <<"        -buffer=<number>      History buffer size (Number of messages)"<<endl
    <<"        -service=<string>     Service name to accept commands.        "<<endl
    <<"        -colors               Use colors to indicate message severity "<<endl
    <<"        -display              Show recorded messages immeditely       "<<endl
    <<"                              rather than only recording to history   "<<endl
    <<endl;
}

/// Standard constructor
AlarmLogger::AlarmLogger(int argc, char** argv)    {
  string name;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("service",1,name);
  m_debug = cli.getopt("debug",1) != 0;
  ::lib_rtl_output(LIB_RTL_INFO,"Service name:         %s", name.c_str());
  m_id = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,messageInfoHandler,(long)this,0,0);
}

/// Standard destructor
AlarmLogger::~AlarmLogger()  {
  ::dic_release_service(m_id);
}

/// Dim Info callback overload to process messages
void AlarmLogger::messageInfoHandler(void* tag, void* address, int* size)  {
  if ( address && size && *size>0 ) {
    AlarmLogger* e = *(AlarmLogger**)tag;
    e->handleMessage((const char*)address,*size);
  }
}

const char* flagString(int f) {
  static std::string s;
  if (      f == f_Info           ) return "f_Info";
  else if ( f == f_Warning        ) return "f_Warning";
  else if ( f == f_Error          ) return "f_Error";
  else if ( f == f_Alarm          ) return "f_Alarm";
  else if ( f == f_Init           ) return "f_Init";
  if ( (f&b_Log) == b_Log         ) s += "Log|";
  if ( (f&b_Display) == b_Display ) s += "Disp|";
  if ( (f&b_Info) == b_Info       ) s += "Info|";
  if ( (f&b_Warning) == b_Warning ) s += "Warning|";
  if ( (f&b_Error) == b_Error     ) s += "Error|";
  if ( (f&b_Alarm) == b_Alarm     ) s += "Alarm|";
  if ( (f&b_Clear) == b_Clear     ) s += "Clear|";
  if ( (f&b_Init) == b_Init       ) s += "Init|";
  if ( (f&b_seen) == b_seen       ) s += "seen|";
  s[s.length()-1] = 0;
  return s.c_str();
}


void AlarmLogger::handleMessage(const char* msg, int size) {
  char buffer[9128];
  if ( msg ) {
    char* ptr = ::strstr((char*)msg,": DRec:");
    if ( m_debug ) cout << msg;
    if ( ptr )   {
      size_t reclen = size-7+(ptr-msg), len = 0;
      DiskRecord* r = (DiskRecord*)buffer;
      int res = ::lib_rtl_decode64(ptr+7,reclen,buffer,sizeof(buffer),&len);
      if ( res == 1 ) {
	std::string t = r->partition;
	t += "/";
	t += r->node;
	t += "::";
	t += r->process;
	cout << setw(32) << left << flagString(r->flag) << " " 
	     << setw(12) << left << t << " " << r->message << " [" << r->info() << "]" << endl;
      }
      else {
	cout << "Failed to decode base64 buffer : decode64=" << res << endl;
      }
    }
    else {
      //cout << "Unknown disk record:" << msg;
    }
  }
}

/// Display callback handler
void AlarmLogger::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {      
    default:
      break;
    }
    break;
  default:
    break;
  }
}

static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  return ::printf("%s\n",buffer);
}

extern "C" int romon_logger(int argc, char** argv) {
  ::lib_rtl_install_printer(do_output,0);
  AlarmLogger mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  IocSensor::instance().run();
  return 1;
}
