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
#include "Event/FlavourTag.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedFlavourTag.h"
#include "Event/MCParticle.h"

/** @class PackParticlesAndVertices PackParticlesAndVertices.h
 *
 *  Packs Particles, Vertices and related information.
 *
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class PackParticlesAndVertices : public GaudiAlgorithm
{

public:

  /// Standard constructor
  PackParticlesAndVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackParticlesAndVertices( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  typedef LHCb::Relation1D<LHCb::Particle,LHCb::VertexBase> P2VRELATION;
  typedef LHCb::Relation1D<LHCb::Particle,LHCb::MCParticle> P2MCPRELATION;
  typedef LHCb::Relation1D<LHCb::Particle,int>              Part2IntRelations;

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

  /// Pack a FlavourTag container
  void packAFTContainer ( const LHCb::FlavourTags* fts,
                          LHCb::PackedFlavourTags& pfts );

  /// Pack a RecVertex container
  void packARecVertexContainer ( const LHCb::RecVertices* rverts,
                                 LHCb::PackedRecVertices& prverts );

  /// Pack a 'SmartRef to SmartRef' relations container
  template < typename RELATION >
  void packAP2PRelationContainer ( const RELATION* rels,
                                   LHCb::PackedRelations& prels );

  /// Pack a 'SmartRef to int' relations container
  template < typename RELATION >
  void packAP2IntRelationContainer ( const RELATION* rels,
                                     LHCb::PackedRelations& prels );

private:

  std::string m_inputStream; ///< Input stream root
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_forceReading;      ///< If true, read the containers from file. Else (def) take what is in TES
  bool m_deleteInput;       ///< delete the containers after packing if true.
  bool m_enableCheck;       ///< Flag to turn on automatic unpacking and checking of the output post-packing
  bool m_listRemaining;     ///< list the remaining objects after packing
  std::vector<std::string> m_vetoedConts; ///< Vetoed containers. Will not be packed.
  StandardPacker m_pack;

};

//=========================================================================
//  Pack a 'SmartRef to SmartRef' relations container
//=========================================================================
template < typename RELATION >
inline void
PackParticlesAndVertices::packAP2PRelationContainer ( const RELATION* rels,
                                                      LHCb::PackedRelations& prels )
{
  // Make a new packed data object and save
  prels.relations().push_back( LHCb::PackedRelation() );
  LHCb::PackedRelation& prel = prels.relations().back();
  // reference to original container and key
  prel.container = m_pack.reference64( &prels, rels, 0 );
  prel.start     = prels.sources().size();
  typename RELATION::Range all = rels->relations();
  for ( typename RELATION::Range::iterator itR = all.begin(); all.end() != itR; ++itR )
  {
    prels.sources().push_back( m_pack.reference64( &prels,
                                                   (*itR).from()->parent(),
                                                   (*itR).from()->key() ) );
    prels.dests().push_back  ( m_pack.reference64( &prels,
                                                   (*itR).to()->parent(),
                                                   (*itR).to()->key()   ) );
  }
  prel.end = prels.sources().size();
  if ( !m_deleteInput ) rels->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a 'SmartRef to int' relations container
//=========================================================================
template < typename RELATION >
inline void
PackParticlesAndVertices::packAP2IntRelationContainer ( const RELATION* rels,
                                                        LHCb::PackedRelations& prels )
{
  // Make a new packed data object and save
  prels.relations().push_back( LHCb::PackedRelation() );
  LHCb::PackedRelation& prel = prels.relations().back();
  // reference to original container and key
  prel.container = m_pack.reference64( &prels, rels, 0 );
  prel.start     = prels.sources().size();
  typename RELATION::Range all = rels->relations();
  for ( typename RELATION::Range::iterator itR = all.begin(); all.end() != itR; ++itR )
  {
    prels.sources().push_back( m_pack.reference64( &prels,
                                                   (*itR).from()->parent(),
                                                   (*itR).from()->key() ) );
    prels.dests().push_back  ( (*itR).to() );
  }
  prel.end = prels.sources().size();
  if ( !m_deleteInput ) rels->registry()->setAddress( 0 );
}

#endif // PACKPARTICLESANDVERTICES_H
