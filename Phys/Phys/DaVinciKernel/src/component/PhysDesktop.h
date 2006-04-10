#ifndef PHYSDESKTOP_H 
#define PHYSDESKTOP_H 1

// Include files
// from STL
#include <string>
#include <set>

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/GaudiTool.h"
// from DaVinci
#include "Kernel/IPhysDesktop.h"

// Forward declarations
class IDataProviderSvc;
class IParticleMaker;
class IOnOffline;

/** @class PhysDesktop PhysDesktop.h Kernel/PhysDesktop.h
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

  /// Retrieve the particles container 
  const LHCb::Particle::ConstVector particles();
  
  /// Retrieve the PV from vertex container
  const LHCb::PrimVertex::ConstVector& primaryVertices() ;

  /// Retrieve the secondary vertices
  const LHCb::Vertex::ConstVector secondaryVertices() ;

  /// Add a particle to the DeskTop to which it belongs
  const LHCb::Particle* save( LHCb::Particle* partToSave );
  
  /// Add a vertex to the DeskTop
  const LHCb::Vertex* save( LHCb::Vertex* vertToSave );
  
  /// Save all particles and vertices in the DeskTop to the TES
  StatusCode saveDesktop();
  
  /// Save a vector of Particles in TES.
  /// If a particle is composite its descendents are also saved, but the
  /// rest of the particles in the Desktop are not saved.
  StatusCode saveTrees( LHCb::Particle::ConstVector& );
  
  //Clone all particles given by a list. This duplicates information on the TES
  // and should be used only when necessary.
  StatusCode cloneTrees( LHCb::Particle::ConstVector& );
  
  /// Save all Particles and its descendent with a given particleID code,
  /// the rest of the particles in the Desktop are not saved.
  StatusCode saveTrees(int partid);
  
  /// Save all particles and vertices specified in the lists, this is
  /// used internally and no tree structure is propagated.
  StatusCode saveDesktop( LHCb::Particle::ConstVector& pToSave, 
                          LHCb::Vertex::ConstVector& vToSave);
  
  /// Find all particles & vertices in a tree. 
  void findAllTree( const LHCb::Particle* part, 
                    LHCb::Particle::ConstVector& parts, 
                    LHCb::Vertex::ConstVector& verts );
  
  /// Internal method to clean the local data
  StatusCode cleanDesktop();
  
  //hrp
  // Set output location
  void imposeOutputLocation(std::string outputLocationString);

  std::string getOutputLocation(){ return m_outputLocn ;};

  /// Make sure the PhysDesktop has written out the container
  StatusCode writeEmptyContainerIfNeeded() ;
  

private: // methods

  StatusCode makeParticles();      ///< Make particles
  StatusCode getPrimaryVertices(); ///< get PV
  StatusCode getParticles();       ///< get Particles
  
  ///  Is in Desktop
  inline bool inDesktop(const LHCb::Particle* P){
    verbose() << "LHCb::Particle " << P << " is in desktop " << (m_partsInTES.find(P)!=m_partsInTES.end()) << endmsg ;
    return (m_partsInTES.find(P)!=m_partsInTES.end()) ;};
  ///  Is in Desktop
  inline bool inDesktop(const LHCb::Vertex* V){
    verbose() << "Vertex " << V << " is in desktop " << (m_vertsInTES.find(V)!=m_vertsInTES.end()) << endmsg ;
    return (m_vertsInTES.find(V)!=m_vertsInTES.end());};
  inline void setInDesktop(const LHCb::Particle* P){m_partsInTES.insert(P); 
  verbose() << "Inserted LHCb::Particle " << P << " (" << m_partsInTES.size() << ")" << endmsg ; return ;}; ///< Add to Desktop
  inline void setInDesktop(const LHCb::Vertex* V){m_vertsInTES.insert(V); 
  verbose() << "Inserted Vertex " << V << " (" << m_vertsInTES.size() << ")" << endmsg ; 
  return ;}; ///< Add to Desktop
  

private: // data

  /// TES pathname for Primary Vertices 
  std::string m_primVtxLocn;
  /// TES pathname for Input Particles & Vertices from previous processing
  std::vector<std::string> m_inputLocn;
  /// TES pathname for Output Particles & Vertices
  std::string m_outputLocn;
  
  LHCb::Particle::ConstVector m_parts;          ///< Local Container of particles
  LHCb::Vertex::ConstVector m_secVerts;         ///< Local Container of secondary vertices
  LHCb::PrimVertex::ConstVector m_primVerts;    ///< Local Container of primary vertices
  
  std::set<const LHCb::Particle*> m_partsInTES ; ///< particles that do not need to be deleted 
  std::set<const LHCb::Vertex*>   m_vertsInTES ; ///< vertices  that do not need to be deleted 

  IParticleMaker* m_pMaker;        ///< Reference to LHCb::Particle maker tool

  /// Identify specific type of particle maker requested (Property)
  std::string m_pMakerType;  

  /// turn on location warnings
  bool m_locationWarned ;
  
  IOnOffline* m_OnOffline ;   ///< locate PV

};

#endif // PHYSDESKTOP_H
