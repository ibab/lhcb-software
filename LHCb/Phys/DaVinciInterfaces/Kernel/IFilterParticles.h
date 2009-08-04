// $Id: IFilterParticles.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IFILTERPARTICLES_H 
#define KERNEL_IFILTERPARTICLES_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

/** @class IFilterParticles IFilterParticles.h Kernel/IFilterParticles.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2007-07-20
 */
class GAUDI_API IFilterParticles : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IFilterParticles, 2, 0);

  /// Test if filter is satisfied on ensemble of particles
  virtual bool isSatisfied( const LHCb::Particle::ConstVector& ) const = 0;
  /// Test if filter is satisfied on ensemble of particles
  virtual bool operator()( const LHCb::Particle::ConstVector& ) const = 0;

};
#endif // KERNEL_IFILTERPARTICLES_H
