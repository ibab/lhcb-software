
// local
#include "Event/PackedMCRichSegment.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichSegmentPacker::pack( const DataVector & segs,
                                PackedDataVector & psegs ) const
{
  const char ver = psegs.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    psegs.data().reserve( segs.size() );
    for ( const Data * seg : segs )
    {
      psegs.data().emplace_back( PackedData() );
      auto & pseg = psegs.data().back();

      pseg.key = seg->key();

      pseg.history = seg->historyCode();

      pseg.trajPx.reserve( seg->trajectoryPoints().size() );
      pseg.trajPy.reserve( seg->trajectoryPoints().size() );
      pseg.trajPz.reserve( seg->trajectoryPoints().size() );
      for ( const auto& T : seg->trajectoryPoints() )
      {
        pseg.trajPx.push_back( m_pack.position(T.x()) );
        pseg.trajPy.push_back( m_pack.position(T.y()) );
        pseg.trajPz.push_back( m_pack.position(T.z()) );
      }

      pseg.trajMx.reserve( seg->trajectoryMomenta().size() );
      pseg.trajMy.reserve( seg->trajectoryMomenta().size() );
      pseg.trajMz.reserve( seg->trajectoryMomenta().size() );
      for ( const auto& M : seg->trajectoryMomenta() )
      {
        pseg.trajMx.push_back( m_pack.energy(M.x()) );
        pseg.trajMy.push_back( m_pack.energy(M.y()) );
        pseg.trajMz.push_back( m_pack.energy(M.z()) );
      }

      if ( seg->mcParticle() )
      {
        pseg.mcParticle  = ( UNLIKELY( 0==ver ) ? 
                             m_pack.reference32( &psegs,
                                                 seg->mcParticle()->parent(),
                                                 seg->mcParticle()->key() ) :
                             m_pack.reference64( &psegs,
                                                 seg->mcParticle()->parent(),
                                                 seg->mcParticle()->key() ) );
      }

      if ( seg->mcRichTrack() )
      {
        pseg.mcRichTrack = ( UNLIKELY( 0==ver ) ?
                             m_pack.reference32( &psegs,
                                                 seg->mcRichTrack()->parent(),
                                                 seg->mcRichTrack()->key() ) :
                             m_pack.reference64( &psegs,
                                                 seg->mcRichTrack()->parent(),
                                                 seg->mcRichTrack()->key() ) );
      }

      pseg.mcPhotons.reserve( seg->mcRichOpticalPhotons().size() );
      for ( const auto& P : seg->mcRichOpticalPhotons() )
      {
        pseg.mcPhotons.push_back( UNLIKELY( 0==ver ) ? 
                                  m_pack.reference32( &psegs,
                                                      P->parent(),
                                                      P->key() ) :
                                  m_pack.reference64( &psegs,
                                                      P->parent(),
                                                      P->key() ) );
      }

      pseg.mcHits.reserve( seg->mcRichHits().size() );
      for ( const auto& H : seg->mcRichHits() )
      {
        pseg.mcHits.push_back( UNLIKELY( 0==ver ) ? 
                               m_pack.reference32( &psegs,
                                                   H->parent(),
                                                   H.linkID() ) :
                               m_pack.reference64( &psegs,
                                                   H->parent(),
                                                   H.linkID() ) );
      }
      
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichSegmentPacker", StatusCode::FAILURE );
  }
}

