/*
 * StatusService.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: Beat Jost
 */

#include "dim/dis.hxx"
#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
#include "components/MonitorSvc.h"
#include "Gaucho/StatusService.h"
StatusService::StatusService(MonitorSvc *msvc): DimService((RTL::processName() + "/ServiceStatus").c_str(), (char *) "C")
{
  m_monitorSvc = msvc;
  m_MonSys = 0;
}
StatusService::~StatusService()
{
}
void StatusService::serviceHandler()
{
  Gaudi::StateMachine::State msvc_state;
  std::string statusline;
  m_MonSys = &MonSys::m_instance();
  std::vector<MonSubSys*> *ssyslist;
  ssyslist = m_MonSys->getSubsyslist();
  if(ssyslist != 0)
  {
    for (size_t i = 0; i < ssyslist->size(); i++)
    {
      MonSubSys *ssys;
      ssys = ssyslist->at(i);
      if (ssys->m_type == MONSUBSYS_Histogram)
      {
        if (ssys->start_done)
        {
          statusline.append("Histogram Subsystem started ");
        }
        else
        {
          statusline.append("Histogram Subsystem stopped ");
        }
      }
      else if (ssys->m_type == MONSUBSYS_Counter)
      {
        if (ssys->start_done)
        {
          statusline.append("Counter Subsystem started ");
        }
        else
        {
          statusline.append("Counter Subsystem stopped ");
        }
      }
    }
  }
  msvc_state = m_monitorSvc->FSMState();
  if (m_monitorSvc->m_i_startState)
  {
    statusline.append("Monitor Service i_start called ");
  }
  else
  {
    statusline.append("Monitor Service i_stop called ");
  }
  switch (msvc_state)
  {
    case Gaudi::StateMachine::OFFLINE:
      statusline.append("Monitor Service OFFLINE");
    case Gaudi::StateMachine::CONFIGURED:
      statusline.append("Monitor Service CONFIGURED");
    case Gaudi::StateMachine::INITIALIZED:
      statusline.append("Monitor Service INITIALIZED");
    case Gaudi::StateMachine::RUNNING:
      statusline.append("Monitor Service RUNNING");
  }
  setData((char*) statusline.c_str());
  return;
}
