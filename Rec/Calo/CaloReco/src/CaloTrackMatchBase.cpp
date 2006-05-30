// $Id: CaloTrackMatchBase.cpp,v 1.11 2006-05-30 09:42:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.9  2004/10/26 20:35:58  ibelyaev
//  improve properties of all Track-related algorithms
// 
// ============================================================================
// Include files
// ============================================================================
#include <functional>
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
// Track
#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// local
#include "CaloTrackMatchBase.h"

// ============================================================================
/** @file CaloTrackMatchBase.cpp
 *
 * Implementation file for class : CaloTrackMatchBase
 *
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @param type tool type   (useless) 
 *  @param name tool name   
 *  @param parent pointer to parent object 
 *         (algorithm, service or another tool)
 */
// ============================================================================
CaloTrackMatchBase::CaloTrackMatchBase
( const std::string& type   ,
  const std::string& name   , 
  const IInterface*  parent )
  : GaudiTool ( type , name , parent ) 
  //
  , m_state        ( 0          )
  , m_prevTrack    ( 0          )
  , m_bad          ( 1.0e+10    )
  , m_optimized    ( false      ) 
  //
  , m_zmin         ( -1 * Gaudi::Units::meter )   
  , m_zmax         ( 30 * Gaudi::Units::meter )
  , m_pdgID        ( 211        )
  , m_pid          ( 211        )
  , m_extrapolator ( 0          )
  , m_extrapolatorName("TrFirstCleverExtrapolator")//Default to be modified
  //
  , m_precision    ( 0.1        ) 
  , m_tolerance    ( 10 * Gaudi::Units::mm    ) 
  //
  , m_aux_sym2D ( )
  , m_aux_sym3D ( )
{ 
  // interfaces 
  declareInterface<ICaloTrackMatch>   ( this ) ;
  declareInterface<IIncidentListener> ( this ) ;  
  // properties 
  declareProperty ( "Extrapolator" , m_extrapolatorName ) ;
  declareProperty ( "ZMin"         , m_zmin             ) ;
  declareProperty ( "ZMax"         , m_zmax             ) ;
  declareProperty ( "PID"          , m_pdgID            ) ;
  declareProperty ( "Bad"          , m_bad              ) ;
  declareProperty ( "Tolerance"    , m_tolerance        ) ;
  declareProperty ( "Precision"    , m_precision        ) ;
  declareProperty ( "Optimized"    , m_optimized        ) ;
};
// ============================================================================

// ============================================================================
/** standard initialization
 *  @return status code
 */
// ============================================================================
StatusCode CaloTrackMatchBase::initialize()
{
  /// initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
  { return Error ( "Could not initialize the base class GaudiTool!" , sc ) ; }
  
  if ( m_tolerance <= 0.0  ) 
  { return Error ( " Non-positive 'Tolerance' is set! " ) ; }
  
  if ( m_precision >  0.5  ) 
  { Warning ( " Probalems with 'Precision' property (>1/2) " ) ; }
  
  // locate the extrapolator 
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName ) ;
  if ( 0 == m_extrapolator ) { return StatusCode::FAILURE ; }
  
  // subscribe to incidents 
  incSvc() -> addListener( this , IncidentType::EndEvent , 10 );
  
  // set particle ID 
  m_pid =  LHCb::ParticleID( m_pdgID );
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalize
 *  @return status code
 */
// ============================================================================
StatusCode CaloTrackMatchBase::finalize()
{ 
  // delete state 
  if ( m_state != 0 ) { delete m_state; m_state = 0; }
  // reset track 
  m_prevTrack = 0 ;
  // finalize  the base class
  return GaudiTool::finalize();
};
// ============================================================================

// ============================================================================
/** handle the incident
 *  @see IIncidentListener 
 *  @param incident incident to be handled 
 */
