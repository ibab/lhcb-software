//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/DimPropServer.h,v 1.1.1.1 2003-06-05 09:34:45 vanphil Exp $
#ifndef GAUCHO_DIMPROPSERVER_H
#define GAUCHO_DIMPROPSERVER_H 1

#include "dim/dis.hxx"

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

class DimPropServer : public DimRpc {
public:
  DimPropServer(char* thisnodename,ISvcLocator* svclocator );
  virtual ~DimPropServer();
  void rpcHandler();
private:
  char* m_name;
  IAlgManager* m_algman;
  ISvcLocator* m_svcloc;
  IProperty* m_appmgr;
  IMessageSvc* m_msgsvc;
};

#endif //GAUCHO_DIMPROPSERVER_H
