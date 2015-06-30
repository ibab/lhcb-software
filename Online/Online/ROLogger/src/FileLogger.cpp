// $Id: FileLogger.cpp,v 1.14 2010-04-15 16:04:59 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/FileLogger.cpp,v 1.14 2010-04-15 16:04:59 frankb Exp $

#include "ROLogger/FileLogger.h"

// Framework include files
#include <sstream>
#include <cstring>
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "UPI/UpiSensor.h"
#include "UPI/UpiDisplay.h"
#include "UPI/upidef.h"

#include "ROLogger/PartitionListener.h"
#include "ROLoggerDefs.h"
extern "C" {
#include "dis.h"
}
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#define vsnprintf _vsnprintf
#define stat64     __stat64
#define stat64Func _stat64
#define fopen64    fopen
#else
#include <unistd.h>
#define stat64Func stat64
#endif
static const char*  s_SevList[] = {"VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL"};

using namespace ROLogger;
using namespace std;

static void closeUPI() {
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

/// Standard constructor
FileLogger::FileLogger(int argc, char** argv) 
  : MessageLogger(3,true,false), m_useTimeForName(true), m_connected(false)
{
  bool not_use_time = false;
  string partition, title, facility;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("partition",1,partition);
  cli.getopt("facility",1,facility);
  cli.getopt("logdir",1,m_outdir);
  cli.getopt("severity",1,m_severity);
  not_use_time = cli.getopt("notime",1) != 0;
  if ( not_use_time ) m_useTimeForName = false;
  if ( partition.empty() ) {
    ::lib_rtl_output(LIB_RTL_INFO,"You have to supply a partition name to display its data.");
    ::lib_rtl_sleep(3000);
    closeUPI();
  }
  if ( m_outdir.empty() ) {
    ::lib_rtl_output(LIB_RTL_INFO,"You have to supply an output file name.");
    ::lib_rtl_sleep(3000);
    closeUPI();
  }
  if ( facility.empty() && partition == "LHCb1" ) facility = "lhcb1";
  else if ( facility.empty() && partition == "LHCb2" ) facility = "lhcb2";
  else if ( facility.empty() && partition == "LHCbA" ) facility = "lhcba";
  else if ( facility.empty() ) facility = "gaudi";

  m_id = UpiSensor::instance().newID();
  title = "Error logger:"+partition;
  ::strncpy(m_msgSeverity,s_SevList[m_severity-1],sizeof(m_msgSeverity));
  m_msgSeverity[sizeof(m_msgSeverity)-1]=0;
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,title.c_str(),RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM1,    "Current output file:","");
  ::upic_add_comment(CMD_COM2,    "","");
  ::upic_set_param(m_msgSeverity,1, "A7",m_msgSeverity, 0,0,s_SevList,sizeof(s_SevList)/sizeof(s_SevList[0]),1);
  ::upic_add_command(CMD_SEVERITY,"Log Severity level ^^^^^^^ ","");
  ::upic_add_command(CMD_SHOW,    "Show summary","");
  ::upic_add_command(CMD_START,   "Connect to messages","");
  ::upic_add_command(CMD_CONNECT, "Switch output file","");
  ::upic_add_command(CMD_CLOSE,   "Exit Logger","");
  ::upic_close_menu();
  m_colors  = false;
  m_display = true;
  m_output  = 0;
  m_outdir += "/";
  m_outdir += partition;
  openOutput();
  if ( 0 == m_output ) {
    ::lib_rtl_sleep(3000);
    closeUPI();
  }
  m_listener = new PartitionListener(this,partition,facility);
  UpiSensor::instance().add(this,m_id);
  m_quit = new UPI::ReallyClose(m_id,CMD_CLOSE);
  ::upic_set_cursor(m_id,CMD_SHOW,0);
  m_printRunNo = 1;
}

/// Standard destructor
FileLogger::~FileLogger()  {
  delete m_listener;
  delete m_quit;
}

/// Open new output file
FILE* FileLogger::openOutput() {
  struct stat64 st;
  char txt[1024], tmbuff[64];
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  ::strftime(tmbuff,sizeof(tmbuff),"%Y.%m.%d.log",now);
  if ( m_useTimeForName )
    ::snprintf(txt,sizeof(txt),"%s_%s",m_outdir.c_str(),tmbuff);
  else
    ::snprintf(txt,sizeof(txt),"%s.log",m_outdir.c_str());
  if ( m_output ) ::fclose(m_output);
  if ( ::stat64Func(txt,&st) == 0 ) {
    ::lib_rtl_output(LIB_RTL_INFO,"The output file: %s  exists already. Reconnecting.....",txt);
  }
  m_output = ::fopen64(txt,"a+");
  if ( 0 == m_output ) {
    ::lib_rtl_output(LIB_RTL_INFO,"Cannot open output file: %s [%s]",txt,RTL::errorString());
  }
  else {
    ::lib_rtl_output(LIB_RTL_INFO,"Opened output file:%s",txt);
    ::upic_replace_comment(m_id,CMD_COM2,txt,"");
    m_date = now->tm_wday;
  }
  return m_output;
}

