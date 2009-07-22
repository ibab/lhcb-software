// $Id: IParticleArrayFilter.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef KERNEL_IPARTICLEARRAYFILTER_H 
#define KERNEL_IPARTICLEARRAYFILTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleArrayFilter ( "IParticleArrayFilter", 1, 0 );

/** @class IParticleArrayFilter IParticleArrayFilter.h Kernel/IParticleArrayFilter.h
 *  
 *  Apply a filter to an array of Particles and either remove those that 
 *  do not pass from the array or fill a new one with those that pass.
 *  @author Juan Palacios
 *  @date   2007-07-24
 */
class IParticleArrayFilter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleArrayFilter; }

  /// Filter and put results into new array
  virtual StatusCode filter( const LHCb::Particle::ConstVector&, 
                             LHCb::Particle::ConstVector& ) const = 0;
  /// Filter and remove elements that do not pass filter from array
  virtual StatusCode filter( LHCb::Particle::ConstVector& ) const = 0;



protected:

private:

};
#endif // KERNEL_IPARTICLEARRAYFILTER_H
