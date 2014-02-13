// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: Runable to run Moore standalone on a single node.
//  Author     : M.Frank
//====================================================================
// $Header: $
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <ctime>
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "UPI/upi_menu.h"
#include "UPI/UpiSensor.h"
#include "UpiMooreMonitor.h"

using namespace std;
using namespace LHCb;

UpiMooreMonitor::UpiMooreMonitor(int mode, Interactor* parent, Interactor* feed) 
  : m_id(UpiSensor::newID()), m_lines(0), m_mode(mode), m_update(false),
    m_parent(parent), m_feed(0), m_ioc(&IocSensor::instance())
{
  char text1[256], text2[256];
  const char *title = "????";
  if ( m_mode == 0 )  {
    title = "Event Rates";
    ::snprintf(text1,sizeof(text1),"%-16s %15s %-19s %s",
	       "---------------","--Time [msec]--",
	       "----Input [Hz]----","---Output [Hz]----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s %-19s %s",
	       "Process name","Total","Delta","Instant Cumulative","Instant Cumulative");
  }
  else if ( m_mode == 1 )  {
    title = "Event Counters";
    ::snprintf(text1,sizeof(text1),"%-16s %15s %-25s %s",
	       "---------------","--Time [msec]--",
	       "-----Events consumed-----","-----Events produced-----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s   %-25s %s",
	       "Process name","Total","Delta",
	       "Start Previous      Now","Start Previous      Now");
  }
  else if ( m_mode == 2 )  {
    title = "Event Information: Rates and Counters";
    ::snprintf(text1,sizeof(text1),"%-16s %15s  %-25s  %-18s %-26s  %s",
	       "---------------","--Time [msec]--",
	       "-----Events consumed-----","----Input [Hz]----",
	       "-----Events produced-----","---Output [Hz]----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s    %-25s%-18s   %-25s %s",
	       "Process name","Total","Delta",
	       "Start Previous      Now","Instant Cumulative",
	       "Start Previous      Now","Instant Cumulative");
  }
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,title,"Statistics",title);
  ::upic_add_comment(COM_HEADER1,text1,"");
  ::upic_add_comment(COM_HEADER2,text2,"");
  ::upic_add_comment(COM_STATISTICS_1+m_lines++,"--","");
  ::upic_add_comment(COM_STATISTICS_1+m_lines++,"--","");
  ::upic_add_command(MooreTest::CMD_CLOSE,"Close>","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,MooreTest::CMD_CLOSE,0);
  UpiSensor::instance().add(this,m_id);
  setFeed(feed);
}

/// Standard destructor
UpiMooreMonitor::~UpiMooreMonitor()   {
  if ( m_id > 0 ) {
    UpiSensor::instance().remove(this,m_id);
    ::upic_delete_menu(m_id);
  }
  if ( m_feed )  {
    m_ioc->send(m_feed,MooreTest::CMD_REMOVE_MOORE_CLIENT,this);
  }
  if ( m_parent )  {
    m_ioc->send(m_parent,MooreTest::CMD_REMOVE_MOORE_CLIENT,this);
  }
}

void UpiMooreMonitor::setFeed(Interactor* feed)   {
  m_feed = feed;
  if ( m_feed )  {
    m_ioc->send(m_feed,MooreTest::CMD_ADD_MOORE_CLIENT,this);
  }
}

void UpiMooreMonitor::handle (const Event& e)   {
  switch ( e.eventtype )    {
  case UpiEvent:
    switch(e.command_id)   {
    case MooreTest::CMD_CLOSE:
      if ( m_feed )  {
	IocSensor::instance().send(m_feed,MooreTest::CMD_REMOVE_MOORE_CLIENT,this);
      }
      m_ioc->send(this,e.command_id);
      break;
    default:
      break;
    }
    break;
  case IocEvent:
    switch(e.type)  {
    case MooreTest::CMD_UPDATE_MOORE_INFO:
      m_update = true;
      update(*(Statistics*)e.data);
      m_update = false;
      break;
    case MooreTest::CMD_CLOSE:
      while ( m_update ) ::lib_rtl_sleep(10);
      delete this;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void UpiMooreMonitor::update(const Statistics& stat)  {
  char text[256];
  int currLine = 0;
  for(Statistics::const_iterator i=stat.begin(); i!=stat.end(); ++i)   {
    const UserStats& us = (*i).second;
    const string& name = (*i).first;
    double inst_time_diff = (double(us.now.time.tv_sec-us.last.time.tv_sec)*1e9  + double(us.now.time.tv_nsec-us.last.time.tv_nsec))/1e9;
    double inst_seen_rate = inst_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_seen-us.last.evt_seen)/inst_time_diff;
    double inst_prod_rate = inst_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_prod-us.last.evt_prod)/inst_time_diff;
  
    double glob_time_diff = (double(us.now.time.tv_sec-us.start.time.tv_sec)*1e9 + double(us.now.time.tv_nsec-us.start.time.tv_nsec))/1e9;
    double glob_seen_rate = glob_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_seen-us.start.evt_seen)/glob_time_diff;
    double glob_prod_rate = glob_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_prod-us.start.evt_prod)/glob_time_diff;
    //cout << "Diff:" << glob_time_diff << " " << inst_time_diff << endl;

    const char* p = strchr_safe(name.c_str(),'_');
    const char* q = p ? strchr_safe(p+1,'_') : name.c_str();
    const char* n = q ? q+1 : p ? p+1 : name.c_str();
    if ( m_mode == 0 )  {
      ::snprintf(text,sizeof(text),"%-16s %7ld %7ld%8.0f %10.0f %8.0f %10.0f",n,
		 long(glob_time_diff*1e3),long(inst_time_diff*1e3),
		 inst_seen_rate, glob_seen_rate,inst_prod_rate, glob_prod_rate);
    }
    else if ( m_mode == 1 )  {
      ::snprintf(text,sizeof(text),"%-16s %7ld %7ld %7ld %8ld %8ld %7ld %8ld %8ld",n,
		 long(glob_time_diff*1e3),long(inst_time_diff*1e3),
		 us.start.evt_seen, us.last.evt_seen, us.now.evt_seen,
		 us.start.evt_prod, us.last.evt_prod, us.now.evt_prod);
    }
    else if ( m_mode == 2 )  {
      ::snprintf(text,sizeof(text),"%-16s %7ld %7ld %8ld %8ld %8ld %8.0f %10.0f %7ld %8ld %8ld %9.0f %10.0f",n,
		 long(glob_time_diff*1e3),long(inst_time_diff*1e3),
		 us.start.evt_seen, us.last.evt_seen, us.now.evt_seen, inst_seen_rate, glob_seen_rate,
		 us.start.evt_prod, us.last.evt_prod, us.now.evt_prod, inst_prod_rate, glob_prod_rate);
    }

    if ( currLine < m_lines )
      ::upic_replace_comment(m_id,COM_STATISTICS_1+currLine,text,"");
    else  {
      ::upic_insert_comment(m_id,MooreTest::CMD_CLOSE,COM_STATISTICS_1+currLine,text,"");
      ++m_lines;
    }
    currLine++;

  }
  if ( m_lines > currLine )   {
    for(int i=currLine; i<m_lines;++i)  {
      ::upic_replace_comment(m_id,COM_STATISTICS_1+currLine,"","");
    }
  }
}
