// $Id: IParticleFilter.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLEFILTER_H 
#define DAVINCIKERNEL_IPARTICLEFILTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

/** @class IParticleFilter IParticleFilter.h Kernel/IParticleFilter.h
 *  Interface Class for ParticleFilter. 
 *  Given a vector of Particles, provides a sub-vector of Particles
 *  satisfying a set of FilterCriteriums
 *  @author Paul Colrain
 *  @date   14/03/2002
 *
 *  Added the byPID methods
 *  @author P. Koppenburg
 *  @date   16/12/2004  
 */

class GAUDI_API IParticleFilter : virtual public IAlgTool {

public:

  DeclareInterfaceID(IParticleFilter, 2, 0);

  /// Filter
  virtual StatusCode filter( const LHCb::Particle::ConstVector&, 
                             LHCb::Particle::ConstVector& ) = 0;
  
  /// Filter negative particles
  virtual StatusCode filterNegative( const LHCb::Particle::ConstVector&, 
                                     LHCb::Particle::ConstVector& ) = 0; 

  /// Filter positive particles
  virtual StatusCode filterPositive( const LHCb::Particle::ConstVector&, 
                                     LHCb::Particle::ConstVector& ) = 0; 
  
   /// get particles by PID
  /// These two methods allow to get a subset of particles
  /// with the same PID without having to use the PIDFilterCriterion
  /// -> The same instance of ParticleFilter can be used for all
  /// Particles. 
  /// LHCb::Particle::ConstVector myKaons, myPiPlus, myPiMinus;
  /// sc = particleFilter()->filterByPID(parts, myKaons,   "K+");
  /// sc = particleFilter()->filterByPID(parts, myPiPlus,  "pi+", false);
  /// sc = particleFilter()->filterByPID(parts, myPiMinus, "pi-", false);
  /// would return a vector of kaons (both sign) and pions.
  ///
  virtual StatusCode filterByPID( const LHCb::Particle::ConstVector&,
                                  LHCb::Particle::ConstVector&,  
                                  const std::string&, 
                                  const bool alsoCC = true ) = 0;

  /// get particles by PID
  virtual StatusCode filterByPID( const LHCb::Particle::ConstVector&,
                                  LHCb::Particle::ConstVector&,  
                                  const int& pid, 
                                  const bool alsoCC = true ) = 0;
                    
};
#endif // DAVINCIKERNEL_IPARTICLEFILTER_H


