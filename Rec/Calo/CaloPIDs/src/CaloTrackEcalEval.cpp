// $Id: CaloTrackEcalEval.cpp,v 1.4 2005-05-08 09:34:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackEcalEval.h"
// ============================================================================

// ============================================================================
/** @file 
 *
 *  Implementation file for class CaloTrackEcalEval
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackEcalEvalFactory 
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloTrackEcalEval>         s_Factory ;
const        IToolFactory&CaloTrackEcalEvalFactory = s_Factory ;
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
CaloTrackEcalEval::CaloTrackEcalEval
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent )
  , m_bad    ( -1000 ) 
  , m_table  ( 0     )
{
  // declare interfaces 
  declareInterface<ICaloTrackIdEval>  ( this ) ;
  // declare interfaces 
  declareInterface<IIncidentListener> ( this ) ;
  // declare properties 
  declareProperty ( "BadValue"  , m_bad    ) ;
  declareProperty ( "Table"     , m_itable ) ;
};
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
CaloTrackEcalEval::~CaloTrackEcalEval()
{ if ( 0 != m_table ) { m_table = 0 ; } } ;

// ============================================================================
/** standard initialization method 
 *  @see CaloTool 
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTrackEcalEval::initialize ()
{
  StatusCode sc = CaloTool::initialize();
  if ( sc.isFailure() ) { return sc ; }
  incSvc() -> addListener ( this , IncidentType::EndEvent , 10 );  
  if ( 0 != m_table ) { m_table = 0 ; } 
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
StatusCode    CaloTrackEcalEval::finalize ()
{
  if ( 0 != m_table ) { m_table = 0 ;  } 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** handle the incident 
 *  @see Incident 
 *  @see IIncidentListener 
 *  @see IIncidentSvc 
 *  @param inc incident to be handled 
 */
// ============================================================================
void CaloTrackEcalEval::handle 
( const Incident& /* inc */  ) 
{ if ( 0 != m_table ) { m_table = 0 ; } } ;
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloTrackIdEval
 *  It evaluates the Ecal energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  the value of the estimator
 */  
// ============================================================================
double CaloTrackEcalEval::operator() 
  ( const TrStoredTrack* track ) const 
{
  double value  = m_bad ;
  StatusCode sc = process( track , value );
  if ( sc.isFailure() ) 
  { Error ( " operator(): error from process()" , sc ) ; return m_bad ; }
  // 
  return value ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloTrackIdEval
 *  It evaluates the Ecal energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  value  (return) the value of the estimator
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackEcalEval::process    
( const TrStoredTrack* track , 
  double&              value ) const 
{
  if ( 0 == track   ) 
  { return Error("Track points to NULL!"); }
  
  if ( 0 == m_table ) 
  {
    const ITable* itable = get<ITable>( m_itable ) ;
    Assert ( 0 != itable , "Relation Table points to NULL!"        ) ;
    m_table = itable->inverse() ;
    Assert ( 0 != m_table , "Unable to extract the inverse table!" ) ;
  }
  
  // get range of related hypos with chi2 weights  
  const Range range = m_table -> relations ( track ) ;
  
  if( !range.empty() ) { value = range.front().weight() ; }
  else                 { value = m_bad                  ; }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloTrackIdEval 
 *  It evaluated the Track ID estimators using the calorimeter information  
 *  @param  track  pointer to the object to be processed
 *  @param  value  (return) the value of the estimator
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloTrackEcalEval::process    
( const TrgTrack* /* track */ , 
  double&         /* value */ ) const 
{ return Error(" process( TrgTrack* ): method is not implementer yet" ) ; } ;
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloTrackIdEval 
 *  It evaluated the Track ID estimators using the calorimeter information  
 *  @param  track  pointer to the object to be processed
 *  @param  the value of the estimator
 */  
// ============================================================================
double CaloTrackEcalEval::operator() 
  ( const TrgTrack*      track ) const 
{ 
  double value = 0 ;
  process ( track , value ) ;
  return value ;
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
