// $Id: IParticleFilter.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLEFILTER_H 
#define DAVINCIKERNEL_IPARTICLEFILTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleFilter("IParticleFilter", 1 , 2); 

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

class IParticleFilter : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleFilter;
  }

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


