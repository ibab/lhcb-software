// $Id: PackedMCRichHit.cpp,v 1.5 2010-04-11 14:27:14 jonrob Exp $

// local
#include "Event/PackedMCRichHit.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichHitPacker::pack( const DataVector & hits,
                            PackedDataVector & phits ) const
{
  const char ver = phits.packingVersion();
  if ( 0 == ver || 1 == ver )
  {
    phits.data().reserve( hits.size() );
    for ( const auto * hit : hits )
    {
      phits.data().emplace_back( PackedData() );
      auto & phit = phits.data().back();
      phit.x          = m_pack.position ( hit->entry().x() );
      phit.y          = m_pack.position ( hit->entry().y() );
      phit.z          = m_pack.position ( hit->entry().z() );
      phit.energy     = m_pack.energy   ( hit->energy()    );
      phit.tof        = m_pack.time     ( hit->timeOfFlight() );
      phit.sensDetID  = hit->sensDetID().key();
      phit.history    = hit->historyCode();
      if ( hit->mcParticle() )
      {
        phit.mcParticle = ( UNLIKELY( 0==ver ) ?
                            m_pack.reference32( &phits,
                                                hit->mcParticle()->parent(),
                                                hit->mcParticle()->key() ) :
                            m_pack.reference64( &phits,
                                                hit->mcParticle()->parent(),
                                                hit->mcParticle()->key() ) );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichHitPacker", StatusCode::FAILURE );
  }
}

void MCRichHitPacker::unpack( const PackedDataVector & phits,
                              DataVector             & hits ) const
{
  const char ver = phits.packingVersion();
  if ( 0 == ver || 1 == ver )
  {
    hits.reserve( phits.data().size() );
    for ( const auto & phit : phits.data() )
    {
      // make and save new hit in container
      auto * hit  = new Data();
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
        if ( ( 0!=ver && m_pack.hintAndKey64(phit.mcParticle,&phits,&hits,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(phit.mcParticle,&phits,&hits,hintID,key) ) )
        {
          SmartRef<LHCb::MCParticle> ref(&hits,hintID,key);
          hit->setMCParticle( ref );
        }
        else { parent().Error( "Corrupt MCRichHit MCParticle SmartRef detected." ).ignore(); }
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichHitPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichHitPacker::check( const DataVector & dataA,
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
    // Hit position
    ok &= ch.comparePoints( "Entry Point", (*iA)->entry(), (*iB)->entry() );
    // energy
    ok &= ch.compareEnergies( "Energy", (*iA)->energy(), (*iB)->energy() );
    // tof
    ok &= ch.compareDoubles( "TOF", (*iA)->timeOfFlight(), (*iB)->timeOfFlight() );
    // Detector ID
    ok &= ch.compareInts( "SensDetID", (*iA)->sensDetID(), (*iB)->sensDetID() );
    // History code
    ok &= ch.compareInts( "HistoryCode", (*iA)->historyCode(), (*iB)->historyCode() );
    // MCParticle reference
    ok &= ch.comparePointers( "MCParticle", (*iA)->mcParticle(), (*iB)->mcParticle() );

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCRichHit data packing :-" << endmsg
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
