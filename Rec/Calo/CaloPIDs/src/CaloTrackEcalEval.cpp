// $Id: CaloTrackEcalEval.cpp,v 1.2 2004-02-17 12:06:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/03/13 18:52:02  ibelyaev
// The first import of new package 
//
// Revision 1.1  2002/11/17 17:09:27  ibelyaev
//  new set of options and tools
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
// local
#include "CaloTrackEcalEval.h"

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
  , m_bad    ( -1000        ) 
  , m_associatorType ( "AssociatorWeighted<CaloHypo,TrStoredTrack,float>" )
  , m_associatorName ( "ElectronMatch"                                    )
  , m_associator     ( 0 )
  , m_table          ( 0 )
{
  // declare interfaces 
  declareInterface<ICaloTrackIdEval>  ( this ) ;
  // declare interfaces 
  declareInterface<IIncidentListener> ( this ) ;
  // declare properties 
  declareProperty ( "BadValue"       , m_bad            ) ;
  declareProperty ( "AssociatorType" , m_associatorType ) ;
  declareProperty ( "AssociatorName" , m_associatorName ) ;
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
StatusCode    CaloTrackEcalEval::initialize ()
{
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Coudl not initialize the base class CaloTool",sc); }
  
  incSvc() -> addListener( this , "EndEvent"   , 10 );
  
  // locate the associator 
  m_associator = tool<IAsct>( m_associatorType , m_associatorName );
  if( 0 == m_associator ) { return StatusCode::FAILURE ; }
  
  m_table = 0 ;
  
  return StatusCode::SUCCESS ;
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
void CaloTrackEcalEval::handle( const Incident& /* inc */  ) { m_table = 0 ; };
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
  if( sc.isFailure() ) 
  { Error(" operator(): error from process()",sc) ; return m_bad ; }
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
  if( 0 == track   ) 
  { return Error("Track points to NULL!"); }
  
  if( 0 == m_table ) { m_table = m_associator->inverse() ; }
  Assert( 0 != m_table , "The Associatioh table points to NULL!");
  
  // get range of related hypos with chi2 weights  
  const Range range = m_table -> relations( track );
  
  if( !range.empty() ) { value = range.front().weight() ; }
  else                 { value = m_bad                  ; }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================


// ============================================================================
// The END
// ============================================================================
