//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/DimCmdServer.h,v 1.1 2005-03-22 16:39:17 evh Exp $
#ifndef GAUCHO_DIMCMDSERVER_H
#define GAUCHO_DIMCMDSERVER_H 1

#include "dis.hxx"
#include "dic.hxx"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/IHistogramSvc.h"



/** @class DimPropServer DimCmdServer.h Gaucho/DimCmdServer.h

    This class get/sets allows execution of commands via dim, in particular
    for dynamic publishing of histograms

    @author Eric van Herwijnen
*/

// forward declarations
class ISvcLocator;
class IMessageSvc;
class IHistogram1D;

class DimCmdServer : public DimCommand {
public:
  DimCmdServer(char* thisnodename,ISvcLocator* svclocator );
  virtual ~DimCmdServer();
  void commandHandler();
private:
  char* m_name;
  char* m_nodename;
  ISvcLocator* m_svcloc;
  ISvcLocator* m_HDS;
  IMessageSvc* m_msgsvc;
  IHistogram1D* myhisto;
  IMonitorSvc* m_publishsvc;
};

#endif //GAUCHO_DIMCMDSERVER_H
