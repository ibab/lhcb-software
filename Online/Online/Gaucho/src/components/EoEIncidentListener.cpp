/*
 * EoEIncidentListener.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: beat
 */
#include "Gaucho/EoEIncidentListener.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MonitorSvc.h"

EoEIncidentListener::EoEIncidentListener(const std::string& , ISvcLocator* svcloc, long  )
{
  m_endTime=m_startTime=m_proclimit=m_EvtNr=m_RunNr=0;
  m_MonSvc = 0;
  m_executing = false;
  svcloc->service("IncidentSvc",incs,true);
  incs->addListener(this,"DAQ_END_EVENT");
  incs->addListener(this,"DAQ_BEGIN_EVENT");
}
EoEIncidentListener::~EoEIncidentListener()
{
  incs->removeListener(this);
}

void EoEIncidentListener::setMonitorSvc(IGauchoMonitorSvc *m)
{
  m_MonSvc = m;
}
void EoEIncidentListener::handle(const Incident &i)
{
  if (i.type() == "DAQ_END_EVENT")
  {
    if (!m_executing)
    {
//      printf("-----------------EoE Handler called without executing event... NOT unlocking Monitor system\n");
      m_endTime = time(0);
      time_t proctime;
      proctime = m_endTime-m_startTime;
      if (proctime > m_proclimit)
      {
        ::lib_rtl_output(LIB_RTL_INFO,"+++++Very long processing time %d seconds(>%d) RunNr %d, L0EvtNr %d\n",
            proctime, m_proclimit,this->m_RunNr,this->m_EvtNr);
      }
      m_executing = false;
      return;
    }
    m_executing = false;
    m_MonSvc->UnLock();
//    printf("-----------------EoE Handler Monitor System UN-LockED\n");
  }
  else if (i.type() == "DAQ_BEGIN_EVENT")
  {
//    m_MonSvc->Lock();
//    printf("-----------------EoE Handler Monitor System LockED\n");
  }
}
