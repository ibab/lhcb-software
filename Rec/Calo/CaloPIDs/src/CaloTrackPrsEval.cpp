// $Id: CaloTrackPrsEval.cpp,v 1.3 2004-09-02 18:55:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 12:06:15  ibelyaev
//  update for rrading of histograms from the file
//
// Revision 1.1.1.1  2003/03/13 18:52:02  ibelyaev
// The first import of new package 
//
// Revision 1.1  2002/11/17 17:09:27  ibelyaev
//  new set of options and tools
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
#include "CaloTrackPrsEval.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class CaloTrackPrsEval
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackPrsEvalFactory 
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloTrackPrsEval>         s_Factory ;
const        IToolFactory&CaloTrackPrsEvalFactory = s_Factory ;
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
CaloTrackPrsEval::CaloTrackPrsEval
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent )
  , m_bad              ( -100 * GeV ) 
  // associator to charged clusters 
  , m_input            ( CaloDigitLocation::Prs )
  // extrapolator 
  , m_extrapolatorType ( "TrLinearExtrapolator" )
  , m_extrapolatorName (            )
  , m_extrapolator     ( 0          )
  // particle ID for extrapolator 
  , m_pid              ( 211        )
  , m_pidPDG           ( 211        )
  // tolerance parameter 
  , m_tol              ( 3          ) // number of sigma
  // shower size 
  , m_shower           ( 1 * mm     ) // effective "size of EM shower" 
  // minimal track error
  , m_safe             ( 0.5 * mm   ) // effective "reasonable" error 
  // technical 
  , m_digits           ( 0          )
  // technical
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
  // define the appropriate defaults for detector data 
  setDetName      ( DeCalorimeterLocation:: Prs ) ;
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
StatusCode    CaloTrackPrsEval::initialize ()
{
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
    { return Error("Coudl not initialize the base class CaloTool",sc); }
  
  // set the detector 
  const DeCalorimeter* detector = getDet<DeCalorimeter>( detName () );
  if( 0 == detector ) { return StatusCode::FAILURE ; }
  setDet( detector );

  // calculate the z position of calorimeter  
  m_z = det()->geometry()->toGlobal( HepPoint3D( 0 , 0 , 0 ) ).z() ;
  
  // subscribe the incident 
  incSvc() -> addListener( this , IncidentType::EndEvent   , 10 );
  
  // locate the extrapoaltor
  m_extrapolator = 
    tool<ITrExtrapolator>  ( m_extrapolatorType , m_extrapolatorName  ) ;
  
  // set pid 
  m_pid = ParticleID( m_pidPDG );
  
  if( 0 >= m_tol    ) { Warning("NonPositive  Tolerance  parameter"  ) ; }
  if( 0 >= m_shower ) { Warning("NonPositive  ShowerSize parameter" ) ; }
  
  m_digits = 0 ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
namespace Local
{
  // ==========================================================================
  /**  pure technical method
   *   (just a copy of std::transform with
   *   a minor "improvement")
   */
  // ==========================================================================
  template <class InputIterator, class OutputIterator, class UnaryOperation>
  inline OutputIterator 
  transform_ref(InputIterator   first  , 
                InputIterator   last   ,
                OutputIterator  result , 
                UnaryOperation& op     )
  {
    for ( ; first != last; ++first, ++result) { *result = op(*first); }
    return result;   
  };
  // ==========================================================================
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
StatusCode CaloTrackPrsEval::finalize   ()
{
  // reset detector 
  setDet( (const DeCalorimeter*) 0 );
  // finalize the base class 
  return CaloTool::finalize();
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
void CaloTrackPrsEval::handle( const Incident& /* inc */  ) { m_digits = 0 ; };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloTrackIdEval
 *  It evaluates the Prs energy deposition for the given track 
 *  @param  track  pointer to thobject to be processed
 *  @param  the value of the estimator
 */  
// ============================================================================
double CaloTrackPrsEval::operator() 
  ( const TrStoredTrack* track ) const 
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
StatusCode CaloTrackPrsEval::process    
( const TrStoredTrack* track , 
  double&              value ) const 
{
  // avoid long names 
  typedef std::vector<const CaloDigit*> Digits ;
  
  MsgStream log ( msgSvc() , name() ) ;
  
  // set the initial value to some "bad value"
  value =   0 ;
  // check arguments 
  if( 0 == track    ) { return Error("Track points to NULL!"      , 100 ) ; }
  
  // get the container of digits 
  if( 0 == m_digits ) { m_digits = get<CaloDigits>( m_input ) ; }
  if( 0 == m_digits ) { return StatusCode::FAILURE            ; }
  
  StatusCode sc = findTrackProjection( track , m_z ) ;
  if( sc.isFailure() ) 
    { return Error("Track is not extrapolated to '"+detName()+"'" , sc ) ; }
  
  // find "central" cell 
  const HepPoint3D trackPoint ( m_trX , m_trY , m_z       ) ;
  const CaloCellID trackCell  ( det()->Cell( trackPoint ) ) ;
  
  /// track does not touch the detector 
  if( CaloCellID() == trackCell )
    { value = m_bad ; return StatusCode::SUCCESS ; }            // ATTENTION !
  
  // get all neighbor cells  
  const CaloNeighbors& cells = det()->neighborCells( trackCell ) ;
  
  // get all neighbor digits 
  Digits digits        ( cells.size   () + 1 , (const CaloDigit*) 0 ) ;
  // add the central cell to the list of neighbor cells 
  *digits.begin() =    (*m_digits)( trackCell ) ;
  // get all neighbor cells 
  Local::transform_ref ( cells.begin  ()     , 
                         cells.end    ()     , 
                         digits.begin () + 1 , *m_digits );     // ATTENTION ! 
  
  // loop over all cells 
  for( Digits::const_iterator idigit = digits.begin() ;
       digits.end() != idigit ; ++idigit ) 
    {
      const CaloDigit* digit = *idigit ;
      if( 0 == digit ) { continue ; }
      
      // get the cell position and size 
      const HepPoint3D cell(       det()->cellCenter( digit->cellID() ) ) ;
      const double     size( 0.5 * det()->cellSize  ( digit->cellID() ) ) ;
      
      // accumulate the energy 
      if( fabs( cell.x() - m_trX ) <= size + m_tol * m_trXe + m_shower &&
          fabs( cell.y() - m_trY ) <= size + m_tol * m_trYe + m_shower ) 
        { value += digit->e() ; } 
    }
  
  // clear the temporary container 
  digits.clear() ;
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** find a track projection to a certain Z 
 *  @param  track track to be projected 
 *  @param  z     z-position of projection
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackPrsEval::findTrackProjection
( const TrStoredTrack* track , 
  const double         z     ) const 
{
  // 
  m_tr = false ;                                               // ATTENTION 
  if( 0 == track ){ return Error("findTrackPosition: Invalid Track" ) ; }
  
  // get the state 
  const TrState* state0 = track->closestState( z );
  if( 0 == state0 ){ return Error("findTrackPosition: Invalid State" ) ; }
  
  // clone && extrapolate  
  TrState* state = state0->clone() ;                                 // "NEW"
  StatusCode sc  = state->extrapolate( m_extrapolator , z , m_pid );
  if( sc.isFailure() ) 
    {
      delete state ; state = 0 ;                                    // DELETE 
      return Error("Error from Extrapolator",sc);                  
    }
  
  // find position "P" 
  const TrStateP* stateP = dynamic_cast<TrStateP*> ( state ) ;
  if( 0 != stateP ) 
    {
      m_trX  =       stateP ->  x  () ;
      m_trY  =       stateP ->  y  () ;
      m_trXe = sqrt( stateP -> eX2 () ) ;
      m_trYe = sqrt( stateP -> eY2 () ) ;
      m_tr   = true             ;                          // ATTENTION !!!
      /// 
      m_trXe = m_trXe < m_safe ? m_safe : m_trXe ;
      m_trYe = m_trYe < m_safe ? m_safe : m_trYe ;      
    }
  else
    { // find position "L"
      const TrStateL* stateL = dynamic_cast<TrStateL*> ( state ) ;
      if( 0 != stateL ) 
        {
          m_trX  =       stateL ->  x  () ;
          m_trY  =       stateL ->  y  () ;
          m_trXe = sqrt( stateL -> eX2 () ) ;
          m_trYe = sqrt( stateL -> eY2 () ) ;
          m_tr   = true             ;                      // ATTENTION !!!
          m_trXe = m_trXe < m_safe ? m_safe : m_trXe ;
          m_trYe = m_trYe < m_safe ? m_safe : m_trYe ;      
        }
    }
  
  // delete state 
  if( 0 != state ) { delete state ; state = 0 ; }
  
  if( !m_tr ) { return Error("findTrackPosition: Absolutely invalid state!"); }
  
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
StatusCode CaloTrackPrsEval::process    
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
double CaloTrackPrsEval::operator() 
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
