// $Id: CheckOverlap.h,v 1.4 2005-02-21 08:56:42 pkoppenb Exp $
#ifndef CHECKOVERLAP_H 
#define CHECKOVERLAP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// From PhysEvent
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

// from DaVinci
#include "Kernel/ICheckOverlap.h"

/** @class CheckOverlap CheckOverlap.h
 *  
 *  Tool to check if more than one particle originate 
 *  form the same protoparticle.
 *
 *  @author Jose' Helder Lopes
 *  @date   28/06/2002
 */
class CheckOverlap : public GaudiTool,
                     virtual public ICheckOverlap {
public:
  /// Standard constructor
  CheckOverlap( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  ///< Destructor
  virtual ~CheckOverlap( ) { };
  //===========================================================================
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Argument: parts is a vector of pointers to particles.
  ///  Create an empty vector of pointers to protoparticles. 
  ///  Call the real check method.
  bool foundOverlap( ConstParticleVector & parts );
  
  /// backward-compatible method
  bool foundOverlap( ParticleVector & parts  );
  /// backward-compatible method
  bool foundOverlap( const ParticleVector & parts  );

  //===========================================================================
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Arguments: particle1 up to particle4 are pointers to particles.
  ///  Create a ParticleVector and fill it with the input particles.
  ///  Create an empty vector of pointers to protoparticles. 
  ///  Call the real check method.
  
  bool foundOverlap( const Particle* particle1, 
                     const Particle* particle2 );
  
  bool foundOverlap( const Particle* particle1, 
                     const Particle* particle2, 
                     const Particle* particle3);
  
  bool foundOverlap( const Particle* particle1,
                     const Particle* particle2,
                     const Particle* particle3,
                     const Particle* particle4 );
  
  //===========================================================================
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Continue a previous check using the contents of the vector of pointers 
  /// to protoparticles.(Most intended for internal use by the other methods).
  /// Arguments: parts is a vector of pointer to particles. 
  ///            proto is a vector of pointers to protoparticles.
  //  Real checking method. Scans the tree headed by parts. Add each
  //  protoparticle to proto if it is a new one. Returns true otherwise.
  //  If called directly by the user, it will continue a previous check, 
  //  not start a new one!
  //===========================================================================
  bool foundOverlap( ConstParticleVector & parts,
                     std::vector<const ContainedObject* > & proto );
  
protected:
  
private:
  
}; // End of class header. 

// Implement inline methods:



// The real checking method is implemented in CheckOverlap.cpp
//===========================================================================  
/// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
//===========================================================================
template <class T> std::vector<const T*> toStdVector( const SmartRefVector<T>& 
                                                      refvector );

#endif // CHECKOVERLAP_H
