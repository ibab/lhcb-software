// $Id: PhysDesktop.h,v 1.4 2004-08-12 12:33:53 pkoppenb Exp $
#ifndef PHYSDESKTOP_H 
#define PHYSDESKTOP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiTools/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
// from DaVinci
#include "DaVinciTools/IPhysDesktop.h"

// Forward declarations
class IDataProviderSvc;
class IParticleMaker;

/** @class PhysDesktop PhysDesktop.h DaVinciTools/PhysDesktop.h
 *  A Desktop upon which the user keeps his particles and vertices
 *  @author Sandra Amato and Gloria Corti
 *  @date   18/02/2002
 *
 */
class PhysDesktop : public GaudiTool,
                    virtual public IPhysDesktop,
                    virtual public IIncidentListener {
  
public:
  
  /// Standard constructor
  PhysDesktop( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~PhysDesktop(){ } ///< Destructor
  
  /// IListener interface for cleaning at end of event
  virtual void handle(const Incident&);
  
  /// Initialize method 
  StatusCode initialize();
  
  /// Finalize method 
  StatusCode finalize();
  
  /// Load Input particles and vertices (in local data) from various input
  /// locations filled with previous processings and  
  /// create new Particles starting from reconstruction objects as
  /// requested in jobOptions. The creation of new Particles if delegated
  /// to ParticleMakers. Only DVAlgorithm::sysExecute() should call this function.
  StatusCode getEventInput();
  
  // Obsolete version to permit backwards compatibility. To be removed in some months
  StatusCode getInput();
  
  /// Retrieve the particles container 
  const ParticleVector& particles();
  
  /// Retrieve the vertex container 
  const VertexVector& vertices();
  
  /// Add a particle to the DeskTop to which it belongs
  Particle* createParticle( Particle* partToSave );
  
  /// Add a vertex to the DeskTop
  Vertex* createVertex( Vertex* vertToSave );
  
  /// Save all particles and vertices in the DeskTop to the TES
  StatusCode saveDesktop();
  
  /// Save a vector of Particles in TES.
  /// If a particle is composite its descendents are also saved, but the
  /// rest of the particles in the Desktop are not saved.
  StatusCode saveTrees( ParticleVector& );
  
  /// Save all Particles and its descendent with a given particleID code,
  /// the rest of the particles in the Desktop are not saved.
  StatusCode saveTrees(int partid);
  
  /// Save a vector of vertices. all descendents of a vertex are also saved
  StatusCode saveTrees( VertexVector& );
  
  /// Save all particles and vertices specified in the lists, this is
  /// used internally and no tree structure is propagated.
  StatusCode saveDesktop( ParticleVector& pToSave, 
                          VertexVector& vToSave);
  
  /// Find all particles & vertices in a tree. 
  void findAllTree( Particle* part, 
                    ParticleVector& parts, VertexVector& verts );
  
  /// Find all particles & vertices in a tree. 
  void findAllTree( Vertex* vert, 
                    ParticleVector& parts, VertexVector& verts );
  
  /// Internal method to clean the local data
  StatusCode cleanDesktop();
  
  /// The standard event data service
  IDataProviderSvc* eventSvc() const;

  //hrp
  // Set output location
  void imposeOutputLocation(std::string outputLocationString);

protected:
  
  
private:
  
  /// TES pathname for Primary Vertices 
  std::string m_primVtxLocn;
  /// TES pathname for Input Particles & Vertices from previous processing
  std::vector<std::string> m_inputLocn;
  /// TES pathname for Output Particles & Vertices
  std::string m_outputLocn;
  
  ParticleVector m_parts;          ///< Local Container of particles
  VertexVector m_verts;            ///< Local Container of vertices

  IDataProviderSvc* m_EDS;         ///< Event data service
  IParticleMaker* m_pMaker;        ///< Reference to Particle maker tool

  /// Identify specific type of particle maker requested (Property)
  std::string m_pMakerType;  

  // Variable needed to ensure that calling getInput
  // by user (which is now not needed anymore) will give only one
  // message warning, not one per event
  // To be removed from code when calling by user not tolerated anymore
  bool m_getInputWarned;

    
};
#endif // PHYSDESKTOP_H
