
//-----------------------------------------------------------------------------
/** @file MuonPIDsFromProtoParticlesAlg.cpp
 *
 * Implementation file for algorithm MuonPIDsFromProtoParticlesAlg
 *
 * CVS Log :-
 * $Id: MuonPIDsFromProtoParticlesAlg.cpp,v 1.8 2007-07-13 15:25:24 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "MuonPIDsFromProtoParticlesAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonPIDsFromProtoParticlesAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPIDsFromProtoParticlesAlg::MuonPIDsFromProtoParticlesAlg( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputProtoParticles", m_protoPloc  = ProtoParticleLocation::Charged );
  declareProperty( "OutputMuonPIDs",      m_muonPIDloc = MuonPIDLocation::Default       );
  declareProperty( "InputMuonTracks",     m_muonTrackLoc = TrackLocation::Muon          );
}

//=============================================================================
// Destructor
//=============================================================================
MuonPIDsFromProtoParticlesAlg::~MuonPIDsFromProtoParticlesAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonPIDsFromProtoParticlesAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Creating MuonPIDs '" << m_muonPIDloc << "' from ProtoParticles at '"
         << m_protoPloc << "'" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonPIDsFromProtoParticlesAlg::execute()
{

  // check data is not already there
  if ( exist<MuonPIDs>( m_muonPIDloc ) )
  {
    return Warning( "Data already exists at " + m_muonPIDloc +
                    " -> Will NOT replace", StatusCode::SUCCESS );
  }

  // load ProtoParticles
  const ProtoParticles * protos = get<ProtoParticles>( m_protoPloc );

  // Are Muon Tracks available ?
  const Tracks * muonTracks = ( !exist<Tracks>(m_muonTrackLoc) ? NULL :
                                get<Tracks>(m_muonTrackLoc) );
  if ( !muonTracks )
  {
    Warning( "Muon Tracks unavailable at " + m_muonTrackLoc, StatusCode::SUCCESS );
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Found " << muonTracks->size() << " Muon Tracks at " << m_muonTrackLoc << endreq;
  }

  // new MuonPID container
  MuonPIDs * mpids = new MuonPIDs();
  put( mpids, m_muonPIDloc );

  // loop over protos and re-create RichPIDs
  for ( ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {

    // get Track pointer
    const Track * track = (*iP)->track();
    if ( !track )
    {
      Warning( "Charged ProtoParticle has NULL Track pointer" );
      continue;
    }

    // does this proto have any Muon info in it ?
    if ( (*iP)->hasInfo(ProtoParticle::MuonPIDStatus) )
    {

      // new MuonPID
      MuonPID * pid = new MuonPID();

      // Add to container with same key as Track
      mpids->insert( pid, track->key() );

      // make sure proto points to this MuonPID
      (*iP)->setMuonPID( pid );

      // copy info

      // history word
      pid->setStatus( static_cast<unsigned int>((*iP)->info(ProtoParticle::MuonPIDStatus,0)) );

      // Track Pointer
      pid->setIDTrack( track );

      // PID info
      pid->setMuonLLMu( (*iP)->info(ProtoParticle::MuonMuLL,    0) );
      pid->setMuonLLBg( (*iP)->info(ProtoParticle::MuonBkgLL,   0) );
      pid->setNShared ( static_cast<int>((*iP)->info(ProtoParticle::MuonNShared, 0)) );

      // Muon Track (if it exists, it will have same key as primary track)
      Track * muonTrack = ( NULL != muonTracks ? muonTracks->object(track->key()) : NULL );
      if ( muonTrack )
      {
        if ( msgLevel(MSG::DEBUG) )
        { debug() << "Adding MuonTrack " << muonTrack->key() << " to MuonPID object" << endreq; }
        pid->setMuonTrack( muonTrack );
      }

    } // has muon info

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << mpids->size() << " MuonPIDs at " << m_muonPIDloc << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
