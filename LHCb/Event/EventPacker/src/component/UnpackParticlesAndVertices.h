// $Id: $
#ifndef UNPACKPARTICLESANDVERTICES_H
#define UNPACKPARTICLESANDVERTICES_H 1

#include <memory>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/Particle2LHCbIDs.h"

#include "Relations/Relation1D.h"
#include "Relations/RelationWeighted1D.h"

#include "Event/StandardPacker.h"
#include "Event/PackedTrack.h"
#include "Event/PackedProtoParticle.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedParticle.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedFlavourTag.h"
#include "Event/PackedPartToRelatedInfoRelation.h"

#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/MuonPID.h"
#include "Event/RichPID.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/FlavourTag.h"
#include "Event/RecVertex.h"
#include "Event/MCParticle.h"
#include "Event/RelatedInfoMap.h"

/** @class UnpackParticlesAndVertices UnpackParticlesAndVertices.h
 *
 *  Unpacks all Packed Particles and related objects
 *
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class UnpackParticlesAndVertices : public GaudiAlgorithm
{

public:

  /// Standard constructor
  UnpackParticlesAndVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackParticlesAndVertices( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  template < class FROM, class TO, class FROMCONT, class TOCONT >
  void unpackP2PRelations( const std::string & location );

  template < class FROM, class TO, class FROMCONT, class TOCONT, class WEIGHT >
  void unpackP2PWeightedRelations( const std::string & location );

  template < class FROM, class TO, class FROMCONT >
  void unpackP2IntRelations( const std::string & location );

private:

  std::string m_inputStream; ///< Input stream root

  std::string m_postFix;

  StandardPacker m_pack{this}; ///< Standard Packer

  /// Track packer
  const LHCb::TrackPacker m_trackPacker{this};
  /// Muon PID packer
  const LHCb::MuonPIDPacker m_muonPacker{this};
  /// Rich PID packer
  const LHCb::RichPIDPacker m_richPacker{this};
  /// ProtoParticle packer
  const LHCb::ProtoParticlePacker m_protoPacker{this};
  /// Particle Packer
  const LHCb::ParticlePacker m_partPacker{this};
  /// Vertex Packer
  const LHCb::VertexPacker m_vertPacker{this};
  /// Flavour Tag Packer
  const LHCb::FlavourTagPacker m_ftPacker{this};
  /// Rec Vertex Packer
  const LHCb::RecVertexPacker m_rvPacker{this};
  /// Related Info Packer
  const LHCb::RelatedInfoRelationsPacker m_rInfoPacker{this};

};

template < class FROM, class TO, class FROMCONT, class TOCONT >
inline void
UnpackParticlesAndVertices::unpackP2PRelations( const std::string & location )
{
  typedef LHCb::Relation1D<FROM,TO> RELATION;

  unsigned int nbRelContainer(0), nbRel(0);

  RELATION * rels = nullptr;
  const LHCb::PackedRelations* prels = getIfExists<LHCb::PackedRelations>( location );
  if ( nullptr != prels )
  {
    for ( const auto & prel : prels->relations() )
    {
      int indx = prel.container >> 32;
      const std::string & containerName = prels->linkMgr()->link( indx )->path() + m_postFix;
      rels = new RELATION();
      put( rels, containerName );
      ++nbRelContainer;
      FROMCONT * srcContainer = nullptr;
      int prevSrcLink = -1;
      DataObject* dstContainer = nullptr;
      int prevDstLink = -1;
      for ( int kk = prel.start;  prel.end > kk; ++kk )
      {
        int srcLink(0), srcKey(0);
        m_pack.indexAndKey64( prels->sources()[kk], srcLink, srcKey );
        if ( srcLink != prevSrcLink )
        {
          prevSrcLink = srcLink;
          const std::string & srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<FROMCONT>( srcName );
        }
        FROM* from = srcContainer->object( srcKey );
        int dstLink(0), dstKey(0);
        m_pack.indexAndKey64( prels->dests()[kk], dstLink, dstKey );
        if ( dstLink != prevDstLink )
        {
          prevDstLink = dstLink;
          const std::string & dstName = prels->linkMgr()->link( dstLink )->path();
          dstContainer = get<DataObject>( dstName );
        }
        TOCONT * _to = dynamic_cast<TOCONT*>(dstContainer);
        TO* to = ( _to ? _to->object(dstKey) : nullptr );
        if ( !to ) info() << "Unknown objec: Container type " << (dstContainer->clID()>>16)
                          << "+" << (dstContainer->clID()&0xFFFF)
                          << " key " << dstKey << endmsg;
        rels->relate( from, to );
        ++nbRel;
      }
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbRel << " relations in " << nbRelContainer << " containers"
            << " from " << location
            << endmsg;
  }

}

template < class FROM, class TO, class FROMCONT, class TOCONT, class WEIGHT >
inline void
UnpackParticlesAndVertices::unpackP2PWeightedRelations( const std::string & location )
{
  typedef LHCb::RelationWeighted1D<FROM,TO,WEIGHT> RELATION;

  unsigned int nbRelContainer(0), nbRel(0);

  RELATION * rels = nullptr;
  const LHCb::PackedWeightedRelations* prels = getIfExists<LHCb::PackedWeightedRelations>( location );
  if ( nullptr != prels )
  {
    for ( const auto & prel : prels->relations() )
    {
      int indx = prel.container >> 32;
      const std::string & containerName = prels->linkMgr()->link( indx )->path() + m_postFix;
      rels = new RELATION();
      put( rels, containerName );
      ++nbRelContainer;
      FROMCONT * srcContainer = nullptr;
      int prevSrcLink = -1;
      DataObject* dstContainer = nullptr;
      int prevDstLink = -1;
      for ( int kk = prel.start;  prel.end > kk; ++kk )
      {
        int srcLink(0), srcKey(0);
        m_pack.indexAndKey64( prels->sources()[kk], srcLink, srcKey );
        if ( srcLink != prevSrcLink )
        {
          prevSrcLink = srcLink;
          const std::string & srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<FROMCONT>( srcName );
        }
        FROM* from = srcContainer->object( srcKey );
        int dstLink(0), dstKey(0);
        m_pack.indexAndKey64( prels->dests()[kk], dstLink, dstKey );
        if ( dstLink != prevDstLink )
        {
          prevDstLink = dstLink;
          const std::string & dstName = prels->linkMgr()->link( dstLink )->path();
          dstContainer = get<DataObject>( dstName );
        }
        const WEIGHT wgt = prels->weights()[kk];
        TOCONT * _to = dynamic_cast<TOCONT*>(dstContainer);
        TO* to = ( _to ? _to->object(dstKey) : nullptr );
        if ( !to ) info() << "Unknown objec: Container type " << (dstContainer->clID()>>16)
                          << "+" << (dstContainer->clID()&0xFFFF)
                          << " key " << dstKey << endmsg;
        rels->relate( from, to, wgt );
        ++nbRel;
      }
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbRel << " relations in " << nbRelContainer << " containers"
            << " from " << location
            << endmsg;
  }

}

template < class FROM, class TO, class FROMCONT >
inline void
UnpackParticlesAndVertices::unpackP2IntRelations( const std::string & location )
{
  typedef LHCb::Relation1D<FROM,TO> RELATION;

  unsigned int nbRelContainer(0), nbRel(0);

  LHCb::PackedRelations* prels = getIfExists<LHCb::PackedRelations>( location );
  if ( nullptr != prels )
  {
    for ( std::vector<LHCb::PackedRelation>::iterator itR = prels->relations().begin();
          prels->relations().end() != itR; ++itR )
    {
      const LHCb::PackedRelation& prel = *itR;
      const int indx = prel.container >> 32;
      const std::string & containerName = prels->linkMgr()->link(indx)->path() + m_postFix;
      RELATION * rels = new RELATION();
      put( rels, containerName );
      ++nbRelContainer;
      FROMCONT * srcContainer = nullptr;
      int prevSrcLink = -1;
      for ( int kk = prel.start; prel.end > kk; ++kk )
      {
        int srcLink(0), srcKey(0);
        m_pack.indexAndKey64( prels->sources()[kk], srcLink, srcKey );
        if ( srcLink != prevSrcLink )
        {
          prevSrcLink = srcLink;
          const std::string & srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<FROMCONT>( srcName );
        }
        FROM * from = srcContainer->object( srcKey );
        TO to       = (TO) prels->dests()[kk];
        rels->relate( from, to );
        ++nbRel;
      }
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbRel << " relations in " << nbRelContainer << " containers"
            << " from " << location
            << endmsg;
  }

}

#endif // UNPACKPARTICLESANDVERTICES_H
