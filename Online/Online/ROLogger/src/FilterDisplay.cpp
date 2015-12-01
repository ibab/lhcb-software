//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/FilterDisplay.cpp,v 1.8 2010-09-03 13:57:25 frankb Exp $

// Framework include files
#include "ROLogger/FilterDisplay.h"
#include "ROLogger/MessageLine.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
#include <cstring>
#include <sstream>
#include <fstream>
extern "C" {
#include "dic.h"
}

using namespace ROLogger;
using namespace std;

const char* s_filterType[] = {
  "Exact match",
    "Case blind",
    "Wild card match",
    "Wild card case blind"
    //0123456789012345678901234567890123456789
};

const char* s_matchType[] = {
  "Ignore filter where field matches",
    "Apply filter where field matches",
    "If no match, ignore the rest",
    //0123456789012345678901234567890123456789
};

/// Standard constructor
FilterDisplay::FilterDisplay(Interactor* parent, Interactor* msg, Interactor* history) 
: m_parent(parent), m_msg(msg), m_history(history)
{ //                      123456789
  static const char* hist[] = {"[History]"};
  static const char* mess[] = {"[Messages]"};
  m_id = UpiSensor::instance().newID();
  ::snprintf(m_file,sizeof(m_file),"Logfilters.txt");
  ::upic_open_window();
  ::upic_open_detached_menu(m_id,0,0,"Filter Editor",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,this);
  ::upic_add_comment(CMD_COM1,   "---------------------------------------------------","");
  ::upic_add_comment(CMD_COM2,   "---------------------------------------------------","");
  ::upic_add_command(CMD_RESET,  "Reset menu to defaults","");
  ::upic_add_command(CMD_ADD,    "Add filter","");
  ::upic_add_command(CMD_CLEAR,  "Clear filters","");
  ::upic_set_param(m_buff, 1, "A9", hist[0], 0,0,hist,1,1);
  ::upic_set_param(m_buff, 2, "A10", mess[0],  0,0,mess,1,1);
  ::upic_add_command(CMD_APPLY,  "Apply to ^^^^^^^^^  /  ^^^^^^^^^^","");
  ::upic_add_command(CMD_SAVE,   "Save  filters","");
  ::upic_add_command(CMD_LOAD,   "Load  filters","");
  ::upic_set_param(m_file, 1, "A45", m_file,  0,0,0,0,0);
  ::upic_add_command(CMD_CONNECT,"->File:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_add_comment(CMD_COM5,   "---------------------------------------------------","");
  ::upic_add_command(CMD_CLOSE,  "Close","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id, CMD_CLOSE, 0);
  UpiSensor::instance().add(this,m_id);
  addCommands();
}

/// Standard destructor
FilterDisplay::~FilterDisplay()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("FilterDisplay> Close window.","");
}

/// Add all commands to build filters
void FilterDisplay::addCommands() {
  int cmd_id = CMD_COMLAST;
  ::snprintf(m_node[0],sizeof(m_node[0]),"*");
  ::snprintf(m_node[1],sizeof(m_node[1]),"%s",s_filterType[3]);
  ::snprintf(m_node[2],sizeof(m_node[2]),"%s",s_matchType[0]);
  ::upic_set_param(m_node[0], 1, "A37", m_node,  0,0,0,0,0);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, " Node      match:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_node[1], 1, "A37", s_filterType[3],0,0,s_filterType,sizeof(s_filterType)/sizeof(s_filterType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           type  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_node[2], 1, "A37", s_matchType[0],0,0,s_matchType,sizeof(s_matchType)/sizeof(s_matchType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           logic ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");

  ::memset(m_utgid,0,sizeof(m_utgid));
  ::snprintf(m_utgid[0],sizeof(m_utgid[0]),"%s","*");
  ::snprintf(m_utgid[1],sizeof(m_utgid[1]),"%s",s_filterType[3]);
  ::snprintf(m_utgid[2],sizeof(m_utgid[2]),"%s",s_matchType[0]);
  ::upic_set_param(m_utgid[0], 1, "A37", m_utgid,  0,0,0,0,0);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, " UTGID     match:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_utgid[1], 1, "A37", s_filterType[3],0,0,s_filterType,sizeof(s_filterType)/sizeof(s_filterType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           type  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_utgid[2], 1, "A37", s_matchType[0],0,0,s_matchType,sizeof(s_matchType)/sizeof(s_matchType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           logic ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");

  ::memset(m_comp,0,sizeof(m_comp));
  ::snprintf(m_comp[0],sizeof(m_comp[0]),"*");
  ::snprintf(m_comp[1],sizeof(m_comp[1]),s_filterType[3]);
  ::snprintf(m_comp[2],sizeof(m_comp[2]),s_matchType[0]);
  ::upic_set_param(m_comp[0], 1, "A37", m_comp,  0,0,0,0,0);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, " COMPONENT match:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_comp[1], 1, "A37", s_filterType[3],0,0,s_filterType,sizeof(s_filterType)/sizeof(s_filterType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           type  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_comp[2], 1, "A37", s_matchType[0],0,0,s_matchType,sizeof(s_matchType)/sizeof(s_matchType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           logic ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");

  ::memset(m_mess,0,sizeof(m_mess));
  ::snprintf(m_mess[0],sizeof(m_mess[0]),"*");
  ::snprintf(m_mess[1],sizeof(m_mess[1]),"%s",s_filterType[3]);
  ::snprintf(m_mess[2],sizeof(m_mess[2]),"%s",s_matchType[0]);
  ::upic_set_param(m_mess[0], 1, "A37", m_mess,  0,0,0,0,0);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, " MESSAGE   match:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_mess[1], 1, "A37", s_filterType[3],0,0,s_filterType,sizeof(s_filterType)/sizeof(s_filterType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           type  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_mess[2], 1, "A37", s_matchType[0],0,0,s_matchType,sizeof(s_matchType)/sizeof(s_matchType[0]),1);
  ::upic_insert_command(m_id,CMD_COM2,++cmd_id, "           logic ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
}

