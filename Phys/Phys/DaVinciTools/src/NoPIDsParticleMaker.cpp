// $Id: NoPIDsParticleMaker.cpp,v 1.7 2004-07-28 14:15:24 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/07/08 10:14:26  pkoppenb
// HLT
//
// Revision 1.5  2004/05/11 16:01:25  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.4  2004/04/22 02:55:13  ibelyaev
//  fix a problem with release of some tools/services
//
// Revision 1.3  2004/03/11 13:02:14  pkoppenb
// Split DaVinciTools into DaVinciTools and DaVinciKernel
//
// Revision 1.2  2004/01/14 19:00:06  gcorti
// compatible with LHCb v15r0
//
// Revision 1.1  2003/04/30 13:41:25  gcorti
// new particlemaker for HLT studies
// 
// ============================================================================
// Include files
// STD/STL
#include <algorithm>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 
// Event 
#include "Event/Particle.h"
// ============================================================================
// local
#include "NoPIDsParticleMaker.h"
// ============================================================================

// ============================================================================
/** @file 
 *  
 * Implementation file for class : NoPIDsParticleMaker
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-17
 */
// ============================================================================


// ============================================================================
/** @var NoPIDsParticleMakerFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<NoPIDsParticleMaker>         s_Factory ;
const        IToolFactory&NoPIDsParticleMakerFactory = s_Factory ; 
// ============================================================================

namespace
{
  std::string to_upper( const std::string& in )
  {
    std::string  out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  };
};



// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
NoPIDsParticleMaker::NoPIDsParticleMaker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  , m_ppSvc  ( 0 ) 
  , m_pid    ( "UNDEFINED" ) 
  , m_apid   (   ) 
  , m_pp     ( 0 ) 
  , m_app    ( 0 )
  , m_inputs ( 1 , ProtoParticleLocation::Charged )
  , m_CL     ( 50 * perCent  ) 
  , m_calls  ( 0 ) 
  , m_sum    ( 0 ) 
  , m_sum2   ( 0 )
  , m_longTracks ( true )
  , m_downstreamTracks ( true )  // set to false for HLT
  , m_vttTracks ( true )         // set to false for HLT
{ 
  // declare interface 
  declareInterface<IParticleMaker> ( this ) ;
  // properties 
  declareProperty ( "Particle" , m_pid    ) ;
  declareProperty ( "Inputs"   , m_inputs ) ; 
  declareProperty ( "CL"       , m_CL     ) ; 
  declareProperty ( "UseLongTracks",     m_longTracks );
  declareProperty ( "UseDownstreamTracks", m_downstreamTracks );
  declareProperty ( "UseUpstreamTracks",      m_vttTracks );
};
// ============================================================================

// ============================================================================
/// virtual detructor
// ============================================================================
NoPIDsParticleMaker::~NoPIDsParticleMaker(){}
// ============================================================================

// ============================================================================
/** standard initialization of tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode NoPIDsParticleMaker::initialize    () 
{
  // initialize the base 
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error ( "Unable to initilize the base class 'CaloTool'" , sc ) ; }
  
  // locate services 
  
  if( 0 == evtSvc  () )
    { return Error ( " IDataProviderSvc* points to NULL "   ) ; }
  
  if( 0 != m_ppSvc    ) { m_ppSvc  = 0 ; }
  m_ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc" , true ) ;
  if( 0 == ppSvc   () )
  { return Error ( " IParticlePropertySvc* points to NULL "     ) ; }
  
  // remove the duplicates from list of input containers 
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
  
  MsgStream msg( msgSvc() , name () );
  msg << MSG::INFO 
      << " Particle/AntiParticle to be created\t " 
      << "'"   << m_pid 
      << "'/'" << m_apid << endreq ;
  if ( !m_longTracks ) msg << MSG::INFO << "Filtering out long tracks"<< endreq;
  if ( !m_downstreamTracks ) msg <<  MSG::INFO << 
                               "Filtering out downstream tracks" << endreq;
  if ( !m_vttTracks ) msg <<  MSG::INFO << 
                        "Filtering out upstream tracks" << endreq;
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


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


// ============================================================================
/** standard finalization  of tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode NoPIDsParticleMaker::finalize      () 
{
  MsgStream log ( msgSvc() , name () ) ;
  const std::string stars ( 80 , '*' );
  log << MSG::INFO << stars << endreq ;
  
  if( 0 != m_ppSvc    ) { m_ppSvc  = 0 ; }
  
  double mean  = m_sum ;
  if ( 0 != m_calls  ) { mean  = m_sum  / m_calls                ; }
  double sigma = 0     ;
  if ( 0 != m_calls  ) { sigma = m_sum2 / m_calls - mean * mean  ; }
  if ( sigma > 0     ) { sigma = sqrt( sigma )                   ; }
  
  log << MSG::ALWAYS
      << " created '"    << m_pid 
      << "' and '"       << m_apid  
      << " : "           << m_sum
      << " per "         << m_calls
      << " calls ("      << mean 
      << "+-"            << sigma 
      << ")/event"       << endreq ;
  // finalize the base 
  return CaloTool::finalize ();
};
// ============================================================================

// ============================================================================
/** Make the particles 
 *  @see IParticleMaker 
 *  @param particles  vector of particles  
 */
