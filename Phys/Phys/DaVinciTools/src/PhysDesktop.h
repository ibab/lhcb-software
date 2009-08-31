// $Id: PhysDesktop.h,v 1.38 2009-08-31 20:48:11 jpalac Exp $
#ifndef PHYSDESKTOP_H 
#define PHYSDESKTOP_H 1

// Include files
// from STL
#include <string>
#include <set>

// from Gaudi
//#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/GaudiTool.h"
// from DaVinci
#include "Kernel/IPhysDesktop.h"
#include "Kernel/DaVinciFun.h"
// Forward declarations
class IDataProviderSvc;
class IOnOffline;
class IRelatedPVFinder ;

/** @class PhysDesktop PhysDesktop.h Kernel/PhysDesktop.h
 *  
 *  A Desktop upon which the user keeps his particles and vertices
 *
 *  See IPhysDesktop interface for more details  
 *
 *  @author Sandra Amato and Gloria Corti
 *  @date   18/02/2002
 *
 */
class PhysDesktop : public GaudiTool,
                    virtual public IPhysDesktop {
  
public:
  
  /// Standard constructor
  PhysDesktop( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~PhysDesktop(){ } ///< Destructor
  
  /// IListener interface for cleaning at end of event
  //  virtual void handle(const Incident&);
  
  virtual StatusCode initialize();
  
  virtual StatusCode finalize();

  /// set InputLocations (for DVAlgorithm)
  virtual StatusCode setInputLocations(const std::vector<std::string> & ) ;

  /// set O->PV InputLocations (for DVAlgorithm)
  virtual StatusCode setP2PVInputLocations(const std::vector<std::string> & location) ;


  /// Load Input particles and vertices (in local data) from various input
  /// locations filled with previous processings and  
  /// create new Particles starting from reconstruction objects as
  /// requested in jobOptions. The creation of new Particles if delegated
  /// to ParticleMakers. 
  /// Only DVAlgorithm::sysExecute() should call this function.
  virtual StatusCode getEventInput();

  /// Retrieve the local particle container
  virtual const LHCb::Particle::ConstVector& particles() const;

  /// Retrieve the PVs from the TES
  virtual const LHCb::RecVertex::Container* primaryVertices() const;

  /// Retrieve the local secondary vertex container
  virtual const LHCb::Vertex::ConstVector& secondaryVertices() const;

  /// retrieve the Particle->Primary vertex relations
  virtual const Particle2Vertex::LightTable& Particle2VertexRelations() const;

  /// retrieve the Particle->Primary vertex relations
  virtual Particle2Vertex::LightTable& Particle2VertexRelations();

  /// Keep for future use: Register the new particles in the Desktop, 
  /// pass ownership, return pointer to new particle
  virtual const LHCb::Particle* keep( const LHCb::Particle* input ) ;

  /// Keep for future use: Register the new vertices in the Desktop, 
  /// pass ownership, return pointer to new vertex
  virtual const LHCb::Vertex* keep( const LHCb::Vertex* input ) ;

  /// Keep for future use: Register re-fitted primary vertices in the Desktop, 
  /// pass ownership, return pointer to new vertex. Vertices will only
  /// be stored in the TES if they are related to a particle being stored
  /// there.
  virtual const LHCb::RecVertex* keep( const LHCb::RecVertex* input ) ;

  /// Save particles, vertices and particle->vertices relations to the TES  
  virtual StatusCode saveDesktop() const{
    if (msgLevel(MSG::VERBOSE)) verbose() << "Save all new particles and vertices in desktop " << endmsg;
    return saveDesktop( m_parts, m_secVerts );
  }
  
  /// Save a vector of Particles
  /// If a particle is composite its descendents are also saved
  virtual StatusCode saveTrees( const LHCb::Particle::ConstVector& ) const;
  
  /// Clone all particles given by a list. This duplicates information 
  /// on the TES and should be used only when necessary. (Used by Filters)
  virtual StatusCode cloneTrees( const LHCb::Particle::ConstVector& );
  
  /// Save all Particles with a given particleID code
  virtual StatusCode saveTrees(int partid) const;

  /// Clean desktop
  virtual StatusCode cleanDesktop();
  
  /// Impose output location
  virtual void imposeOutputLocation(const std::string& outputLocationString);

  /// Get output location
  virtual const std::string& getOutputLocation() const { return m_outputLocn ;}

  /// Make sure the PhysDesktop has written out the container
  virtual StatusCode writeEmptyContainerIfNeeded() ;

  /// Get the vertex with the highest weight in the association
  /// between LHCb::Particle and LHCb::VertexBase
  virtual const LHCb::VertexBase* relatedVertex(const LHCb::Particle* part) const;
  
  /**
   * Get a pointer to the Particle->PV relator tool
   **/
  virtual const IRelatedPVFinder* relatedPVFinder() const;

  /// Establish a relation between an LHCb::Particle and an LHCb::VertexBase
  virtual void relate(const LHCb::Particle*   part, 
                      const LHCb::VertexBase* vert );

  /// Obtain a range of weighted LHCb::VertexBase related to an LHCb::Particle
  virtual Particle2Vertex::Table::Range particle2Vertices(const LHCb::Particle* part ) const;

  virtual const std::string& primaryVertexLocation() const 
  { 
    return m_primVtxLocn; 
  }

private:
  /// Find all particles & vertices in a tree. 
  void findAllTree( const LHCb::Particle* part, 
                    LHCb::Particle::ConstVector& parts, 
                    LHCb::Vertex::ConstVector& verts ) const;
  
  /// Save all particles and vertices specified in the lists, this is
  /// used internally and no tree structure is propagated.
  inline StatusCode saveDesktop( const LHCb::Particle::ConstVector& pToSave, 
                                 const LHCb::Vertex::ConstVector& vToSave) const 
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Save specified particles and vertices " << endmsg;
    saveParticles(pToSave);
    saveVertices(vToSave);
    return StatusCode::SUCCESS;
  }
  
  /// Save all Particles in the Desktop to the TES
  void saveParticles(const LHCb::Particle::ConstVector& pToSave) const;

  /// Save all Vertices in the Desktop to the TES
  void saveVertices(const LHCb::Vertex::ConstVector& vToSave) const;

  /// Save all Vertices in the Desktop to the TES
  void saveRefittedPVs(const LHCb::RecVertex::ConstVector& vToSave) const;

  /// Save the Particle->Vertex relations table in the Desktop to the TES
  void saveP2PVRelations(const LHCb::Particle::ConstVector& pToSave) const;

  StatusCode getPrimaryVertices(); ///< get PV

  StatusCode getParticles();       ///< get Particles

  StatusCode getInputRelations();       ///< get all Relation tables

  StatusCode getInputRelations(std::vector<std::string>::const_iterator begin,
                               std::vector<std::string>::const_iterator end);       ///< get Relation table

  /// Add to list of Particles known to be in TES
  /// inline private method for speed.
  inline const Particle2Vertex::LightTable& i_p2PVTable()  const { 
    return m_p2VtxTable; 
  }

  inline Particle2Vertex::LightTable& i_p2PVTable() { 
    return m_p2VtxTable; 
  }

  inline Particle2Vertex::Table::Range i_particle2Vertices(const LHCb::Particle* part ) const 
  {
    return i_p2PVTable().i_relations(part);
  }
  
  inline const LHCb::VertexBase* i_relatedVertexFromTable(const LHCb::Particle* part ) const 
  {
    return DaVinci::bestVertexBase(i_p2PVTable().i_relations(part));
  }

  /// print out particle in verbose mode
  inline void printOut(std::string head, const LHCb::Particle* P) const{
    if (0==P) Exception("Null particle");
    always() << head << " PID= " << P->particleID().pid()
             << " P= " << P->momentum() << " m= " << P->measuredMass()
             << " par= " << (0!=P->parent())  << endmsg ;
    return;
  }

  /// print out vertex in verbose mode
  inline void printOut(std::string head, const LHCb::Vertex* V)const{
    if (0==V) Exception("Null Vertex");
    always() << head << " position " << V->position()
             << " d= " << V->outgoingParticles().size()
             << " par= " << (0!=V->parent())  << endmsg ;
    return;
  }

  void storeRelationsInTable(const LHCb::Particle* part);

  /**
   * 
   * Store a range of Particle -> VertexBase relations in the local
   * relations table. 
   * <b>Note: If relations for the same particle already exist they are
   * removed before inserting the new ones!</b> 
   * 
   * @author Juan Palacios juan.palacios@nikhef.nl
   */

  virtual void overWriteRelations(Particle2Vertex::Table::Range::const_iterator begin,
                                  Particle2Vertex::Table::Range::const_iterator end);

  /**
   *
   * Add leading m_OnOffline->trunkOnTES()+"/" string to input locations,
   * unless these contain a "/", in which case leave them untouched.
   * <b> Examples: </b> 
   *
   * "MyBsSelection" becomes "Phys/MyBsSelection" if m_OnOffline->online() ==false and 
   * "MyBsSelection" becomes "Hlt/MyBsSelection" if m_OnOffline->online() == true.
   * "Phys/MyBsSelection" remains unchanged.
   * "Anything/MyBsSelection" remains unchanged.
   * "/MyBsSelection" remains unchanged, but is most likely an error!
   * 
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 2008-10-31
   * @todo put this in some tool (not necessarily in the Gaudi sense of the word)
   *
   */
  void fixInputLocations(std::vector<std::string>::iterator begin,
                         std::vector<std::string>::iterator end);
  

private: // data

  /// TES pathname for Primary Vertices 
  std::string m_primVtxLocn;
  /// TES pathname for Input Particles & Vertices from previous processing
  std::vector<std::string> m_inputLocations;
  /// TES pathname for Output Particles & Vertices
  std::string m_outputLocn;

  /** 
   *  TES location of default Particle->PV relaitons tables.
   *  These are the InputLocations + "/Particle2VertexRelations"
   */
  std::vector<std::string> m_p2PVDefaultLocations;

  /// TES locations of user-defined Particle->PV relations
  std::vector<std::string> m_p2PVInputLocations;
  
  LHCb::Particle::ConstVector m_parts;          ///< Local Container of particles
  LHCb::Vertex::ConstVector m_secVerts;         ///< Local Container of secondary vertices

  LHCb::RecVertex::ConstVector m_refitPVs;    ///< Local Container of re-fitted primary vertices
  LHCb::RecVertex::Container* m_primVerts;    ///< Local Container of primary vertices

  IOnOffline* m_OnOffline ;   ///< locate PV

  Particle2Vertex::LightTable m_p2VtxTable; ///< Table of Particle to PV relations

  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV

  /// Name of PVrelator. Default is RelatedPVFinder. You can choose another 
  /// instance by setting RelatedPVFinderName to RelatedPVFinder/MyFinder and then configuring
  /// ToolSvc.MyFinder.
  std::string m_pvRelatorName ; 

  /// shortcut instead of printing warnings
  void inline Info(const std::string& mess, const StatusCode sc = StatusCode::SUCCESS, 
                   const MSG::Level level = MSG::DEBUG ) const {
    Print(mess, sc, level).ignore();
    counter(mess)++;
    return ;
  }

};

#endif // PHYSDESKTOP_H
