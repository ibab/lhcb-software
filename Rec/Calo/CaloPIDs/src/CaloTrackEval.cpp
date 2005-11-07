// $Id: CaloTrackEval.cpp,v 1.5 2005-11-07 12:16:10 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/09/02 18:55:36  ibelyaev
//  prepare for HLT/Trigger development
//
// Revision 1.3  2004/02/17 12:06:15  ibelyaev
//  update for rrading of histograms from the file
//
// Revision 1.2  2003/07/17 14:45:35  ibelyaev
//  fix for HcalE estimator and new options
//
// Revision 1.1.1.1  2003/03/13 18:52:02  ibelyaev
// The first import of new package 
//
// Revision 1.3  2003/01/19 12:08:31  ibelyaev
//  fix for tiny memory leak in class CaloTrackEval
//
// Revision 1.2  2002/12/09 17:43:09  cattanem
// bug fixes
//
// Revision 1.1  2002/12/01 14:22:57  ibelyaev
//  Hcal stuff and updated S-coprrections
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event 
#include "Event/CaloCluster.h"
// local
#include "CaloTrackEval.h"

// ============================================================================
/** @file 
 *  Implementation file for class CaloTrackEval
 *  @author Vladimir Romanovsky Vladimir.Romanovski@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackEvalFactory 
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloTrackEval>         s_Factory ;
const        IToolFactory&CaloTrackEvalFactory = s_Factory ;
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
CaloTrackEval::CaloTrackEval
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent )
  , m_bad              ( -100 * GeV ) 
  // Calorimeter digits 
  , m_input            ( CaloDigitLocation::Hcal )
  // extrapolator 
  , m_extrapolatorType ( "TrackLinearExtrapolator" )
  , m_extrapolatorName (            )
  , m_extrapolator     ( 0          )
  // particle ID for extrapolator 
  , m_pid              ( 211        )
  , m_pidPDG           ( 211        )
  // tolerance parameter 
  , m_tol              ( 3          ) // number of sigma
  // shower size 
  , m_shower           ( 1 * cm     ) // effective "size of EM shower" 
  // minimal track error
  , m_safe             ( 1 * mm     ) // effective "reasonable" error 
  // technical 
  , m_z                ( -1 * km    ) 
  , m_st               ( 100 * mm   )  // step size 
  , m_numst            ( 8          )  // number of "steps"  
  , m_digits           ( 0          )  
  // technical
  , m_state            ( 0          )
  , m_tr               ( false      ) 
  , m_trX              ( -1 * km    ) 
  , m_trY              ( -1 * km    ) 
  , m_trXe             (  1 *  m    ) 
  , m_trYe             (  1 *  m    ) 
{
  // declare interfaces 
  declareInterface<ICaloTrackIdEval>  ( this ) ;
  // declare interfaces 
  declareInterface<IIncidentListener> ( this ) ;
  // declare properties 
  declareProperty ( "BadValue"         , m_bad              ) ;
  declareProperty ( "ExtrapolatorName" , m_extrapolatorName ) ;
  declareProperty ( "PID"              , m_pidPDG           ) ;
  declareProperty ( "Tolerance"        , m_tol              ) ;
  declareProperty ( "ShowerSize"       , m_shower           ) ;
  declareProperty ( "Digits"           , m_input            ) ;
  declareProperty ( "NumStep"          , m_numst            ) ;
  declareProperty ( "Step"             , m_st               ) ;
  declareProperty ( "Input"            , m_input            ) ;
  // define the approprate default for detector data 
  setDetName      ( DeCalorimeterLocation::Hcal );
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
StatusCode    CaloTrackEval::initialize ()
{
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Coudl not initialize the base class CaloTool",sc); }
  
  // set the detector 
  const DeCalorimeter* detector = getDet<DeCalorimeter>( detName () );
  if( 0 == detector ) { return StatusCode::FAILURE ; }
  setDet( detector );
  
  // calculate the z position of calorimeter's center   
  m_z = det()->geometry()->toGlobal( HepPoint3D( 0 , 0 , 0 ) ).z() ;
  
  incSvc() -> addListener( this , IncidentType::EndEvent   , 10 );
  
  // locate the extrapoaltor
  m_extrapolator = 
    tool<ITrackExtrapolator>  ( m_extrapolatorType , m_extrapolatorName ) ;
  
  // set pid 
  m_pid = ParticleID( m_pidPDG );
  
  if( 0 >= m_tol    ) { Warning( "NonPositive  Tolerance  parameter"  ) ; }
  if( 0 >= m_shower ) { Warning( "NonPositive  ShowerSize parameter" ) ; }
  
  if( 0 != m_state ) { delete m_state ; m_state = 0 ; }
  
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
void CaloTrackEval::handle( const Incident& /* inc */  ) 
{
  // delete the state 
  if( 0 != m_state ) { delete m_state ; m_state = 0 ; }
  // reset  digits  
  m_digits = 0 ; 
};
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloTrackIdEval
 *  It evaluates the Prs energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  the value of the estimator
 */  
