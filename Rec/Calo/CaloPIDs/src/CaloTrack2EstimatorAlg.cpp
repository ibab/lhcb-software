// $Id: CaloTrack2EstimatorAlg.cpp,v 1.5 2005-05-08 09:34:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relation1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
// ============================================================================

/** @class CaloTrack2EstimatorAlg CaloTrack2EstimatorAlg.cpp
 *  
 *  The generic algorithm to associate Tracks to some Calo estimators
 * 
 *  The obvious concrete implementation:
 *   - Prs  energy (used for e+/e- ID)
 *   - Hcal energy (used for e+/e- and mu+/mu- ID)
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-14
 */
class CaloTrack2EstimatorAlg : public CaloTrackAlg
{
  // friend factory for instantiation
  friend class AlgFactory<CaloTrack2EstimatorAlg>;
public:
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();
protected:
  /** Standard constructor
   *  @param name algorithm name 
   *  @param pSvc service locator 
   */
  CaloTrack2EstimatorAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc );
  // destructor 
  virtual ~CaloTrack2EstimatorAlg() ;
private:
  // actual type of evaluator to be used 
  std::string         m_evalType ;
  // actual name of evaluator to be used
  std::string         m_evalName ;
  // evaluator itself 
  ICaloTrackIdEval*   m_eval     ;
  // low  limit 
  double              m_low      ;
  // high limit 
  double              m_high     ;
  // number of skips 
  unsigned long       m_skip     ;
};

// ============================================================================
/** @var CaloTrack2EstimatorAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloTrack2EstimatorAlg>         s_factory ;
const        IAlgFactory&CaloTrack2EstimatorAlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloTrack2EstimatorAlg::CaloTrack2EstimatorAlg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : CaloTrackAlg ( name , pSvc ) 
  , m_evalType    (          )
  , m_evalName    (          )
  , m_eval        (  0       )
  , m_low         ( -1.0e+20 ) // no default low limit 
  , m_high        (  1.0e+20 ) // no default high limit 
  , m_skip        (  0       ) 
  ///
{
  declareProperty ( "EvaluatorType" , m_evalType    ) ;
  declareProperty ( "EvaluatorName" , m_evalName    ) ;
  // track flags 
  declareProperty ( "LowLimit"      , m_low         ) ;
  declareProperty ( "HighLimit"     , m_high        ) ;
  // define the default appropriate input data
  setInputData    ( TrStoredTrackLocation:: Default ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloTrack2EstimatorAlg::~CaloTrack2EstimatorAlg() {}; 
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrack2EstimatorAlg::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_eval = tool<ICaloTrackIdEval> ( m_evalType , m_evalName ) ;
  if ( 0 == m_eval ) { return Error ( "Evaluator is not located" ) ;}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrack2EstimatorAlg::finalize() 
{  
  if ( 0 != m_skip ) 
  { always () << " Number of 'skips' is " << m_skip << endreq ; }  
  return CaloAlgorithm::finalize() ;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrack2EstimatorAlg::execute() 
{
  // avoid long names 
  typedef const TrStoredTrack              Track   ;
  typedef const TrStoredTracks             Tracks  ;
  typedef Relation1D<TrStoredTrack,float>  Table   ;
  
  // get the tracks from the store 
  const Tracks* tracks = get<Tracks>( inputData() );
  if( 0 == tracks      ) { return StatusCode::FAILURE ; }           // RETURN
  
  // create and register new relation object 
  Table*     table = new Table ( tracks->size() );
  StatusCode sc    = put( table , outputData() );
  if ( sc.isFailure()   ) { return StatusCode::FAILURE ; }            // RETURN
  
  // loop over all tracks 
  for ( Tracks::const_iterator itrack = tracks->begin() ; 
        tracks->end() != itrack ; ++itrack ) 
  {
    const Track* track = *itrack ;
    // skip NULLs 
    if ( 0 ==   track   ) { continue ; }
    // should we use this track ? 
    if ( !use ( track ) ) { continue ; }
    // perform the actual evaluation 
    const double value = (*m_eval)( track );
    // skip 
    if ( value < m_low || value > m_high ) { ++m_skip ; continue ; }
    // fill the relation table 
    table -> i_push ( track , value ) ;   // NB!!!  use "i_push"
  };
  
  // use "i_sort" ( mandatory after "i_push" ) ;
  table ->  i_sort() ;                    // NB !!! use "i_sort"

  if ( msgLevel ( MSG::DEBUG ) ) 
  {
    debug() 
      << " The total number of booked relations " 
      << table->relations().size() << endreq;
  }

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


