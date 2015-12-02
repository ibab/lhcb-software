/*
 * EoEIncidentListener.h
 *
 *  Created on: Jun 4, 2010
 *      Author: beat
 */

#ifndef EOEINCIDENTLISTENER_H_
#define EOEINCIDENTLISTENER_H_
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/SmartIF.h"
class Incident;
class MonitorSvc;
class ISvcLocator;
class IGauchoMonitorSvc;
class IIncidentSvc;

class EoEIncidentListener : public implements1<IIncidentListener>
{
public:
  EoEIncidentListener(const std::string& name, ISvcLocator* svcloc, long shots );
  void setMonitorSvc(IGauchoMonitorSvc* );
  virtual ~EoEIncidentListener();
  void handle(const Incident&);
  bool m_executing;
  time_t m_startTime;
  time_t m_endTime;
  time_t m_proclimit;
  int m_RunNr;
  long long int m_EvtNr;
private:
  IGauchoMonitorSvc *m_MonSvc;
  IIncidentSvc *incs;
};
#endif /* EOEINCIDENTLISTENER_H_ */
