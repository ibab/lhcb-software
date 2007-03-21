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
class IRelatedPVFinder ;

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
  const LHCb::Particle::ConstVector& particles() const;
  
  /// Retrieve the PV from vertex container
  const LHCb::RecVertex::ConstVector& primaryVertices();

  /// Retrieve the secondary vertices
  const LHCb::Vertex::ConstVector& secondaryVertices() const;

  /// Add a particle to the DeskTop to which it belongs
  const LHCb::Particle* save( const LHCb::Particle* partToSave );
  
  /// Add a vertex to the DeskTop
  const LHCb::Vertex* save( const LHCb::Vertex* vertToSave );
  
  /// Save all particles and vertices in the DeskTop to the TES
  StatusCode saveDesktop() const;
  
  /// Save a vector of Particles in TES.
  /// If a particle is composite its descendents are also saved, but the
  /// rest of the particles in the Desktop are not saved.
  StatusCode saveTrees( const LHCb::Particle::ConstVector& ) const;
  
  //Clone all particles given by a list. This duplicates information on the TES
  // and should be used only when necessary.
  StatusCode cloneTrees( const LHCb::Particle::ConstVector& );
  
  /// Save all Particles and its descendent with a given particleID code,
  /// the rest of the particles in the Desktop are not saved.
  StatusCode saveTrees(int partid) const;

public:
  /// Find all particles & vertices in a tree. 
  void findAllTree( const LHCb::Particle* part, 
                    LHCb::Particle::ConstVector& parts, 
                    LHCb::Vertex::ConstVector& verts ) const;
  
  /// Internal method to clean the local data
  StatusCode cleanDesktop();
  
  //hrp
  // Set output location
  void imposeOutputLocation(const std::string& outputLocationString);

  std::string getOutputLocation() const { return m_outputLocn ;}

  /// Make sure the PhysDesktop has written out the container
  StatusCode writeEmptyContainerIfNeeded() ;

  /// Get the vertex with the highest weight in the association
  /// between LHCb::Particle and LHCb::VertexBase
  virtual const LHCb::VertexBase* relatedVertex(const LHCb::Particle* part);
  
  /// Establish a relation between an LHCb::Particle and an LHCb::VertexBase
  virtual void relate(const LHCb::Particle*   part, 
                      const LHCb::VertexBase* vert,
                      const double weight=1.);

  /// Obtain the weight relating an LHCb::Particle and an LHCb::VertexBase
  virtual double weight(const LHCb::Particle*   part, 
                        const LHCb::VertexBase* vert ) const;
  
  /// Obtain a range of weighted LHCb::VertexBase related to an LHCb::Particle
  virtual Particle2Vertex::Range particle2Vertices(const LHCb::Particle* part ) const;

  /// Obtain a copy of the current 1D relations table 
  /// relating LHCb::Particles to LHCb::VertexBases
  virtual const Particle2Vertex::Table particle2VertexTable() const;

private: // methods

