// $Id: ParticleDescendants.h,v 1.1 2005-10-21 14:48:07 pkoppenb Exp $
#ifndef PARTICLEDESCENDANTS_H 
#define PARTICLEDESCENDANTS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleDescendants.h"            // Interface


/** @class ParticleDescendants ParticleDescendants.h
 *  
 *  Return the descendants of a Particle
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-10-19
 */
class ParticleDescendants : public GaudiTool, virtual public IParticleDescendants {
  
public: 
  /// Standard constructor
  ParticleDescendants( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ParticleDescendants( ); ///< Destructor

  // Return all descendants of a Particle
  const ParticleVector descendants(const Particle* )  ;
  
  // Return all descendants of level i for a Particle
  const ParticleVector descendants(const Particle*, int )  ;
  
  // Return all stable descendants of a Particle
  const ParticleVector finalStates(const Particle* )  ;

protected:

  // Return all daughters of a Particle
  bool addDaughters(const ParticleVector&, ParticleVector&);

  // Return all daughters of a Particle
  bool addDaughters(const Particle*, ParticleVector&);


private:

};
#endif // PARTICLEDESCENDANTS_H
