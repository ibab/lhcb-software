// $Id: CaloTrackHcalEval.cpp,v 1.1 2002-11-17 17:09:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event
#include "Event/CaloDigit.h"
// local
#include "CaloTrackHcalEval.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class CaloTrackHcalEval
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackHcalEvalFactory 
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloTrackHcalEval>         s_Factory ;
const        IToolFactory&CaloTrackHcalEvalFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service xor tool)
 */
// ============================================================================
CaloTrackHcalEval::CaloTrackHcalEval
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool   ( type, name , parent     )
  , m_bad      ( -100 * GeV              )
  , m_input    ( CaloDigitLocation::Hcal )
  , m_eventSvc ( 0 ) 
  , m_incSvc   ( 0 ) 
  , m_digits   ( 0 ) 
{
  // declare interfaces 
  declareInterface<ICaloTrackIdEval>  ( this ) ;
  declareInterface<IIncidentListener> ( this ) ;
  // declare properties 
  declareProperty ( "BadValue" , m_bad           ) ;
  declareProperty ( "Input"    , m_input         ) ;
  // define the appropriate default for detector data 
  setDetName      ( DeCalorimeterLocation:: Hcal ) ;  
};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloTool 
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTrackHcalEval::initialize ()
{
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloTool",sc); }
  
  // set the detector 
  const DeCalorimeter* detector = get( detSvc () , detName () , detector );
  if( 0 == detector ) { return StatusCode::FAILURE ; }
  setDet( detector );
  
  // subscribe the incident 
  sc = serviceLocator() -> service ( "IncidentSvc" , m_incSvc , true );
  if( sc.isFailure() ) { return Error("Could not locate IIncidentSvc!", sc );}
  if( 0 == m_incSvc  ) { return Error("Could not locate IIncidentSvc!"     );}
  m_incSvc -> addListener( this , "EndEvent"   , 10 );
  
  sc = serviceLocator() -> service( "EventDataSvc" , m_eventSvc , true );
  if( sc.isFailure()   ) { return Error("Could not locate EventDataSvs", sc );}
  if( 0 == m_eventSvc  ) { return Error("Could not locate EventDataSvc"     );}
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @see CaloTool 
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTrackHcalEval::finalize ()
{
  // release services 
  if( 0 != m_eventSvc ) { m_eventSvc -> release() ; m_eventSvc = 0 ; }
  if( 0 != m_incSvc   ) { m_incSvc   -> release() ; m_incSvc   = 0 ; }
  //
  return CaloTool::finalize();
};
// ============================================================================
/** handle the incident 
 *  @see Incident 
 *  @see IIncidentListener 
 *  @see IIncidentSvc 
 *  @param inc incident to be handled 
 */
// ============================================================================
void CaloTrackHcalEval::handle( const Incident& /* inc */  ) { m_digits = 0 ; };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloTrackIdEval
 *  It evaluates the Hcal energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  the value of the estimator
 */  
// ============================================================================
double CaloTrackHcalEval::operator() 
  ( const TrStoredTrack* track ) const 
{
  double value  = m_bad ;
  StatusCode sc = process( track , value );
  if( sc.isFailure() ) 
    { Error(" operator(): error from process()",sc) ; return m_bad ; }
  // 
  return value ;
};
// ============================================================================


// ============================================================================
/** The main processing method 
 *  @see ICaloTrackIdEval
 *  It evaluates the Hcal energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  value  (return) the value of the estimator
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackHcalEval::process    
( const TrStoredTrack* track , 
  double&              value ) const 
{
  if( 0 == track    ) 
    { return Error("Track points to NULL!"); }
  
  if( 0 == m_digits ) { m_digits = get( m_eventSvc , m_input , m_digits ) ; }
  if( 0 == m_digits )                        { return StatusCode::FAILURE ; }
  
  ///
  /// put here a real code 
  ///
  value = 3 * GeV  ;
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================


// ============================================================================
// The END
// ============================================================================
