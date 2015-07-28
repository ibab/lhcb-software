// $Id: AlarmDisplay.cpp,v 1.6 2010-09-03 14:47:45 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/AlarmDisplay.cpp,v 1.6 2010-09-03 14:47:45 frankb Exp $

#include "ROMon/AlarmDisplay.h"
#include "ROMon/FarmMonitor.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <cstring>
#include <sstream>
#include <memory>
#include <set>

using namespace ROMon;
using namespace SCR;
using namespace std;

static AlarmDisplay* s_fd = 0;
// Max. 15 seconds without update allowed
static int UPDATE_TIME_MAX = 15;

static void help() {
  cout << "  romon_farm -option [-option]" << endl
       << "       -all                         Show all subfarms." << endl
       << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
       << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << endl
       << endl;
}

MessageWindow::MessageWindow(AlarmDisplay* parent, const string& title,int height, int width, int level) 
  : InternalDisplay(parent,title), m_level(level)
{
  int color = NORMAL;
  AlarmSummary summary;
  if ( level&ERR_LVL_MONITOR ) color = NORMAL;
  if ( level&ERR_LVL_WARNING ) color = CYAN|BOLD;
  if ( level&ERR_LVL_ERROR   ) color = RED|BOLD;
  if ( level&ERR_LVL_ALARM   ) color = RED|INVERSE|BOLD;
  ::scrc_begin_pasteboard_update(m_pasteboard);
  ::scrc_create_display(&m_display,height, width, NORMAL, ON, title.c_str());
  ::scrc_set_border(m_display,m_title.c_str(),color|BOLD);
  ::scrc_put_chars(m_display," Node         Time         Message",BOLD|INVERSE, 1,1,1);
  ::scrc_end_pasteboard_update (m_pasteboard);
  update(&summary);
}

/// Update display content
void MessageWindow::update(const void* data) {
  char text[1024];
  const AlarmSummary& a = *(AlarmSummary*)data;
  AlarmSummary::const_iterator j = a.begin();
  int background = NORMAL;
  if ( m_level&ERR_LVL_MONITOR ) background = NORMAL;
  if ( m_level&ERR_LVL_WARNING ) background = BLUE;
  if ( m_level&ERR_LVL_ERROR   ) background = RED;
  if ( m_level&ERR_LVL_ALARM   ) background = RED|INVERSE|BOLD;

  for(int line = 1; line<m_display->rows; ) {
    if ( j != a.end() ) {
      const AlarmSum& s = (*j).second;
      if ( ((*j).first&m_level) != 0 ) {
        int color = NORMAL;
        if ( s.nodes.size() < 3 ) {
          for(size_t k=0; k<s.nodes.size(); ++k) {
            const Alarm* al = s.nodes[k];
            color = al->color();
            ::snprintf(text,sizeof(text),
                       "%-13s %-12s %s %s",al->node.c_str(),al->time().c_str(),
                       al->message(),al->description.c_str());
            ::scrc_put_chars(m_display,text,color,++line,1,1);
          }
        }
        else  {
          const Alarm* al = s.nodes[0];
          color = al->color();
          ::snprintf(text,sizeof(text),"%-4d %-8s %-12s %s",int(s.nodes.size()),"alarms",
                     al->time().c_str(),al->message());
          set<string> opts;
          for(size_t k=0; k<s.nodes.size(); ++k) {
            const string& item = s.nodes[k]->node;
            if ( opts.find(item) == opts.end() ) {
              opts.insert(item);
              ::strcat(text," ");
              ::strcat(text,item.c_str());
              if ( k>5 ) {
                ::strcat(text," ..... ");
                break;
              }
            }
          }
          ::scrc_put_chars(m_display,text,color,++line,1,1);
        }
      }
      ++j;
    }
    else { // Fill up with empty strings
      ::scrc_put_chars(m_display,"",background,++line,1,1);
    }
  }
}

