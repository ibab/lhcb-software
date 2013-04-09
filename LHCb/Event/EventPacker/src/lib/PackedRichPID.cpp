
// local
#include "Event/PackedRichPID.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void RichPIDPacker::pack( const DataVector & pids,
                          PackedDataVector & ppids ) const
{
  ppids.data().reserve( pids.size() );
  if ( 2 == ppids.packingVersion() ||
       1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( DataVector::const_iterator iD = pids.begin();
          iD != pids.end(); ++iD )
    {
      const Data & pid = **iD;
      ppids.data().push_back( PackedData() );
      PackedData & ppid = ppids.data().back();
      // fill data
      ppid.key   = pid.key();
      ppid.pidResultCode = (int)pid.pidResultCode();
      ppid.dllEl = m_pack.deltaLL( pid.particleDeltaLL(Rich::Electron)       );
      ppid.dllMu = m_pack.deltaLL( pid.particleDeltaLL(Rich::Muon)           );
      ppid.dllPi = m_pack.deltaLL( pid.particleDeltaLL(Rich::Pion)           );
      ppid.dllKa = m_pack.deltaLL( pid.particleDeltaLL(Rich::Kaon)           );
      ppid.dllPr = m_pack.deltaLL( pid.particleDeltaLL(Rich::Proton)         );
      if ( ppids.packingVersion() > 0 )
        ppid.dllBt = m_pack.deltaLL( pid.particleDeltaLL(Rich::BelowThreshold) );
      if ( NULL != pid.track() )
      {
        ppid.track = m_pack.reference( &ppids,
                                       pid.track()->parent(),
                                       pid.track()->key() );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "RichPIDPacker", StatusCode::FAILURE );
  }
}

void RichPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector       & pids ) const
{
  pids.reserve( ppids.data().size() );
  if ( 2 == ppids.packingVersion() ||
       1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( PackedDataVector::Vector::const_iterator iD = ppids.data().begin();
          iD != ppids.data().end(); ++iD )
    {
      const PackedData & ppid = *iD;
      // make and save new pid in container
      Data * pid  = new Data();
      if ( ppids.packingVersion() < 2 ) { pids.add( pid ); }
      else                 { pids.insert( pid, ppid.key ); }
      // Fill data from packed object
      pid->setPidResultCode( ppid.pidResultCode );
      pid->setParticleDeltaLL( Rich::Electron,  (float)m_pack.deltaLL(ppid.dllEl) );
      pid->setParticleDeltaLL( Rich::Muon,      (float)m_pack.deltaLL(ppid.dllMu) );
      pid->setParticleDeltaLL( Rich::Pion,      (float)m_pack.deltaLL(ppid.dllPi) );
      pid->setParticleDeltaLL( Rich::Kaon,      (float)m_pack.deltaLL(ppid.dllKa) );
      pid->setParticleDeltaLL( Rich::Proton,    (float)m_pack.deltaLL(ppid.dllPr) );
      if ( ppids.packingVersion() > 0 )
        pid->setParticleDeltaLL( Rich::BelowThreshold, (float)m_pack.deltaLL(ppid.dllBt) );
      if ( -1 != ppid.track )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppid.track, &ppids, &pids, hintID, key );
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid->setTrack( ref );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "RichPIDPacker", StatusCode::FAILURE );
  }
}

StatusCode RichPIDPacker::check( const DataVector & dataA,
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
    // key
    ok &= (*iA)->key() == (*iB)->key();
    // History code
    ok &= (*iA)->pidResultCode() == (*iB)->pidResultCode();
    // Track reference
    ok &= (*iA)->track() == (*iB)->track();
    // DLLs
    ok &= (*iA)->particleLLValues().size() == (*iB)->particleLLValues().size();
    std::vector<float>::const_iterator iLLA((*iA)->particleLLValues().begin());
    std::vector<float>::const_iterator iLLB((*iB)->particleLLValues().begin());
    for ( ; iLLA != (*iA)->particleLLValues().end() && iLLB != (*iB)->particleLLValues().end();
          ++iLLA, ++iLLB )
    {
      ok &= ch.compareDoubles( "Delta(LL)", *iLLA, *iLLB );
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      const std::string loc = ( dataA.registry() ?
                                dataA.registry()->identifier() : "Not in TES" );
      parent().warning() << "Problem with RichPID data packing :-" << endmsg
                         << "  Original PID key=" << (**iA).key() 
                         << " in '" << loc << "'" << endmsg
                         << **iA << endmsg
                         << "  Unpacked PID" << endmsg
                         << **iB << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
