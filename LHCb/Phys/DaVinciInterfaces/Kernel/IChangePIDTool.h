// $Id: IChangePIDTool.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_ICHANGEPIDTOOL_H 
#define KERNEL_ICHANGEPIDTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

/** @class IChangePIDTool IChangePIDTool.h Kernel/IChangePIDTool.h
 *   
 *  Interface for ChangePIDTool.
 *  Changes the PID of a Particle or of every element of a list of Particles.
 *
 *  @author Patrick Spradlin
 *  @date   2006-12-13
 */
class GAUDI_API IChangePIDTool : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IChangePIDTool, 2, 0);
  
  /// Change the PID of a single particle
  virtual LHCb::Particle changePID( const LHCb::Particle & ) = 0;

  /// Change the PID of each member of a vector of particles
  virtual std::vector<LHCb::Particle> changePID( const LHCb::Particle::ConstVector & ) = 0;

  /// Templated findDecay method working on particle container iterators.
  template <class PARTICLE>
  inline std::vector<LHCb::Particle> changePID( PARTICLE begin,
                                                PARTICLE end    )  {
    return changePID ( LHCb::Particle::ConstVector ( begin , end ) );
  }

private:

};
#endif // KERNEL_ICHANGEPIDTOOL_H
