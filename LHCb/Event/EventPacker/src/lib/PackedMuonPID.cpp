
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
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    ppid.MuonLLMu = m_pack.deltaLL(pid.MuonLLMu());
    ppid.MuonLLBg = m_pack.deltaLL(pid.MuonLLBg());
    ppid.nShared  = (int)pid.nShared();
    ppid.status   = (int)pid.Status();
    if ( pid.idTrack() )
    {
      ppid.idtrack = ( UNLIKELY( 1 >= ver ) ?
                       m_pack.reference32( &ppids,
                                           pid.idTrack()->parent(),
                                           pid.idTrack()->key() ) :
                       m_pack.reference64( &ppids,
                                           pid.idTrack()->parent(),
                                           pid.idTrack()->key() ) );
    }
    if ( pid.muonTrack() )
    {
      ppid.mutrack = ( UNLIKELY( 1 >= ver ) ?
                       m_pack.reference32( &ppids,
                                           pid.muonTrack()->parent(),
                                           pid.muonTrack()->key() ) :
                       m_pack.reference64( &ppids,
                                           pid.muonTrack()->parent(),
                                           pid.muonTrack()->key() ) );
    }
  }
}

void MuonPIDPacker::pack( const DataVector & pids,
                          PackedDataVector & ppids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    ppids.data().reserve( pids.size() );
    for ( const Data * pid : pids )
    {
      ppids.data().emplace_back( PackedData() );
      auto & ppid = ppids.data().back();
      // save the key
      ppid.key = pid->key();
      // fill the rest of the data
      pack( *pid, ppid, ppids );
    }
  }
}

void MuonPIDPacker::unpack( const PackedData       & ppid,
                            Data                   & pid,
                            const PackedDataVector & ppids,
                            DataVector             & pids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    pid.setMuonLLMu( m_pack.deltaLL(ppid.MuonLLMu) );
    pid.setMuonLLBg( m_pack.deltaLL(ppid.MuonLLBg) );
    pid.setNShared( ppid.nShared );
    pid.setStatus( ppid.status );
    if ( -1 != ppid.idtrack )
    {
      int hintID(0), key(0);
      if ( ( 1 <  ver && m_pack.hintAndKey64(ppid.idtrack,&ppids,&pids,hintID,key) ) ||
           ( 1 >= ver && m_pack.hintAndKey32(ppid.idtrack,&ppids,&pids,hintID,key) ) )
      {
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid.setIDTrack( ref );
      }
      else { parent().Error( "Corrupt MuonPID Track SmartRef detected." ).ignore(); }
    }
    if ( -1 != ppid.mutrack )
    {
      int hintID(0), key(0);
      if ( ( 1 <  ver && m_pack.hintAndKey64(ppid.mutrack,&ppids,&pids,hintID,key) ) ||
           ( 1 >= ver && m_pack.hintAndKey32(ppid.mutrack,&ppids,&pids,hintID,key) ) )
      {
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid.setMuonTrack( ref );
      }
      else { parent().Error( "Corrupt MuonPID MuTrack SmartRef detected." ).ignore(); }
    }
  }
}

void MuonPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector             & pids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    pids.reserve( ppids.data().size() );
    for ( const PackedData & ppid : ppids.data() )
    {
      // make and save new pid in container
      Data * pid  = new Data();
      if ( 0 == ver ) { pids.add( pid ); }
      else            { pids.insert( pid, ppid.key ); }
      // Fill data from packed object
      unpack( ppid, *pid, ppids, pids );
    }
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
  ok &= ch.compareInts( "Key", dataA.key(), dataB.key() );
  // History code
  ok &= ch.compareInts( "Status", dataA.Status(), dataB.Status() );
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
