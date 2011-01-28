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
  ~EoEIncidentListener();
  void handle(const Incident&);
private:
  IGauchoMonitorSvc *m_MonSvc;
  IIncidentSvc *incs;
};
#endif /* EOEINCIDENTLISTENER_H_ */