/// Remove all commands to build filters
void FilterDisplay::removeCommands() {
  for(int i=0; i<12; ++i) {
    ::upic_delete_command(m_id,i+1+CMD_COMLAST);
  }
}

unsigned char _matchType(const char* filter, const char* match)  {
  unsigned char r = 0;
  if      ( ::strcmp(s_filterType[0],filter)==0 ) r += Filter::MATCH_EXACT;
  else if ( ::strcmp(s_filterType[1],filter)==0 ) r += Filter::MATCH_NOCASE;
  else if ( ::strcmp(s_filterType[2],filter)==0 ) r += Filter::MATCH_WILD;
  else if ( ::strcmp(s_filterType[3],filter)==0 ) r += Filter::MATCH_WILD+Filter::MATCH_NOCASE;
  if      ( ::strcmp(s_matchType[0], match) ==0 ) r += Filter::MATCH_SELECT;
  else if ( ::strcmp(s_matchType[1], match) ==0 ) r += Filter::MATCH_INVERSE;
  else if ( ::strcmp(s_matchType[2], match) ==0 ) r += Filter::MATCH_REQUIRE;
  return r;
}

const char* s_clean(char* str, size_t len) {
  for(char* s=str+len-1;s>str; --s) 
    if ( *s==' '||*s==0 ) 
      *s=0;
    else
      break;
  // upic_write_message2("'%s'",str);
  return str;
}

/// Build filter object and add it to display
void FilterDisplay::addFilter()   {
  Filter f;
  for(int j=0; j<3; ++j) {
    s_clean(m_node[j],sizeof(m_node[j]));
    s_clean(m_utgid[j],sizeof(m_utgid[j]));
    s_clean(m_comp[j],sizeof(m_comp[j]));
    s_clean(m_mess[j],sizeof(m_mess[j]));
  }
  f.setNodeMatch      (m_node[0],  _matchType(m_node[1],  m_node[2]));
  f.setUtgidMatch     (m_utgid[0], _matchType(m_utgid[1], m_utgid[2]));
  f.setComponentMatch (m_comp[0],  _matchType(m_comp[1],  m_comp[2]));
  f.setMessageMatch   (m_mess[0],  _matchType(m_mess[1],  m_mess[2]));
  addFilter(f);
}

