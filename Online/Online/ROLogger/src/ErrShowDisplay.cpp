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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrShowDisplay.cpp,v 1.4 2008-05-27 18:54:04 frankb Exp $

// Framework include files
#include "ROLogger/ErrShowDisplay.h"
#include "ROLogger/MessageLine.h"
#include "ROLogger/Filter.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
#include <sstream>
#include <fstream>
#include <cerrno>
#include <dirent.h>

using namespace ROLogger;


static const int   s_NumList[]  = {1,10,50,100,200,300,500,1000,5000,10000};
static const char* s_SevList[]  = {"VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL"};
static const char* s_PartList[] = {"TDET",
				   "LHCb",
				   "VELOA","VELOC","VELO",
				   "TT","IT","ST",
				   "OTA","OTC","OT",
				   "RICH1","RICH2","RICH",
				   "PRS","ECAL","HCAL","CALO",
				   "MUONA","MUONC","MUON"
};
static const char* s_timeFmt = "%d-%m-%Y %H:%M:%S";

static void clean_str(char* n,size_t len)  {
  n[len-1] = 0;
  if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
}

/// Standard constructor
ErrShowDisplay::ErrShowDisplay(Interactor* parent, Interactor* msg, const std::string& part) 
  : m_parent(parent), m_msg(msg), m_numMsg(200)
{
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  m_id = UpiSensor::instance().newID();
  ::strcpy(m_name,part.empty() ? "LHCb" : part.c_str());
  ::strcpy(m_node,"*");
  ::strcpy(m_process,"*");
  ::strcpy(m_component,"*");
  ::strcpy(m_message,"*");
  ::strcpy(m_severity,s_SevList[2]);
  ::strcpy(m_logDir,"/group/online/dataflow/logs/");
  ::strcpy(m_outFileName,"Output.dat");
  ::strftime(m_endTime,sizeof(m_endTime),s_timeFmt,now);
  ::strftime(m_startTime,sizeof(m_startTime),s_timeFmt,now);

  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Error logger",RTL::processName().c_str(),RTL::nodeName().c_str());

  ::upic_add_comment(CMD_COM0,           "Log file directory:","");
  ::upic_set_param(m_logDir,1,    "A53", m_logDir, 0,0,0,0,0);
  ::upic_add_command(CMD_DIR,            "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM1,           "------------------  Search Zone  ------------------","");
  ::upic_set_param(m_name,1,      "A10", m_name, 0,0,s_PartList,sizeof(s_PartList)/sizeof(s_PartList[0]),0);
  ::upic_add_command(CMD_CONNECT,        "Select Partition: ^^^^^^^^^","");
  ::upic_set_param(m_startTime,1, "A20", m_startTime, 0,0,0,0,0);
  ::upic_add_command(CMD_START,          "Retrieve messages starting from ^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_endTime,  1, "A20", m_endTime, 0,0,0,0,0);
  ::upic_add_command(CMD_STOP,           "                          until ^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM2,           "","");
  ::upic_set_param(m_severity, 1,  "A7", m_severity, 0,0,s_SevList,sizeof(s_SevList)/sizeof(s_SevList[0]),1);
  ::upic_add_command(CMD_SEVERITY,       "Severity for messages ^^^^^^^","");
  // ::upic_set_param(&m_numMsg,  1,  "I6", m_numMsg,0,0,s_NumList,sizeof(s_NumList)/sizeof(s_NumList[0]),0);
  // ::upic_add_command(CMD_COM3,           "Show             ^^^^^^ Messages","");
  ::upic_set_param(m_node,     1, "A16", m_node,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_NODE,      "Node match:      ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_process,  1, "A16", m_process,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_PROCESS,   "Process match:   ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_component,1, "A16", m_component,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_COMPONENT, "Component match: ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_message,  1, "A37", m_message,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_MESSAGE,   "Message match: ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM5,           "---------------------------------------------------","");
  ::upic_add_command(CMD_SHOW,           "Show list of log files","");
  ::upic_add_command(CMD_LOAD,           "Show result","");
  ::upic_add_command(CMD_SAVE,           "Write to File:","");
  ::upic_set_param(m_outFileName,1,"A53", m_outFileName, 0,0,0,0,0);
  ::upic_add_command(CMD_FILE,           "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM6,           "---------------------------------------------------","");
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
  struct tm start, stop, curr;
  time_t begin, end, now;
  files.clear();
  ::strptime(m_endTime,   s_timeFmt, &stop);
  ::strptime(m_startTime, s_timeFmt, &start);
  begin = ::mktime(&start);
  end   = ::mktime(&stop);
  if ( (end - begin) > 0 ) {
    std::string first, dirname = m_logDir;
    dirname += m_name;
    DIR* dir = opendir(dirname.c_str());
    if ( dir ) {
      char fmt[1024];
      struct dirent *dp;
      dirname += "/";
      ::sprintf(fmt,"%s_%%Y.%%m.%%d-%%H.%%M.%%S.log",m_name);
      while ((dp=readdir(dir)) != 0) {
	const char* fname = dp->d_name;
	if ( 0 != ::strptime(fname,fmt,&curr) ) {
	  now = ::mktime(&curr);
	  if ( now >= begin && now <= end ) {
	    files.push_back(dirname+fname);
	    ::upic_write_message2("Adding file:%s %d %d %d",files.back().c_str(),(int)begin,(int)now,(int)end);
	  }
	  else if ( now < begin ) {
	    first = dirname+fname;
	  }
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
void ErrShowDisplay::processFile(const std::string& fname, FILE* output) {
  std::ifstream in(fname.c_str(),std::ios::in);
  char text[1024], tim[32], *p;
  if ( in.good() ) {
    int flag = Filter::MATCH_WILD+Filter::MATCH_NOCASE+Filter::MATCH_SELECT;
    IocSensor& ioc = IocSensor::instance();
    Filter f;
    struct tm start, stop, curr;
    time_t begin, end;
    ::strptime(m_endTime,   s_timeFmt, &stop);
    ::strptime(m_startTime, s_timeFmt, &start);
    begin = ::mktime(&start);
    end   = ::mktime(&stop);
    f.setNodeMatch      (m_node,     flag);
    f.setUtgidMatch     (m_process,  flag);
    f.setComponentMatch (m_component,flag);
    f.setMessageMatch   (m_message,  flag);
    ::sprintf(text,"Logger output:%s",fname.c_str());
    ioc.send(m_msg,CMD_START,new std::string(text));
    ::strftime(tim,12,"%Y-",&start);
    tim[17] = 0;
    begin = ::mktime(gmtime(&begin));
    end = ::mktime(gmtime(&end));
    int sev = MessageLine::severityLevel(m_severity);
    while(in.getline(text,sizeof(text)).good()) {
      p = ::strchr(text,'\n');
      if ( p ) *p = 0;
      ::strncpy(&tim[5],text,12);
      ::strptime(tim,"%Y-%b%d-%H%M%S",&curr);
      time_t now = ::mktime(&curr);
      if ( now >= begin && now <= end ) {
	MessageLine line(text);
	if ( f.acceptMessage(line) ) {
	  if ( line.msgSeverity() >= sev ) {
	    if ( output == 0 ) {
	      ioc.send(m_msg,CMD_SHOW,new std::string(text));
	    }
	    else {
	      ::fprintf(output,"%s\n",text);
	    }
	    continue;
	  }
	}
      }
      // ::sprintf(text,"Rejected: %d %s %d %d",(int)now,tim,(int)begin,(int)end);
      // ioc.send(m_msg,CMD_SHOW,new std::string(text));
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
  catch(const std::exception& e) {
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
    showMessages(outFile);
    ::fclose(outFile);
    ::strftime(txt,sizeof(txt),"%b%d-%H%M%S [ALWAYS] Output file written:",now);
    ::strcat(txt,m_outFileName);
    IocSensor::instance().send(m_msg,CMD_SHOW,new std::string(txt));
    return;
  }
  ::sprintf(txt,"Cannot open file %s: %s",m_outFileName,::strerror(errno));
  IocSensor::instance().send(m_msg,CMD_SHOW,new std::string(txt));
  ::upic_write_message2(txt);
}

void ErrShowDisplay::handle(const Event& ev) {
  typedef std::vector<std::string> _SV;
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

      //ioc.send(m_msg,ev.command_id,new std::string(m_severity));
    default:
      break;
    }
    break;
  default:  // Fall through: Handle request by client
    break;
  }
}
