// $Id: PackedRichPID.cpp,v 1.4 2009-11-07 12:20:27 jonrob Exp $

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
  if ( 0 == ppids.packingVersion() )
  {
    for ( DataVector::const_iterator iD = pids.begin();
          iD != pids.end(); ++iD )
    {
      Data & pid = **iD;
      ppids.data().push_back( PackedData() );
      PackedData & ppid = ppids.data().back();
      ppid.pidResultCode = (int)pid.pidResultCode();
      ppid.llValues = pid.particleLLValues();
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
    mess << "Unknown packed data version " << ppids.packingVersion();
    throw GaudiException( mess.str(), "RichPIDPacker", StatusCode::FAILURE );
  }
}

void RichPIDPacker::unpack( const PackedDataVector & ppids,
                            DataVector       & pids ) const
{
  pids.reserve( ppids.data().size() );
  if ( 0 == ppids.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = ppids.data().begin();
          iD != ppids.data().end(); ++iD )
    {
      const PackedData & ppid = *iD;
      // make and save new pid in container
      Data * pid  = new Data();
      pids.add( pid );
      // Fill data from packed object
      pid->setPidResultCode( ppid.pidResultCode );
      pid->setParticleLLValues( ppid.llValues );
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
    mess << "Unknown packed data version " << ppids.packingVersion();
    throw GaudiException( mess.str(), "RichPIDPacker", StatusCode::FAILURE );
  }
}

StatusCode RichPIDPacker::check( const DataVector & dataA,
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
    // History code
    ok &= (*iA)->pidResultCode() == (*iB)->pidResultCode();
    // Track reference
    ok &= (*iA)->track() == (*iB)->track();
    // DLLs (do when packed)
    ok &= (*iA)->particleLLValues().size() == (*iB)->particleLLValues().size();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with RichPID data packing :-" << endmsg
                       << "  Original PID : " << **iA
                       << endmsg
                       << "  Unpacked PID : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
