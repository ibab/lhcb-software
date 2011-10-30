// $Id$
#ifndef FINDCLONETOOL_H 
#define FINDCLONETOOL_H 1

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

namespace LHCb {  
  class IParticlePropertySvc ;
}

/** @class FindCloneTool FindCloneTool.h
 *  
 *  Tool to check if more than one particle originates 
 *  form the same set of protoparticles.
 *
 * FindCloneTool is a useful tool to be called after FilterDesktop
 * It takes a vector of particles and returns a vector of all unique particles
 * So-called clones are removed.
 * A clone is defined as a particle for which *all* protos are duplicates of another
 * in the same vector.
 * the RemoveClones algorithm uses this tool by default to filter the local
 * storage into just those particles which are unique
 *
 *  @author Rob Lambert
 *  @date   05/06/2008
 */
class FindCloneTool : public GaudiTool,
                     virtual public ICheckOverlap {

public:
  /// Standard constructor
  FindCloneTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  ///< Destructor
  virtual ~FindCloneTool( ) { };
  //===========================================================================
  /// Check for duplicate particles in a vector.
  /* Uses protoparticles, and demands they all must be duplicated.
   * Argument: parts is a vector of pointers to particles.
   *  Create an empty vector of pointers to protoparticles. 
   *  Call the real check method.
   */
  bool foundOverlap( const LHCb::Particle::ConstVector & parts ); 
  bool foundOverlap( const LHCb::Particle::Vector & parts );

  //===========================================================================
  /// Check for duplicate particles in a vector.
  /* Uses protoparticles, and demands they all must be duplicated.
   *  Create a ParticleVector and fill it with the input particles.
   *  Create an empty vector of pointers to protoparticles. 
   *  Call the real check method.
   */
  
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
  /// Dummy functions to satisfy inheritance
  //===========================================================================

  bool foundOverlap( const LHCb::Particle::ConstVector & parts, 
                     std::vector<const LHCb::ProtoParticle*> & proto); 
  
  bool foundOverlap( const LHCb::Particle::Vector & parts,
                     std::vector<const LHCb::ProtoParticle*> & proto);

  
  //===========================================================================
  /// Check for duplicate particles in a vector meaty function
  /**
   * calls searchOverlap which fills a vector of bools with 
   * whether or not they are clones
   */
  //===========================================================================
  bool foundOverlap( const LHCb::Particle::ConstVector & parts,
                     std::vector< std::vector<const LHCb::ProtoParticle*> > & proto,
                     std::vector<bool> & isclone);
  
  bool foundOverlap( const LHCb::Particle::Vector & parts,
                     std::vector< std::vector<const LHCb::ProtoParticle*> > & proto,
                     std::vector<bool> & isclone);

  /// Check and remove duplicate particles
  /** 
   * Check for duplicate use of all protoparticle to produce decay tree
   * of any particle in vector. Removes found particles from vector.
   */
  StatusCode removeOverlap( LHCb::Particle::ConstVector& ) ;

  ///Same as removeOverlap( LHCb::Particle::ConstVector& )
  StatusCode removeOverlap( LHCb::Particle::Vector& ) ;

protected:
  
  /// Fill a vector with the protos used by a given particle.
  StatusCode addOrigins( const LHCb::Particle*, 
                         std::vector<const LHCb::ProtoParticle*>&);

  /// Fill a vector of vectors of protos which are the protos used by some particles
  StatusCode addOrigins( const LHCb::Particle::Vector&, 
                         std::vector< std::vector<const LHCb::ProtoParticle*> >&);

  /// Fill a vector of vectors of protos which are the protos used by some particles
  StatusCode addOrigins( const LHCb::Particle::ConstVector&, 
                         std::vector< std::vector<const LHCb::ProtoParticle*> >&);

  /// The actual search function, which fills a vector of bools with whether this particle is a clone.
  bool searchOverlap( std::vector< std::vector<const LHCb::ProtoParticle*> > & proto, std::vector<bool> & isclone );
  
  
private:

  /// Accessor for ParticlePropertySvc
  LHCb::IParticlePropertySvc* m_ppSvc ;
  
}; // End of class header. 

#endif // FINDCLONETOOL_H
