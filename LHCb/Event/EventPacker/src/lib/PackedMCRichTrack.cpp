
// local
#include "Event/PackedMCRichTrack.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichTrackPacker::pack( const DataVector & tracks,
                              PackedDataVector & ptracks ) const
{
  const auto ver = ptracks.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    ptracks.data().reserve( tracks.size() );
    for ( const auto * track : tracks )
    {
      ptracks.data().emplace_back( PackedData() );
      auto & ptrack = ptracks.data().back();

      ptrack.key = track->key();

      for ( const auto& S : track->mcSegments() )
      {
        ptrack.mcSegments.emplace_back( UNLIKELY( 0==ver ) ?
                                        m_pack.reference32( &ptracks,
                                                            S->parent(),
                                                            S->key() ) :
                                        m_pack.reference64( &ptracks,
                                                            S->parent(),
                                                            S->key() ) );
      }

      if ( track->mcParticle() )
      {
        ptrack.mcParticle = ( UNLIKELY( 0==ver ) ?
                              m_pack.reference32( &ptracks,
                                                  track->mcParticle()->parent(),
                                                  track->mcParticle()->key() ) :
                              m_pack.reference64( &ptracks,
                                                  track->mcParticle()->parent(),
                                                  track->mcParticle()->key() ) );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichTrackPacker", StatusCode::FAILURE );
  }
}

void MCRichTrackPacker::unpack( const PackedDataVector & ptracks,
                                DataVector       & tracks ) const
{
  const auto ver = ptracks.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    tracks.reserve( ptracks.data().size() );
    for ( const auto & ptrack : ptracks.data() )
    {
      // make and save new hit in container
      auto * track  = new Data();
      tracks.insert( track, ptrack.key );

      int hintID(0), key(0);

      for ( const auto& S : ptrack.mcSegments )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64( S, &ptracks, &tracks, hintID, key ) ) ||
             ( 0==ver && m_pack.hintAndKey32( S, &ptracks, &tracks, hintID, key ) ) )
        {
          SmartRef<LHCb::MCRichSegment> ref(&tracks,hintID,key);
          track->addToMcSegments( ref );
        }
        else { parent().Error( "Corrupt MCRichTrack MCRichSegment SmartRef detected." ).ignore(); }
      }

      if ( -1 != ptrack.mcParticle )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64( ptrack.mcParticle, &ptracks, &tracks, hintID, key ) ) ||
             ( 0==ver && m_pack.hintAndKey32( ptrack.mcParticle, &ptracks, &tracks, hintID, key ) ) )
        {
          SmartRef<LHCb::MCParticle> ref(&tracks,hintID,key);
          track->setMcParticle( ref );
        }
        else { parent().Error( "Corrupt MCRichTrack MCParticle SmartRef detected." ).ignore(); }
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << ver;
    throw GaudiException( mess.str(), "MCRichTrackPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichTrackPacker::check( const DataVector & dataA,
                                     const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Loop over data containers together and compare
  auto iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;
    // Key
    ok &= ch.compareInts( "Key", (*iA)->key(), (*iB)->key() );
    // MCParticle
    ok &= ch.comparePointers( "MCParticle", (*iA)->mcParticle(), (*iB)->mcParticle() );
    // MCSegments

    const bool sameSize = ch.compareInts( "#MCSegments",
                                          (*iA)->mcSegments().size(), (*iB)->mcSegments().size() );
    ok &= sameSize;
    if ( sameSize )
    {
      auto jA((*iA)->mcSegments().begin()), jB((*iB)->mcSegments().begin());
      for ( ; jA != (*iA)->mcSegments().end() && jB != (*iB)->mcSegments().end();
            ++jA, ++jB )
      {
        ok &= ch.comparePointers( "MCSegment", jA->target(), jB->target() );
      }
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCRichTrack data packing :-" << endmsg
                         << "  Original Track : " << **iA
                         << endmsg
                         << "  Unpacked Track : " << **iB
                         << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // return
  return sc;
}
