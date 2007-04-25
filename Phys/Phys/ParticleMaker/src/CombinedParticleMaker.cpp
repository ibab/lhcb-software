//-----------------------------------------------------------------------------
/** @file CombinedParticleMaker.cpp
 *
 * Implmentation file for Particle maker CombinedParticleMaker
 *
 * CVS Log :-
 * $Id: CombinedParticleMaker.cpp,v 1.23 2007-04-25 16:18:58 pkoppenb Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Tokenizer.h"
#include "CaloUtils/CaloMomentum.h"
// local
#include "CombinedParticleMaker.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( CombinedParticleMaker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombinedParticleMaker::CombinedParticleMaker( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_p2s     ( NULL ),
    m_trSel   ( NULL ),
    m_brem    ( NULL )
{

  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);

  // Job options
  declareProperty( "InputProtoParticles", m_input =  ProtoParticleLocation::Charged);

  // Particle types to create
  m_particleList.push_back("muon");
  m_particleList.push_back("electron");
  m_particleList.push_back("kaon");
  m_particleList.push_back("proton");
  m_particleList.push_back("pion");
  declareProperty( "Particles", m_particleList );

  // ProtoParticle filters to use for each type
  declareProperty( "ElectronFilter", m_elProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "MuonFilter",     m_muProtoFilter = "ProtoParticleMUONFilter" );
  declareProperty( "PionFilter",     m_piProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "KaonFilter",     m_kaProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "ProtonFilter",   m_prProtoFilter = "ProtoParticleCALOFilter" );
  declareProperty( "AddBremPhoton",  m_addBremPhoton = true );
  declareProperty( "ExclusiveSelection", m_exclusive = false );
  declareProperty( "MinPercentForPrint", m_minPercForPrint = 0.01 );

  // Test PID info consistency
  declareProperty( "CheckPIDConsistency", m_testPIDinfo = true );

}

CombinedParticleMaker::~CombinedParticleMaker( ) { }

//=============================================================================
// Standard initialize method
//=============================================================================
StatusCode CombinedParticleMaker::initialize()
{
  // intialize base class
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize GaudiTool base class" );

  if ( m_particleList.empty() )
  {
    return Error( "A list of particles types must be specified" );
  }

  // Particle properties service
  IParticlePropertySvc * ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );

  // particle tool
  m_p2s = tool<IParticle2State>("Particle2State");

  // BremStrahlung added
  m_brem = tool<IBremAdder>("BremAdder","BremAdder", this);

  info() << "Will produce : " << m_particleList << endmsg;
  if ( m_exclusive ) info() << "Using exclusive selection policy" << endmsg;

  // loop over selection and load ProtoParticle selectors
  m_protoMap.clear();
  for ( std::vector<std::string>::const_iterator iPart = m_particleList.begin();
        iPart != m_particleList.end(); ++iPart )
  {
    // Get particle tyoe name
    const std::string name = convertName( *iPart );
    // get pp name and tooltype
    std::string ppName, toolType;
    if      ( "Muon"     == name )
    {
      ppName   = "mu+";
      toolType = m_muProtoFilter;
    }
    else if ( "Electron" == name )
    {
      ppName   = "e+";
      toolType = m_elProtoFilter;
    }
    else if ( "Kaon"     == name )
    {
      ppName   = "K+";
      toolType = m_kaProtoFilter;
    }
    else if ( "Proton"   == name )
    {
      ppName   = "p+";
      toolType = m_prProtoFilter;
    }
    else if ( "Pion"     == name )
    {
      ppName   = "pi+";
      toolType = m_piProtoFilter;
    }
    else
    {
      return Error( "Unknown particle selection '" + *iPart + "'" );
    }

    // Get particle properties
    const ParticleProperty * partProp = ppSvc->find( ppName );

    // load tool into map
    info() << "Particle type " << name << " using ProtoParticle Filter '"
           << toolType << "'" << endmsg;
    const IProtoParticleFilter * t = tool<IProtoParticleFilter>( toolType, name, this );
    m_protoMap.push_back( ProtoPair(partProp,t) );

  }

  return sc;
}

std::string CombinedParticleMaker::convertName( const std::string & in ) const
{
  std::string first  = in.substr(0,1);
  std::transform( first.begin(),  first.end(),  first.begin (),  ::toupper ) ;
  std::string second = in.substr(1);
  std::transform( second.begin(), second.end(), second.begin (), ::tolower ) ;
  return first+second;
}

//===========================================================================
// Finalize
//===========================================================================
StatusCode CombinedParticleMaker::finalize()
{
  // Printout tallies
  for ( TrackMap::const_iterator iT = m_nTracks.begin();
        iT != m_nTracks.end(); ++iT )
  {
    const TrackTally & tally = (*iT).second;
    const double tkSel = 100 * ( tally.totProtos>0 ? (double)tally.selProtos/(double)tally.totProtos : 0 );
    if ( tkSel > m_minPercForPrint )
    {
      info() << "Selected " << tkSel << "% of '" << (*iT).first << "' ProtoParticles" << endmsg;
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
  return GaudiTool::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CombinedParticleMaker::makeParticles( Particle::ConstVector & parts )
{

  // Load the ProtoParticles
  ProtoParticles * protos = get<ProtoParticles>( m_input );
  if ( protos->empty() )
  {
    return Warning( "Charged ProtoParticles container is empty at " + m_input,
                    StatusCode::SUCCESS );
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Making Particles from " << protos->size() << " ProtoParticles at "
            << m_input << endmsg;
  }

  // loop over ProtoParticles
  for ( ProtoParticles::const_iterator iProto = protos->begin();
        protos->end() != iProto; ++iProto )
  {
    // get point to track (should always exist for charged tracks)
    const Track * track = (*iProto)->track();
    if ( !track ) return Error( "Charged ProtoParticle has null track reference !" );

    TrackTally & tally = m_nTracks[ track->type() ];
    ++tally.totProtos;

    // Select tracks
    verbose() << "Trying Track " << track->key() << endmsg;
    if ( !m_trSel->accept(*track) ) continue;
    verbose() << " -> Track selected" << track->key() ;
    if ( track->hasStateAt( LHCb::State::FirstMeasurement )) 
      verbose() << " " << track->stateAt( LHCb::State::FirstMeasurement ).momentum() ;
    verbose() << endmsg;
    ++tally.selProtos;

    // Do PID checks ?
    if ( m_testPIDinfo ) checkPIDInfo(*iProto);

    // loop over particle types to make
    for ( ProtoMap::const_iterator iP = m_protoMap.begin();
          iP != m_protoMap.end(); ++iP )
    {
      const bool selected = (*iP).second->isSatisfied( *iProto );
      verbose() << " -> Particle type " << (*iP).first->particle()
                << " selected=" << selected << endmsg;
      bool madeP(false);
      if ( selected )
      {
        // make a new Particle
        Particle * part = new Particle();
        // fill Parameters
        const StatusCode sc = fillParticle( *iProto, (*iP).first, part );
        if ( sc.isFailure() )
        {
          Warning( "Failed to fill Particle -> rejected" );
          delete part;
        }
        else
        {
          // add to container
          parts.push_back(part);
          // increment tally
          tally.addToType( (*iP).first->particle() );
          madeP = true;
        }
      } // ProtoParticle selected

      // Exclusive selection ?
      if ( m_exclusive && madeP ) break;

    } // loop over particle types to make

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
      Error( "ProtoParticle has RICH information but NULL RichPID SmartRef !" ); 
    }
  }
  // test MUON links
  if ( proto->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) )
  {
    const LHCb::MuonPID * mpid = proto->muonPID();
    if ( !mpid )
    {
      Error( "ProtoParticle has MUON information but NULL MuonPID SmartRef !" ); 
    }
  }
}

//=========================================================================
// Fill particles parameters
//=========================================================================
StatusCode CombinedParticleMaker::fillParticle( const ProtoParticle* proto,
                                                const ParticleProperty* pprop,
                                                Particle* particle ) const
{
  // Start filling particle with orgininating ProtoParticle
  particle->setProto(proto);

  // ParticleID
  const int pID = pprop->jetsetID() * (int)(proto->charge());
  particle->setParticleID( ParticleID( pID ) );

  // Confidence level
  setConfLevel(proto,pprop,particle);

  // Mass and error
  particle->setMeasuredMass(pprop->mass());
  particle->setMeasuredMassErr(0); // Should really put PDG value here

  /*
  for ( std::vector< LHCb::State*>::const_iterator s = proto->track()->states().begin() ;
        s != proto->track()->states().end() ; ++s){
    verbose() << " A   State is at " << (*s)->position() << " and has slopes " << (*s)->slopes() << endmsg  ;  
  }
  */

  LHCb::State& usedState = proto->track()->firstState() ; // backup
  if ( proto->track()->hasStateAt( LHCb::State::ClosestToBeam )){ // default: closest to beam
    usedState = proto->track()->stateAt( LHCb::State::ClosestToBeam );
    verbose() << "Using state at " << usedState.position() << endmsg ;
  } else if ( proto->track()->hasStateAt( LHCb::State::FirstMeasurement )){ // if not available: first measurement
    usedState = proto->track()->stateAt( LHCb::State::FirstMeasurement );
    verbose() << "Using state at " << usedState.position() << endmsg ;
  } else Warning("No state closest to beam or at first measurement for track. Using first state instead") ;
  

  // finally, set Particle infor from State using tool
  verbose() << "Making Particle " << pprop->particle() << " from Track with P= " 
            << usedState.momentum() << endmsg ;
  StatusCode sc = m_p2s->state2Particle( usedState, *particle );
  verbose() << "Made   Particle " << pprop->particle() << " with            P= " << particle->momentum() << endmsg ;

  // Add BremmStrahlung for electrons
  if (sc.isSuccess() && "e+" == pprop->particle() && m_addBremPhoton ){
    if( m_brem->addBrem( particle ) )
      debug() << " ------- BremStrahlung has been added to the particle" << endmsg;
  }
  return sc;
}

void
CombinedParticleMaker::setConfLevel( const LHCb::ProtoParticle * proto,
                                     const ParticleProperty    * pprop,
                                     LHCb::Particle            * particle ) const
{
  // Definition of confidence level needs to be re-assessed
  const double ve  = proto->info( ProtoParticle::CombDLLe,  -999.0 );
  const double vmu = proto->info( ProtoParticle::CombDLLmu, -999.0 );
  const double vk  = proto->info( ProtoParticle::CombDLLk,  -999.0 );
  const double vp  = proto->info( ProtoParticle::CombDLLp,  -999.0 );
  double confLevel = 1./(1.+ve+vmu+vk+vp); // conf level for pion
  if( "e+" == pprop->particle() )
  {
    confLevel = ve*confLevel;
  }
  else if ( "mu+" == pprop->particle() )
  {
    confLevel = vmu*confLevel;
  }
  else if ( "K+" == pprop->particle() )
  {
    confLevel = vk*confLevel;
  }
  else if ( "p+" == pprop->particle() )
  {
    confLevel = vp*confLevel;
  }
  particle->setConfLevel( confLevel );
}
