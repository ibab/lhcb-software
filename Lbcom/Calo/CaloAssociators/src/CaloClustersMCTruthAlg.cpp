// $Id: CaloClustersMCTruthAlg.cpp,v 1.8 2004-11-23 14:13:21 cattanem Exp $
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
#include "CaloClustersMCTruthAlg.h"

// ============================================================================
/** @file CaloClustersMCTruthAlg.cpp
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
  
  debug() << "==> Execute" << endreq;
  
  // get input clusters 
  Clusters*   clusters  = get<Clusters> ( inputData () ) ;
  if( 0 == clusters  ) { return StatusCode::FAILURE ; }
  
  // get mc particles 
  Particles*  particles = get<Particles> ( m_particles ) ;
  if( 0 == particles ) { return StatusCode::FAILURE ; }
  
  // get the detector 
  Detector*   detector  = getDet<DeCalorimeter> ( detData () );
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
