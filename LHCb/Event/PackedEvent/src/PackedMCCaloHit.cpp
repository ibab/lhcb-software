// $Id: PackedMCCaloHit.cpp,v 1.4 2009-11-07 12:20:27 jonrob Exp $

// local
#include "Event/PackedMCCaloHit.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCCaloHitPacker::pack( const DataVector & hits,
                            PackedDataVector & phits ) const
{
  phits.data().reserve( hits.size() );

  if ( 0 == phits.packingVersion() )
  {
    for ( DataVector::const_iterator iD = hits.begin();
          iD != hits.end(); ++iD )
    {
      Data & hit = **iD;
      phits.data().push_back( PackedData() );
      PackedData & phit = phits.data().back();
      phit.activeE      = m_pack.energy   ( hit.activeE() * m_energyScale );
      phit.sensDetID    = hit.sensDetID();
      phit.time         = hit.time();
      if ( NULL != hit.particle() )
      {
        phit.mcParticle = m_pack.reference( &phits,
                                            hit.particle()->parent(),
                                            hit.particle()->key() );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << phits.packingVersion();
    throw GaudiException( mess.str(), "MCCaloHitPacker", StatusCode::FAILURE );
  }
}

void MCCaloHitPacker::unpack( const PackedDataVector & phits,
                              DataVector       & hits ) const
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
      hit->setActiveE   ( m_pack.energy(phit.activeE) / m_energyScale );
      hit->setSensDetID ( phit.sensDetID );
      hit->setTime      ( phit.time );
      if ( -1 != phit.mcParticle )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( phit.mcParticle, &phits, &hits, hintID, key );
        SmartRef<LHCb::MCParticle> ref(&hits,hintID,key);
        hit->setParticle( ref );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << phits.packingVersion();
    throw GaudiException( mess.str(), "MCCaloHitPacker", StatusCode::FAILURE );
  }
}

StatusCode MCCaloHitPacker::check( const DataVector & dataA,
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
    // Detector ID
    ok &= (*iA)->sensDetID() == (*iB)->sensDetID();
    // energy
    ok &= ch.compareEnergies( "Energy", (*iA)->activeE(), (*iB)->activeE() );
    // tof
    ok &= ch.compareDoubles( "TOF", (*iA)->time(), (*iB)->time() );
    // MCParticle reference
    ok &= (*iA)->particle() == (*iB)->particle();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with MCCaloHit data packing :-" << endmsg
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
