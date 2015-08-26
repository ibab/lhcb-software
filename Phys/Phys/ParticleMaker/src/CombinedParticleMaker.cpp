
//-----------------------------------------------------------------------------
/** @file CombinedParticleMaker.cpp
 *
 * Implmentation file for Particle maker CombinedParticleMaker
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "CaloUtils/CaloMomentum.h"

// local
#include "CombinedParticleMaker.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombinedParticleMaker::CombinedParticleMaker( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : ChargedParticleMakerBase ( name , pSvcLocator ),
    m_protoTool              ( NULL ),
    m_partProp               ( NULL )
{
  // ProtoParticle filters to use for each type
  declareProperty( "ElectronFilter", m_elProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "MuonFilter",     m_muProtoFilter = "ProtoParticleMUONFilter" );
  declareProperty( "PionFilter",     m_piProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "KaonFilter",     m_kaProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "ProtonFilter",   m_prProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "MinPercentForPrint", m_minPercForPrint = 0.01 );
  // Test PID info consistency
  declareProperty( "CheckPIDConsistency", m_testPIDinfo = true );
}

//=============================================================================

CombinedParticleMaker::~CombinedParticleMaker( ) { }

//=============================================================================
// Standard initialize method
//=============================================================================
StatusCode CombinedParticleMaker::initialize()
{
  // intialize base class
  const StatusCode sc = ChargedParticleMakerBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize base class" );

  // get tooltype
  std::string toolType = "";
  std::string name = "";
  if      ( m_pid == "pi+" )
  {
    toolType = m_piProtoFilter ;
    name = "Pion" ;
  }
  else if ( m_pid == "mu+" )
  {
    toolType = m_muProtoFilter ;
    name = "Muon" ;
  }
  else if ( m_pid == "K+" )
  {
    toolType = m_kaProtoFilter ;
    name = "Kaon" ;
  }
  else if ( m_pid == "p+" )
  {
    toolType = m_prProtoFilter ;
    name = "Proton" ;
  }
  else if ( m_pid == "e+" )
  {
    toolType = m_elProtoFilter ;
    name = "Electron" ;
  }
  else
  {
    return Error( "Unknown particle selection '" + m_pid + "'" );
  }

  // Get particle properties
  m_partProp = ppSvc()->find( m_pid );
  if (!m_partProp) return Error("Unknown Particle Property for "+m_pid);

  // load tool into map
  if (msgLevel(MSG::DEBUG)) debug() << "Particle type " << name << " using ProtoParticle Filter '"
                                    << toolType << "'" << endmsg;
  m_protoTool = tool<IProtoParticleFilter>( toolType, name, this );

  return sc;
}


//===========================================================================
// Finalize
//===========================================================================
StatusCode CombinedParticleMaker::finalize()
{
  // Printout tallies
  for ( const auto& iT : m_nTracks )
  {
    const TrackTally & tally = iT.second;
    const double tkSel = 100 * ( tally.totProtos>0 ? (double)tally.selProtos/(double)tally.totProtos : 0 );
    if ( tkSel > m_minPercForPrint )
    {
      info() << "Selected " << tkSel << "% of '" << iT.first << "' ProtoParticles" << endmsg;
      const double elEff = 100 * (double)tally.el/(double)tally.selProtos;
      const double muEff = 100 * (double)tally.mu/(double)tally.selProtos;
      const double piEff = 100 * (double)tally.pi/(double)tally.selProtos;
      const double kaEff = 100 * (double)tally.ka/(double)tally.selProtos;
      const double prEff = 100 * (double)tally.pr/(double)tally.selProtos;
      if ( elEff>m_minPercForPrint )
        info() << "  -> Electrons " << elEff << "% of selected ProtoParticles" << endmsg;
      if ( muEff>m_minPercForPrint )
        info() << "  -> Muons     " << muEff << "% of selected ProtoParticles" << endmsg;
      if ( piEff>m_minPercForPrint )
        info() << "  -> Pions     " << piEff << "% of selected ProtoParticles" << endmsg;
      if ( kaEff>m_minPercForPrint )
        info() << "  -> Kaons     " << kaEff << "% of selected ProtoParticles" << endmsg;
      if ( prEff>m_minPercForPrint )
        info() << "  -> Protons   " << prEff << "% of selected ProtoParticles" << endmsg;
    }
  }

  // finalize base class
  return ChargedParticleMakerBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CombinedParticleMaker::makeParticles( LHCb::Particle::Vector & parts )
{

  // Load the ProtoParticles
  const LHCb::ProtoParticle::ConstVector& pps = protos() ;
  if (msgLevel(MSG::DEBUG)) debug() << "Making Particles from " << pps.size()
                                    << " ProtoParticles at "<< m_input << endmsg;

  // reserve size
  parts.reserve( pps.size() );

  // loop over ProtoParticles
  for ( LHCb::ProtoParticle::ConstVector::const_iterator iProto = pps.begin();
        pps.end() != iProto; ++iProto )
  {
    // get point to track (should always exist for charged tracks)
    const LHCb::Track * track = (*iProto)->track();
    if ( !track ) return Error( "Charged ProtoParticle has null track reference !" );

    TrackTally & tally = trackTally( track->type() );
    ++tally.totProtos;

    // Select tracks
    if (msgLevel(MSG::VERBOSE)) verbose() << "Trying Track " << track->key() << endmsg;

    if (track->states().empty())
    {
      Warning("Track has empty states. This is likely to be bug https://savannah.cern.ch/bugs/index.php?70979").ignore();
      continue ;
    }
    if ( !trSel()->accept(*track) ) continue;
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << " -> Track selected " << track->key()
                << " " << track->firstState().momentum() << endmsg;
    }
    ++tally.selProtos;

    // Do PID checks ?
    if ( m_testPIDinfo ) checkPIDInfo(*iProto);

    const bool selected = m_protoTool->isSatisfied( *iProto );
    if (msgLevel(MSG::VERBOSE)) verbose() << " -> Particle type " << m_partProp->particle()
                                          << " selected=" << selected << endmsg;
    if ( selected )
    {
      // make a new Particle
      LHCb::Particle * part = new LHCb::Particle();
      // fill Parameters
      const StatusCode sc = fillParticle( *iProto, m_partProp, part );
      if ( sc.isFailure() )
      {
        Warning( "Failed to fill Particle -> rejected" ).ignore();
        delete part;
      }
      else
      {
        // add to container
        parts.push_back(part);
        // increment tally
        tally.addToType( m_partProp->particle() );
      }
    } // ProtoParticle selected

  } // end loop on ProtoParticles

  return StatusCode::SUCCESS;
}

//=========================================================================
// Test PID info
//=========================================================================
void CombinedParticleMaker::checkPIDInfo( const LHCb::ProtoParticle * proto ) const
{
  // test RICH links
  if ( proto->hasInfo( LHCb::ProtoParticle::RichPIDStatus ) )
  {
    const LHCb::RichPID * rpid = proto->richPID();
    if ( !rpid )
    {
      Error( "ProtoParticle has RICH information but NULL RichPID SmartRef !" ).ignore();
    }
  }
  // test MUON links
  if ( proto->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) )
  {
    const LHCb::MuonPID * mpid = proto->muonPID();
    if ( !mpid )
    {
      Error( "ProtoParticle has MUON information but NULL MuonPID SmartRef !" ).ignore();
    }
  }
}

//=========================================================================
// Fill particles parameters
//=========================================================================
StatusCode
CombinedParticleMaker::fillParticle( const LHCb::ProtoParticle* proto,
                                     const LHCb::ParticleProperty* pprop,
                                     LHCb::Particle* particle ) const
{
  // Start filling particle with orgininating ProtoParticle
  particle->setProto(proto);

  // ParticleID
  const int pID = pprop->particleID().pid() * (int)(proto->charge());
  particle->setParticleID( LHCb::ParticleID( pID ) );

  // Confidence level
  setConfLevel(proto,pprop,particle);

  // Mass and error
  particle->setMeasuredMass(pprop->mass());
  particle->setMeasuredMassErr(0);

  const LHCb::Track* track = proto->track() ;
  const LHCb::State* state = usedState( track );

  // finally, set Particle information from State using tool
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Making Particle " << pprop->particle() << " from Track with P= "
              << state->momentum() << endmsg ;
  const StatusCode sc = p2s()->state2Particle( *state, *particle );
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Made   Particle " << pprop->particle()
              << " with            P= " << particle->momentum() << endmsg ;

  return sc;
}

//=========================================================================
//  set conf level
//=========================================================================
void
CombinedParticleMaker::setConfLevel( const LHCb::ProtoParticle * proto,
                                     const LHCb::ParticleProperty * pprop,
                                     LHCb::Particle * particle ) const
{
  // Definition of confidence level needs to be re-assessed
  const double ve  = proto->info( LHCb::ProtoParticle::CombDLLe,  -999.0 );
  const double vmu = proto->info( LHCb::ProtoParticle::CombDLLmu, -999.0 );
  const double vk  = proto->info( LHCb::ProtoParticle::CombDLLk,  -999.0 );
  const double vp  = proto->info( LHCb::ProtoParticle::CombDLLp,  -999.0 );
  double confLevel = 1.0/(1.0+ve+vmu+vk+vp); // conf level for pion
  if ( "e+" == pprop->particle() )
  {
    confLevel *= ve;
  }
  else if ( "mu+" == pprop->particle() )
  {
    confLevel *= vmu;
  }
  else if ( "K+" == pprop->particle() )
  {
    confLevel *= vk;
  }
  else if ( "p+" == pprop->particle() )
  {
    confLevel *= vp;
  }
  particle->setConfLevel( confLevel );
}

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_ALGORITHM_FACTORY( CombinedParticleMaker )

//-----------------------------------------------------------------------------
