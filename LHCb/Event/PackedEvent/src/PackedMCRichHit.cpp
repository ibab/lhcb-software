// $Id: PackedMCRichHit.cpp,v 1.5 2010-04-11 14:27:14 jonrob Exp $

// local
#include "Event/PackedMCRichHit.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichHitPacker::pack( const DataVector  & hits,
                            PackedDataVector & phits ) const
{
  phits.data().reserve( hits.size() );
  if ( 0 == phits.packingVersion() )
  {
    for ( DataVector::const_iterator iD = hits.begin();
          iD != hits.end(); ++iD )
    {
      const Data & hit = **iD;
      phits.data().push_back( PackedData() );
      PackedData & phit = phits.data().back();
      phit.x          = m_pack.position ( hit.entry().x() );
      phit.y          = m_pack.position ( hit.entry().y() );
      phit.z          = m_pack.position ( hit.entry().z() );
      phit.energy     = m_pack.energy   ( hit.energy()    );
      phit.tof        = m_pack.time     ( hit.timeOfFlight() );
      phit.sensDetID  = hit.sensDetID().key();
      phit.history    = hit.historyCode();
      if ( NULL != hit.mcParticle() )
      {
        phit.mcParticle = m_pack.reference( &phits,
                                            hit.mcParticle()->parent(),
                                            hit.mcParticle()->key() );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)phits.packingVersion();
    throw GaudiException( mess.str(), "MCRichHitPacker", StatusCode::FAILURE );
  }
}

void MCRichHitPacker::unpack( const PackedDataVector & phits,
                              DataVector             & hits ) const
{
  hits.reserve( phits.data().size() );
  if ( 0 == phits.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = phits.data().begin();
          iD != phits.data().end(); ++iD )
    {
      const PackedData & phit = *iD;
      // make and save new hit in container
      Data * hit  = new Data();
      hits.add( hit );
      // Fill data from packed object
      hit->setEntry ( Gaudi::XYZPoint( m_pack.position(phit.x),
                                       m_pack.position(phit.y),
                                       m_pack.position(phit.z) ) );
      hit->setEnergy       ( m_pack.energy(phit.energy)        );
      hit->setTimeOfFlight ( m_pack.time(phit.tof)             );
      hit->setSensDetID    ( LHCb::RichSmartID(phit.sensDetID) );
      hit->setHistoryCode  ( phit.history                      );
      if ( -1 != phit.mcParticle )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( phit.mcParticle, &phits, &hits, hintID, key );
        SmartRef<LHCb::MCParticle> ref(&hits,hintID,key);
        hit->setMCParticle( ref );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)phits.packingVersion();
    throw GaudiException( mess.str(), "MCRichHitPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichHitPacker::check( const DataVector & dataA,
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
    // Hit position
    ok &= ch.comparePoints( "Entry Point", (*iA)->entry(), (*iB)->entry() );
    // energy
    ok &= ch.compareEnergies( "Energy", (*iA)->energy(), (*iB)->energy() );
    // tof
    ok &= ch.compareDoubles( "TOF", (*iA)->timeOfFlight(), (*iB)->timeOfFlight() );
    // Detector ID
    ok &= (*iA)->sensDetID() == (*iB)->sensDetID();
    // History code
    ok &= (*iA)->historyCode() == (*iB)->historyCode();
    // MCParticle reference
    ok &= (*iA)->mcParticle() == (*iB)->mcParticle();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with MCRichHit data packing :-" << endmsg
                       << "  Original Hit : " << **iA
                       << endmsg
                       << "  Unpacked Hit : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
