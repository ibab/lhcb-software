// $Id: IParticleFilter.h,v 1.3 2004-12-16 15:48:02 pkoppenb Exp $
#ifndef DAVINCITOOLS_IPARTICLEFILTER_H 
#define DAVINCITOOLS_IPARTICLEFILTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleFilter("IParticleFilter", 1 , 2); 

/** @class IParticleFilter IParticleFilter.h DaVinciTools/IParticleFilter.h
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
  virtual StatusCode filter( const ParticleVector&, 
                             ParticleVector& ) = 0;
  
  /// Filter negative particles
  virtual StatusCode filterNegative( const ParticleVector&, 
                                     ParticleVector& ) = 0; 

  /// Filter positive particles
  virtual StatusCode filterPositive( const ParticleVector&, 
                                     ParticleVector& ) = 0; 
  
   /// get particles by PID
  /// These two methods allow to get a subset of particles
  /// with the same PID without having to use the PIDFilterCriterion
  /// -> The same instance of ParticleFilter can be used for all
  /// Particles. 
  /// ParticleVector myKaons, myPiPlus, myPiMinus;
  /// sc = particleFilter()->filterByPID(parts, myKaons,   "K+");
  /// sc = particleFilter()->filterByPID(parts, myPiPlus,  "pi+", false);
  /// sc = particleFilter()->filterByPID(parts, myPiMinus, "pi-", false);
  /// would return a vector of kaons (both sign) and pions.
  ///
  virtual StatusCode filterByPID( const ParticleVector&,
                                  ParticleVector&,  
                                  const std::string&, 
                                  const bool alsoCC = true ) = 0;

  /// get particles by PID
  virtual StatusCode filterByPID( const ParticleVector&,
                                  ParticleVector&,  
                                  const int& pid, 
                                  const bool alsoCC = true ) = 0;
                    
};
#endif // DAVINCITOOLS_IPARTICLEFILTER_H


