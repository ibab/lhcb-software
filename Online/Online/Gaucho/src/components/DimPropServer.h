//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/components/DimPropServer.h,v 1.3 2008-03-28 10:59:12 evh Exp $
#ifndef GAUCHO_DIMPROPSERVER_H
#define GAUCHO_DIMPROPSERVER_H 1

#include "dis.hxx"
#include "AIDA/IHistogram1D.h"
#include "GaudiKernel/IHistogramSvc.h"

/** @class DimPropServer DimPropServer.h Gaucho/DimPropServer.h

This class get/sets properties of Gaudi Algorithms/ApplicationMgr on request
as a Dim Rpc service. A Dim Rpc client can connect and give string commands
for this purpose.

@author Philippe Vannerem
*/

// forward declarations
class ISvcLocator;
class IAlgManager;
class IProperty;
class IMessageSvc;
//class IHistogram1D;
class IHistogram;

class DimPropServer : public DimRpc {
public:
  DimPropServer(std::string name,ISvcLocator* svclocator );
  virtual ~DimPropServer();
  void rpcHandler();
private:
  IAlgManager* m_algman;
  ISvcLocator* m_svcloc;
  IProperty* m_appmgr;
  IMessageSvc* m_msgsvc;
  IHistogram* myhisto;
  DataObject* mydataobject;
};

#endif //GAUCHO_DIMPROPSERVER_H
