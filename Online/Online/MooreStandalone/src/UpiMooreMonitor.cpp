// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Runable to run Moore standalone on a single node.

//  Author     : M.Frank
//====================================================================
// $Header: $

// Framework include files
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "CPP/Interactor.h"
#include "GaudiKernel/Service.h"
#include "Internals.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb {

  /** @class UpiMooreMonitor UpiMooreMonitor.h MooresStandalone/UpiMooreMonitor....h
   *
   *   Class definition of UpiMooreMonitor.
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class UpiMooreMonitor : public Service, public MooreTest::ResultMonitor, virtual public Interactor  {
  protected:
    int m_id;
    int m_lines;
    int m_currLine;
    int m_mode;
    IocSensor* m_ioc;
    enum Cmds {  
      CMD_WRITE_MESSAGE = 1,
      COM_HEADER1 = 1,
      COM_HEADER2,
      COM_STATISTICS_1,
      CMD_CLOSE=99999
    };
  public:
    /// Initializing constructor
    UpiMooreMonitor(const std::string& nam, ISvcLocator* log);
    virtual ~UpiMooreMonitor();
    StatusCode initialize();
    StatusCode finalize();
    virtual void begin();
    virtual void end();
    virtual void output(const char* buffer);
    virtual void outputHeader();
    virtual void outputClient(const std::string& name, const MooreTest::UserStats& us);
    virtual void handle (const Event& event);
  };
}


#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <ctime>
#include "GaudiKernel/SvcFactory.h"
#include "RTL/strdef.h"
#include "UPI/upi_menu.h"
#include "UPI/UpiSensor.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,UpiMooreMonitor)

using namespace std;
using namespace LHCb;

UpiMooreMonitor::UpiMooreMonitor(const std::string& nam, ISvcLocator* loc) 
  : Service(nam, loc), m_id(0), m_lines(0), m_currLine(0)
{
  declareProperty("Mode",m_mode=2);
}

UpiMooreMonitor::~UpiMooreMonitor()   {
}

StatusCode UpiMooreMonitor::initialize()   {
  StatusCode sc = Service::initialize();
  char text1[256], text2[256];
  m_id = UpiSensor::newID();
  m_ioc = &IocSensor::instance();

  if ( m_mode == 0 )  {
    ::snprintf(text1,sizeof(text1),"%-16s %15s  %-19s  %s",
	       "---------------","--Time [msec]--",
	       "----Input [Hz]----","---Output [Hz]----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s  %-19s  %s",
	       "Process name","Total","Delta","Instant Cumulative","Instant Cumulative");
  }
  else if ( m_mode == 1 )  {
    ::snprintf(text1,sizeof(text1),"%-16s %15s  %-25s  %s",
	       "---------------","--Time [msec]--",
	       "-----Events consumed-----","-----Events produced-----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s    %-26s %s",
	       "Process name","Total","Delta",
	       "Start Previous      Now","Start Previous      Now");
  }
  else if ( m_mode == 2 )  {
    ::snprintf(text1,sizeof(text1),"%-16s %15s  %-25s %-19s %-26s  %s",
	       "---------------","--Time [msec]--",
	       "-----Events consumed-----","----Input [Hz]----",
	       "-----Events produced-----","---Output [Hz]----");
    ::snprintf(text2,sizeof(text2),"%-16s%8s %7s    %-25s%-18s   %-25s %s",
	       "Process name","Total","Delta",
	       "Start Previous      Now","Instant Cumulative",
	       "Start Previous      Now","Instant Cumulative");
  }
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,name().c_str(),"Statistics",name().c_str());
  ::upic_add_comment(COM_HEADER1,text1,"");
  ::upic_add_comment(COM_HEADER2,text2,"");
  ::upic_add_comment(COM_STATISTICS_1+m_lines++,"--","");
  ::upic_add_comment(COM_STATISTICS_1+m_lines++,"--","");
  ::upic_add_command(CMD_CLOSE,"Close>","");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,CMD_CLOSE,0);
  UpiSensor::instance().add(this,m_id);
  return sc;
}

StatusCode UpiMooreMonitor::finalize()   {
  if ( m_id > 0 ) {
    UpiSensor::instance().remove(this,m_id);
    ::upic_delete_menu(m_id);
  }
  return Service::finalize();
}

void UpiMooreMonitor::handle (const Event& e)   {
  switch ( e.eventtype )    {
  case UpiEvent:
    switch(e.command_id)   {
    case CMD_CLOSE:
    default:
      m_ioc->send(this,e.command_id);
      break;
    }
    break;
  case IocEvent:
    switch(e.type)  {
    case CMD_CLOSE:
      delete this;
      break;
    case CMD_WRITE_MESSAGE:
      ::upic_write_message((char*)e.data,"");
      ::free(e.data);
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void UpiMooreMonitor::output(const char* buffer)  {
  if ( buffer )   {
    m_ioc->send(this, CMD_WRITE_MESSAGE, ::strdup(buffer));
  }
}

void UpiMooreMonitor::begin()  {
  m_currLine = 0;
}

void UpiMooreMonitor::end()  {
  if ( m_lines > m_currLine )   {
    for(int i=m_currLine; i<m_lines;++i)  {
      ::upic_replace_comment(m_id,COM_STATISTICS_1+m_currLine,"","");
    }
  }
}

void UpiMooreMonitor::outputHeader()  {
}

void UpiMooreMonitor::outputClient(const std::string& name, const MooreTest::UserStats& us)  {
  char text[256];
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
    ::snprintf(text,sizeof(text),"%-16s %7ld %7ld %8.0f %10.0f %9.0f %10.0f",n,
	       long(glob_time_diff*1e3),long(inst_time_diff*1e3),
	       inst_seen_rate, glob_seen_rate,inst_prod_rate, glob_prod_rate);
  }
  else if ( m_mode == 1 )  {
    ::snprintf(text,sizeof(text),"%-16s %7ld %7ld %8ld %8ld %8ld %8ld %8ld %8ld",n,
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

  if ( m_currLine < m_lines )
    ::upic_replace_comment(m_id,COM_STATISTICS_1+m_currLine,text,"");
  else  {
    ::upic_insert_comment(m_id,CMD_CLOSE,COM_STATISTICS_1+m_currLine,text,"");
    ++m_lines;
  }
  m_currLine++;
}

