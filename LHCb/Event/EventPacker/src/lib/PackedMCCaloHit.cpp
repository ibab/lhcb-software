// $Id: PackedMCCaloHit.cpp,v 1.5 2010-04-11 14:27:14 jonrob Exp $

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
  const char ver = phits.packingVersion();

  if ( 0 == ver || 1 == ver )
  {
    phits.data().reserve( hits.size() );
    for ( const Data * hit : hits )
    {
      phits.data().push_back( PackedData() );
      PackedData & phit = phits.data().back();
      phit.activeE      = m_pack.energy   ( hit->activeE() * m_energyScale );
      phit.sensDetID    = hit->sensDetID();
      phit.time         = hit->time();
      if ( NULL != hit->particle() )
      {
        phit.mcParticle = ( UNLIKELY( 0==ver ) ? 
                            m_pack.reference32( &phits,
                                                hit->particle()->parent(),
                                                hit->particle()->key() ) :
                            m_pack.reference64( &phits,
                                                hit->particle()->parent(),
                                                hit->particle()->key() ) );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCCaloHitPacker", StatusCode::FAILURE );
  }
}

void MCCaloHitPacker::unpack( const PackedDataVector & phits,
                              DataVector       & hits ) const
{
  const char ver = phits.packingVersion();
  if ( 0 == ver || 1 == ver )
  {
    hits.reserve( phits.data().size() );
    for ( const PackedData & phit : phits.data() )
    {
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
        if ( ( 0!=ver && m_pack.hintAndKey64(phit.mcParticle,&phits,&hits,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(phit.mcParticle,&phits,&hits,hintID,key) ) )
        {
          SmartRef<LHCb::MCParticle> ref(&hits,hintID,key);
          hit->setParticle( ref );
        }
        else { parent().Error( "Corrupt MCCaloHit MCParticle SmartRef detected." ).ignore(); }
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCCaloHitPacker", StatusCode::FAILURE );
  }
}

StatusCode MCCaloHitPacker::check( const DataVector & dataA,
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
    // Detector ID
    ok &= ch.compareInts( "SensDetID", (*iA)->sensDetID(), (*iB)->sensDetID() );
    // energy
    ok &= ch.compareEnergies( "Energy", (*iA)->activeE(), (*iB)->activeE() );
    // tof
    ok &= ch.compareDoubles( "TOF", (*iA)->time(), (*iB)->time() );
    // MCParticle reference
    ok &= ch.comparePointers( "MCParticle", (*iA)->particle(), (*iB)->particle() );

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCCaloHit data packing :-" << endmsg
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
