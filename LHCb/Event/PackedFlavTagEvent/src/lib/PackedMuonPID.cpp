// $Id: PackedMuonPID.cpp,v 1.4 2010-04-11 14:27:15 jonrob Exp $

// local
#include "Event/PackedMuonPID.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MuonPIDPacker::pack( const DataVector & pids,
                          PackedDataVector & ppids ) const
{
  ppids.data().reserve( pids.size() );
  if ( 1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( DataVector::const_iterator iD = pids.begin();
          iD != pids.end(); ++iD )
    {
      const Data & pid = **iD;
      ppids.data().push_back( PackedData() );
      PackedData & ppid = ppids.data().back();
      // fill data
      ppid.key      = pid.key();
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
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "MuonPIDPacker", StatusCode::FAILURE );
  }
}

void MuonPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector       & pids ) const
{
  pids.reserve( ppids.data().size() );
  if ( 1 == ppids.packingVersion() ||
       0 == ppids.packingVersion()  )
  {
    for ( PackedDataVector::Vector::const_iterator iD = ppids.data().begin();
          iD != ppids.data().end(); ++iD )
    {
      const PackedData & ppid = *iD;
      // make and save new pid in container
      Data * pid  = new Data();
      if ( ppids.packingVersion() == 0 ) { pids.add( pid ); }
      else                  { pids.insert( pid, ppid.key ); }
      // Fill data from packed object
      pid->setMuonLLMu( m_pack.deltaLL(ppid.MuonLLMu) );
      pid->setMuonLLBg( m_pack.deltaLL(ppid.MuonLLBg) );
      pid->setNShared( ppid.nShared );
      pid->setStatus( ppid.status );
      if ( -1 != ppid.idtrack )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppid.idtrack, &ppids, &pids, hintID, key );
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid->setIDTrack( ref );
      }
      if ( -1 != ppid.mutrack )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppid.mutrack, &ppids, &pids, hintID, key );
        SmartRef<LHCb::Track> ref(&pids,hintID,key);
        pid->setMuonTrack( ref );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ppids.packingVersion();
    throw GaudiException( mess.str(), "MuonPIDPacker", StatusCode::FAILURE );
  }
}

StatusCode MuonPIDPacker::check( const DataVector & dataA,
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
    ok &= (*iA)->Status() == (*iB)->Status();
    // Track references
    ok &= (*iA)->idTrack()   == (*iB)->idTrack();
    ok &= (*iA)->muonTrack() == (*iB)->muonTrack();
    // DLLs
    ok &= ch.compareDoubles( "MuonLLMu", (*iA)->MuonLLMu(), (*iB)->MuonLLMu() );
    ok &= ch.compareDoubles( "MuonLLBg", (*iA)->MuonLLBg(), (*iB)->MuonLLBg() );

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      const std::string loc = ( dataA.registry() ?
                                dataA.registry()->identifier() : "Not in TES" );
      parent().warning() << "Problem with MuonPID data packing :-" << endmsg
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
