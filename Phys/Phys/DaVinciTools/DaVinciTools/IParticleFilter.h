// $Id: IParticleFilter.h,v 1.1 2002-03-27 20:27:45 gcorti Exp $
#ifndef DAVINCITOOLS_IPARTICLEFILTER_H 
#define DAVINCITOOLS_IPARTICLEFILTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleFilter\
                           ("IParticleFilter", 1 , 0); 

/** @class IParticleFilter IParticleFilter.h 
 *
 *  @author Paul Colrain
 *  @date   14/03/2002
 */

class IParticleFilter : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleFilter;
  }

  /// Filter
  virtual StatusCode filter( const ParticleVector&, ParticleVector& ) = 0;
  
  /// Filter positive particles
  virtual StatusCode filterPositive( const ParticleVector&, 
                                                    ParticleVector& ) = 0; 
  
  /// Filter negative particles
  virtual StatusCode filterNegative( const ParticleVector&, 
                                                    ParticleVector& ) = 0; 

};
#endif // DAVINCITOOLS_IPARTICLEFILTER_H


