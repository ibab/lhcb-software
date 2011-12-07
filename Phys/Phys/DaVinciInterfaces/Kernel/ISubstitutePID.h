// $Id: $
#ifndef KERNEL_ISUBSTITUTEPID_H 
#define KERNEL_ISUBSTITUTEPID_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
// ==========================================================================

static const InterfaceID IID_ISubstitutePID ( "ISubstitutePID", 1, 0 );

/** @class ISubstitutePID ISubstitutePID.h Kernel/ISubstitutePID.h
 *  
 *  Interface for SubstitutePIDTool
 *
 * @see SubstitutePIDTool
 * @see SubstitutePID
 *
 *  @author Patrick Koppenburg
 *  @date   2011-12-07
 */
class ISubstitutePID : virtual public IAlgTool {
public: 
  typedef std::map<std::string,std::string>  SubstitutionMap ;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ISubstitutePID; }
  
  /// substitute PID for particles
  virtual StatusCode substitute(const LHCb::Particle::ConstVector& input,
                                LHCb::Particle::ConstVector& output) = 0 ;

  /// perform the actual substitution 
  virtual unsigned int substitute ( LHCb::Particle* p ) = 0;
  
  /// decode the substitution code
  virtual StatusCode decodeCode( SubstitutionMap newMap ) = 0;

protected:

private:

};
#endif // KERNEL_ISUBSTITUTEPID_H
