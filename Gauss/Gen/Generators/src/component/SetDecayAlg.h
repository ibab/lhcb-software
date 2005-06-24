// $Id: SetDecayAlg.h,v 1.2 2005-06-24 16:32:47 gcorti Exp $
#ifndef GENERATORS_SETDECAYALG_H
#define GENERATORS_SETDECAYALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

class IEvtGenTool ;

class SetDecayAlg : public GaudiAlgorithm {
public:
  SetDecayAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SetDecayAlg( ) ;
  
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  // Interface to EvtGen
  IEvtGenTool * m_evtTool ;
  std::string m_eventLoc;
};

#endif // GENERATORS_SETDECAYALG_H

