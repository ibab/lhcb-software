// $Id: IPhysDesktop.h,v 1.1 2002-03-27 20:27:46 gcorti Exp $
#ifndef DAVINCITOOLS_IPHYSDESKTOP_H 
#define DAVINCITOOLS_IPHYSDESKTOP_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/IDataProviderSvc.h"

// Forward declarations
class StatusCode;
class Vertex;
class Particle;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IPhysDesktop("IPhysDesktop", 1, 0);


/** @class IPhysDesktop IPhysDesktop.h
 *  
 *  This class creates a particle and a vertex keyed containers
 *  @author Sandra Amato
 *  @date   18/02/2002
 */
class IPhysDesktop : virtual public IAlgTool {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPhysDesktop; }

  // Fill the particle and vertex containers
  virtual StatusCode getInput() = 0;

  // Retrieve the particles containers
  virtual const ParticleVector& particles() = 0;

  // Retrieve the vertex container
  virtual const VertexVector& vertices() = 0;

  // Add the particles  to the DeskTop
  virtual Particle* createParticle( Particle* input=0 ) = 0;

  // Add the vertices  to the DeskTop
  virtual Vertex* createVertex( Vertex* input=0 ) = 0;

  // Save the particles to the TES
  virtual StatusCode saveDesktop() = 0;
 
  // Save a vector of Particles
  // If a particle is composite its descendents are also saved
  virtual StatusCode saveTrees( ParticleVector& ) = 0;
  
  // Save a vector of vertices. all descendents of a vertex are also saved
  virtual StatusCode saveTrees( VertexVector& ) = 0;

  //Save all Particles with a given particleID code
  virtual StatusCode saveTrees( int pid ) = 0;

  //Save all Particles with a given particleID
  // virtual StatusCode saveTrees( const ParticleID& pid ) = 0;

  //Save all Particles with a given particleID, and a minimum ConfLevel
  //  virtual StatusCode saveTrees( int pid, double cl ) = 0;


protected:

private:

};
#endif // DAVINCITOOLS_IPHYSDESKTOP_H


