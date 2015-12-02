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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrShowDisplay.cpp,v 1.14 2010-09-03 13:57:25 frankb Exp $

// Framework include files
#include "ROLogger/ErrShowDisplay.h"
#include "ROLogger/MessageLine.h"
#include "ROLogger/Filter.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "RTL/time.h"
#include "RTL/readdir.h"
#include "ROLoggerDefs.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include <cerrno>
#ifdef _WIN32
static const char* LOG_DIR="G:/online/dataflow/logs/";
#else
static const char* LOG_DIR="/group/online/dataflow/logs/";
#endif

using namespace ROLogger;
using namespace std;

static const char* s_SevList[]  = {"VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL"};
static const char* s_PartList[] = {
  "TDET",
  "LHCb",
  "VELOA","VELOC","VELO",
  "TT","IT","ST",
  "OTA","OTC","OT",
  "RICH1","RICH2","RICH",
  "PRS","ECAL","HCAL","CALO",
  "MUONA","MUONC","MUON"
};
static const char* s_timeFmt = "%d-%b-%Y %H:%M:%S";

static void clean_str(char* n,size_t len)  {
  n[len-1] = 0;
  if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
}

/// Standard constructor
ErrShowDisplay::ErrShowDisplay(CPP::Interactor* parent, CPP::Interactor* msg, const string& part) 
: m_parent(parent), m_msg(msg), m_numMsg(200)
{
  ::tzset();
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  m_id = UpiSensor::instance().newID();
  ::snprintf(m_name,sizeof(m_name),"%s",part.empty() ? "LHCb" : part.c_str());
  ::snprintf(m_node,sizeof(m_node),"%s","*");
  ::snprintf(m_process,sizeof(m_process),"%s","*");
  ::snprintf(m_component,sizeof(m_component),"%s","*");
  ::snprintf(m_message,sizeof(m_message),"%s","*");
  ::snprintf(m_severity,sizeof(m_severity),"%s",s_SevList[2]);
  ::snprintf(m_logDir,sizeof(m_logDir),"%s",LOG_DIR);
  ::snprintf(m_outFileName,sizeof(m_outFileName),"%s","Output.dat");
  ::strftime(m_endTime,sizeof(m_endTime),s_timeFmt,now);
  ::strftime(m_startTime,sizeof(m_startTime),s_timeFmt,now);

  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Error logger",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,this);
  ::upic_add_comment(CMD_COM0,           "Log file directory:","");
  ::upic_set_param(m_logDir,1,    "A55", m_logDir, 0,0,0,0,0);
  ::upic_add_command(CMD_DIR,            "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM1,           "-------------------  Search Zone  -------------------","");
  ::upic_set_param(m_name,1,      "A10", m_name, 0,0,s_PartList,sizeof(s_PartList)/sizeof(s_PartList[0]),0);
  ::upic_add_command(CMD_CONNECT,        "Select Partition: ^^^^^^^^^","");
  ::upic_set_param(m_startTime,1, "A21", m_startTime, 0,0,0,0,0);
  ::upic_add_command(CMD_START,          "Retrieve messages starting from ^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_endTime,  1, "A21", m_endTime, 0,0,0,0,0);
  ::upic_add_command(CMD_STOP,           "                          until ^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM2,           "","");
  ::upic_set_param(m_severity, 1,  "A7", m_severity, 0,0,s_SevList,sizeof(s_SevList)/sizeof(s_SevList[0]),1);
  ::upic_add_command(CMD_SEVERITY,       "Severity for messages ^^^^^^^","");
  // static const int   s_NumList[]  = {1,10,50,100,200,300,500,1000,5000,10000};
  // ::upic_set_param(&m_numMsg,  1,  "I6", m_numMsg,0,0,s_NumList,sizeof(s_NumList)/sizeof(s_NumList[0]),0);
  // ::upic_add_command(CMD_COM3,           "Show             ^^^^^^ Messages","");
  ::upic_set_param(m_node,     1, "A16", m_node,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_NODE,      "Node match:      ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_process,  1, "A16", m_process,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_PROCESS,   "Process match:   ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_component,1, "A16", m_component,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_COMPONENT, "Component match: ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_message,  1, "A39", m_message,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_MESSAGE,   "Message match: ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM5,           "-----------------------------------------------------","");
  ::upic_add_command(CMD_SHOW,           "Show list of log files","");
  ::upic_add_command(CMD_LOAD,           "Show result","");
  ::upic_add_command(CMD_SAVE,           "Write to File:","");
  ::upic_set_param(m_outFileName,1,"A55", m_outFileName, 0,0,0,0,0);
  ::upic_add_command(CMD_FILE,           "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM6,           "-----------------------------------------------------","");
  ::upic_add_command(CMD_CLOSE,          "Close","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,CMD_CONNECT,0);
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
ErrShowDisplay::~ErrShowDisplay()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// Retrieve all files corresponding to this request
void ErrShowDisplay::getFiles(Files& files)  {
  time_t begin = ::str2time(m_startTime,s_timeFmt);
  time_t end   = ::str2time(m_endTime,  s_timeFmt);
  files.clear();
  if ( (end - begin) >= 0 ) {
    string first, dirname = m_logDir;
    dirname += m_name;
    DIR* dir = opendir(dirname.c_str());
    if ( dir ) {
      char fmt[1024];
      struct dirent *dp;
      dirname += "/";
      ::snprintf(fmt,sizeof(fmt),"%s_%%Y.%%m.%%d-%%H.%%M.%%S.log",m_name);
      while ((dp=readdir(dir)) != 0) {
        time_t now = ::str2time(dp->d_name,fmt);
        if ( now >= begin && now <= end ) {
          files.push_back(dirname+dp->d_name);
          ::upic_write_message2("Adding file:%s",files.back().c_str());
        }
        else if ( now < begin ) {
          first = dirname+dp->d_name;
        }
      }
      if ( !first.empty() ) {
        ::upic_write_message2("Adding first file:%s",first.c_str());
        files.insert(files.begin(),first);
      }
      ::closedir(dir);
      return;
    }
    ::upic_write_message2("The directory %s does not exist:%s",dirname.c_str(),::strerror(errno));
    return;
  }
  ::upic_write_message2("The end-time:%s [%d] is before the start-time:%s [%d]",m_startTime,begin,m_endTime,end);
}

/// Process messages from single file
void ErrShowDisplay::processFile(const string& fname, FILE* output) {
  ifstream in(fname.c_str(),ios::in);
  char text[1024], tim[32], *p;
  if ( in.good() ) {
    int flag = Filter::MATCH_WILD+Filter::MATCH_NOCASE+Filter::MATCH_SELECT;
    IocSensor& ioc = IocSensor::instance();
    Filter f;
    time_t begin = ::str2time(m_startTime,s_timeFmt);
    time_t end   = ::str2time(m_endTime,  s_timeFmt);
    f.setNodeMatch      (m_node,     (unsigned char)flag);
    f.setUtgidMatch     (m_process,  (unsigned char)flag);
    f.setComponentMatch (m_component,(unsigned char)flag);
    f.setMessageMatch   (m_message,  (unsigned char)flag);
    ::snprintf(text,sizeof(text),"Logger output:%s from %s to %s",fname.c_str(),m_startTime,m_endTime);
    ioc.send(m_msg,CMD_START,new string(text));
    ::memcpy(tim,m_startTime+7,4);
    tim[4] = ' ';
    tim[17] = 0;
    int sev = MessageLine::severityLevel(m_severity);
    while(in.getline(text,sizeof(text)).good()) {
      p = ::strchr(text,'\n');
      if ( p ) *p = 0;
      ::strncpy(&tim[5],text,12);
      time_t now = ::str2time(tim,"%Y %b%d-%H%M%S");
      if ( now >= begin && now <= end ) {
        MessageLine line(text);
        if ( f.acceptMessage(line) ) {
          if ( line.msgSeverity() >= sev ) {
            if ( output == 0 ) {
              ioc.send(m_msg,CMD_SHOW,new string(text));
            }
            else {
              ::fprintf(output,"%s\n",text);
            }
            continue;
          }
        }
      }
      // ::snprintf(text,sizeof(text),"Rejected: %d %s %d %d",(int)now,tim,(int)begin,(int)end);
      // ioc.send(m_msg,CMD_SHOW,new string(text));
    }
    return;
  }
  ::upic_write_message2("Failed to open input file:%s",fname.c_str());
}

/// Show all files corresponding to this request
void ErrShowDisplay::showFiles() {
  Files files;
  getFiles(files);
  for(size_t i=0; i<files.size(); ++i) {
    ::upic_write_message2("[%d]  %s",(int)i,files[i].c_str());
  }
  ::upic_write_message2("Total: %d files to be processed.",(int)files.size());
}

/// Show all messages corresponding to this request
void ErrShowDisplay::showMessages(FILE* output) {
  try {
    Files files;
    getFiles(files);
    for(size_t i=0; i<files.size(); ++i) {
      ::upic_write_message2("Processing file [%d] .... %s",(int)i,files[i].c_str());
      processFile(files[i],output);
    }
  }
  catch(const exception& e) {
    ::upic_write_message2("Error during message processing:%s",e.what());
  }
  catch(...) {
    ::upic_write_message2("Unknown error during message processing.");
  }
}

/// Show all messages corresponding to this request
void ErrShowDisplay::saveMessages() {
  char txt[1024];
  FILE* outFile = ::fopen(m_outFileName,"w");
  if ( outFile ) {
    time_t tim = ::time(0);
    tm* now = ::localtime(&tim);
    txt[sizeof(txt)-1] = 0;
    showMessages(outFile);
    ::fclose(outFile);
    ::strftime(txt,sizeof(txt),"%b%d-%H%M%S [ALWAYS] Output file written:",now);
    ::strncat(txt,m_outFileName,sizeof(txt)-strlen(txt)-1);
    txt[sizeof(txt)-1]=0;
    IocSensor::instance().send(m_msg,CMD_SHOW,new string(txt));
    return;
  }
  ::snprintf(txt,sizeof(txt),"Cannot open file %s: %s",m_outFileName,::strerror(errno));
  IocSensor::instance().send(m_msg,CMD_SHOW,new string(txt));
  ::upic_write_message2(txt);
}

void ErrShowDisplay::handle(const CPP::Event& ev) {
  IocSensor& ioc = IocSensor::instance();
  ioc_data data(ev.data);
  switch(ev.eventtype) {
  case IocEvent:
    //::upic_write_message2("Got IOC command: %d %p",ev.type,ev.data);
    switch(ev.type) {
      case CMD_CLOSE:
        ioc.send(m_parent,CMD_DELETE,this);
        return;
      case CMD_SHOW:
        showFiles();
        return;
      case CMD_LOAD:
        showMessages(0);
        return;
      case CMD_SAVE:
        saveMessages();
        return;
      default:
        break;
    }
    break;
  case UpiEvent:
    switch(ev.command_id) {
  case CMD_BACKSPACE:
    ioc.send(this,CMD_CLOSE,this);
    return;
  case CMD_SHOW:
  case CMD_LOAD:
  case CMD_CLOSE:
  case CMD_SAVE:
    ioc.send(this,ev.command_id,this);
    return;
  case CMD_DIR:
    clean_str(m_logDir,sizeof(m_logDir));
    return;
  case CMD_FILE:
    clean_str(m_outFileName,sizeof(m_outFileName));
    return;
  case CMD_STOP:
    m_endTime[20]=0;
    return;
  case CMD_START:
    m_startTime[20]=0;
    return;
  case CMD_CONNECT:
    clean_str(m_name,sizeof(m_name));
    return;
  case CMD_WILD_NODE:
    clean_str(m_node,sizeof(m_node));
    return;
  case CMD_WILD_MESSAGE:
    clean_str(m_message,sizeof(m_message));
    return;
  case CMD_WILD_PROCESS:
    clean_str(m_process,sizeof(m_process));
    return;
  case CMD_WILD_COMPONENT:
    clean_str(m_component,sizeof(m_component));
    return;
  case CMD_SEVERITY:
    clean_str(m_severity,sizeof(m_severity));
    return;
    //ioc.send(m_msg,ev.command_id,new string(m_severity));
  default:
    break;
    }
    break;
  default:  // Fall through: Handle request by client
    break;
  }
}
