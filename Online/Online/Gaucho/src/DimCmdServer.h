//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimCmdServer.h,v 1.4 2008-01-22 14:53:42 evh Exp $
#ifndef GAUCHO_DIMCMDSERVER_H
#define GAUCHO_DIMCMDSERVER_H 1

#include "dis.hxx"
#include "dic.hxx"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IIncidentListener.h"


/** @class DimPropServer DimCmdServer.h

This class get/sets allows execution of commands via dim, in particular
for dynamic publishing of histograms

@author Eric van Herwijnen
*/

// forward declarations
class ISvcLocator;
class IMessageSvc;
//class IHistogram1D;
class IHistogram;
class IIncidentSvc;

class DimCmdServer : public DimCommand {
protected:
  IIncidentSvc* m_incidentSvc;
public:
  DimCmdServer(std::string name,ISvcLocator* svclocator );
  virtual ~DimCmdServer();
  void commandHandler();
private:
  ISvcLocator* m_svcloc;
  ISvcLocator* m_HDS;
  IMessageSvc* m_msgsvc;
  IHistogram* myhisto;
  DataObject* mydataobject;
  IMonitorSvc* m_publishsvc;
 
};

#endif //GAUCHO_DIMCMDSERVER_H
