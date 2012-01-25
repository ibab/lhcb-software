// $Id: $
#ifndef PACKPARTICLESANDVERTICES_H 
#define PACKPARTICLESANDVERTICES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/StandardPacker.h"
#include "Relations/Relation1D.h"
#include "Event/Particle.h"
#include "Event/PackedParticle.h"
#include "Event/Vertex.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"

/** @class PackParticlesAndVertices PackParticlesAndVertices.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class PackParticlesAndVertices : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackParticlesAndVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackParticlesAndVertices( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

  typedef LHCb::Relation1D<LHCb::Particle, LHCb::VertexBase> RELATION;

protected:
  void selectContainers ( DataObject* obj, std::vector<std::string>& names, std::string postFix, bool forceRead=false );
  
  void packAParticleContainer ( LHCb::Particles* parts, LHCb::PackedParticles& pparts );
  
  void packAVertexContainer ( LHCb::Vertices* verts, LHCb::PackedVertices& pverts );
  
  void packARelationContainer ( RELATION* rels, LHCb::PackedRelations& prels );
  
private:
  std::string m_inputStream;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_forceReading;      ///< If true, read teh containers from file. Else (def) take what is in TES
  StandardPacker m_pack;
};
#endif // PACKPARTICLESANDVERTICES_H
