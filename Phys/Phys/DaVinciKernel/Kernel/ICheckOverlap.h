// $Id: ICheckOverlap.h,v 1.4 2005-02-24 09:13:16 pkoppenb Exp $
#ifndef DAVINCIKERNEL_ICHECKOVERLAP_H 
#define DAVINCIKERNEL_ICHECKOVERLAP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
typedef std::vector<const Particle*> ConstParticleVector ;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ICheckOverlap("ICheckOverlap", 2, 1);

/** @class ICheckOverlap ICheckOverlap.h
 *  
 *  Tool to check if more than one particle originate 
 *  from the same protoparticle.
 *
 *  @author Jose Helder Lopes
 *  @date   28/06/2002
 */

class ICheckOverlap : virtual public IAlgTool {
  
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICheckOverlap; }
  
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Argument: parts is a vector of pointers to particles.  
  virtual bool ICheckOverlap::foundOverlap( ConstParticleVector & parts  ) = 0;
  
  /// backward-compatible methods
  virtual bool ICheckOverlap::foundOverlap( ParticleVector & parts  ) = 0;
  /// backward-compatible methods
  virtual bool ICheckOverlap::foundOverlap( const ParticleVector & parts  ) = 0; 

  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const Particle* ) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const Particle*, const Particle* ) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const Particle*, const Particle*, const Particle*) = 0;
  
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const Particle*,const Particle*,
                             const Particle*,const Particle*) = 0;

  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( ParticleVector& ) = 0;

  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( ConstParticleVector& ) = 0;

protected:
  
private:
  
};
#endif // DAVINCIKERNEL_ICHECKOVERLAP_H