/// Print single message retrieved from error logger
void FileLogger::printMessage(const char* msg, bool crlf) {
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  if ( m_date != now->tm_wday ) {
    m_output = openOutput();
  }
  MessageLogger::printMessage(msg,crlf);
  if ( m_connected ) ::upic_write_message(msg+12,"");  
}

/// Display callback handler
void FileLogger::handle(const Event& ev) {
  typedef vector<string> _SV;
  IocSensor& ioc = IocSensor::instance();
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case UpiEvent:
    m_msgSeverity[sizeof(m_msgSeverity)-1] = 0;
    if ( ::strchr(m_msgSeverity,' ')  ) *::strchr(m_msgSeverity,' ') = 0;
    switch(ev.command_id)  {
  case UPI::UpiDisplay::CMD_CLOSE:
    UpiSensor::instance().remove(this,m_id);
    ::upic_delete_menu(m_id);
    closeUPI();
    return;
  case CMD_SEVERITY:
    ioc.send(this,ev.command_id,new string(m_msgSeverity));
    return;
  case CMD_CLOSE:
    //UpiSensor::instance().remove(this,m_id);
    //::upic_delete_menu(m_id);
    //closeUPI();
    m_quit->invoke();
    return;
  case CMD_START:
    if ( m_connected )
      ::upic_replace_command(m_id,CMD_START,"Connect to messages","");
    else
      ::upic_replace_command(m_id,CMD_START,"Disconnect from messages","");
    m_connected = !m_connected;
    return;
  case CMD_CONNECT:
    openOutput();
    return;
  case CMD_SHOW:
    ::upic_write_message(getSummary().c_str(),"");
    return;
    }
    break;
  case IocEvent:
    switch(ev.type) {
  case CMD_UPDATE: {
    _SV v = m_farms;      
    string tmp;
    stringstream s;
    if ( !m_storage.empty() ) v.push_back(m_storage);
    if ( !m_monitoring.empty() ) v.push_back(m_monitoring);
    if ( !m_reconstruction.empty() ) v.push_back(m_reconstruction);
    tmp = "Farm content:";
    for(_SV::const_iterator i=v.begin();i!=v.end();++i) {
      const string& n = *i;
      s << n << ends;
      tmp += n;
      tmp += " ";
    }
    s << ends;
    ::upic_write_message(tmp.c_str(),"");
    tmp = s.str();
    removeAllServices();
    handleMessages(tmp.c_str(),tmp.c_str()+tmp.length());
    return;
  }
  case CMD_SEVERITY:
    setMessageSeverity(m_msgSeverity);
    delete data.str;
    return;
  case CMD_UPDATE_NODES:
    delete data.vec;
    return;
  case CMD_CONNECT_STORAGE:
    m_storage = *data.str;
    delete data.str;
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_CONNECT_MONITORING:
    m_monitoring = *data.str;
    delete data.str;
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_CONNECT_RECONSTRUCTION:
    m_reconstruction = *data.str;
    delete data.str;
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_DISCONNECT_STORAGE:
    m_storage = "";
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_DISCONNECT_MONITORING:
    m_monitoring = "";
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_DISCONNECT_RECONSTRUCTION:
    m_reconstruction = "";
    ioc.send(this,CMD_UPDATE,this);
    return;
  case CMD_UPDATE_RUNNUMBER: {
    char text[32];
    ::snprintf(text,sizeof(text),"%ld",ev.iocData<long>());
    newRunNumber(text);
    }
    return;
  case CMD_UPDATE_FARMS:
    m_farms = *data.vec;
    delete data.vec;
    ioc.send(this,CMD_UPDATE,this);
    return;
  default:
    break;
    }
    break;
  default:
    MessageLogger::handle(ev);
    return;
  }
  ::lib_rtl_output(LIB_RTL_INFO,"Received unknown input.....%d",ev.eventtype);
}


static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  ::upic_write_message(buffer,"");
  return len;
}

extern "C" int romon_file_logger(int argc, char** argv) {
  UpiSensor& s = UpiSensor::instance();
  ::lib_rtl_install_printer(do_output,0);
  FileLogger mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  s.run();
  return 1;
}