// ============================================================================
double CaloTrackEval::operator() 
  ( const Track* track ) const 
{
  double value  = 0.0                      ;
  StatusCode sc = process( track , value ) ;
  if( sc.isFailure() ) 
    { Error(" operator(): error from process()" , sc ) ; return m_bad ; }
  // 
  return value ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 * 
 *  @see ICaloTrackIdEval
 *  It evaluates the Prs energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  value  (return) the value of the estimator
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackEval::process    
( const Track* track , 
  double&              value ) const 
{
  // avoid long names 
  typedef std::vector<const CaloDigit*> Digits ;
  typedef std::vector<const CaloDigit*> DigVec ;
  
  m_tr = false ;
  if( 0 != m_state  ) { delete m_state ; m_state = 0 ; }
  
  // set the initial value to null 
  value = m_bad ;
  // check arguments 
  if( 0 == track    ) { return Error("Track points to NULL!"      , 100 ) ; }
  
  // get the container of digits 
  if( 0 == m_digits ) { m_digits = get<CaloDigits> ( m_input ) ; }
  if( 0 == m_digits ) { return StatusCode::FAILURE             ; }
  
  if( 0 == track    ) { return Error("findTrackPosition: Invalid Track" ) ; }
  
  const State state0 = track->closestState( m_z - m_numst * m_st ) ;
  //OD if( 0 == state0 ){ return Error("findTrackPosition: Invalid State" ) ; }
  m_state = state0.clone() ;
  
  DigVec digused;
  
  for (int itr = -m_numst; itr <= m_numst; ++itr)
  {
    
    StatusCode sc = findTrackProjection( m_z + itr*m_st ) ;
    if( sc.isFailure() ) 
    { return Error("Track is not extrapolated to '"+detName()+"'" , sc ) ; }
    
    // find "central" cell 
    const HepPoint3D trackPoint ( m_trX , m_trY , m_z  + itr*m_st ) ;
    const CaloCellID trackCell  ( det()->Cell( trackPoint ) ) ;
    
    /// track does not touch the detector 
    if( CaloCellID() == trackCell ) 
    {
      continue;
    }
    else 
    {
      const CaloDigit* digit = m_digits->object( trackCell );
      if( 0 != digit ) digused.push_back( digit );
      
      // get all neighbor cells  
      const CaloNeighbors& cells = det()->neighborCells( trackCell ) ;
      for( CaloNeighbors::const_iterator in = cells.begin() ; 
           cells.end() != in ; ++in )
      {
        // get the cell position and size 
        const HepPoint3D cell(det()->cellCenter( *in )) ;
        const double    size( 0.5 * det()->cellSize ( *in ) ) ;
        if( fabs( cell.x() - m_trX ) <= 
            size + m_tol * m_trXe + m_shower &&
            fabs( cell.y() - m_trY ) <= 
            size + m_tol * m_trYe + m_shower ) 
        {
          const CaloDigit* digit = m_digits->object( *in );
          if( 0 != digit ) 
          { 
            digused.push_back( digit );
          } 
        }
      }
    }
  }
  
  // reset initial value 
  value = 0 ;
  if( 0 == digused.size () )  return StatusCode::SUCCESS ;  
  
  if( !digused.empty() )
  {
    // eliminate duplicates 
    std::stable_sort( digused.begin() , digused.end() );  
    DigVec::iterator it = std::unique( digused.begin() , digused.end() );
    digused.erase( it , digused.end() );     
  }  
  
  // reset initial value 
  value = 0 ;
  for(DigVec::iterator id = digused.begin(); digused.end() != id; ++id )
    {
      value = value + (*id)->e();
    }  
  
  return StatusCode::SUCCESS ;  
};

// ============================================================================

// ============================================================================
/** find a track projection to a certain Z 
 *  @param  z     z-position of projection
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackEval::findTrackProjection
(  const double         z     ) const 
{
  //OD StatusCode sc  = m_state->extrapolate( m_extrapolator , z , m_pid );
  StatusCode sc = m_extrapolator->propagate(*m_state , z , m_pid );
  if( sc.isFailure() ) 
  {
    return Error("Error from Extrapolator",sc);                  
  }
  
  // find position "P" 
  //const TrStateP* stateP = dynamic_cast<TrStateP*> ( m_state ) ;
  //if( 0 != stateP ) 
  m_trX  =       m_state ->  x  () ;
  m_trY  =       m_state ->  y  () ;
  m_trXe = sqrt( m_state -> errX2 () ) ;
  m_trYe = sqrt( m_state -> errY2 () ) ;
  m_tr   = true             ;                          // ATTENTION !!!
  /// 
  m_trXe = m_trXe < m_safe ? m_safe : m_trXe ;
  m_trYe = m_trYe < m_safe ? m_safe : m_trYe ;      
    
    if( !m_tr ) { return Error("findTrackPosition: Absolutely invalid state!"); }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
