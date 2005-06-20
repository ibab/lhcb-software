// $Id: SignalDecayAlg.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
#ifndef GENERATORS_SIGNALDECAYALG_H
#define GENERATORS_SIGNALDECAYALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

class IEvtGenTool ;

/** @class SignalDecayAlg 
 *  
 *  Algorithm which creates a predefined signal decay mode
 *  before PythiaAlg is executed.
 *  The signal particle is generated at rest and its b-flavour will
 *  be given to Pythia to force the hadronization into it.
 * 
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */

class SignalDecayAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  SignalDecayAlg(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~SignalDecayAlg( ) ;
  
  /// Initialize method
  virtual StatusCode initialize();
  /// Execute method
  virtual StatusCode execute();
  /// Finalize method
  virtual StatusCode finalize();

private:
  // Interface to the EvtGenTool
  IEvtGenTool * m_evtTool ;
};

#endif // GENERATORS_SIGNALDECAYALG_H
