// $Id$
#ifndef CHECKVELOOVERLAP_H 
#define CHECKVELOOVERLAP_H 1

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

/** @class CheckVeloOverlap CheckVeloOverlap.h
 *  
 *  Tool to check if particles share velo hits
 *
 *  @author P. Koppenburg, based on code from Miriam Calvo
 *  @date   02/06/2005
 */
class CheckVeloOverlap : public GaudiTool,
                         virtual public ICheckOverlap {
public:
  /// Standard constructor
  CheckVeloOverlap( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  ///< Destructor
  virtual ~CheckVeloOverlap( ) { };
  //===========================================================================
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Argument: parts is a vector of pointers to particles.
  ///  Create an empty vector of pointers to protoparticles. 
  ///  Call the real check method.
  
  /// backward-compatible method
  bool foundOverlap( const LHCb::Particle::ConstVector & parts  );

  //===========================================================================
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Arguments: particle1 up to particle4 are pointers to particles.
  ///  Create a ParticleVector and fill it with the input particles.
  ///  Create an empty vector of pointers to protoparticles. 
  ///  Call the real check method.
  
  bool foundOverlap( const LHCb::Particle* );
  bool foundOverlap( const LHCb::Particle*,
                     const LHCb::Particle* );
  bool foundOverlap( const LHCb::Particle*,
                     const LHCb::Particle*,
                     const LHCb::Particle*);
  bool foundOverlap( const LHCb::Particle*,
                     const LHCb::Particle*,
                     const LHCb::Particle*,
                     const LHCb::Particle*);
  
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
  bool foundOverlap( const LHCb::Particle::ConstVector & parts,
                     std::vector<const LHCb::ProtoParticle* > & proto );
  
  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  StatusCode removeOverlap( LHCb::Particle::ConstVector& ) ;

  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  StatusCode removeOverlap( LHCb::Particle::Vector& ) ;

protected:
  
  StatusCode addOrigins( const LHCb::Particle::ConstVector&, 
                         std::vector<const LHCb::ProtoParticle*>&);

  /// Only look at protoparticles
  bool searchOverlap( std::vector<const LHCb::ProtoParticle* > & proto );
  bool shareVeloClusters( const LHCb::ProtoParticle*, 
                          const LHCb::ProtoParticle* );
  
private:
  
  //  int m_maxClusters ; /// Maximum acceptable number of common clusters
  double m_maxClusterFraction ; /// Maximum acceptable fraction of common cluster

}; // End of class header. 

// Implement inline methods:



// The real checking method is implemented in CheckVeloOverlap.cpp
//===========================================================================  
/// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
//===========================================================================
template <class T> std::vector<const T*> toStdVector( const SmartRefVector<T>& 
                                                      refvector );

#endif // CHECKVELOOVERLAP_H