void MCRichSegmentPacker::unpack( const PackedDataVector & psegs,
                                  DataVector       & segs ) const
{
  segs.reserve( psegs.data().size() );
  const char ver = psegs.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    for ( const auto & pseg : psegs.data() )
    {
      auto * seg = new Data();
      segs.insert( seg, pseg.key );

      seg->setHistoryCode( pseg.history );

      std::vector<int>::const_iterator ix(pseg.trajPx.begin()), iy(pseg.trajPy.begin()), iz(pseg.trajPz.begin());
      for ( ; ix != pseg.trajPx.end(); ++ix, ++iy, ++iz )
      {
        seg->addToTrajectoryPoints( Gaudi::XYZPoint( m_pack.position(*ix),
                                                     m_pack.position(*iy),
                                                     m_pack.position(*iz) ) );
      }

      std::vector<int>::const_iterator jx(pseg.trajMx.begin()), jy(pseg.trajMy.begin()), jz(pseg.trajMz.begin());
      for ( ; jx != pseg.trajMx.end(); ++jx, ++jy, ++jz )
      {
        seg->addToTrajectoryMomenta( Gaudi::XYZVector( m_pack.energy(*jx),
                                                       m_pack.energy(*jy),
                                                       m_pack.energy(*jz) ) );
      }

      int hintID(0), key(0);

      if ( -1 != pseg.mcParticle )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64(pseg.mcParticle,&psegs,&segs,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(pseg.mcParticle,&psegs,&segs,hintID,key) ) )
        {
          SmartRef<LHCb::MCParticle> refa(&segs,hintID,key);
          seg->setMcParticle( refa );
        }
        else { parent().Error( "Corrupt MCRichSegment MCParticle SmartRef detected." ).ignore(); }
      }

      if ( -1 != pseg.mcRichTrack )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64(pseg.mcRichTrack,&psegs,&segs,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(pseg.mcRichTrack,&psegs,&segs,hintID,key) ) )
        {
          SmartRef<LHCb::MCRichTrack> refb(&segs,hintID,key);
          seg->setMCRichTrack( refb );
        }
        else { parent().Error( "Corrupt MCRichSegment MCRichTrack SmartRef detected." ).ignore(); }
      }

      for ( const auto& P : pseg.mcPhotons )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64(P,&psegs,&segs,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(P,&psegs,&segs,hintID,key) ) )
        {
          SmartRef<LHCb::MCRichOpticalPhoton> ref(&segs,hintID,key);
          seg->addToMCRichOpticalPhotons( ref );
        }
        else { parent().Error( "Corrupt MCRichSegment MCRichOpticalPhoton SmartRef detected." ).ignore(); }
      }

      for ( const auto& H : pseg.mcHits )
      {
        if ( ( 0!=ver && m_pack.hintAndKey64( H, &psegs, &segs, hintID, key ) ) ||
             ( 0==ver && m_pack.hintAndKey32( H, &psegs, &segs, hintID, key ) ) )
        {
          SmartRef<LHCb::MCRichHit> ref(&segs,hintID,key);
          seg->addToMCRichHits( ref );
        }
        else { parent().Error( "Corrupt MCRichSegment MCRichHit SmartRef detected." ).ignore(); }
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichSegmentPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichSegmentPacker::check( const DataVector & dataA,
                                       const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;
    // Key
    ok &= ch.compareInts( "Key", (*iA)->key(), (*iB)->key() );
    // History code
    ok &= ch.compareInts( "HistoryCode", (*iA)->historyCode(), (*iB)->historyCode() );
    // Trajectory points
    const bool sameSizeTrajP = ch.compareInts( "#TrajPoints",
                                               (*iA)->trajectoryPoints().size(),
                                               (*iB)->trajectoryPoints().size() );
    ok &= sameSizeTrajP;
    if ( sameSizeTrajP )
    {
      std::vector<Gaudi::XYZPoint>::const_iterator tA((*iA)->trajectoryPoints().begin());
      std::vector<Gaudi::XYZPoint>::const_iterator tB((*iB)->trajectoryPoints().begin());
      for ( ; tA != (*iA)->trajectoryPoints().end() &&  tB != (*iB)->trajectoryPoints().end();
            ++tA, ++tB )
      {
        ok &= ch.comparePoints( "Traj. Point", *tA, *tB );
      }
    }
    // Trajectory momenta
    const bool sameSizeTrajM = ch.compareInts( "#TrajMomenta",
                                               (*iA)->trajectoryMomenta().size(),
                                               (*iB)->trajectoryMomenta().size() );
    ok &= sameSizeTrajM;
    if ( sameSizeTrajM )
    {
      std::vector<Gaudi::XYZVector>::const_iterator tA((*iA)->trajectoryMomenta().begin());
      std::vector<Gaudi::XYZVector>::const_iterator tB((*iB)->trajectoryMomenta().begin());
      for ( ; tA != (*iA)->trajectoryMomenta().end() && tB != (*iB)->trajectoryMomenta().end();
            ++tA, ++tB )
      {
        ok &= ch.compareEnergies( "Traj. Momenta", *tA, *tB );
      }
    }
    // MCParticle
    ok &= ch.comparePointers( "MCParticle", (*iA)->mcParticle(), (*iB)->mcParticle() );
    // MCRichTrack
    ok &= ch.comparePointers( "MCRichTrack", (*iA)->mcRichTrack(), (*iB)->mcRichTrack() );
    // MCPhotons
    const bool sameSizePhots = ch.compareInts( "#MCRichPhotons",
                                               (*iA)->mcRichOpticalPhotons().size(),
                                               (*iB)->mcRichOpticalPhotons().size() );
    ok &= sameSizePhots;
    if ( sameSizePhots )
    {
      SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator jA((*iA)->mcRichOpticalPhotons().begin());
      SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator jB((*iB)->mcRichOpticalPhotons().begin());
      for ( ; jA != (*iA)->mcRichOpticalPhotons().end() && jB != (*iB)->mcRichOpticalPhotons().end();
            ++jA, ++jB )
      {
        ok &= ch.comparePointers( "MCRichPhoton", jA->target(), jB->target() );
      }
    }
    // MCHits
    const bool sameSizeHits = ch.compareInts( "#MCRichHits",
                                              (*iA)->mcRichHits().size(),
                                              (*iB)->mcRichHits().size() );
    ok &= sameSizeHits;
    if ( sameSizeHits )
    {
      SmartRefVector<LHCb::MCRichHit>::const_iterator jA((*iA)->mcRichHits().begin());
      SmartRefVector<LHCb::MCRichHit>::const_iterator jB((*iB)->mcRichHits().begin());
      for ( ; jA != (*iA)->mcRichHits().end() && jB != (*iB)->mcRichHits().end();
            ++jA, ++jB )
      {
        ok &= ch.comparePointers( "MCRichHit", jA->target(), jB->target() );
      }
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCRichSegment data packing :-" << endmsg
                         << "  Original Segment : " << **iA
                         << endmsg
                         << "  Unpacked Segment : " << **iB
                         << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // return final status
  return sc;
}
