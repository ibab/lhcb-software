// $Id: CaloClustersMCTruth2Alg.cpp,v 1.4 2004-11-23 14:13:21 cattanem Exp $
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event 
#include "Event/MCParticle.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CasloEvent/Event
#include "Event/CaloCluster.h"
#include "Event/CaloMCTools.h"
// local
#include "CaloClustersMCTruth2Alg.h"

// ============================================================================
/** @file CaloClustersMCTruth2Alg.cpp
 * 
 *  Implementation file for class : CaloClustersMCTruth2Alg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloClustersMCTruth2Alg>         s_factory ;
const        IAlgFactory&CaloClustersMCTruth2AlgFactory = s_factory ; 
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloClustersMCTruth2Alg::CaloClustersMCTruth2Alg
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc ) 
{};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClustersMCTruth2Alg::~CaloClustersMCTruth2Alg() {}; 
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth2Alg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const CaloClusters                               Clusters  ;
  typedef const DeCalorimeter                              Detector  ;
  typedef RelationWeighted1D<CaloCluster,MCParticle,float> Table     ;
  
  debug() << "==> Execute" << endreq;
  
  // get input clusters 
  Clusters*   clusters  =    get<Clusters>      ( inputData () ) ;
  if( 0 == clusters  )  { return StatusCode::FAILURE ; }
  
  // get the detector 
  Detector*   detector  = getDet<DeCalorimeter> ( detData   () );
  if( 0 == detector  )               { return StatusCode::FAILURE ; }
  
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }
  
  if( 0 == clusters -> size () ) 
  { Warning("Empty container of clusters '" + inputData() + "'"); }
  if( 0 == clusters -> size () )     { return StatusCode::SUCCESS ; }
  
  
  // create the MCtruth evaluator 
  LargestDeposition<CaloCluster> evaluator;
  
  // fill the relation data 
  for( Clusters::const_iterator icluster = clusters->begin() ;
       clusters->end() != icluster ; ++icluster ) 
  {
    const CaloCluster* cluster = *icluster ;
    if( 0 == cluster  ) { continue ; }                   // Skip NULLs 
    
    // use the evaluator to extract MCtruth information (follow references)
    const ParticlePair p( evaluator( cluster ) );
    const MCParticle* particle = p.second ;
    const double      energy   = p.first  ;
    if( 0 == particle || 0 == energy ) { continue ; }    // Skip NULLs 
    
    // put relation to relation table 
    table->relate( cluster , particle , (float) energy * activeToTotal );
  };
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
