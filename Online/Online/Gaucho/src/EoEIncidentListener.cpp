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
  m_MonSvc = 0;
  svcloc->service("IncidentSvc",incs,true);
  incs->addListener(this,IncidentType::EndEvent);
}
EoEIncidentListener::~EoEIncidentListener()
{
  incs->removeListener(this,IncidentType::EndEvent);
}

void EoEIncidentListener::setMonitorSvc(IGauchoMonitorSvc *m)
{
  m_MonSvc = m;
}
void EoEIncidentListener::handle(const Incident &i)
{
  if (i.type() == IncidentType::EndEvent)
  {
    m_MonSvc->UnLock();
//    printf("-----------------EoE Handler Monitor System UN-LockED\n");
  }
}