private:
  /// Save all particles and vertices specified in the lists, this is
  /// used internally and no tree structure is propagated.
  inline StatusCode saveDesktop( const LHCb::Particle::ConstVector& pToSave, 
                                 const LHCb::Vertex::ConstVector& vToSave) const 
  {
    verbose() << "Save specified particles and vertices " << endmsg;
    saveParticles(pToSave);
    saveVertices(vToSave);
    return StatusCode::SUCCESS;
  }
  
  //===========================================================================
  /// Save all Particles in the Desktop to the TES
  //===========================================================================
  void saveParticles(const LHCb::Particle::ConstVector& pToSave) const;
  //===========================================================================
  /// Save all Vertices in the Desktop to the TES
  //===========================================================================
  void saveVertices(const LHCb::Vertex::ConstVector& vToSave) const;
  //===========================================================================
  /// Save the Particle->Vertex relations table in the Desktop to the TES
  //===========================================================================
  void saveTable(const LHCb::Particle::ConstVector& pToSave) const;
  //===========================================================================
  StatusCode makeParticles();      ///< Make particles
  //===========================================================================
  StatusCode getPrimaryVertices(); ///< get PV
  //===========================================================================
  StatusCode getParticles();       ///< get Particles
  //===========================================================================
  StatusCode getRelations();       ///< get Relation table
  //===========================================================================
  ///  Is in Desktop
  inline bool inDesktop(const LHCb::Particle* P) const {
    verbose() << "LHCb::Particle " << P << " is in desktop " 
              << (m_partsInTES.find(P)!=m_partsInTES.end()) << endmsg ;
    return (m_partsInTES.find(P)!=m_partsInTES.end()) ;
  }
  ///  Is in Desktop
  inline bool inDesktop(const LHCb::Vertex* V)  const {
    verbose() << "Vertex " << V << " is in desktop " 
              << (m_vertsInTES.find(V)!=m_vertsInTES.end()) << endmsg ;
    return (m_vertsInTES.find(V)!=m_vertsInTES.end());
  }
  ///< Add to Desktop
  inline void setInDesktop(const LHCb::Particle* P) {
    m_partsInTES.insert(P); 
    verbose() << "Inserted LHCb::Particle " 
              << P << " (" << m_partsInTES.size() << ")" 
              << endmsg ; return ;
  }
  ///< Add to Desktop
  inline void setInDesktop(const LHCb::VertexBase* V) {
    m_vertsInTES.insert(V); 
    verbose() << "Inserted Vertex " << V << " (" 
              << m_vertsInTES.size() << ")" << endmsg ; 
    return ;
  }

  ///< inline private method for speed.
  inline const Particle2Vertex::Table* i_p2PVTable()  const { 
    return &m_p2VtxTable; 
  }
  inline Particle2Vertex::Table* i_p2PVTable() { 
    return &m_p2VtxTable; 
  }

  ///< Find the position of an LHCb::VertexBase inside a range
  inline Particle2Vertex::Range::iterator findTo(const Particle2Vertex::Range& range, const LHCb::VertexBase* to ) const 
  {
    for (Particle2Vertex::Range::iterator r = range.begin(); 
         range.end()!=r; 
         ++r) {
      if ( to == r->to() ) { return r; }  
    }
    return range.end();
  }
  

private: // data

  /// TES pathname for Primary Vertices 
  std::string m_primVtxLocn;
  /// TES pathname for Input Particles & Vertices from previous processing
  std::vector<std::string> m_inputLocn;
  /// TES pathname for Output Particles & Vertices
  std::string m_outputLocn;
  
  LHCb::Particle::ConstVector m_parts;          ///< Local Container of particles
  LHCb::Vertex::ConstVector m_secVerts;         ///< Local Container of secondary vertices
  LHCb::RecVertex::ConstVector m_primVerts;    ///< Local Container of primary vertices
  
  std::set<const LHCb::Particle*> m_partsInTES ; ///< particles that do not need to be deleted 
  std::set<const LHCb::VertexBase*>   m_vertsInTES ; ///< vertices  that do not need to be deleted 

  IParticleMaker* m_pMaker;        ///< Reference to LHCb::Particle maker tool

  /// Identify specific type of particle maker requested (Property)
  std::string m_pMakerType;  

  /// turn on location warnings
  bool m_locationWarned ;
  
  IOnOffline* m_OnOffline ;   ///< locate PV

  Particle2Vertex::Table m_p2VtxTable; ///< Table of Particle to PV relations

  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV

  /// Name of PVrelator. Default is RelatedPVFinder. You can choose another 
  /// instance by setting RelatedPVFinderName to RelatedPVFinder/MyFinder and then configuring
  /// ToolSvc.MyFinder.
  std::string m_pvRelatorName ; 

};

#endif // PHYSDESKTOP_H