/// Standard constructor
AlarmDisplay::AlarmDisplay(int argc, char** argv)
  : InternalDisplay(0,""),
    m_posCursor(0), m_subPosCursor(0), m_anchorX(10), m_anchorY(20)
{
  string svc;
  time_t tm = time(0);
  char txt[128], buff[256];
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   2, m_name = "ALL");
  s_fd = this;
  m_title = "  Alarm monitor  ";
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width, BOLD, ON, m_title.c_str());
  show(2,2);
  ::strftime(txt,sizeof(txt),"%b %d %Y    %H:%M:%S",::localtime(&tm));
  ::snprintf(buff,sizeof(buff),"  Partition: %s     %s",m_name.c_str(),txt);
  ::scrc_put_chars(m_display,buff,BOLD,1,2,1);
  ::scrc_end_pasteboard_update(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  svc = "/"+m_name+"/FarmMonitor/Alarms";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  IocSensor::instance().send(this,CMD_CONNECT,0UL);
}

/// Standard destructor
AlarmDisplay::~AlarmDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  ::scrc_begin_pasteboard_update(m_pasteboard);
  if ( m_alarmDisplay ) {
    MouseSensor::instance().remove(m_alarmDisplay->display());
    delete m_alarmDisplay;
    m_alarmDisplay = 0;
  }
  if ( m_warnDisplay ) {
    MouseSensor::instance().remove(m_warnDisplay->display());
    delete m_warnDisplay;
    m_warnDisplay = 0;
  }
  if ( m_monDisplay ) {
    MouseSensor::instance().remove(m_monDisplay->display());
    delete m_monDisplay;
    m_monDisplay = 0;
  }
  close();
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_delete_pasteboard(m_pasteboard);
  m_pasteboard = 0;
  ::scrc_resetANSI();
  ::printf("Farm display deleted and resources freed......\n");
}

/// DIM command service callback
void AlarmDisplay::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    AlarmDisplay* disp = *(AlarmDisplay**)tag;
    IocSensor::instance().send(disp,CMD_ADD,new string(((char*)address)));
  }
}

/// Show context dependent help window
int AlarmDisplay::showHelpWindow() {
  return WT_SUCCESS;
}

/// Set cursor to position
void AlarmDisplay::set_cursor(InternalDisplay* /* updater */) {

}

/// Keyboard rearm action
int AlarmDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int AlarmDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  return s_fd->handleKeyboard(key);
}
 
/// Handle new incoming alarm
void AlarmDisplay::handleAlarm(const std::string& alarm) {
  const char* text = alarm.c_str();
  unsigned int val;
  AlarmSummary summary;
  AlarmSummary::iterator k;
  DisplayAlarms::iterator i;
  bool removeAll = false;
  if ( alarm.length()>5 && text[5] == 'A' ) {      // CLEARALL
    removeAll = true;
  }
  else if ( (i=m_alarms.find(val=Alarm::hash32(text+17))) == m_alarms.end() ) {
    if ( text[0] == 'D' ) {         // DECLARE
      Alarm* alm = new Alarm(text);
      m_alarms[val] = alm;
    }
  }
  else  if ( text[0] == 'C' ) {     // CLEAR
    delete (*i).second;
    m_alarms.erase(i);
  }
  std::auto_ptr<Alarm> rem_alm;
  if ( removeAll ) {
    rem_alm = std::auto_ptr<Alarm>(new Alarm(ERR_REMOVEDALL,time(0),"",""));
    summary[ERR_REMOVEDALL].nodes.push_back(rem_alm.get());
  }
  else {
    for(i=m_alarms.begin(); i != m_alarms.end(); ++i)
      summary[(*i).second->code].nodes.push_back((*i).second);
  }
  time_t tm = time(0);
  char txt[64], buff[132];
  ::strftime(txt,sizeof(txt),"%b %d %Y    %H:%M:%S",::localtime(&tm));
  ::snprintf(buff,sizeof(buff),"  Partition: %s     %s",m_name.c_str(),txt);
  ::scrc_begin_pasteboard_update (m_pasteboard);
  ::scrc_put_chars(m_display,buff,BOLD,1,2,1);
  m_alarmDisplay->update(&summary);
  m_warnDisplay->update(&summary);
  m_monDisplay->update(&summary);
  ::scrc_end_pasteboard_update (m_pasteboard);
  IocSensor::instance().send(this,CMD_UPDATE,0UL);
}

