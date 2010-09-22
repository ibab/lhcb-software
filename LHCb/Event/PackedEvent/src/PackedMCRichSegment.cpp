// $Id: PackedMCRichSegment.cpp,v 1.5 2010-04-11 14:27:15 jonrob Exp $

// local
#include "Event/PackedMCRichSegment.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichSegmentPacker::pack( const DataVector       & segs,
                                PackedDataVector & psegs ) const
{
  psegs.data().reserve( segs.size() );

  if ( 0 == psegs.packingVersion() )
  {
    for ( DataVector::const_iterator iD = segs.begin();
          iD != segs.end(); ++iD )
    {
      const Data & seg = **iD;
      psegs.data().push_back( PackedData() );
      PackedData & pseg = psegs.data().back();

      pseg.key = seg.key();

      pseg.history = seg.historyCode();

      pseg.trajPx.reserve( seg.trajectoryPoints().size() );
      pseg.trajPy.reserve( seg.trajectoryPoints().size() );
      pseg.trajPz.reserve( seg.trajectoryPoints().size() );
      for ( std::vector<Gaudi::XYZPoint>::const_iterator iT = seg.trajectoryPoints().begin();
            iT != seg.trajectoryPoints().end(); ++iT )
      {
        pseg.trajPx.push_back( m_pack.position((*iT).x()) );
        pseg.trajPy.push_back( m_pack.position((*iT).y()) );
        pseg.trajPz.push_back( m_pack.position((*iT).z()) );
      }

      pseg.trajMx.reserve( seg.trajectoryMomenta().size() );
      pseg.trajMy.reserve( seg.trajectoryMomenta().size() );
      pseg.trajMz.reserve( seg.trajectoryMomenta().size() );
      for ( std::vector<Gaudi::XYZVector>::const_iterator iM = seg.trajectoryMomenta().begin();
            iM != seg.trajectoryMomenta().end(); ++iM )
      {
        pseg.trajMx.push_back( m_pack.energy((*iM).x()) );
        pseg.trajMy.push_back( m_pack.energy((*iM).y()) );
        pseg.trajMz.push_back( m_pack.energy((*iM).z()) );
      }

      if ( NULL != seg.mcParticle() )
      {
        pseg.mcParticle  = m_pack.reference( &psegs,
                                             seg.mcParticle()->parent(),
                                             seg.mcParticle()->key() );
      }

      if ( NULL != seg.mcRichTrack() )
      {
        pseg.mcRichTrack = m_pack.reference( &psegs,
                                             seg.mcRichTrack()->parent(),
                                             seg.mcRichTrack()->key() );
      }

      pseg.mcPhotons.reserve( seg.mcRichOpticalPhotons().size() );
      for ( SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator iP =
              seg.mcRichOpticalPhotons().begin();
            iP != seg.mcRichOpticalPhotons().end(); ++iP )
      {
        pseg.mcPhotons.push_back( m_pack.reference( &psegs,
                                                    (*iP)->parent(),
                                                    (*iP)->key() ) );
      }

      pseg.mcHits.reserve( seg.mcRichHits().size() );
      for ( SmartRefVector<LHCb::MCRichHit>::const_iterator iH = seg.mcRichHits().begin();
            iH != seg.mcRichHits().end(); ++iH )
      {
        pseg.mcHits.push_back( m_pack.reference( &psegs,
                                                 (*iH)->parent(),
                                                 (*iH).linkID() ) );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)psegs.packingVersion();
    throw GaudiException( mess.str(), "MCRichSegmentPacker", StatusCode::FAILURE );
  }
}

void MCRichSegmentPacker::unpack( const PackedDataVector & psegs,
                                  DataVector       & segs ) const
{
  segs.reserve( psegs.data().size() );

  if ( 0 == psegs.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = psegs.data().begin();
          iD != psegs.data().end(); ++iD )
    {
      const PackedData & pseg = *iD;
      Data * seg  = new Data();
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
        m_pack.hintAndKey( pseg.mcParticle, &psegs, &segs, hintID, key );
        SmartRef<LHCb::MCParticle> refa(&segs,hintID,key);
        seg->setMcParticle( refa );
      }

      if ( -1 != pseg.mcRichTrack )
      {
        m_pack.hintAndKey( pseg.mcRichTrack, &psegs, &segs, hintID, key );
        SmartRef<LHCb::MCRichTrack> refb(&segs,hintID,key);
        seg->setMCRichTrack( refb );
      }

      for ( std::vector<int>::const_iterator iP = pseg.mcPhotons.begin();
            iP != pseg.mcPhotons.end(); ++iP )
      {
        m_pack.hintAndKey( *iP, &psegs, &segs, hintID, key );
        SmartRef<LHCb::MCRichOpticalPhoton> ref(&segs,hintID,key);
        seg->addToMCRichOpticalPhotons( ref );
      }

      for ( std::vector<int>::const_iterator iH = pseg.mcHits.begin();
            iH != pseg.mcHits.end(); ++iH )
      {
        m_pack.hintAndKey( *iH, &psegs, &segs, hintID, key );
        SmartRef<LHCb::MCRichHit> ref(&segs,hintID,key);
        seg->addToMCRichHits( ref );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)psegs.packingVersion();
    throw GaudiException( mess.str(), "MCRichSegmentPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichSegmentPacker::check( const DataVector & dataA,
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
    // History code
    ok &= (*iA)->historyCode() == (*iB)->historyCode();
    // Trajectory points
    bool sameSizeTrajP = (*iA)->trajectoryPoints().size() == (*iB)->trajectoryPoints().size();
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
    const bool sameSizeTrajM = (*iA)->trajectoryMomenta().size() == (*iB)->trajectoryMomenta().size();
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
    ok &= (*iA)->mcParticle() == (*iB)->mcParticle();
    // MCRichTrack
    ok &= (*iA)->mcRichTrack() == (*iB)->mcRichTrack();
    // MCPhotons
    const bool sameSizePhots = (*iA)->mcRichOpticalPhotons().size() == (*iB)->mcRichOpticalPhotons().size();
    ok &= sameSizePhots;
    if ( sameSizePhots )
    {
      SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator jA((*iA)->mcRichOpticalPhotons().begin());
      SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator jB((*iB)->mcRichOpticalPhotons().begin());
      for ( ; jA != (*iA)->mcRichOpticalPhotons().end() && jB != (*iB)->mcRichOpticalPhotons().end();
            ++jA, ++jB )
      {
        ok &= *jA == *jB;
      }
    }
    // MCHits
    const bool sameSizeHits = (*iA)->mcRichHits().size() == (*iB)->mcRichHits().size();
    ok &= sameSizeHits;
    if ( sameSizeHits )
    {
      SmartRefVector<LHCb::MCRichHit>::const_iterator jA((*iA)->mcRichHits().begin());
      SmartRefVector<LHCb::MCRichHit>::const_iterator jB((*iB)->mcRichHits().begin());
      for ( ; jA != (*iA)->mcRichHits().end() && jB != (*iB)->mcRichHits().end();
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
      parent.warning() << "Problem with MCRichSegment data packing :-" << endmsg
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
