// $Id: IPhysDesktop.h,v 1.3 2005-08-09 14:11:59 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IPHYSDESKTOP_H 
#define DAVINCIKERNEL_IPHYSDESKTOP_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class StatusCode;
class Vertex;
class PrimVertex;
class Particle;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IPhysDesktop("IPhysDesktop", 1, 2);


/** @class IPhysDesktop IPhysDesktop.h Kernel/IPhysDesktop.h
 *  Interface for the Desk Top
 *  upon which the user keeps his particles and vertices
 *  @author Sandra Amato
 *  @date   18/02/2002
 */
class IPhysDesktop : virtual public IAlgTool {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPhysDesktop; }

  /// Fill the particle and vertex containers. Called by DVAlgorithm::sysExecute()
  virtual StatusCode getEventInput() = 0;

  /// Retrieve the particles containers
  virtual const ParticleVector& particles() = 0;

  /// Retrieve the vertex container : primaries and secondaries!
  virtual const VertexVector& vertices() = 0;

  /// Retrieve the PV from vertex container
  virtual const VertexVector& primaryVertices() = 0;

  /// Retrieve the PV as primary vertex. Needs a different name
  virtual const std::vector<const PrimVertex*> primaries() = 0;

  /// Retrieve the secondary vertices
  virtual const VertexVector& secondaryVertices() = 0;

  /// Add the particles  to the Desktop
  virtual Particle* createParticle( Particle* input=0 ) = 0;

  /// Add the vertices  to the Desktop
  virtual Vertex* createVertex( Vertex* input=0 ) = 0;

  /// Save the particles to the TES
  virtual StatusCode saveDesktop() = 0;
 
  /// Save the particles to the TES
  virtual StatusCode saveDesktop(ParticleVector&,VertexVector& ) = 0;
 
  /// Save a vector of Particles
  /// If a particle is composite its descendents are also saved
  virtual StatusCode saveTrees( ParticleVector& ) = 0;
  
  /// Save a vector of vertices. all descendents of a vertex are also saved
  virtual StatusCode saveTrees( VertexVector& ) = 0;

  /// Save all Particles with a given particleID code
  virtual StatusCode saveTrees( int pid ) = 0;

  /// Clone all particles given by a list. This duplicates information on the TES
  /// and should be used only when necessary.
  virtual StatusCode cloneTrees( ParticleVector& ) = 0;

  //Save all Particles with a given particleID
  // virtual StatusCode saveTrees( const ParticleID& pid ) = 0;

  //Save all Particles with a given particleID, and a minimum ConfLevel
  //  virtual StatusCode saveTrees( int pid, double cl ) = 0;

  /// Impose output location
  virtual void imposeOutputLocation(std::string outputLocationString) = 0;

  /// Get output location
  virtual std::string getOutputLocation() = 0 ;
  
  /// Find all particles & vertices in a tree. 
  // virtual void findAllTree( Particle*, ParticleVector&, VertexVector& ) = 0;
  // virtual void findAllTree( Vertex*, ParticleVector&, VertexVector& )= 0;
  

protected:

private:

};
#endif // DAVINCIKERNEL_IPHYSDESKTOP_H


