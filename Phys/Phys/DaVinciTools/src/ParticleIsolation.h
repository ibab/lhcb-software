#ifndef PARTICLEISOLATION_H 
#define PARTICLEISOLATION_H 1

#include "Kernel/DVAlgorithm.h"

/** @class ParticleIsolation ParticleIsolation.h 
 *
 *  Algorithm to add Particle isolation variables 
 *  calculated by ConeVariables tool to particle extra info
 *
 *  @author Anton Poluektov
 *  @date   19/02/2012
 */

class ParticleIsolation : public DVAlgorithm {

public: 
  /// Standard constructor
  ParticleIsolation( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ParticleIsolation( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: 
   
  std::string m_coneVariablesToolName;

  IConeVariables* m_coneVar;

};

//=======================================================================//
#endif // PARTICLEISOLATION_H
