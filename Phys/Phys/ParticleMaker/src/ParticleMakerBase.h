// $Id: ParticleMakerBase.h,v 1.1 2009-04-21 19:15:41 pkoppenb Exp $
#ifndef PARTICLEMAKERBASE_H 
#define PARTICLEMAKERBASE_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "Kernel/IPhysDesktop.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticle2State.h"

/** @class ParticleMakerBase ParticleMakerBase.h
 *
 *  Base class for all Particle Maker Algorithms
 *
 *  @author P. Koppenburg
 *  @date   2009-04-21
 */

class ParticleMakerBase : public DVAlgorithm {
public:
  /// Standard constructor
  ParticleMakerBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleMakerBase( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

  virtual StatusCode initialize() ; ///< Finalize
  virtual StatusCode finalize(){return DVAlgorithm::finalize();} ; ///< Finalize

protected:
  /// The method that each implementation should implement
  virtual StatusCode makeParticles(LHCb::Particle::Vector& ) = 0 ;
  
protected:
  /// Particle to state convertion tool
  IParticle2State* m_p2s ;

private:

};
#endif // PARTICLEMAKERBASE
