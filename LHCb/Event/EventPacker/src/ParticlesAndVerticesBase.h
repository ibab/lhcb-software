// $Id: $
#ifndef ParticlesAndVerticesBase_H
#define ParticlesAndVerticesBase_H 1

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

/** @class ParticlesAndVerticesBase ParticlesAndVerticesBase.h
 *
 *  Base class with useful methods for Particle and Vertex related packing
 *
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class ParticlesAndVerticesBase : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ParticlesAndVerticesBase( const std::string& name,
                            ISvcLocator* pSvcLocator );

  virtual ~ParticlesAndVerticesBase( ); ///< Destructor

protected:

  typedef LHCb::Relation1D<LHCb::Particle, LHCb::VertexBase> P2VRELATION;

private:

  /// Select the data containers in the TES, of a given type, to write out
  void selectContainers ( DataObject* obj,
                          std::vector<std::string>& names,
                          const unsigned int classID,
                          const bool forceRead=false );

  /// Pack a Particle container
  void packAParticleContainer ( const LHCb::Particles* parts,
                                LHCb::PackedParticles& pparts );

  /// pack a vertex container
  void packAVertexContainer ( const LHCb::Vertices* verts,
                              LHCb::PackedVertices& pverts );

  /// Pack a RecVertex container
  void packARecVertexContainer ( const LHCb::RecVertices* rverts,
                                 LHCb::PackedRecVertices& prverts );

  /// Pack a relations container
  void packARelationContainer ( const RELATION* rels,
                                LHCb::PackedRelations& prels );

  /// Pack a Particle to LHCbID map
  void packAParticleLHCbIDContainer ( const DaVinci::Map::Particle2LHCbIDs* partIds,
                                      LHCb::PackedParticle2Ints& pPartIds );

protected:

  std::string m_inputStream; ///< Input stream root
  bool m_alwaysOutput;       ///< Flag to turn on the creation of output, even when input is missing
  bool m_forceReading;       ///< If true, read the containers from file. Else (def) take what is in TES
  bool m_deleteInput;        ///< delete the containers after packing if true.
  bool m_enableCheck;        ///< Flag to turn on automatic unpacking and checking of the output post-packing
  bool m_listRemaining;      ///< list the remaining objects after packing
  std::vector<std::string> m_vetoedConts; ///< Vetoed containers. Will not be packed.
  StandardPacker m_pack;

  unsigned int m_clIdParticles;
  unsigned int m_clIdVertices;
  unsigned int m_clIdRecVertices;
  unsigned int m_clIdPart2Vert;
  unsigned int m_clIdPart2Ints;

};

#endif // ParticlesAndVerticesBase_H
