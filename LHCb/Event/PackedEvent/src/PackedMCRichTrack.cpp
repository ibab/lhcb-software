// $Id: PackedMCRichTrack.cpp,v 1.5 2010-04-11 14:27:15 jonrob Exp $

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
  ptracks.data().reserve( tracks.size() );

  if ( 0 == ptracks.packingVersion() )
  {
    for ( DataVector::const_iterator iD = tracks.begin();
          iD != tracks.end(); ++iD )
    {
      Data & track = **iD;
      ptracks.data().push_back( PackedData() );
      PackedData & ptrack = ptracks.data().back();

      ptrack.key = track.key();

      for ( SmartRefVector<LHCb::MCRichSegment>::const_iterator iS =
              track.mcSegments().begin();
            iS != track.mcSegments().end(); ++iS )
      {
        ptrack.mcSegments.push_back( m_pack.reference( &ptracks,
                                                       (*iS)->parent(),
                                                       (*iS)->key() ) );
      }

      if ( NULL != track.mcParticle() )
      {
        ptrack.mcParticle = m_pack.reference( &ptracks,
                                              track.mcParticle()->parent(),
                                              track.mcParticle()->key() );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ptracks.packingVersion();
    throw GaudiException( mess.str(), "MCRichTrackPacker", StatusCode::FAILURE );
  }
}

void MCRichTrackPacker::unpack( const PackedDataVector & ptracks,
                                DataVector       & tracks ) const
{
  tracks.reserve( ptracks.data().size() );
  if ( 0 == ptracks.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = ptracks.data().begin();
          iD != ptracks.data().end(); ++iD )
    {
      const PackedData & ptrack = *iD;
      // make and save new hit in container
      Data * track  = new Data();
      tracks.insert( track, ptrack.key );

      int hintID(0), key(0);

      for ( std::vector<int>::const_iterator iS = ptrack.mcSegments.begin();
            iS != ptrack.mcSegments.end(); ++iS )
      {
        m_pack.hintAndKey( *iS, &ptracks, &tracks, hintID, key );
        SmartRef<LHCb::MCRichSegment> ref(&tracks,hintID,key);
        track->addToMcSegments( ref );
      }

      if ( -1 != ptrack.mcParticle )
      {
        m_pack.hintAndKey( ptrack.mcParticle, &ptracks, &tracks, hintID, key );
        SmartRef<LHCb::MCParticle> ref(&tracks,hintID,key);
        track->setMcParticle( ref );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ptracks.packingVersion();
    throw GaudiException( mess.str(), "MCRichTrackPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichTrackPacker::check( const DataVector & dataA,
                                     const DataVector & dataB,
                                     GaudiAlgorithm & parent ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent);

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;
    // Key
    ok &= (*iA)->key() == (*iB)->key();
    // MCParticle
    ok &= (*iA)->mcParticle() == (*iB)->mcParticle();
    // MCSegments
    const bool sameSize = (*iA)->mcSegments().size() == (*iB)->mcSegments().size();
    ok &= sameSize;
    if ( sameSize )
    {
      SmartRefVector<LHCb::MCRichSegment>::const_iterator jA((*iA)->mcSegments().begin());
      SmartRefVector<LHCb::MCRichSegment>::const_iterator jB((*iB)->mcSegments().begin());
      for ( ; jA != (*iA)->mcSegments().end() && jB != (*iB)->mcSegments().end();
            ++jA, ++jB )
      {
        ok &= *jA == *jB;
      }
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with MCRichTrack data packing :-" << endmsg
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
