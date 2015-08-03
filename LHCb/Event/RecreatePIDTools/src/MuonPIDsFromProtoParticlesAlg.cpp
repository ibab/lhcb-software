
//-----------------------------------------------------------------------------
/** @file MuonPIDsFromProtoParticlesAlg.cpp
 *
 * Implementation file for algorithm MuonPIDsFromProtoParticlesAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "MuonPIDsFromProtoParticlesAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonPIDsFromProtoParticlesAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPIDsFromProtoParticlesAlg::MuonPIDsFromProtoParticlesAlg( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPloc  = ProtoParticleLocation::HltCharged;
    m_muonPIDloc = MuonPIDLocation::Hlt;
    m_muonTrackLoc = TrackLocation::Muon; // CRJ : What for HLT ???
  }
  else
  {
    m_protoPloc    = ProtoParticleLocation::Charged;
    m_muonPIDloc   = MuonPIDLocation::Default;
    m_muonTrackLoc = TrackLocation::Muon;
  }
  declareProperty( "InputProtoParticles", m_protoPloc    );
  declareProperty( "OutputMuonPIDs",      m_muonPIDloc   );
  declareProperty( "InputMuonTracks",     m_muonTrackLoc );
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
    debug() << "Data already exists at '" << m_muonPIDloc
            << "' -> Will NOT replace" << endmsg;
    return StatusCode::SUCCESS;
  }

  // new MuonPID container
  MuonPIDs * mpids = new MuonPIDs();
  put( mpids, m_muonPIDloc );

  // Do ProtoParticles exist ?
  const ProtoParticles * protos = getIfExists<ProtoParticles>( m_protoPloc );
  if ( protos )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Found " << protos->size() << " ProtoParticles at " << m_protoPloc << endmsg;
    }

    // Are Muon Tracks available ?
    const Tracks * muonTracks = getIfExists<Tracks>(m_muonTrackLoc);
    if ( NULL == muonTracks )
    {
      Warning( "Muon Tracks unavailable at " + m_muonTrackLoc, StatusCode::SUCCESS ).ignore();
    }
    else if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Found " << muonTracks->size() << " Muon Tracks at " << m_muonTrackLoc << endmsg;
    }

    // loop over protos and re-create RichPIDs
    for ( ProtoParticles::const_iterator iP = protos->begin();
          iP != protos->end(); ++iP )
    {

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Trying ProtoParticle " << (*iP)->key() << endmsg;
      }

      // get Track pointer
      const Track * track = (*iP)->track();
      if ( !track )
      {
        Warning( "Charged ProtoParticle has NULL Track pointer" ).ignore();
        continue;
      }

      // does this proto have any Muon info in it ?
      if ( (*iP)->hasInfo(ProtoParticle::MuonPIDStatus) )
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " -> Has MuonPID information" << endmsg;
        }

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

        // Work around for old MuonPID data objects without IsMuonLoose
        if ( !pid->IsMuonLoose()  && pid->IsMuon() ) { pid->setIsMuonLoose(true); }
        // Work around for old MuonPID data objects without InAcceptance
        if ( !pid->InAcceptance() && pid->IsMuon() ) { pid->setInAcceptance(true); }

        // Muon Track (if it exists, it will have same key as primary track)
        Track * muonTrack = ( NULL != muonTracks ? muonTracks->object(track->key()) : NULL );
        if ( muonTrack )
        {
          if ( msgLevel(MSG::DEBUG) )
          { debug() << " -> Adding MuonTrack " << muonTrack->key() << endmsg; }
          pid->setMuonTrack( muonTrack );
        }

      } // has muon info

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << mpids->size() << " MuonPIDs at " << m_muonPIDloc << endmsg;
    }

  }
  else
  {
    Warning( "No ProtoParticles at '" + m_protoPloc + 
             "' -> Empty MuonPIDs created at '" + m_muonPIDloc + "'" ).ignore();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
