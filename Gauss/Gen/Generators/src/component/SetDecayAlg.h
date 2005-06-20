// $Id: SetDecayAlg.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
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
};

#endif // GENERATORS_SETDECAYALG_H

