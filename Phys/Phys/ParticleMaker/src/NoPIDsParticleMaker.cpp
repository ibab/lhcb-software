// $Id: NoPIDsParticleMaker.cpp,v 1.15 2009-04-21 19:15:41 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// PartProp
#include "Kernel/ParticleProperty.h" 

// local
#include "NoPIDsParticleMaker.h"
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : NoPIDsParticleMaker
//
// 2006-01-23 : I. Belyaev
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_ALGORITHM_FACTORY( NoPIDsParticleMaker );

namespace
{
  std::string to_upper( const std::string& in )
  {
    std::string  out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  };
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NoPIDsParticleMaker::NoPIDsParticleMaker(  const std::string& name,ISvcLocator* pSvcLocator )
  : ParticleMakerBase ( name , pSvcLocator ), 
    m_pid    ( "UNDEFINED" ) 
  , m_apid   (   ) 
  , m_pp     ( 0 ) 
  , m_app    ( 0 )
  , m_inputs ( 1 , LHCb::ProtoParticleLocation::Charged )
  , m_CL     ( 50 * perCent  ) 
  , m_calls  ( 0 ) 
  , m_sum    ( 0 ) 
  , m_sum2   ( 0 )
  , m_longTracks ( true )
  , m_downstreamTracks ( false )  // set to false for HLT
  , m_vttTracks ( false )         // set to false for HLT
  , m_veloTracks ( false )         // 
{
  declareProperty ( "Particle" , m_pid    ) ;
  declareProperty ( "Inputs"   , m_inputs ) ; 
  declareProperty ( "CL"       , m_CL     ) ; 
  declareProperty ( "UseLongTracks",     m_longTracks );
  declareProperty ( "UseDownstreamTracks", m_downstreamTracks );
  declareProperty ( "UseUpstreamTracks",      m_vttTracks );
  declareProperty ( "UseVeloTracks",      m_veloTracks );

}
//=============================================================================
// Destructor
//=============================================================================
NoPIDsParticleMaker::~NoPIDsParticleMaker() {} 

//=============================================================================
// Destructor
//=============================================================================
StatusCode NoPIDsParticleMaker::initialize() {
  StatusCode sc = ParticleMakerBase::initialize();

  std::sort( m_inputs.begin () , m_inputs.end () ) ;
  m_inputs.erase ( std::unique( m_inputs.begin () , 
                                m_inputs.end   () ) , m_inputs.end () ) ;

  const std::string Upper ( to_upper( m_pid ) ) ;
  if      ( "PIONS"     == Upper ) { m_pid = "pi+"    ; } 
  else if ( "PI"        == Upper ) { m_pid = "pi+"    ; } 
  else if ( "PION"      == Upper ) { m_pid = "pi+"    ; } 
  else if ( "KAONS"     == Upper ) { m_pid = "K+"     ; } 
  else if ( "KAON"      == Upper ) { m_pid = "K+"     ; } 
  else if ( "K"         == Upper ) { m_pid = "K+"     ; } 
  else if ( "MUONS"     == Upper ) { m_pid = "mu+"    ; }
  else if ( "MUON"      == Upper ) { m_pid = "mu+"    ; }
  else if ( "MU"        == Upper ) { m_pid = "mu+"    ; }
  else if ( "ELECTRONS" == Upper ) { m_pid = "e+"     ; }
  else if ( "ELECTRON"  == Upper ) { m_pid = "e+"     ; }
  else if ( "POSITRON"  == Upper ) { m_pid = "e+"     ; }
  else if ( "E"         == Upper ) { m_pid = "e+"     ; }
  else if ( "PROTONS"   == Upper ) { m_pid = "p+"     ; }
  else if ( "PROTON"    == Upper ) { m_pid = "p+"     ; }
  else if ( "P"         == Upper ) { m_pid = "p+"     ; }

  sc = setPPs( m_pid ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Particle/Antiparticle are unknown for '"+m_pid+"'",sc);}
  if ( 0 == m_pp || 0 == m_app )
  { return Error ( "Particle/Antiparticle are invalid for '"+m_pid+"'"  );}
  
  if (  m_pp->charge() > m_app->charge() ) { std::swap( m_pp , m_app ) ; }
  m_pid  = m_pp  -> particle () ;
  m_apid = m_app -> particle () ;
  
  info() << " Particle/AntiParticle to be created\t " 
         << "'"   << m_pid 
         << "'/'" << m_apid << endmsg ;
  if ( !m_longTracks ) info() << "Filtering out long tracks"<< endmsg;
  if ( !m_downstreamTracks ) info() << "Filtering out downstream tracks" << endmsg;
  if ( !m_vttTracks ) info() << "Filtering out upstream tracks" << endmsg;
  if ( m_veloTracks ) info() << "Keeping velo tracks" << endmsg;
  if (!(( m_longTracks ) || ( m_downstreamTracks) || ( m_vttTracks)|| ( m_veloTracks))){
    Warning("You have chosen to veto all tracks.") ;
  }

  return StatusCode::SUCCESS;
  
} 

// ============================================================================
/// set particle properties for particle and for antiparticle  
// ============================================================================
StatusCode NoPIDsParticleMaker::setPPs( const std::string& pid )
{
  if ( 0 == ppSvc() ) { return StatusCode ( 110 ) ; }
  // get the properties of the particle 
  m_pp = ppSvc  () -> find( pid ) ;
  if ( 0 == m_pp    ) { return StatusCode ( 111 ) ; }
  // get the the antiparticle 
  m_app = m_pp -> antiParticle () ;
  if ( 0 == m_app   ) { return StatusCode ( 112 ) ; }
  m_apid = m_app -> particle () ;  
  return StatusCode::SUCCESS ;
};
// ============================================================================
StatusCode NoPIDsParticleMaker::finalize() 
{
  const std::string stars ( 80 , '*' );
  info() << stars << endmsg ;
  
  if( 0 != m_ppSvc    ) { m_ppSvc  = 0 ; }
  
  double mean  = m_sum ;
  if ( 0 != m_calls  ) { mean  = m_sum  / m_calls                ; }
  double sigma = 0     ;
  if ( 0 != m_calls  ) { sigma = m_sum2 / m_calls - mean * mean  ; }
  if ( sigma > 0     ) { sigma = sqrt( sigma )                   ; }
  
  always()
    << " created '"    << m_pid 
    << "' and '"       << m_apid  
    << " : "           << m_sum
    << " per "         << m_calls
    << " calls ("      << mean 
    << "+-"            << sigma 
    << ")/event"       << endmsg ;
  // finalize the base 
  return ParticleMakerBase::finalize ();
};
//=============================================================================
// Dispatch the making of particles 
//=============================================================================
StatusCode NoPIDsParticleMaker::makeParticles
( LHCb::Particle::Vector & particles ){
  
  // increase the counter 
  ++m_calls ; 
  
  size_t number = 0 ;
  // get all input data
  for( Addresses::const_iterator input = m_inputs.begin() ;
       m_inputs.end() != input ; ++input ) 
  {
    if ( !exist<LHCb::ProtoParticles>( *input ) ){
      Warning("No ProtoParticles at "+*input).ignore();
      continue ;
    }
    const LHCb::ProtoParticles* pps = get<LHCb::ProtoParticles>( *input );
    if( !pps ) {  Warning ("Invalid container '"+(*input)+"' ") ; continue ; }
    // loop over all protoparticles 
    for(LHCb:: ProtoParticles::const_iterator ipp = pps->begin() ; 
         pps->end() != ipp ; ++ipp ) 
    {
      const LHCb::ProtoParticle* pp = *ipp ;
      if ( 0 == pp                ) { continue ; }              // CONTINUE
      if ( 0 == pp -> charge ()   ) { continue ; }              // CONTINUE
          
      const LHCb::Track* ptrack = pp->track();
      if ( 0==ptrack ) {
        Warning("Charged protoparticle with no Track found. Ignoring.");
        continue ;
      }
      // require that at leat one of the accepted types is OK
      if (!((( m_veloTracks ) && ( ptrack->checkType( LHCb::Track::Velo))) ||
            (( m_longTracks ) && ( ptrack->checkType( LHCb::Track::Long))) ||
            (( m_downstreamTracks ) && ( ptrack->checkType( LHCb::Track::Downstream))) ||
            (( m_vttTracks ) && ( ptrack->checkType( LHCb::Track::Upstream))))) continue ;
          
      LHCb::Particle* particle = new LHCb::Particle();
          
      StatusCode sc = StatusCode::FAILURE ;
      if      ( m_pp  -> charge() == pp -> charge () ) 
      { sc = fillParticle( pp , m_pp , particle ) ; }
      else if ( m_app -> charge() == pp -> charge () ) 
      { sc = fillParticle( pp , m_app , particle ) ; }
          
      if( sc.isFailure() ) 
      {
        Warning("Error from 'fillParticle'", sc );
        delete particle ; particle = 0 ; continue ;           // CONTINUE
      }
          
      // put particle into the output container 
      particles.push_back( particle );
      ++number ;
          
    } // loop over protoparticles
  }; // loop over containers
  
  m_sum   +=          number ;
  m_sum2  += number * number ;

  return StatusCode::SUCCESS ;  
}

// ============================================================================
/** Fill the particle from protoparticle using ID  
 *  @param proto    pointer to ProtoParticle
 *  @param property particle property information
 *  @param particle particle (output) 
 *  @return status code 
 */
// ============================================================================
StatusCode NoPIDsParticleMaker::fillParticle
( const LHCb::ProtoParticle*    proto    ,
  const LHCb::ParticleProperty* property , 
  LHCb::Particle*               particle ) const 
{
  if ( 0 == proto    ) 
  { return Error ( "fillParticle: ProtoParticle*    is NULL" , 120 ) ; }
  if ( 0 == property ) 
  { return Error ( "fillParticle: ParticleProperty* is NULL" , 121 ) ; }
  if ( 0 == particle ) 
  { return Error ( "fillParticle: Particle*         is NULL" , 122 ) ; }
  
  const double mass = property -> mass() ;
  
  particle -> setParticleID   ( property -> particleID () ) ;
  particle -> setConfLevel    ( m_CL ) ;
  particle -> setMeasuredMass ( mass ) ;
  
  particle -> setProto( proto ) ;
  //
  const LHCb::Track* track = proto->track() ;  
  //
  const LHCb::State* state   = 0 ;
  // default closest to the beam 
  if ( 0 == state ) { state = track->stateAt ( LHCb::State::ClosestToBeam    ) ; }
  if ( 0 == state ) { state = track->stateAt ( LHCb::State::FirstMeasurement ) ; }
  if ( 0 == state ) 
  {
    Warning("No state closest to beam or at first measurement for track. Using first state instead") ;
    state = &track->firstState() ;    
  }
  if ( msgLevel ( MSG::VERBOSE ) ) 
  { verbose() << "Using state at " << state->position() << endmsg ; }
  
  return m_p2s->state2Particle(*state,*particle);
};
// ============================================================================