// ============================================================================
StatusCode NoPIDsParticleMaker::makeParticles ( ParticleVector & particles ) 
{
  // increase the counter 
  ++m_calls ; 
  
  size_t number = 0 ;
  // get all input data
  for( Addresses::const_iterator input = m_inputs.begin() ;
       m_inputs.end() != input ; ++input ) 
    {
      SmartDataPtr<ProtoParticles> pps( evtSvc() , *input );
      if( !pps ) {  Warning ("Invalid container '"+(*input)+"' ") ; continue ; }
      // loop over all protoparticles 
      for( ProtoParticles::const_iterator ipp = pps->begin() ; 
           pps->end() != ipp ; ++ipp ) 
        {
          const ProtoParticle* pp = *ipp ;
          if ( 0 == pp                ) { continue ; }              // CONTINUE
          if ( 0 == pp -> charge ()   ) { continue ; }              // CONTINUE
          
          if (( !m_longTracks ) || ( !m_downstreamTracks) || ( !m_vttTracks)){
            const TrStoredTrack* ptrack = pp->track();
            if ( ptrack ) {
              if (( !m_longTracks ) && ( ptrack->isLong())){ continue ;}
              if (( !m_downstreamTracks ) && ( ptrack->isDownstream())){ 
                continue ;}
              if (( !m_vttTracks ) && ( ptrack->isUpstream())){ continue ;}
            }            
          }
          
          Particle* particle = new Particle();
          
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
};  
// ============================================================================

// ============================================================================
/** Fill the particle from protoparticle using ID  
 *  @param proto    pointer to ProtoParticle
 *  @param property particle property information
 *  @param particle particle (output) 
 *  @return status code 
 */
// ============================================================================
StatusCode NoPIDsParticleMaker::fillParticle 
( const ProtoParticle*    proto    ,
  const ParticleProperty* property , 
  Particle*               particle ) const 
{
  if ( 0 == proto    ) 
    { return Error ( "fillParticle: ProtoParticle*    is NULL" , 120 ) ; }
  if ( 0 == property ) 
    { return Error ( "fillParticle: ParticleProperty* is NULL" , 121 ) ; }
  if ( 0 == particle ) 
    { return Error ( "fillParticle: Particle*         is NULL" , 122 ) ; }
  
  const double mass = property -> mass() ;
  
  particle -> setParticleID ( ParticleID ( property -> jetsetID () ) ) ;
  particle -> setConfLevel  ( m_CL ) ;
  particle -> setMass       ( mass ) ;
  
  const TrStateP* state = proto -> trStateP () ;
  if ( 0 == state ) 
    { return Error ( "fillParticle: TrStateP*         is NULL" , 123 ) ; }
  
  // point on the track 
  particle -> setPointOnTrack( HepPoint3D( state -> x () , 
                                           state -> y () , 
                                           state -> z () ) ) ;

  // 4-momentum
  const double p    = state -> p  () ;
  const double sX   = state -> tx () ;
  const double sY   = state -> ty () ;     
  const double pZ   = p / sqrt( 1.0 + sX * sX + sY * sY ) ;
  HepLorentzVector mom;
  mom.setE  ( sqrt( mass * mass + p * p ) ) ;
  mom.setPx ( sX * pZ  ) ;
  mom.setPy ( sY * pZ  ) ;
  mom.setPz (      pZ  ) ;
  particle -> setMomentum( mom ) ;
  
  
  const HepSymMatrix& Cov = state -> pCovMatrix () ;
  
  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  HepSymMatrix pointOnTrackErr   ( Cov.sub ( 1 , 3 ) ) ;
  pointOnTrackErr ( 3 , 1 ) = 0.0;
  pointOnTrackErr ( 3 , 2 ) = 0.0;
  pointOnTrackErr ( 3 , 3 ) = 0.0;
  particle -> setPointOnTrackErr ( pointOnTrackErr ) ;
  
  // Set slope+Momentum error:
  HepSymMatrix slpMomErr         ( Cov.sub ( 3 , 5 ) ) ;
  particle -> setSlopesMomErr    ( slpMomErr       ) ;

  // Set position-slopes correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posSlopesCorr( 3 , 3 , 0 ) ;
  for( int i = 1; i <= 3; i++ ) 
    { 
      for ( int j = 1; j <= 2; j++ ) 
        { posSlopesCorr ( i , j ) = Cov( i + 2 , j ) ; } 
    }
  
  particle -> setPosSlopesCorr ( posSlopesCorr ) ;

  particle -> setOrigin        ( proto         ) ;
 
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
