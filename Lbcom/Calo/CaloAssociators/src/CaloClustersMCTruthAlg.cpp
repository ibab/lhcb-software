// $Id: CaloClustersMCTruthAlg.cpp,v 1.5 2002-06-16 17:19:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/06/13 12:28:48  ibelyaev
//  new options and new algorithm
//
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// Event 
#include "Event/MCParticle.h"
#include "Event/KeyedObject.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CasloEvent/Event
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
  : CaloAlgorithm ( name , svc                  ) 
  , m_threshold   ( 10 * perCent                ) 
  , m_particles   ( MCParticleLocation::Default )
{ 
  declareProperty( "Threshold"   , m_threshold  ) ;
  declareProperty( "MCParticles" , m_particles  ) ;
};
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
  typedef const DeCalorimeter                              Detector  ;
  typedef const MCParticles                                Particles ;
  typedef RelationWeighted1D<CaloCluster,MCParticle,float> Table     ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input clusters 
  Clusters*   clusters  = get ( eventSvc () , inputData () , clusters ) ;
  if( 0 == clusters  ) { return StatusCode::FAILURE ; }
  
  // get mc particles 
  Particles*  particles = get( eventSvc  () , m_particles  , particles ) ;
  if( 0 == particles ) { return StatusCode::FAILURE ; }
  
  // get the detector 
  Detector*   detector  = get ( detSvc   () , detData   () , detector );
  if( 0 == detector  ) { return StatusCode::FAILURE ; }
  
  // scale factor for recalculation of Eactive into Etotal 
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }
  
  // create the MCtruth evaluator 
  EnergyFromMCParticle<CaloCluster> evaluator;
  
  // loop over all clusters 
  for( Clusters::const_iterator cluster = clusters->begin() ;
       clusters->end() != cluster ; ++cluster ) 
    {
      // Skip NULLs
      if( 0 == *cluster ) { continue ; }
      // define "current cut" value 
      const double  cut = m_threshold * (*cluster)->e() ;
      // loop over all MC truth particles 
      for( Particles::const_iterator    particle = particles->begin() ; 
           particles->end() != particle ; ++particle )
        {    
          // Skip NULLS  
          if( 0 == *particle ) { continue ; } 
          // Skip low momentum particles  
          if( (*particle)->momentum().e() < 0.90 * cut ) { continue ; }
          // use the evaluator to extract exact MCTruth information 
          const double  energy =  
            evaluator( *cluster , *particle ) * activeToTotal ;
          // skip small energy depositions
          if(  cut > energy  ) { continue ; }
          // put relation to relation table 
          table->relate( *cluster , *particle , (float) energy );     
        };
    }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
