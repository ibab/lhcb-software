// $Id: PhysDesktop.h,v 1.1 2002-03-27 20:35:01 gcorti Exp $
#ifndef COMPONENT_PHYSDESKTOP_H 
#define COMPONENT_PHYSDESKTOP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "DaVinciTools/IPhysDesktop.h"
//#include "GaudiKernel/IDataProviderSvc.h"

// Forward declarations
class IDataProviderSvc;
class IParticlePropertySvc;

/** @class PhysDesktop PhysDesktop.h component/PhysDesktop.h
 *  Base class for all PhysDesktop tools
 *
 *  @author Sandra Amato
 *  @date   18/02/2002

 */
class PhysDesktop : public AlgTool,
                    virtual public IPhysDesktop {

public:

  /// Standard constructor
  PhysDesktop( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~PhysDesktop(){ } ///< Destructor

  /// IListener interface implementation
  /// cannot be used virtual void handle(const Incident&);

  /// Initialize method (IPhysDesktop)
  virtual StatusCode initialize();

  /// Finalize method (IPhysDesktop)
  virtual StatusCode finalize();

  /// getInput to be moved to concrete PhysDesktop, when we have
  /// the base class fill the particle and vertex containers
  virtual StatusCode getInput();

  /// Retrieve the particles container (IPhysDesktop)
  const ParticleVector& particles();

  /// Retrieve the vertex container (IPhysDesktop)
  const VertexVector& vertices();

  /// Add a particle to the DeskTop (IPhysDesktop)
  Particle* createParticle( Particle* partToSave );

  /// Add a vertex to the DeskTop (IPhysDesktop)
  Vertex* createVertex( Vertex* vertToSave );

  /// Save all particles and vertices in the DeskTop to the TES (IPhysDesktop)
  StatusCode saveDesktop();
  
  /// Save a vector of Particles.
  /// If a particle is composite its descendents are also saved
  StatusCode saveTrees( ParticleVector& );
  
  /// Save all Particles and its descendent with a given particleID code
  StatusCode saveTrees(int partid);

  /// Save a vector of vertices. all descendents of a vertex are also saved
  StatusCode saveTrees( VertexVector& );

  /// Save all particles and vertices specified in the lists
  StatusCode saveDesktop( ParticleVector& pToSave, 
                          VertexVector& vToSave);
  
  /// Find all particles & vertices in a tree 
  void findAllTree( Particle* part, 
                    ParticleVector& parts, VertexVector& verts );

  /// Find all particles & vertices in a tree 
  void findAllTree( Vertex* vert, 
                    ParticleVector& parts, VertexVector& verts );

  /// Internal method to clean before loading new event
  StatusCode cleanDesktop();

  /// The standard event data service
  IDataProviderSvc* eventSvc() const;

  /// The standar particle property service
  IParticlePropertySvc* ppSvc() const;

protected:
  

private:

  std::vector<std::string>  m_particleNames; ///< Name of Particles to populate
                                             ///< PhysDesktop
  std::vector<int> m_ids;    ///< PDGid of Particles to populate PhysDesktop
  std::vector<double> m_confLevels;  ///< Cut Value on the pid Hypothesys
                                     /// Confidence Level

  std::string m_primVtxLocn; ///< TES pathname for Primary Vertices  
  std::string m_protoLocn;   ///< TES pathname for ProtoParticles
  std::vector<std::string> m_inputLocn;   ///< TES pathname for Input Particles & Vertices
  std::string m_outputLocn;  ///< TES pathname for Output Particles & Vertices

  IDataProviderSvc* m_EDS;           ///< Event data service  
  IParticlePropertySvc* m_ppSvc;     ///< Pointer to Particle Property Service

  ParticleVector m_parts;            ///< Initial Container of particles
  VertexVector m_verts;              ///< Initial Container of vertex

};
#endif // COMPONENT_PHYSDESKTOP_H
