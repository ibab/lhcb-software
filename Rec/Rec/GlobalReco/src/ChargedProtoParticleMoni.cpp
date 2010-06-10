
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMoni.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleMoni
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleMoni.cpp,v 1.5 2009-09-03 11:09:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoParticleMoni.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleMoni );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleMoni::ChargedProtoParticleMoni( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : GaudiHistoAlg  ( name , pSvcLocator ),
    m_nEvts        ( 0 )
{
  // histo base dir
  setProperty ( "HistoTopDir", "PROTO/" );

  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_tracksPath = "";
    m_protoPath  = "";
  }
  else
  {
    m_tracksPath = LHCb::TrackLocation::Default;
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  }

  // Input data
  declareProperty( "InputTrackLocation", m_tracksPath );

  // output data
  declareProperty( "OutputProtoParticleLocation", m_protoPath );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleMoni::~ChargedProtoParticleMoni() {}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoParticleMoni::finalize()
{
  // summary printout
  printStats(MSG::INFO);

  // execute base class finalise and return
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleMoni::execute()
{
  using namespace Gaudi::Units;

  // Load the Track objects
  const LHCb::Tracks * tracks = get<LHCb::Tracks>(m_tracksPath);

  // Load the ProtoParticles
  const LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>(m_protoPath);

  // count events
  ++m_nEvts;

  // Loop over tracks
  for ( LHCb::Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {
    // track tally
    TrackTally & tally = m_nTracks[ (*iTrack)->type() ];

    // count all tracks
    ++tally.totTracks;

    // Does this track have a proto (uses same ey convention) ?
    LHCb::ProtoParticle * proto = protos->object((*iTrack)->key());
    profile1D( (*iTrack)->p(), 100.0 * (int)(NULL != proto),
               "% Tracks with ProtoParticles V Momentum", 0*GeV, 100*GeV, 100 );

    // Proceeed with tracks with ProtoParticles
    if ( !proto ) continue;
    ++tally.selTracks;

    // Printout ProtoParticles
    if ( msgLevel(MSG::VERBOSE) ) { verbose() << *proto << endmsg; }

    // RICH info
    const bool hasRICH = proto->hasInfo(LHCb::ProtoParticle::RichPIDStatus);
    if ( hasRICH ) { ++tally.richTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasRICH,
               "% ProtoParticles with RICH info V Momentum", 0*GeV, 100*GeV, 100 );

    // Muon INFO
    const bool hasMUON = proto->hasInfo(LHCb::ProtoParticle::MuonPIDStatus);
    if ( hasMUON ) { ++tally.muonTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasMUON,
               "% ProtoParticles with MUON info V Momentum", 0*GeV, 100*GeV, 100 );

    // ECAL info
    const bool hasECAL = proto->hasInfo(LHCb::ProtoParticle::InAccEcal);
    if ( hasECAL ) { ++tally.ecalTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasECAL,
               "% ProtoParticles with CALO-ECAL info V Momentum", 0*GeV, 100*GeV, 100 );

    // Brem info
    const bool hasBREM = proto->hasInfo(LHCb::ProtoParticle::InAccBrem);
    if ( hasBREM ) { ++tally.bremTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasBREM,
               "% ProtoParticles with CALO-BREM info V Momentum", 0*GeV, 100*GeV, 100 );

    // Spd info
    const bool hasSPD = proto->hasInfo(LHCb::ProtoParticle::InAccSpd);
    if ( hasSPD ) { ++tally.spdTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasSPD,
               "% ProtoParticles with CALO-SPD info V Momentum", 0*GeV, 100*GeV, 100 );

    // PRS info
    const bool hasPRS = proto->hasInfo(LHCb::ProtoParticle::InAccPrs);
    if ( hasPRS ) { ++tally.prsTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasPRS,
               "% ProtoParticles with CALO-PRS info V Momentum", 0*GeV, 100*GeV, 100 );

    // Add Hcal info
    const bool hasHCAL = proto->hasInfo(LHCb::ProtoParticle::InAccHcal);
    if ( hasHCAL ) { ++tally.hcalTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasHCAL,
               "% ProtoParticles with CALO-HCAL info V Momentum", 0*GeV, 100*GeV, 100 );

    // Add Velo dE/dx info
    const bool hasDEDX = proto->hasInfo(LHCb::ProtoParticle::VeloCharge);
    if ( hasDEDX ) { ++tally.velodEdxTracks; }
    profile1D( (*iTrack)->p(), 100.0 * (int)hasDEDX,
               "% ProtoParticles with VELO dE/dx info V Momentum", 0*GeV, 100*GeV, 100 );

  }

  // return
  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Print Stats
//=============================================================================
void ChargedProtoParticleMoni::printStats( const MSG::Level level ) const
{
  MsgStream & msg = msgStream(level);

  // Statistical tools
  const Rich::PoissonEffFunctor eff("%5.1f +-%4.1f");
  const Rich::StatDivFunctor    occ("%6.2f +-%5.2f");

  const std::string lines(148,'-');
  const std::string LINES(148,'=');

  msg << LINES << endmsg;
  msg << " # Tracks Per Event         |    Track     |     RICH         MUON         ECAL         "
      << "BREM         SPD          PRS          HCAL     VELO(dE/dx)" << endmsg;
  msg << lines << endmsg;
  for ( TrackMap::const_iterator iT = m_nTracks.begin();
        iT != m_nTracks.end(); ++iT )
  {
    const TrackTally & tally = (*iT).second;
    msg << " " << occ(tally.selTracks,m_nEvts);
    msg << boost::format(" %|-12|") % (*iT).first;
    msg << "| " << eff(tally.selTracks,tally.totTracks) << " |";
    msg << eff(tally.richTracks,tally.selTracks) << " ";
    msg << eff(tally.muonTracks,tally.selTracks) << " ";
    msg << eff(tally.ecalTracks,tally.selTracks) << " ";
    msg << eff(tally.bremTracks,tally.selTracks) << " ";
    msg << eff(tally.spdTracks,tally.selTracks)  << " ";
    msg << eff(tally.prsTracks,tally.selTracks)  << " ";
    msg << eff(tally.hcalTracks,tally.selTracks) << " ";
    msg << eff(tally.velodEdxTracks,tally.selTracks);
    msg << endmsg;
  }
  msg << LINES << endmsg;

}
