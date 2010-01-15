// $Id: IPhysDesktop.h,v 1.12 2010-01-15 08:22:02 jpalac Exp $
#ifndef DAVINCIKERNEL_IPHYSDESKTOP_H 
#define DAVINCIKERNEL_IPHYSDESKTOP_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
// Forward declarations
class StatusCode;

/** @class IPhysDesktop IPhysDesktop.h Kernel/IPhysDesktop.h
 *
 *  Interface for the Desktop upon which the user keeps his particles and vertices.
 *
 *  This tool hides all the I/O from the user and takes care of all ownership.
 *  The user should never have to type new in a DVAlgorithm using the PhysDesktop.
 *
 *  The PhysDesktop is interfaced by desktop() in DVAlgorithm
 *
 *  Example of use
 *
 *  @code
 * 
 *  StatusCode TutorialAlgorithm::execute() {
 *    LHCb::Particle::ConstVector muons = desktop()->particles(); // get particles
 *    sc = particleFilter()->filterNegative(muons,MuMinus);
 *    if (sc) sc = particleFilter()->filterPositive(muons,MuPlus);
 *    if (!sc) { 
 *      err() << "Error while filtering" << endmsg ;
 *      return sc ;
 *    }
 *    for ( LHCb::Particle::ConstVector::const_iterator imp =  MuPlus.begin() ;
 *          imp != MuPlus.end() ; ++imp ){
 *      for ( LHCb::Particle::ConstVector::const_iterator imm =  MuMinus.begin() ;
 *            imm != MuMinus.end() ; ++imm ){
 *        LHCb::Vertex MuMuVertex;
 *        LHCb::Particle Jpsi(m_jPsiID);
 *        StatusCode scFit = vertexFitter()->fit(*(*imp),*(*imm),Jpsi,MuMuVertex);
 *        if (!scFit) continue ;
 *        if ( MuMuVertex.chi2() > m_jPsiChi2 ) continue ; // chi2 cut
 *        desktop()->keep(&Jpsi);  // keep in Desktop
 *        setFilterPassed(true);
 *      }
 *    }
 *    sc = desktop()->saveDesktop() ;  // save the desktop to the TES
 *    return sc ;
 *  }
 *  @endcode
 *
 *  The keep() methods pass the ownership to the desktop and return a pointer to
 *  the new particle. 
 *
 *  The saveDesktop() and saveTrees(...) methods then save to the TES.
 *
 *  @author Sandra Amato
 *  @date   18/02/2002
 */
class GAUDI_API IPhysDesktop : virtual public IAlgTool {

public:

  DeclareInterfaceID(IPhysDesktop, 2, 0);

  /// Load Input particles and vertices (in local data) from various input
  /// locations filled with previous processings and  
  /// create new Particles starting from reconstruction objects as
  /// requested in jobOptions. The creation of new Particles if delegated
  /// to ParticleMakers. 
  /// Only DVAlgorithm::sysExecute() should call this function.
  virtual StatusCode getEventInput() = 0;

  /// set OutputLocation (from DVAlgorithm)
  virtual void setOutputLocation(const std::string&) = 0 ;

  /// set InputLocations (from DVAlgorithm)
  virtual StatusCode setInputLocations(const std::vector<std::string>&) = 0 ;

  /// set Particle->PV relations input TES locations (from DVAlgorithm)
  virtual StatusCode setP2PVInputLocations(const std::vector<std::string>&) = 0 ;

  /// 
  virtual void setWriteP2PV(const bool&) = 0;

  ///
  virtual void setUsingP2PV(const bool&) = 0;

  /// Retrieve the local particle container
  virtual const LHCb::Particle::ConstVector& particles() const = 0;

  /// Retrieve the local secondary vertex container
  virtual const LHCb::Vertex::ConstVector& secondaryVertices() const = 0;

  /// retrieve the Particle->Primary vertex relations
  virtual const Particle2Vertex::LightTable& Particle2VertexRelations() const = 0;

  /// retrieve the Particle->Primary vertex relations
  virtual Particle2Vertex::LightTable& Particle2VertexRelations() = 0;

  virtual Particle2Vertex::Map& Particle2VertexMap() = 0;

  virtual const Particle2Vertex::Map& Particle2VertexMap() const = 0;
  

  /// Store relations over-writing previously existing ones
  virtual void overWriteRelations(Particle2Vertex::Table::Range::const_iterator begin,
                                  Particle2Vertex::Table::Range::const_iterator end) = 0;

  /// Keep for future use: Register the new particles in the Desktop, 
  /// pass ownership, return pointer to new particle
  virtual const LHCb::Particle* keep( const LHCb::Particle* input ) = 0;

  /// Keep for future use: Register the new vertices in the Desktop, 
  /// pass ownership, return pointer to new vertex
  virtual const LHCb::Vertex* keep( const LHCb::Vertex* input ) = 0;

  /// Keep for future use: Register re-fitted primary vertices in the Desktop, 
  /// pass ownership, return pointer to new vertex. Vertices will only
  /// be stored in the TES if they are related to a particle being stored
  /// there.
  virtual const LHCb::RecVertex* keep( const LHCb::RecVertex* input ) = 0;


  /// Save particles, vertices and particle->vertices relations to the TES
  virtual StatusCode saveDesktop() const = 0;

  /// Save a vector of Particles
  /// If a particle is composite its descendents are also saved
  virtual StatusCode saveTrees( const LHCb::Particle::ConstVector& ) const = 0;

  /// Save all Particles with a given particleID code
  virtual StatusCode saveTrees( int pid ) const = 0;

  /// Clone all particles given by a list. This duplicates information 
  /// on the TES and should be used only when necessary. (Used by Filters)
  virtual StatusCode cloneTrees( const LHCb::Particle::ConstVector& ) = 0;

  /// Get output location
  virtual const std::string& getOutputLocation() const = 0 ;

  /// Make sure the PhysDesktop has written out the container
  virtual StatusCode writeEmptyContainerIfNeeded() = 0 ;

  /// Get the vertex with the highest weight in the association
  /// between LHCb::Particle and LHCb::VertexBase
  /// Deprecated!
  virtual const LHCb::VertexBase* relatedVertex(const LHCb::Particle* part) const = 0;


  /// Establish a relation between an LHCb::Particle and an LHCb::VertexBase
  virtual void relate(const LHCb::Particle* part, 
                      const LHCb::VertexBase* vert ) = 0;

  /// Obtain a range of weighted LHCb::VertexBase related to an LHCb::Particle
  virtual Particle2Vertex::Table::Range particle2Vertices(const LHCb::Particle* part ) const = 0;

  /// Clean desktop
  virtual StatusCode cleanDesktop() = 0;
  
};
#endif // DAVINCIKERNEL_IPHYSDESKTOP_H


