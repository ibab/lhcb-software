// $Id: CaloClustersMCTruthAlg.cpp,v 1.1 2002-04-08 15:53:07 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/RelationWeighted2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// Event 
#include "Event/MCParticle.h"
// Event
#include "Event/CaloCluster.h"
#include "Event/CaloMCTools.h"
// local
#include "CaloClustersMCTruthAlg.h"

// ============================================================================
/** @file CaloClusterMCTruth.cpp
 * 
 *  Implementation file for class : CaloClustersMCTruthAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloClustersMCTruthAlg>         s_factory ;
const        IAlgFactory&CaloClustersMCTruthAlgFactory = s_factory ; 
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloClustersMCTruthAlg::CaloClustersMCTruthAlg
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc ) 
{};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClustersMCTruthAlg::~CaloClustersMCTruthAlg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruthAlg::initialize()
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruthAlg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  /// finalize thebase class
  return CaloAlgorithm::finalize () ;
};

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruthAlg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const CaloClusters                               Clusters  ;
  typedef RelationWeighted2D<CaloCluster,MCParticle,float> Table     ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input clusters 
  Clusters*   clusters  = get<Clusters>   ( eventSvc () , inputData() ) ;
  if( 0 == clusters  ) { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }
  
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
      table->relate( cluster , particle , (float) energy );
    };
  
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
