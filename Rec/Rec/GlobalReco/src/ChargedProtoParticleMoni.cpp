
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMoni.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleMoni
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleMoni.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ChargedProtoParticleMoni::ChargedProtoParticleMoni( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
    : GaudiHistoAlg  ( name , pSvcLocator )
{
  // histo base dir
  setProperty ( "HistoTopDir", "PROTO/" );

  // Input data
  declareProperty( "InputTrackLocation", 
                   m_tracksPath = LHCb::TrackLocation::Default );

  // output data
  declareProperty( "OutputProtoParticleLocation", 
                   m_protoPath = LHCb::ProtoParticleLocation::Charged );
}

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
  const LHCb::Tracks * tracks = getIfExists<LHCb::Tracks>(m_tracksPath);
  if ( !tracks ) return Warning( "No Tracks at '" + m_tracksPath + "'", 
                                 StatusCode::SUCCESS );

  // Load the ProtoParticles
  const LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>(m_protoPath);
  if ( !protos ) return Warning( "No ProtoParticles at '" + m_protoPath + "'", 
                                 StatusCode::SUCCESS );
  
  // count events
  ++m_nEvts;

  // Loop over tracks
  for ( const LHCb::Track * track : *tracks )
  {
    // track tally
    auto& tally = trackTally( track->type() );

    // count all tracks
    ++tally.totTracks;

    // Does this track have a proto ?
    const LHCb::ProtoParticle * proto = getProto( protos, track );

    // Track Type
    std::string type = std::to_string( track->type() ) + "/";

    // Eff. for making ProtoParticles
    profile1D( track->p(), 100.0 * (int)(proto!=nullptr),
               type+"trackToProtoEff",
               "% Tracks with ProtoParticles V Momentum", 0*GeV, 100*GeV, 100 );

    // Proceed only with tracks with ProtoParticles
    if ( !proto ) continue;

    // count tracks with ProtoParticles
    ++tally.selTracks;

    // Printout ProtoParticles + PID info
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << "ProtoParticle : " << *proto << endmsg;
      if ( proto->richPID() ) { verbose() << " -> RichPID : " << *(proto->richPID()) << endmsg; }
      if ( proto->muonPID() ) { verbose() << " -> MuonPID : " << *(proto->muonPID()) << endmsg; }
    }

    // RICH info
    const bool hasRICH = proto->hasInfo(LHCb::ProtoParticle::RichPIDStatus);
    if ( hasRICH ) { ++tally.richTracks; }
    profile1D( track->p(), 100.0 * (int)hasRICH,
               type+"protosWithRICH",
               "% ProtoParticles with RICH info V Momentum", 0*GeV, 100*GeV, 100 );

    // Muon INFO
    const bool hasMUON = proto->hasInfo(LHCb::ProtoParticle::InAccMuon);
    if ( hasMUON ) { ++tally.muonTracks; }
    profile1D( track->p(), 100.0 * (int)hasMUON,
               type+"protosWithMUON",
               "% ProtoParticles with MUON info V Momentum", 0*GeV, 100*GeV, 100 );

    // ECAL info
    const bool hasECAL = proto->hasInfo(LHCb::ProtoParticle::InAccEcal);
    if ( hasECAL ) { ++tally.ecalTracks; }
    profile1D( track->p(), 100.0 * (int)hasECAL,
               type+"protosWithECAL",
               "% ProtoParticles with CALO-ECAL info V Momentum", 0*GeV, 100*GeV, 100 );

    // Brem info
    const bool hasBREM = proto->hasInfo(LHCb::ProtoParticle::InAccBrem);
    if ( hasBREM ) { ++tally.bremTracks; }
    profile1D( track->p(), 100.0 * (int)hasBREM,
               type+"protosWithBREM",
               "% ProtoParticles with CALO-BREM info V Momentum", 0*GeV, 100*GeV, 100 );

    // Spd info
    const bool hasSPD = proto->hasInfo(LHCb::ProtoParticle::InAccSpd);
    if ( hasSPD ) { ++tally.spdTracks; }
    profile1D( track->p(), 100.0 * (int)hasSPD,
               type+"protosWithSPD",
               "% ProtoParticles with CALO-SPD info V Momentum", 0*GeV, 100*GeV, 100 );

    // PRS info
    const bool hasPRS = proto->hasInfo(LHCb::ProtoParticle::InAccPrs);
    if ( hasPRS ) { ++tally.prsTracks; }
    profile1D( track->p(), 100.0 * (int)hasPRS,
               type+"protosWithPRS",
               "% ProtoParticles with CALO-PRS info V Momentum", 0*GeV, 100*GeV, 100 );

    // Add Hcal info
    const bool hasHCAL = proto->hasInfo(LHCb::ProtoParticle::InAccHcal);
    if ( hasHCAL ) { ++tally.hcalTracks; }
    profile1D( track->p(), 100.0 * (int)hasHCAL,
               type+"protosWithHCAL",
               "% ProtoParticles with CALO-HCAL info V Momentum", 0*GeV, 100*GeV, 100 );

    // Add Velo dE/dx info
    const bool hasDEDX = proto->hasInfo(LHCb::ProtoParticle::VeloCharge);
    if ( hasDEDX ) { ++tally.velodEdxTracks; }
    profile1D( track->p(), 100.0 * (int)hasDEDX,
               type+"protosWithVELOdEdx",
               "% ProtoParticles with VELO dE/dx info V Momentum", 0*GeV, 100*GeV, 100 );

    // Muon consistency checks
    if ( ( proto->hasInfo(LHCb::ProtoParticle::MuonPIDStatus) ||
           proto->hasInfo(LHCb::ProtoParticle::MuonMuLL)      ||
           proto->hasInfo(LHCb::ProtoParticle::MuonBkgLL)     ||
           proto->hasInfo(LHCb::ProtoParticle::MuonNShared) )
         && !proto->muonPID() )
    {
      Warning( "ProtoParticle has MuonPIDStatus but no NuonPID SmartRef" ).ignore();
    }
    if ( proto->muonPID() &&
         !proto->muonPID()->IsMuonLoose() && proto->muonPID()->IsMuon() )
    {
      Warning( "ProtoParticle MuonPID has IsMuonLoose=False but IsMuon=True" ).ignore();
    }

  }

  // return
  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Find the ProtoParticle created from a given Track
//=============================================================================
const LHCb::ProtoParticle *
ChargedProtoParticleMoni::getProto( const LHCb::ProtoParticles * protos,
                                    const LHCb::Track * track ) const
{
  const LHCb::ProtoParticle * proto = nullptr;

  // Check Track pointer is OK
  if ( track )
  {

    // First try using same track key convention
    proto = protos->object(track->key());
    if ( !proto || proto->track() != track ) 
    {
      // Key convention failed, so try direct search
      auto iP = std::find_if( std::begin(*protos), std::end(*protos),
                              [&](LHCb::ProtoParticles::const_reference i) {
                                  return i->track() == track;
                              });
      proto = ( iP != std::end(*protos) ? *iP : nullptr );
    } // not found via key

  } // track OK

  return proto;
}

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
  for ( const auto& iT : m_nTracks )
  {
    const TrackTally & tally = iT.second;
    msg << " " << occ(tally.selTracks,m_nEvts);
    msg << boost::format(" %|-12|") % iT.first;
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
