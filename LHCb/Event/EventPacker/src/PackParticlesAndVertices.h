// $Id: $
#ifndef PACKPARTICLESANDVERTICES_H 
#define PACKPARTICLESANDVERTICES_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"

#include "Kernel/StandardPacker.h"
#include "Kernel/Particle2LHCbIDs.h"

#include "Relations/Relation1D.h"

#include "Kernel/LHCbID.h"
#include "Event/Particle.h"
#include "Event/PackedParticle.h"
#include "Event/Vertex.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedParticle2Ints.h"

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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  typedef LHCb::Relation1D<LHCb::Particle, LHCb::VertexBase> RELATION;

protected:
  void selectContainers ( DataObject* obj, std::vector<std::string>& names, unsigned int classID, bool forceRead=false );
  
  void packAParticleContainer ( LHCb::Particles* parts, LHCb::PackedParticles& pparts );
  
  void packAVertexContainer ( LHCb::Vertices* verts, LHCb::PackedVertices& pverts );
  
  void packARecVertexContainer ( LHCb::RecVertices* rverts, LHCb::PackedRecVertices& prverts );
  
  void packARelationContainer ( RELATION* rels, LHCb::PackedRelations& prels );
  
  void packAParticleLHCbIDContainer ( DaVinci::Map::Particle2LHCbIDs* partIds, LHCb::PackedParticle2Ints& pPartIds );
  
private:
  std::string m_inputStream; ///< Input stream root
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_forceReading;      ///< If true, read the containers from file. Else (def) take what is in TES
  bool m_deleteInput;       ///< delete the containers after packing if true.
  bool m_listRemaining;     ///< list the remaining objects after packing
  std::vector<std::string> m_vetoedConts; ///< Vetoed containers. Will not be packed.
  StandardPacker m_pack;

  unsigned int m_clIdParticles;
  unsigned int m_clIdVertices;
  unsigned int m_clIdRecVertices;
  unsigned int m_clIdPart2Vert;
  unsigned int m_clIdPart2Ints;
};
#endif // PACKPARTICLESANDVERTICES_H
