// $Id: PackedMuonPID.cpp,v 1.4 2010-04-11 14:27:15 jonrob Exp $

// local
#include "Event/PackedMuonPID.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MuonPIDPacker::pack( const Data & pid,
                          PackedData & ppid,
                          PackedDataVector & ppids ) const
{
  ppid.MuonLLMu = m_pack.deltaLL(pid.MuonLLMu());
  ppid.MuonLLBg = m_pack.deltaLL(pid.MuonLLBg());
  ppid.nShared  = (int)pid.nShared();
  ppid.status   = (int)pid.Status();
  if ( NULL != pid.idTrack() )
  {
    ppid.idtrack = m_pack.reference( &ppids,
                                     pid.idTrack()->parent(),
                                     pid.idTrack()->key() );
  }
  if ( NULL != pid.muonTrack() )
  {
    ppid.mutrack = m_pack.reference( &ppids,
                                     pid.muonTrack()->parent(),
                                     pid.muonTrack()->key() );
  }
}

void MuonPIDPacker::pack( const DataVector & pids,
                          PackedDataVector & ppids ) const
{
  ppids.data().reserve( pids.size() );
  if ( 1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( const Data * pid : pids )
    {
      ppids.data().push_back( PackedData() );
      PackedData & ppid = ppids.data().back();
      // save the key
      ppid.key = pid->key();
      // fill the rest of the data
      pack( *pid, ppid, ppids );
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "MuonPIDPacker", StatusCode::FAILURE );
  }
}

void MuonPIDPacker::unpack( const PackedData       & ppid,
                            Data                   & pid,
                            const PackedDataVector & ppids,
                            DataVector             & pids ) const
{
  pid.setMuonLLMu( m_pack.deltaLL(ppid.MuonLLMu) );
  pid.setMuonLLBg( m_pack.deltaLL(ppid.MuonLLBg) );
  pid.setNShared( ppid.nShared );
  pid.setStatus( ppid.status );
  if ( -1 != ppid.idtrack )
  {
    int hintID(0), key(0);
    m_pack.hintAndKey( ppid.idtrack, &ppids, &pids, hintID, key );
    SmartRef<LHCb::Track> ref(&pids,hintID,key);
    pid.setIDTrack( ref );
  }
  if ( -1 != ppid.mutrack )
  {
    int hintID(0), key(0);
    m_pack.hintAndKey( ppid.mutrack, &ppids, &pids, hintID, key );
    SmartRef<LHCb::Track> ref(&pids,hintID,key);
    pid.setMuonTrack( ref );
  }
}

void MuonPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector             & pids ) const
{
  pids.reserve( ppids.data().size() );
  if ( 1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( const PackedData & ppid : ppids.data() )
    {
      // make and save new pid in container
      Data * pid  = new Data();
      if ( ppids.packingVersion() == 0 ) { pids.add( pid ); }
      else                  { pids.insert( pid, ppid.key ); }
      // Fill data from packed object
      unpack( ppid, *pid, ppids, pids );
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "MuonPIDPacker", StatusCode::FAILURE );
  }
}

StatusCode MuonPIDPacker::check( const Data & dataA,
                                 const Data & dataB ) const
{
  // checker
  const DataPacking::DataChecks ch(parent());

  // assume OK from the start
  bool ok = true;

  // key
  ok &= dataA.key() == dataB.key();
  // History code
  ok &= dataA.Status() == dataB.Status();
  // Track references
  ok &= ch.comparePointers( "Track", dataA.idTrack(), dataB.idTrack() );
  ok &= ch.comparePointers( "MuonTrack", dataA.muonTrack(), dataB.muonTrack() );
  // DLLs
  ok &= ch.compareDoubles( "MuonLLMu", dataA.MuonLLMu(), dataB.MuonLLMu() );
  ok &= ch.compareDoubles( "MuonLLBg", dataA.MuonLLBg(), dataB.MuonLLBg() );

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.parent() && dataA.parent()->registry() ?
                              dataA.parent()->registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with MuonPID data packing :-" << endmsg
                       << "  Original PID key=" << dataA.key()
                       << " in '" << loc << "'" << endmsg
                       << dataA << endmsg
                       << "  Unpacked PID" << endmsg
                       << dataB << endmsg;
  }

  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}

StatusCode MuonPIDPacker::check( const DataVector & dataA,
                                 const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    sc = sc && check( **iA, **iB );
  }

  // Return final status
  return sc;
}
