// $Id: IChangePIDTool.h,v 1.1 2006-12-18 11:08:10 jpalac Exp $
#ifndef KERNEL_ICHANGEPIDTOOL_H 
#define KERNEL_ICHANGEPIDTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IChangePIDTool ( "IChangePIDTool", 1, 0 );

/** @class IChangePIDTool IChangePIDTool.h Kernel/IChangePIDTool.h
 *   
 *  Interface for ChangePIDTool.
 *  Changes the PID of a Particle or of every element of a list of Particles.
 *
 *  @author Patrick Spradlin
 *  @date   2006-12-13
 */
class IChangePIDTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IChangePIDTool; }

  /// Change the PID of a single particle
  virtual LHCb::Particle changePID( const LHCb::Particle & ) = 0;

  /// Change the PID of each member of a vector of particles
  virtual std::vector<LHCb::Particle> changePID( const LHCb::Particle::ConstVector & ) = 0;

protected:

private:

};
#endif // KERNEL_ICHANGEPIDTOOL_H