// ============================================================================
void CaloTrackMatchBase::handle ( const Incident& /* incident */ ) 
{
  // reset previous track 
  m_prevTrack = 0 ;
  // delete previous state 
  if( 0 != m_state ) { delete m_state ; m_state = 0 ; }
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
CaloTrackMatchBase::~CaloTrackMatchBase(){};
// ============================================================================

// ============================================================================
/** Find State on specified Z.
 *  @param Object with Track data
 *  @param Z    of the State
 *  @param Zext z for extrapolation 
 *  param  covX allowed X-precision  (sigma**2)
 *  param  covY allowed Y-precision  (sigma**2)
 *  @return standard status code
 */
// ============================================================================
StatusCode CaloTrackMatchBase::findState
( const LHCb::Track* trObj , 
  const double         Z     , 
  const double         Zext  , 
  const double         covX  ,
  const double         covY  ) const 
{
  // check the arguments 
  if ( 0 == trObj ) { return Error ( "Track* points to NULL" ) ; }
  
  // use the previous state ? 
  // the same track as "previous" and state is valid 
  if ( trObj  != m_prevTrack  || 0 == m_state) 
  {
    // get new state 
    if ( 0 != m_state ) { delete m_state ; m_state = 0 ; }
    
    const LHCb::State st = trObj -> closestState( Z ) ;
    //OD if ( 0 == st ) 
    //{ return Error ( "Error from 'closestState', track=" + bits ( trObj ) ) ; }
    
    // check the state
    if ( st.z() < m_zmin || st.z() > m_zmax ) 
    { 
      debug() << " Problems: " << " Allowed : "  << m_zmin << "/" << m_zmax 
              << " Closest : " << Z << " Found : " << st.z() 
              << " Track "     << bits ( trObj ) << endreq ;
      return Error ( "Closest z is outside of allowed region, track=" + bits ( trObj ) ) ;
    }    
    // clone the state!
    m_state = st .clone() ;
    // Warning ( " Clone2       " + name() ) ;
  }
  
  const double dZ = fabs ( Zext - m_state->z()              ) ;
  const double dX = fabs (   dZ * m_state->stateVector()(2) ) ;
  const double dY = fabs (   dZ * m_state->stateVector()(3) ) ;
  
  const double sX = covX + m_state->covariance()(0,0)  ;
  const double sY = covY + m_state->covariance()(1,1)  ;
  
  if (  ( dX * dX > m_precision * sX   || 
          dY * dY > m_precision * sY ) && 
        ( dZ      > m_tolerance      ) ) 
  {
    // extrapolate the state  
    //OD    StatusCode sc = m_state -> extrapolate( m_extrapolator , Zext , m_pid );
    // Propagation not implemented so far ...
    StatusCode sc = m_extrapolator->propagate(*m_state , Zext , m_pid );

    // Warning ( " Extrapolate2 " + name() ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Error from extrapolator! track=" + bits ( trObj ) , sc ) ; }
  }
  
  // set new value for prev track 
  m_prevTrack = trObj ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** Find State on specified Z.
 *  @param Object with Track data
 *  @param Z of the State
 *  @return standard status code
 */
// ============================================================================
StatusCode CaloTrackMatchBase::findState
( const LHCb::Track* trObj ,
  const double         Z     , 
  const double         Zext  ) const 
{
  // check the arguments 
  if ( 0 == trObj ) { return Error ( "Track* points to NULL" ) ; }
  
  // use the previous state ? 
  // the same track as "previous" and state is valid 
  if ( trObj  != m_prevTrack  || 0 == m_state) 
  {
    // get new state 
    if ( 0 != m_state ) { delete m_state ; m_state = 0 ; }
    
    const LHCb::State st = trObj -> closestState( Z ) ;
    
    // check the state
    if ( st.z() < m_zmin || st.z() > m_zmax ) 
    { 
      debug() << " Problems: "  << " Allowed : "  << m_zmin << "/" << m_zmax 
              << " Closest : "  << Z << " Found : "    << st.z() 
              << " Track "      << bits ( trObj ) << endreq ;
      return Error ( "Closest z is outside of allowed region, track="  
                     + bits( trObj )  ) ;
    }    
    // clone the state!
    m_state = st.clone() ;
    // Warning ( " Clone        " + name() ) ;
  }
  
  if ( fabs( Zext -  m_state->z() ) > m_tolerance ) 
  {
    // extrapolate the state  
    //*StatusCode sc = m_state -> extrapolate( m_extrapolator , Zext , m_pid );
    // Propagation not implemented so far ...
    StatusCode sc = m_extrapolator->propagate(*m_state , Zext , m_pid );
    // Warning ( " Extrapolate  " + name() ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Error from extrapolator! tarck=" + bits ( trObj ) , sc ) ; }
  }
  
  // set new value for prev track 
  m_prevTrack = trObj ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** return the decoded track bits pattern
 *  @param trObj track objects 
 *  @return track bits pattern 
 */
// ============================================================================
std::string CaloTrackMatchBase::bits 
( const LHCb::Track* trObj ) const 
{ 
  if ( 0 == trObj ) { return std::string("<invalid>") ; }
  
  std::string msg( "bits: ") ;
  msg +=  "E:" +  ( (int) trObj -> checkFlag(LHCb::Track::Invalid) ) ;
  msg += "/U:" +  ( (int) !trObj -> checkFlag(LHCb::Track::Clone) ) ;
  msg += "/B:" +  ( (int) trObj -> checkFlag(LHCb::Track::Backward) ) ;
  msg += "/H:" +  ( (int) trObj -> history       () ) ;
  msg += "/L:" +  ( (int) trObj -> checkType (LHCb::Track::Long) ) ;
  msg += "/U:" +  ( (int) trObj -> checkType (LHCb::Track::Upstream) ) ;
  msg += "/D:" +  ( (int) trObj -> checkType (LHCb::Track::Downstream) ) ;
  msg += "/V:" +  ( (int) trObj -> checkType (LHCb::Track::Velo) ) ;
  msg += "/T:" +  ( (int) trObj -> checkType (LHCb::Track::Ttrack ) ) ;
  //  
  return msg ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
