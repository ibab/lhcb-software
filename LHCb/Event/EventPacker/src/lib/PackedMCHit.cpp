
// STL
#include <sstream>

// local
#include "Event/PackedMCHit.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/GaudiException.h"

using namespace LHCb;

void MCHitPacker::pack( const DataVector & hits,
                        PackedDataVector & phits ) const
{
  const char ver = phits.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    phits.data().reserve( hits.size() );
    for ( const Data * hit : hits )
    {
      phits.data().push_back( PackedData() );
      PackedData & phit = phits.data().back();
      phit.sensDetID    = hit->sensDetID();
      phit.entx         = m_pack.position ( hit->entry().x() );
      phit.enty         = m_pack.position ( hit->entry().y() );
      phit.entz         = m_pack.position ( hit->entry().z() );
      phit.vtxx         = m_pack.position ( m_dispScale * hit->displacement().x() );
      phit.vtxy         = m_pack.position ( m_dispScale * hit->displacement().y() );
      phit.vtxz         = m_pack.position ( m_dispScale * hit->displacement().z() );
      phit.energy       = m_pack.energy   ( m_enScale   * hit->energy() );
      phit.tof          = m_pack.time     ( hit->time() );
      phit.mp           = m_pack.energy   ( hit->p() );
      if ( NULL != hit->mcParticle() )
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
    throw GaudiException( mess.str(), "MCHitPacker", StatusCode::FAILURE );
  }
}

void MCHitPacker::unpack( const PackedDataVector & phits,
                          DataVector       & hits ) const
{
  const char ver = phits.packingVersion();
  if ( 1 == ver || 0 == ver )
  {
    hits.reserve( phits.data().size() );
    for ( const PackedData & phit : phits.data() )
    {
      // make and save new hit in container
      Data * hit  = new Data();
      hits.add( hit );
      // Fill data from packed object
      hit->setSensDetID ( phit.sensDetID );
      hit->setEntry ( Gaudi::XYZPoint( m_pack.position(phit.entx),
                                       m_pack.position(phit.enty),
                                       m_pack.position(phit.entz) ) );
      hit->setDisplacement( Gaudi::XYZVector( m_pack.position(phit.vtxx)/m_dispScale,
                                              m_pack.position(phit.vtxy)/m_dispScale,
                                              m_pack.position(phit.vtxz)/m_dispScale ) );
      hit->setEnergy ( m_pack.energy(phit.energy)/m_enScale );
      hit->setTime   ( m_pack.time(phit.tof)                );
      hit->setP      ( m_pack.energy(phit.mp)               );
      if ( -1 != phit.mcParticle )
      {
        int hintID(0), key(0);
        if ( ( 0!=ver && m_pack.hintAndKey64(phit.mcParticle,&phits,&hits,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(phit.mcParticle,&phits,&hits,hintID,key) ) )
        {
          SmartRef<LHCb::MCParticle> ref(&hits,hintID,key);
          hit->setMCParticle( ref );
        }
        else { parent().Error( "Corrupt MCHit MCParticle SmartRef detected." ).ignore(); }
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCHitPacker", StatusCode::FAILURE );
  }
}

StatusCode MCHitPacker::check( const DataVector & dataA,
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
    // Hit position
    ok &= ch.comparePoints( "Entry Point", (*iA)->entry(), (*iB)->entry() );
    // displacement
    ok &= ch.compareVectors( "Displacement", (*iA)->displacement(), (*iB)->displacement() );
    // energy
    ok &= ch.compareEnergies( "Energy", (*iA)->energy(), (*iB)->energy() );
    // tof
    ok &= ch.compareDoubles( "TOF", (*iA)->time(), (*iB)->time() );
    // mp
    ok &= ch.compareEnergies( "Parent |P|", (*iA)->p(), (*iB)->p() );
    // MCParticle reference
    ok &= ch.comparePointers( "MCParticle", (*iA)->mcParticle(), (*iB)->mcParticle() );

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCHit data packing :-" << endmsg
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
