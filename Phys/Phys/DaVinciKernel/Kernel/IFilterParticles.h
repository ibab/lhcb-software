// $Id: IFilterParticles.h,v 1.1 2007-07-20 14:54:22 jpalac Exp $
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

static const InterfaceID IID_IFilterParticles ( "IFilterParticles", 1, 0 );

/** @class IFilterParticles IFilterParticles.h Kernel/IFilterParticles.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2007-07-20
 */
class IFilterParticles : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFilterParticles; }

  /// Test if filter is satisfied on ensemble of particles
  virtual bool isSatisfied( const LHCb::Particle::ConstVector& ) const = 0;
  /// Test if filter is satisfied on ensemble of particles
  virtual bool operator()( const LHCb::Particle::ConstVector& ) const = 0;

protected:

private:

};
#endif // KERNEL_IFILTERPARTICLES_H