/// Add filter object
void FilterDisplay::addFilter(const Filter& f)   {
  char text[256];
  int cmd = CMD_1000, cmd0;
  for (Filters::const_iterator i=m_filters.begin(); i!=m_filters.end(); ++i )
    if ( (*i).first > cmd ) cmd = (*i).first;
  cmd += 5;
  m_filters.push_back(make_pair(cmd,f));
  cmd0 = cmd;
  ::snprintf(text,sizeof(text),"Filter match: Node: [%d]  %s",f.match()[Filter::NODE],f.node.c_str());
  ::upic_insert_comment(m_id,CMD_COM1,++cmd,text,"");
  ::snprintf(text,sizeof(text),"             UTGID: [%d]  %s",f.match()[Filter::UTGID],f.utgid.c_str());
  ::upic_insert_comment(m_id,CMD_COM1,++cmd,text,"");
  ::snprintf(text,sizeof(text),"         Component: [%d]  %s",f.match()[Filter::COMPONENT],f.component.c_str());
  ::upic_insert_comment(m_id,CMD_COM1,++cmd,text,"");
  ::snprintf(text,sizeof(text),"           Message: [%d]  %s",f.match()[Filter::MESSAGE],f.message.c_str());
  ::upic_insert_comment(m_id,CMD_COM1,++cmd,text,"");
  ::upic_insert_command(m_id,CMD_COM1, cmd0, "------------- Remove Filter above -------------","");
}

/// Build filter object and send it to parent
void FilterDisplay::removeFilter(int cmd)   {
  for (Filters::iterator j=m_filters.begin(); j!=m_filters.end(); ++j ) {
    if ( (*j).first == cmd ) {
      for(int i=0; i<5; ++i )
        ::upic_delete_command(m_id,i+cmd);
      m_filters.erase(j);
      return;
    }
  }
}

void FilterDisplay::removeFilters() {
  for (Filters::iterator j=m_filters.begin(); j!=m_filters.end(); ++j ) {
    int cmd = (*j).first;
    for(int i=0; i<5; ++i )
      ::upic_delete_command(m_id,i+cmd);
  }
  m_filters.clear();
}

/// Save filers to file
void FilterDisplay::saveFilters(const string& fname) {
  if ( !fname.empty() ) {
    ofstream out(fname.c_str());
    if ( out.good() ) {
      for (Filters::iterator j=m_filters.begin(); j!=m_filters.end(); ++j )
        (*j).second.write(out);
      ::upic_write_message2("Saved filters to file:%s",fname.c_str());
      return;
    }
    ::upic_write_message2("Failed to open file:%s",fname.c_str());
  }
  ::upic_write_message("Invalid file name.....No action done.","");
}

/// Load filers to file
void FilterDisplay::loadFilters(const string& fname) {
  if ( !fname.empty() ) {
    ifstream in(fname.c_str());
    if ( in.good() ) {
      Filter filter;
      while(filter.read(in)>0) addFilter(filter);
      ::upic_write_message2("Loaded filters from file:%s",fname.c_str());
      return;
    }
    ::upic_write_message2("Failed to open file:%s",fname.c_str());
  }
  ::upic_write_message("Invalid file name.....No action done.","");
}

void FilterDisplay::handle(const Event& ev) { 
  IocSensor& ioc = IocSensor::instance();
  switch(ev.eventtype) {
  case IocEvent:
    //::upic_write_message2("Got IOC command: %d %p",ev.type,ev.data);
    switch(ev.type) {
  case CMD_CLOSE:
  case CMD_BACKSPACE:
    ioc.send(m_parent,CMD_DELETE,this);
    return;
  case CMD_RESET:
    removeCommands();
    addCommands();
    break;
  case CMD_ADD:
    addFilter();
    ::upic_set_cursor(m_id,CMD_RESET,0);
    return;
  case CMD_CLEAR:
    removeFilters();
    ::upic_set_cursor(m_id,CMD_RESET,0);
    return;
  case CMD_SAVE:
    s_clean(m_file,sizeof(m_file));
    saveFilters(m_file);
    return;
  case CMD_LOAD:
    s_clean(m_file,sizeof(m_file));
    loadFilters(m_file);
    return;
  case CMD_APPLY:
    return;
  default:
    break;
    }
    break;
  case UpiEvent:
    switch(ev.command_id) {
  case CMD_ADD:
  case CMD_CLEAR:
  case CMD_RESET:
  case CMD_CLOSE:
  case CMD_BACKSPACE:
  case CMD_SAVE:
  case CMD_LOAD:
    ioc.send(this,ev.command_id,this);
    return;
  case CMD_APPLY: {
    stringstream out;
    for (Filters::iterator j=m_filters.begin(); j!=m_filters.end(); ++j )
      (*j).second.write(out);
    ioc.send(ev.param_id==1 ? m_history : m_msg, ev.command_id, new string(out.str()));
    return;
                  }
  default:
    if ( ev.command_id >= CMD_1000 && ev.command_id < CMD_COM0 )
      removeFilter(ev.command_id);
    break;
    }
    break;
  default:  // Fall through: Handle request by client
    break;
  }
}
