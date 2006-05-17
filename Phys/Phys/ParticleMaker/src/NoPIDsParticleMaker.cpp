// $Id: NoPIDsParticleMaker.cpp,v 1.8 2006-05-17 16:28:20 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 

#include "Kernel/IParticle2State.h"
// local
#include "NoPIDsParticleMaker.h"
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : NoPIDsParticleMaker
//
// 2006-01-23 : I. Belyaev
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NoPIDsParticleMaker );

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
NoPIDsParticleMaker::NoPIDsParticleMaker( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ppSvc  ( 0 ) 
  , m_pid    ( "UNDEFINED" ) 
  , m_apid   (   ) 
  , m_pp     ( 0 ) 
  , m_app    ( 0 )
  , m_inputs ( 1 , LHCb::ProtoParticleLocation::Charged )
  , m_CL     ( 50 * perCent  ) 
  , m_calls  ( 0 ) 
  , m_sum    ( 0 ) 
  , m_sum2   ( 0 )
  , m_longTracks ( true )
  , m_downstreamTracks ( true )  // set to false for HLT
  , m_vttTracks ( true )         // set to false for HLT
  , m_p2s()
{
  declareInterface<IParticleMaker>(this);
  declareProperty ( "Particle" , m_pid    ) ;
  declareProperty ( "Inputs"   , m_inputs ) ; 
  declareProperty ( "CL"       , m_CL     ) ; 
  declareProperty ( "UseLongTracks",     m_longTracks );
  declareProperty ( "UseDownstreamTracks", m_downstreamTracks );
  declareProperty ( "UseUpstreamTracks",      m_vttTracks );

}
//=============================================================================
// Destructor
//=============================================================================
NoPIDsParticleMaker::~NoPIDsParticleMaker() {} 

//=============================================================================
// Destructor
//=============================================================================
StatusCode NoPIDsParticleMaker::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc" , true ) ;

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
  
  m_p2s = tool<IParticle2State>("Particle2State"); // not private

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
  m_app = ppSvc () -> findByStdHepID( -1 * m_pp->jetsetID() ) ;
  if ( 0 == m_app ) { return StatusCode   ( 112 ) ; }
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
  return GaudiTool::finalize ();
};
//=============================================================================
// Dispatch the making of particles 
//=============================================================================
StatusCode NoPIDsParticleMaker::makeParticles( LHCb::Particle::ConstVector & particles ){
  
  // increase the counter 
  ++m_calls ; 
  
  size_t number = 0 ;
  // get all input data
  for( Addresses::const_iterator input = m_inputs.begin() ;
       m_inputs.end() != input ; ++input ) 
  {
    const LHCb::ProtoParticles* pps = get<LHCb::ProtoParticles>( *input );
    if( !pps ) {  Warning ("Invalid container '"+(*input)+"' ") ; continue ; }
    // loop over all protoparticles 
    for(LHCb:: ProtoParticles::const_iterator ipp = pps->begin() ; 
         pps->end() != ipp ; ++ipp ) 
    {
      const LHCb::ProtoParticle* pp = *ipp ;
      if ( 0 == pp                ) { continue ; }              // CONTINUE
      if ( 0 == pp -> charge ()   ) { continue ; }              // CONTINUE
          
      if (( !m_longTracks ) || ( !m_downstreamTracks) || ( !m_vttTracks)){
        const LHCb::Track* ptrack = pp->track();
        if ( ptrack ) {
          if (( !m_longTracks ) && ( ptrack->checkType( LHCb::Track::Long))){ continue ;}
          if (( !m_downstreamTracks ) && ( ptrack->checkType( LHCb::Track::Downstream))){ 
            continue ;}
          if (( !m_vttTracks ) && ( ptrack->checkType( LHCb::Track::Upstream))){ continue ;}
        }            
      }
          
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
StatusCode NoPIDsParticleMaker::fillParticle( const LHCb::ProtoParticle* proto    ,
                                              const ParticleProperty* property , 
                                              LHCb::Particle* particle ) const {
  if ( 0 == proto    ) 
  { return Error ( "fillParticle: ProtoParticle*    is NULL" , 120 ) ; }
  if ( 0 == property ) 
  { return Error ( "fillParticle: ParticleProperty* is NULL" , 121 ) ; }
  if ( 0 == particle ) 
  { return Error ( "fillParticle: Particle*         is NULL" , 122 ) ; }
  
  const double mass = property -> mass() ;
  
  particle -> setParticleID ( LHCb::ParticleID ( property -> pdgID () ) ) ;
  particle -> setConfLevel  ( m_CL ) ;
  particle -> setMeasuredMass( mass ) ;
  
  particle -> setProto( proto ) ;
  const LHCb::State& state = proto->track()->firstState() ;
  return m_p2s->state2Particle(state,*particle);
};
// ============================================================================
