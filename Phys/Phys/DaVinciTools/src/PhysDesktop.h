// $Id: PhysDesktop.h,v 1.55 2010-08-12 15:22:09 jpalac Exp $
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
#include "Kernel/DVAlgorithm.h"
// Forward declarations
//class DVAlgorithm;
class IDataProviderSvc;
//class IOnOffline;

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

// for backwards compatibility. Remove when LHCb v28r2 comes out.
#include "GaudiKernel/HashMap.h"
namespace Particle2Vertex 
{
  typedef GaudiUtils::HashMap<const LHCb::Particle*, const LHCb::VertexBase*> Map;
}


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

  virtual void setOutputLocation(const std::string& location);

  virtual StatusCode setInputLocations(const std::vector<std::string>& locations) ;

  virtual StatusCode setP2PVInputLocations(const std::vector<std::string>& locations) ;


  /// Load Input particles and vertices (in local data) from various input
  /// locations filled with previous processings and  
  /// create new Particles starting from reconstruction objects as
  /// requested in jobOptions. The creation of new Particles if delegated
  /// to ParticleMakers. 
  /// Only DVAlgorithm::sysExecute() should call this function.
  virtual StatusCode getEventInput();

  virtual const LHCb::Particle::ConstVector& particles() const;

  virtual const LHCb::Vertex::ConstVector& secondaryVertices() const;

  virtual const Particle2Vertex::LightTable& Particle2VertexRelations() const;

  virtual Particle2Vertex::LightTable& Particle2VertexRelations();

  virtual Particle2Vertex::Map& Particle2VertexMap();

  virtual const Particle2Vertex::Map& Particle2VertexMap() const;

  virtual const LHCb::Particle* keep( const LHCb::Particle* input ) ;

  virtual const LHCb::Vertex* keep( const LHCb::Vertex* input ) ;

  virtual const LHCb::RecVertex* keep( const LHCb::RecVertex* input ) ;

  virtual StatusCode saveDesktop() const{
    if (msgLevel(MSG::VERBOSE)) verbose() << "Save all new particles and vertices in desktop " << endmsg;
    return saveDesktop( m_parts, m_secVerts );
  }
  
  virtual StatusCode saveTrees( const LHCb::Particle::ConstVector& ) const;
  
  virtual StatusCode cloneTrees( const LHCb::Particle::ConstVector& );
  
  virtual StatusCode saveTrees(int partid) const;

  virtual StatusCode cleanDesktop();
  
  virtual const std::string& getOutputLocation() const { return m_outputLocn ;}

  virtual StatusCode writeEmptyContainerIfNeeded() ;

  virtual const LHCb::VertexBase* relatedVertex(const LHCb::Particle* part) const;
  virtual void relate(const LHCb::Particle*   part, 
                      const LHCb::VertexBase* vert );

  virtual Particle2Vertex::Table::Range particle2Vertices(const LHCb::Particle* part ) const;

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

  void checkRelations(const Particle2Vertex::Table::Range::const_iterator begin,
                      const Particle2Vertex::Table::Range::const_iterator end) const;


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
//   void fixInputLocations(std::vector<std::string>::iterator begin,
//                          std::vector<std::string>::iterator end);
  

  inline bool saveP2PV() const 
  {
    return m_writeP2PV;
  }
  
  inline bool usingP2PV() const 
  {
    // pick up parent's useP2PV() property...
    //return m_dva->useP2PV();
    return m_usingP2PV;
  }

public:
  virtual void setWriteP2PV(const bool& writeP2PV) 
  {
    m_writeP2PV = writeP2PV;
  }

  virtual void setUsingP2PV(const bool& /*usingP2PV*/) 
  {
      always() << " please do not call setUsingP2PV -- obsolete " << endmsg;
  }  

private: // data


  /// Do we want to write the Particle -> PV relations table to the TES?
  bool m_writeP2PV;

  /// Are we using the Particle -> PV relations table internally?
  bool m_usingP2PV;

  /// TES pathname for Input Particles & Vertices from previous processing
  std::vector<std::string> m_inputLocations;
  /// TES pathname for Output Particles & Vertices
  std::string m_outputLocn;

  /// TES locations of user-defined Particle->PV relations
  std::vector<std::string> m_p2PVInputLocations;
  
  LHCb::Particle::ConstVector m_parts;          ///< Local Container of particles
  LHCb::Vertex::ConstVector m_secVerts;         ///< Local Container of secondary vertices

  LHCb::RecVertex::ConstVector m_refitPVs;    ///< Local Container of re-fitted primary vertices

  Particle2Vertex::LightTable m_p2VtxTable; ///< Table of Particle to PV relations

  Particle2Vertex::Map m_p2PVMap;

  DVAlgorithm* m_dva;

};

#endif // PHYSDESKTOP_H
