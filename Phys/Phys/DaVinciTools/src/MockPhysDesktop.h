// $Id: $
#ifndef MOCKPHYSDESKTOP_H 
#define MOCKPHYSDESKTOP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IPhysDesktop.h"            // Interface


/** @class MockPhysDesktop MockPhysDesktop.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2011-01-05
 */
class MockPhysDesktop : public GaudiTool, virtual public IPhysDesktop {
public: 
  /// Standard constructor
  MockPhysDesktop( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MockPhysDesktop( ); ///< Destructor

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

  virtual StatusCode saveDesktop() const ;
  
  virtual StatusCode saveTrees( const LHCb::Particle::ConstVector& ) const;
  
  virtual StatusCode cloneTrees( const LHCb::Particle::ConstVector& );
  
  virtual StatusCode saveTrees(int partid) const;

  virtual StatusCode cleanDesktop();
  
  virtual const std::string& getOutputLocation() const ;

  virtual StatusCode writeEmptyContainerIfNeeded() ;

  virtual const LHCb::VertexBase* relatedVertex(const LHCb::Particle* part) const;
  virtual void relate(const LHCb::Particle*   part, 
                      const LHCb::VertexBase* vert );

  virtual Particle2Vertex::Table::Range particle2Vertices(const LHCb::Particle* part ) const;

  virtual void setWriteP2PV(const bool& writeP2PV) ;

  virtual void setUsingP2PV(const bool& ) ;

  virtual void overWriteRelations(Particle2Vertex::Table::Range::const_iterator begin,
                                  Particle2Vertex::Table::Range::const_iterator end);


private:

  LHCb::Particle::ConstVector m_parts;
  LHCb::Vertex::ConstVector m_verts;
  Particle2Vertex::LightTable m_table;  
  Particle2Vertex::Map m_map;  
  std::string m_location;
  
};
#endif // MOCKPHYSDESKTOP_H
