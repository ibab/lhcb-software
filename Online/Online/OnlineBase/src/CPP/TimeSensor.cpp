#include <cstdio>
#include <cstdlib>
#include <map>
#include "CPP/Event.h"
#include "CPP/TimeSensor.h"   
#include "CPP/Interactor.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

namespace {
  class Period;
  class InteractorTarget;

  class Period {
    friend class ::TimeSensor;
  private:
    int           Time; 
    unsigned long m_alrmID;
    std::string   m_ascTime;
    bool          m_rearmPending;
    bool          m_cyclic;
    Period*       m_next;
  public:
    /// Initializing constructor
    explicit Period( char* );
    /// Default destructor
    ~Period();
    void Set();
    void cancel();
    // unsigned long& id()      {  return m_alrmID;   }
    Period* next()  const    {  return m_next;     }
    void setNext(Period* n)  {  m_next = n;        }
    static int ast( void* param );
  };

  class InteractorTarget {
    friend      class ::TimeSensor;
  private:
    Interactor* Inter;
    void*       Data;
  public:
    /// Initializing constructor
    explicit InteractorTarget( Interactor* i, void* d = 0 ) { Inter = i; Data = d;}
  };

  //----------------------------------------------------------------------------
  Period::Period(char* period) : m_alrmID(0) {
    int day, hour, minute, sec;
    if( *period == '+' )  period++, m_cyclic = true;
    else                  m_cyclic = false;
    m_ascTime = period;
    sscanf(m_ascTime.c_str(), "%d %d:%d:%d.0", &day, &hour, &minute, &sec); 
    Time = sec + day*86400 + hour*3600 + minute*60; 
    m_rearmPending = true;
    m_next = 0;
  }

  //----------------------------------------------------------------------------
  Period::~Period() {
  }
  //----------------------------------------------------------------------------
  void Period::cancel() {
    if ( m_alrmID )  {
      int status = lib_rtl_kill_timer(m_alrmID);
      if ( !lib_rtl_is_success(status) )  {
        lib_rtl_signal_message(status,"Failed to cancel period timer.");
      }
    }
  }

  //----------------------------------------------------------------------------
  void Period::Set() {
    int status = lib_rtl_set_timer(Time*1000, ast, this, &m_alrmID);
    if ( !lib_rtl_is_success(status) )  {
      lib_rtl_signal_message(status,"Failed to set period timer.");
    }
  }

  //----------------------------------------------------------------------------
  int Period::ast( void* param )   {
    Period *period = (Period*)param;
    period->m_alrmID = 0;
    return wtc_insert(WT_FACILITY_SENSOR1, param);
  }

  typedef std::map<Period*, InteractorTarget*>  InteractorTable;
  static Period*         s_periodHead      = 0;
  static InteractorTable s_interactorTable;
}

//----------------------------------------------------------------------------
TimeSensor::TimeSensor() : Sensor( WT_FACILITY_SENSOR1, "TimeSensor" )  {
}

//----------------------------------------------------------------------------
TimeSensor::~TimeSensor()   {
  s_interactorTable.clear();
}

//----------------------------------------------------------------------------
void TimeSensor::add( Interactor* interactor, void* newperiod)    {
  return add(interactor,newperiod,0);
}

//----------------------------------------------------------------------------
void TimeSensor::add( Interactor* interactor, void* newperiod, void* data)    {
  Period *temperiod = new Period( (char*)newperiod );
  InteractorTarget *inttar = new InteractorTarget(interactor,data);
  temperiod->setNext(s_periodHead); 
  s_periodHead = temperiod;  
  s_interactorTable.insert( std::make_pair(temperiod,inttar));
  //---If the TimeSensor is the only active sensor, then it is necessary to
  // rearm it here and not delay it.
  rearm();
  m_rearmPending = false;
}

//----------------------------------------------------------------------------
void TimeSensor::add( Interactor* interactor, int timesec, void* data ) {
  char times[64];
  int day      = (timesec)/86400;
  int hour     = (timesec - day*86400)/3600;
  int minute   = (timesec - day*86400 - hour*3600)/60;
  int sec      = (timesec - day*86400 - hour*3600 - minute*60);
  ::snprintf(times,sizeof(times),"%d %d:%d:%d.0", day, hour, minute, sec); 
  add( interactor, times, data );
}

//----------------------------------------------------------------------------
void TimeSensor::remove( Interactor* interactor, void* data ) {
  for( Period* pd = s_periodHead, *last = 0; pd; last = pd, pd = pd->next())  {
    InteractorTable::iterator i = s_interactorTable.find(pd);
    if ( i != s_interactorTable.end() )  {
      InteractorTarget *it = (*i).second;
      if( it->Inter == interactor && (data == 0 || it->Data == data) )  {
        s_interactorTable.erase(i);
        pd->cancel();
        if( !last )
          s_periodHead = pd->next();
        else
          last->setNext(pd->next());
        delete pd; 
        delete it;
        break;
      } 
    } 
  }
}

//----------------------------------------------------------------------------
void TimeSensor::dispatch( void* id ) {
  Period *period = (Period*)id;
  InteractorTable::iterator i = s_interactorTable.find(period);
  if ( i != s_interactorTable.end() )  {
    InteractorTarget  *it = (*i).second;
    if( period->m_cyclic )    {
      period->m_rearmPending = true;
      Event ev(it->Inter,TimeEvent);
      ev.timer_id   = id;
      ev.timer_data = it->Data;
      ev.target->handle(ev);       
    }
    else    { 
      if ( period == s_periodHead ) {
        s_periodHead = period->m_next;
      }
      else  {
        for(Period* pd = s_periodHead; pd; pd = pd->m_next)  {
          if( pd->m_next == period ) { 
            pd->m_next = period->m_next; break; 
          }
        }
      }
      s_interactorTable.erase(i);
      Event ev(it->Inter, TimeEvent);
      ev.timer_id   = id;
      ev.timer_data = it->Data;
      delete period; 
      delete it;
      ev.target->handle(ev);       
    }
  }
}

//----------------------------------------------------------------------------
void TimeSensor::rearm()  {
  for(Period * pd = s_periodHead; pd; pd = pd->m_next)  {
    if ( pd->m_rearmPending )  {
      pd->Set();
      pd->m_rearmPending = false;
    }
  }
}
//----------------------------------------------------------------------------
TimeSensor& TimeSensor::instance()    {
  static TimeSensor *sensor = new TimeSensor;
  return *sensor;
}