/// Handle keyboard interrupts
int AlarmDisplay::handleKeyboard(int key)    {
  RTL::Lock lock(screenLock());
  try {
    switch (key)    {
    case CTRL_W:
      ::scrc_repaint_screen (m_pasteboard);
      break;
    case 'E':
    case CTRL_E:
      delete this;
    ::exit(0);
    return WT_SUCCESS;
    case 'h':
    case 'H':
    case CTRL_H:
      return showHelpWindow();
    case RETURN_KEY:
    case ENTER:
      //IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
      return WT_SUCCESS;
    case MOVE_UP:
      --m_subPosCursor;
      break;
    case MOVE_DOWN:
      ++m_subPosCursor;
      break;
    case PAGE_UP:
      break;
    case PAGE_DOWN:
      break;
    default:
      return WT_SUCCESS;
    }
  }
  catch(...) {
  }
  set_cursor(this);
  return WT_SUCCESS;
}

/// Interactor overload: Display callback handler
void AlarmDisplay::handle(const Event& ev) {
  int h;
  //const MouseEvent* m = 0;
  RTL::Lock lock(screenLock());

  switch(ev.eventtype) {
  case ScrMouseEvent:
    /* m = */ ev.get<MouseEvent>();
    return;
  case TimeEvent:
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_UPDATE:
      ::scrc_fflush(m_pasteboard);
      //::scrc_repaint_screen(m_pasteboard);
      break;
    case CMD_SETCURSOR:
      set_cursor(ev.iocPtr<InternalDisplay>());
      break;
    case CMD_HANDLE_KEY:
      handleKeyboard(int(ev.iocData<long>()));
      break;
    case CMD_ADD:
      handleAlarm(*ev.iocPtr<string>());
      delete ev.iocPtr<string>();
      return;
    case CMD_CONNECT:
      h = (m_height-11)/2;
      ::scrc_begin_pasteboard_update (m_pasteboard);
      m_alarmDisplay = new MessageWindow(this,"  ALARMS  ",h,m_width,ERR_LVL_ALARM);
      m_alarmDisplay->show(4,2);
      m_warnDisplay  = new MessageWindow(this,"  ERRORS/WARNINGS  ",h,m_width,ERR_LVL_ERROR|ERR_LVL_WARNING);
      m_warnDisplay->show(6+h,2);
      m_monDisplay  = new MessageWindow(this,"  MONITOR  ",7,m_width,ERR_LVL_MONITOR);
      m_monDisplay->show(7+h+h,2);
      ::scrc_end_pasteboard_update (m_pasteboard);
      IocSensor::instance().send(this,CMD_UPDATE,0UL);
      return;
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Standard constructor
AlarmPrinter::AlarmPrinter(int argc, char** argv) : Interactor()    {
  char txt[128];
  time_t tm = time(0);
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition", 2, m_name = "ALL");
  cli.getopt("update",    2, UPDATE_TIME_MAX);
  ::strftime(txt,sizeof(txt),"%b %d %Y    %H:%M:%S",::localtime(&tm));
  cout << "Alarm monitor for partition:" << m_name << "  " << txt << endl;
  string svc = "/"+m_name+"/FarmMonitor/Alarms";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

/// Standard destructor
AlarmPrinter::~AlarmPrinter()  {  
}

/// DIM command service callback
void AlarmPrinter::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    AlarmPrinter* disp = *(AlarmPrinter**)tag;
    IocSensor::instance().send(disp,CMD_ADD,new string(((char*)address)));
  }
}

/// Handle new incoming alarm
void AlarmPrinter::handleAlarm(const std::string& alarm) {
  const char* text = alarm.c_str();
  unsigned int val = Alarm::hash32(text+17);
  DisplayAlarms::iterator i=m_alarms.find(val);
  if ( i == m_alarms.end() ) {
    if ( text[0] == 'D' ) {
      Alarm* alm = new Alarm(alarm);
      m_alarms[val] = alm;
      cout << "NEW alarm:" << hex << val << "  " << *alm << endl;
    }
  }
  else if ( text[0] == 'C' ) {
    cout << "Clear alarm:" << *(*i).second << endl;
    delete (*i).second;
    m_alarms.erase(i);
  }
}

/// Interactor overload: Display callback handler
void AlarmPrinter::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_ADD:
      handleAlarm(*ev.iocPtr<string>());
      delete ev.iocPtr<string>();
      return;
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
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
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  //::upic_write_message(buffer,"");
  return len;
}

extern "C" int romon_alarmprint(int argc,char** argv) {
  AlarmPrinter* disp = new AlarmPrinter(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}

extern "C" int romon_alarmdisp(int argc,char** argv) {
  AlarmDisplay* disp = new AlarmDisplay(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
