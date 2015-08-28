
// local
#include "Event/PackedRichPID.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void RichPIDPacker::pack( const Data & pid,
                          PackedData & ppid,
                          PackedDataVector & ppids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    ppid.pidResultCode = (int)pid.pidResultCode();
    ppid.dllEl = m_pack.deltaLL( pid.particleDeltaLL(Rich::Electron) );
    ppid.dllMu = m_pack.deltaLL( pid.particleDeltaLL(Rich::Muon)     );
    ppid.dllPi = m_pack.deltaLL( pid.particleDeltaLL(Rich::Pion)     );
    ppid.dllKa = m_pack.deltaLL( pid.particleDeltaLL(Rich::Kaon)     );
    ppid.dllPr = m_pack.deltaLL( pid.particleDeltaLL(Rich::Proton)   );
    if ( ver > 0 ) ppid.dllBt = m_pack.deltaLL( pid.particleDeltaLL(Rich::BelowThreshold) );
    if ( NULL != pid.track() )
    {
      ppid.track = ( UNLIKELY( ver < 3 ) ?
                     m_pack.reference32( &ppids,
                                         pid.track()->parent(),
                                         pid.track()->key() ) :
                     m_pack.reference64( &ppids,
                                         pid.track()->parent(),
                                         pid.track()->key() ) );
    }
  }
}

void RichPIDPacker::pack( const DataVector & pids,
                          PackedDataVector & ppids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    ppids.data().reserve( pids.size() );
    for ( const Data * pid : pids )
    {
      // create new packed PID object in container and get reference to it.
      ppids.data().push_back( PackedData() );
      PackedData & ppid = ppids.data().back();
      // fill the key
      ppid.key = pid->key();
      // fill the rest of the data
      pack( *pid, ppid, ppids );
    }
  }
}

void RichPIDPacker::unpack( const PackedData       & ppid,
                            Data                   & pid,
                            const PackedDataVector & ppids,
                            DataVector             & pids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    pid.setPidResultCode( ppid.pidResultCode );
    pid.setParticleDeltaLL( Rich::Electron,  (float)m_pack.deltaLL(ppid.dllEl) );
    pid.setParticleDeltaLL( Rich::Muon,      (float)m_pack.deltaLL(ppid.dllMu) );
    pid.setParticleDeltaLL( Rich::Pion,      (float)m_pack.deltaLL(ppid.dllPi) );
    pid.setParticleDeltaLL( Rich::Kaon,      (float)m_pack.deltaLL(ppid.dllKa) );
    pid.setParticleDeltaLL( Rich::Proton,    (float)m_pack.deltaLL(ppid.dllPr) );
    if ( ver > 0 ) pid.setParticleDeltaLL( Rich::BelowThreshold,
                                           (float)m_pack.deltaLL(ppid.dllBt) );
    if ( -1 != ppid.track )
    {
      int hintID(0), key(0);
      if ( ( ver >= 3 && m_pack.hintAndKey64(ppid.track,&ppids,&pids,hintID,key) ) ||
           ( ver <  3 && m_pack.hintAndKey32(ppid.track,&ppids,&pids,hintID,key) ) )
      {
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid.setTrack( ref );
      }
      else { parent().Error( "Corrupt RichPID Track SmartRef detected." ).ignore(); }
    }
  }
}

void RichPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector       & pids ) const
{
  const char ver = ppids.packingVersion();
  if ( isSupportedVer(ver) )
  {
    pids.reserve( ppids.data().size() );
    for ( const PackedData & ppid : ppids.data() )
    {
      // make and save new pid in container
      Data * pid  = new Data();
      if ( ver < 2 ) { pids.add( pid ); }
      else           { pids.insert( pid, ppid.key ); }
      // Fill data from packed object
      unpack( ppid, *pid, ppids, pids );
    }
  }
}

StatusCode RichPIDPacker::check( const Data & dataA,
                                 const Data & dataB ) const
{
  // checker
  const DataPacking::DataChecks ch(parent());

  // assume OK from the start
  bool ok = true;
  // key
  ok &= ch.compareInts( "Key", dataA.key(), dataB.key() );
  // History code
  ok &= ch.compareInts( "PIDCode", dataA.pidResultCode(), dataB.pidResultCode() );
  // Track reference
  ok &= ch.comparePointers( "Track", dataA.track(), dataB.track() );
  // DLLs
  ok &= dataA.particleLLValues().size() == dataB.particleLLValues().size();
  std::vector<float>::const_iterator iLLA(dataA.particleLLValues().begin());
  std::vector<float>::const_iterator iLLB(dataB.particleLLValues().begin());
  for ( ; iLLA != dataA.particleLLValues().end() && iLLB != dataB.particleLLValues().end();
        ++iLLA, ++iLLB )
  {
    ok &= ch.compareDoubles( "Delta(LL)", *iLLA, *iLLB );
  }

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.parent() && dataA.parent()->registry() ?
                              dataA.parent()->registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with RichPID data packing :-" << endmsg
                       << "  Original PID key=" << dataA.key()
                       << " in '" << loc << "'" << endmsg
                       << dataA << endmsg
                       << "  Unpacked PID" << endmsg
                       << dataB << endmsg;
  }

  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}

StatusCode RichPIDPacker::check( const DataVector & dataA,
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
