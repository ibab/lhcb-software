#include "Online/Sensor.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

namespace {
  static Online::Sensor* SensorHead = 0;
};

Online::Status Online::Sensor::staticRearm(int, int)   {
  for(Sensor *sn = SensorHead; sn; sn = sn->next() )  {
    if( sn->m_rearmPending )   {
      sn->rearm();
      sn->m_rearmPending = false;
    }
  }
  return WT_SUCCESS;
}

Online::Status Online::Sensor::staticAction( int facility, int parameter )  {
  for(Sensor *sn = SensorHead; sn; sn = sn->next() )  {
    if ( sn->m_facility == facility ) {
      sn->m_rearmPending = true;
      sn->dispatch(parameter);
      break;
    }
  }
  return WT_SUCCESS;
}

long Online::Sensor::staticRun(void* param)  {
  Sensor* s = (Sensor*)param;

  return 1;
}

Online::Sensor::Sensor( int facility, const std::string& name, bool wtsubs) {
  m_name     = name;
  m_facility = facility;
  m_next     = SensorHead;
  SensorHead = this;
  m_handle   = 0;

  if( wtsubs )  {
    int status = wtc_subscribe( facility, staticRearm, staticAction );
    if ( status != WT_SUCCESS )  {
      wtc_error(status);
    }
  }
  m_rearmPending = true;
}

Online::Sensor::~Sensor()   {
  int status = wtc_remove( m_facility );
  if ( status != WT_SUCCESS ) wtc_error(status);
}

void Online::Sensor::run()    {
  if ( 0 == m_handle )  {
    int status = lib_rtl_start_thread(staticRun, this, &m_handle);
    if ( lib_rtl_is_success(status) )  {
      return;
    }
  }
}

void Online::Sensor::wait()    {
  while(1)  {
    int status, facility, substatus, param;
    status = wtc_wait( &facility, &param, &substatus );
    if ( status != WT_SUCCESS && status != WT_BADACTIONSTAT && status != WT_NOSUBSCRIBED) {
      printf(" exiting wt_wait status = %d param %d  substat %d, fac %d \n",
             status, param, substatus, facility);
    }
  }
}
