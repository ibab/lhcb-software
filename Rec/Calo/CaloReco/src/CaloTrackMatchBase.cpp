// $Id: CaloTrackMatchBase.cpp,v 1.2 2004-02-17 12:08:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.12  2002/07/17 15:55:15  ibelyaev
// ============================================================================
// Include files
#include <functional>
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
// Track
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"
#include "TrKernel/ITrExtrapolator.h"
// Calo 
#include "Kernel/CaloPrint.h"
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
  : CaloTool ( type , name , parent ) 
  , m_state        ( 0          )
  , m_prevTrack    ( 0          )
  , m_bad          ( 1.0e+10    )
  , m_zmin         ( -1 * meter )   
  , m_zmax         ( 30 * meter )
  , m_pdgID        ( 211        )
  , m_pid          ( 211        )
  , m_extrapolator ( 0          )
  , m_extrapolatorName("TrFirstCleverExtrapolator")
  , m_tolerance    ( 5 * mm     ) 
{ 
  // interfaces 
  declareInterface<ICaloTrackMatch>   ( this ) ;
  declareInterface<IIncidentListener> ( this ) ;  
  // properties 
  declareProperty( "Extrapolator" , m_extrapolatorName ) ;
  declareProperty( "ZMin"         , m_zmin             ) ;
  declareProperty( "ZMax"         , m_zmax             ) ;
  declareProperty( "PID"          , m_pdgID            ) ;
  declareProperty( "Bad"          , m_bad              ) ;
  declareProperty( "Tolerance"    , m_tolerance        ) ;
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
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() )
    return Error( "Could not initialize the base class CaloTool!", TOOL );
  
  // locate the extrapolator 
  m_extrapolator = tool<ITrExtrapolator>( m_extrapolatorName ) ;
  if( 0 == m_extrapolator ) { return StatusCode::FAILURE ; }
  
  // subscribe to incidents 
  incSvc() -> addListener( this , IncidentType::EndEvent , 10 );
  
  // set particle ID 
  m_pid =  ParticleID( m_pdgID );
  
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
  return CaloTool::finalize();
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
/** The main matching method (Stl interface)
 *  @param caloObj  pointer to "calorimeter" object (position)
 *  @param trObj    pointer to tracking object (track)
 *  @return pair of status code/chi2  for matching procedure
 */
//-------------------------------------------------------------------
ICaloTrackMatch::MatchingPair CaloTrackMatchBase::operator()
  ( const CaloPosition *caloObj, const TrStoredTrack *trObj )
{ 
  double chi2;
  StatusCode sc = match( caloObj, trObj, chi2 );
  return MatchingPair( sc, chi2 );
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
CaloTrackMatchBase::~CaloTrackMatchBase(){};
// ============================================================================

// ============================================================================
/** Find TrState on specified Z.
*  @param Object with Track data
 *  @param Z of the TrState
 *  @return standard status code
 */
// ============================================================================
StatusCode CaloTrackMatchBase::findState
( const TrStoredTrack* trObj ,
  const double         Z     , 
  const double         Zext  ) const 
{
  // check the arguments 
  if( 0 == trObj ) { return Error("TrStoredTrack* points to NULL"); }
  
  // use the previous state ? 
  // the same track as "previous" and state is valid 
  if( trObj  != m_prevTrack  || 0 == m_state) 
    {
      // get new state 
      if( 0 != m_state ) { delete m_state ; m_state = 0 ; }
      
      const TrState* st = trObj -> closestState( Z ) ;
      if( 0 == st ) { return Error("Error from 'closestState'"); }
      
      // check the state
      if( st->z() < m_zmin || st->z() > m_zmax ) 
        { 
          MsgStream log( msgSvc() , name() );
          log << MSG::DEBUG 
              << " Problems: "
              << " Allowed : "  << m_zmin << "/" << m_zmax 
              << " Closest : "  << Z 
              << " Found : "    << st->z() 
              << " Track " ;
          CaloPrint print;
          std::string msg( "bits: ") ;
          msg +=  "E:" + print( (int) trObj -> errorFlag () ) ;
          msg += "/U:" + print( (int) trObj -> unique    () ) ;
          msg += "/H:" + print( (int) trObj -> history   () ) ;
          msg += "/f:" + print( (int) trObj -> forward   () ) ;
          msg += "/m:" + print( (int) trObj -> match     () ) ;
          msg += "/v:" + print( (int) trObj -> velo      () ) ;
          msg += "/t:" + print( (int) trObj -> veloTT    () ) ;
          msg += "/b:" + print( (int) trObj -> veloBack  () ) ;
          msg += "/s:" + print( (int) trObj -> seed      () ) ;
          msg += "/u:" + print( (int) trObj -> upstream  () ) ;
          log << msg 
              << "/k:" << (int) trObj -> key () 
              << endreq ;
          return Error("Closest z is outside of allowed region [" + 
                       print( m_zmin ) + "," + print( m_zmax ) + "] " + msg );
        }    
      // clone the state!
      m_state = st -> clone() ;
    }
  
  if( fabs( Zext -  m_state->z() ) > m_tolerance ) 
    {
      // extrapolate the state  
      StatusCode sc = m_state -> extrapolate( m_extrapolator , Zext , m_pid );
      if( sc.isFailure() ) 
        { return Error("Error from extrapolator!", sc ); }
    }
  
  // set new value for prev track 
  m_prevTrack = trObj ;
   
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
