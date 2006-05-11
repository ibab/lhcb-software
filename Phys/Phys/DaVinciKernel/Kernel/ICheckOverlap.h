// $Id: ICheckOverlap.h,v 1.9 2006-05-11 13:34:00 jpalac Exp $
#ifndef DAVINCIKERNEL_ICHECKOVERLAP_H 
#define DAVINCIKERNEL_ICHECKOVERLAP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ICheckOverlap("ICheckOverlap", 2, 1);

/** @class ICheckOverlap ICheckOverlap.h
 *  
 *  Tool to check if more than one particle originate 
 *  from the same source 
 *
 *  @author Jose Helder Lopes, redesigned by P. Koppenburg
 *  @date   28/06/2002
 */

class ICheckOverlap : virtual public IAlgTool {
  
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICheckOverlap; }
  
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Argument: parts is a vector of pointers to particles.  
  virtual bool foundOverlap( const LHCb::Particle::ConstVector & parts  ) = 0;

  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle* ) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*, 
                             const LHCb::Particle* ) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*, 
                             const LHCb::Particle*, 
                             const LHCb::Particle*) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*,
                             const LHCb::Particle*,
                             const LHCb::Particle*,
                             const LHCb::Particle*) = 0;

  /// Check for duplicate use of a protoparticle to produce particles.
  /// Continue a previous check using the contents of the vector of pointers 
  /// to protoparticles.(Most intended for internal use by the other methods).
  /// Arguments: parts is a vector of pointer to particles. 
  ///            proto is a vector of pointers to protoparticles.
  virtual bool foundOverlap( const LHCb::Particle::ConstVector & parts,
                             std::vector<const LHCb::ProtoParticle* > & proto ) = 0 ;
  
  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( LHCb::Particle::ConstVector& ) = 0;

  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( LHCb::Particle::Vector& ) = 0;

protected:
  
private:
  
};
#endif // DAVINCIKERNEL_ICHECKOVERLAP_H
