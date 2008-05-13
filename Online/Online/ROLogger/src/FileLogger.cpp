// $Id: FileLogger.cpp,v 1.2 2008-05-13 08:26:10 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/FileLogger.cpp,v 1.2 2008-05-13 08:26:10 frankb Exp $

// Framework include files
#include <sstream>
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "UPI/UpiSensor.h"
#include "UPI/upidef.h"

#include "ROLogger/PartitionListener.h"
#include "ROLogger/FileLogger.h"
#include "ROLoggerDefs.h"
extern "C" {
#include "dis.h"
}
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

using namespace ROLogger;

static void closeUPI() {
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

/// Standard constructor
FileLogger::FileLogger(int argc, char** argv) : MessageLogger(argc,argv)
{
  std::string partition, title;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("partition",1,partition);
  cli.getopt("logdir",1,m_outdir);
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
  m_id = UpiSensor::instance().newID();
  title = "Error logger:"+partition;
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,title.c_str(),RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM1,   "-------------------------","");
  ::upic_add_comment(CMD_COM2,   "Output file:","");
  ::upic_add_comment(CMD_COM3,   "-------------------------","");
  ::upic_add_command(CMD_CONNECT,"Switch output file","");
  ::upic_add_command(CMD_CLOSE,  "Exit Logger","");
  ::upic_add_comment(CMD_COM4,   "-------------------------","");
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
  m_listener = new PartitionListener(this,partition);
  UpiSensor::instance().add(this,m_id);
}

/// Open new output file
FILE* FileLogger::openOutput() {
  char txt[1024], tmbuff[64];
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  ::strftime(tmbuff,sizeof(tmbuff),"%Y.%m.%d-%H.%M.%S.log",now);
  sprintf(txt,"%s_%s",m_outdir.c_str(),tmbuff);
  if ( m_output ) ::fclose(m_output);
  m_output = ::fopen(txt,"w");
  if ( 0 == m_output ) {
    ::lib_rtl_output(LIB_RTL_INFO,"Cannot open output file: %s [%s]",txt,RTL::errorString());
  }
  else {
    ::lib_rtl_output(LIB_RTL_INFO,"Opened output file:%s",txt);
    std::string t = "Output file:";
    t += txt;
    ::upic_replace_comment(m_id,CMD_COM2,t.c_str(),"");
  }
  return m_output;
}

/// Standard destructor
FileLogger::~FileLogger()  {
  delete m_listener;
}

/// Display callback handler
void FileLogger::handle(const Event& ev) {
  typedef std::vector<std::string> _SV;
  ioc_data data(ev.data);
  const _SV* v = data.vec;
  switch(ev.eventtype) {
  case UpiEvent:
    switch(ev.command_id)  {
  case CMD_CLOSE:
    UpiSensor::instance().remove(this,m_id);
    ::upic_delete_menu(m_id);
    closeUPI();
    return;
  case CMD_CONNECT:
    openOutput();
    return;
    }
    break;
  case IocEvent:
    switch(ev.type) {
  case CMD_UPDATE_NODES:
    printMessage(">>>>>>>>>>> [INFo] Handle: CMD_UPDATE_NODES",true);
    delete data.vec;
    return;
  case CMD_UPDATE_FARMS: {
    printMessage(">>>>>>>>>>> [INFo] Handle: CMD_UPDATE_FARMS",true);
    std::string tmp;
    std::stringstream s;
    for(_SV::const_iterator i=v->begin();i!=v->end();++i) {
      std::string n = *i;
      if ( n == "STORE" ) n = "STORECTL01";
      s << "/" << n << "/gaudi/log" << std::ends;
    }
    s << std::ends;
    tmp = s.str();
    removeAllServices();
    handleMessages(tmp.c_str(),tmp.c_str()+tmp.length());
    delete data.vec;
                         }
                         return;
  default:
    break;
    }
    break;
  default:
    MessageLogger::handle(ev);
    return;
  }
  ::lib_rtl_output(LIB_RTL_INFO,"Received unknown input.....");
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
