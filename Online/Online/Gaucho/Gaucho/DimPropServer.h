//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/DimPropServer.h,v 1.4 2005-03-22 16:39:17 evh Exp $
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
class IHistogram1D;

class DimPropServer : public DimRpc {
public:
  DimPropServer(char* thisnodename,ISvcLocator* svclocator );
  virtual ~DimPropServer();
  void rpcHandler();
private:
  char* m_name;
  char* m_nodename;
  IAlgManager* m_algman;
  ISvcLocator* m_svcloc;
  IProperty* m_appmgr;
  IMessageSvc* m_msgsvc;
  IHistogram1D* myhisto;
};

#endif //GAUCHO_DIMPROPSERVER_H
