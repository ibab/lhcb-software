// $Id: IPhysDesktop.h,v 1.9 2006-03-16 14:54:28 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IPHYSDESKTOP_H 
#define DAVINCIKERNEL_IPHYSDESKTOP_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class StatusCode;
#include "Event/PrimVertex.h"
#include "Event/Particle.h"

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IPhysDesktop("IPhysDesktop", 1, 2);


/** @class IPhysDesktop IPhysDesktop.h Kernel/IPhysDesktop.h
 *  Interface for the Desk Top
 *  upon which the user keeps his particles and vertices
 *  
 *  @todo Method to create a table associating Particles and the PV that have been used
 *
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
  virtual const LHCb::Particle::ConstVector particles() = 0;

  /// Retrieve the PV from vertex container
  virtual const LHCb::PrimVertex::ConstVector& primaryVertices() = 0;

  /// Retrieve the secondary vertices
  virtual const LHCb::Vertex::ConstVector secondaryVertices() = 0;

  /// Add the particles  to the Desktop
  virtual LHCb::Particle* createParticle( LHCb::Particle* input=0 ) = 0;

  /// Add the vertices  to the Desktop
  virtual LHCb::Vertex* createVertex( LHCb::Vertex* input=0 ) = 0;

  /// Save the particles to the TES
  virtual StatusCode saveDesktop() = 0;
 
  /// Save the particles to the TES (used by HLT)
  virtual StatusCode saveDesktop(LHCb::Particle::ConstVector&,
                                 LHCb::Vertex::ConstVector& ) = 0;
 
  /// Save a vector of Particles
  /// If a particle is composite its descendents are also saved
  virtual StatusCode saveTrees( LHCb::Particle::ConstVector& ) = 0;

  /// Save all Particles with a given particleID code
  virtual StatusCode saveTrees( int pid ) = 0;

  /// Clone all particles given by a list. This duplicates information on the TES
  /// and should be used only when necessary. (Used by Filters)
  virtual StatusCode cloneTrees( LHCb::Particle::ConstVector& ) = 0;

  /// Impose output location
  virtual void imposeOutputLocation(std::string outputLocationString) = 0;

  /// Get output location
  virtual std::string getOutputLocation() = 0 ;
  
  /// Find all particles & vertices in a tree. 
  // virtual void findAllTree( LHCb::Particle*, LHCb::Particle::ConstVector&, LHCb::Vertex::ConstVector& ) = 0;
  // virtual void findAllTree( LHCb::Vertex*, LHCb::Particle::ConstVector&, LHCb::Vertex::ConstVector& )= 0;

  /// Make sure the PhysDesktop has written out the container
  virtual StatusCode writeEmptyContainerIfNeeded() = 0 ;
  
protected:

private:

};
#endif // DAVINCIKERNEL_IPHYSDESKTOP_H


