// $Id: CaloClustersMCTruth4Alg.cpp,v 1.4 2004-11-23 14:13:21 cattanem Exp $
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
#include "CaloMCTools.h"
#include "CaloClustersMCTruth4Alg.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : CaloClustersMCTruth4Alg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloClustersMCTruth4Alg>         s_factory ;
const        IAlgFactory&CaloClustersMCTruth4AlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloClustersMCTruth4Alg::CaloClustersMCTruth4Alg
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc     ) 
  , m_threshold1  (   2 * perCent  ) 
  , m_threshold2  ( 250 * MeV      ) 
  , m_clusters    (                )
{ 
  declareProperty      ( "Threshold1"   , m_threshold1            ) ;
  declareProperty      ( "Threshold2"   , m_threshold2            ) ;
  // set the appropriate default value  for input data (MCParticles)
  setInputData         ( MCParticleLocation::Default              ) ;
  m_clusters.push_back ( CaloClusterLocation::   Ecal             ) ;
  m_clusters.push_back ( CaloClusterLocation::   EcalSplit        ) ;
  declareProperty      ( "Clusters"     , m_clusters              ) ;
  // set the appropriate default value  for input data
  setDetData           ( DeCalorimeterLocation:: Ecal             ) ;
  // set the appropriate default value  for output data
  setOutputData        ( "Rec/Relations/EcalClusters2MCParticles" ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClustersMCTruth4Alg::~CaloClustersMCTruth4Alg() {}; 
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth4Alg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const CaloClusters                               Clusters  ;
  typedef const DeCalorimeter                              Detector  ;
  typedef const MCParticles                                Particles ;
  typedef RelationWeighted1D<CaloCluster,MCParticle,float> Table     ;
  typedef std::vector<Clusters*>                           VClusters ;

  debug() << "==> Execute" << endreq;
  
  // get mc particles 
  Particles*  particles = get<Particles>( inputData () ) ;
  if( 0 == particles ) { return StatusCode::FAILURE ; }
  
  // get the detector 
  Detector*   detector  = getDet<DeCalorimeter> ( detData () );
  if( 0 == detector  ) { return StatusCode::FAILURE ; }
  
  VClusters vclusters;
  for( Inputs::const_iterator icont = 
         m_clusters.begin() ; m_clusters.end() != icont ; ++icont ) 
  {
    // get input clusters 
    Clusters*   clusters  = get<Clusters> ( *icont ) ;
    if( 0 == clusters  ) { return StatusCode::FAILURE ; }  
    vclusters.push_back( clusters );
  };
  
  // scale factor for recalculation of Eactive into Etotal 
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }
  
  // create the MCtruth evaluator 
  AllEnergyFromMCParticle<CaloCluster> evaluator;
  
  // loop over the all containers of clusters 
  for ( VClusters::const_iterator cont = vclusters.begin() ; 
        vclusters.end() != cont ; ++cont )
    {
      // get the container of clusters 
      const Clusters* clusters = *cont ;
      if( 0 == clusters ) { continue  ; }
      
      // loop over all clusters in the container 
      for( Clusters::const_iterator cluster = clusters->begin() ;
           clusters->end() != cluster ; ++cluster ) 
        {
          // Skip NULLs
          if( 0 == *cluster ) { continue ; }
          // define "current cut" value 
          const double  cut = m_threshold1 * (*cluster)->e() ;
          // loop over all MC truth particles 
          for( Particles::const_iterator    particle = particles->begin() ; 
               particles->end() != particle ; ++particle )
            {    
              // Skip NULLS  
              if( 0 == *particle ) { continue ; } 
              const double mce = (*particle)->momentum().e() ;
              // Skip very low momentum particles  
              if( mce < 0.75 * cut && mce < m_threshold2 ) { continue ; }
              // use the evaluator to extract exact MCTruth information 
              const double  energy =  
                evaluator( *cluster , *particle ) * activeToTotal ;
              // skip small energy depositions
              if(  cut > energy  ) { continue ; }
              // put relation to relation table 
              table->relate( *cluster , *particle , (float) energy );     
            };
        };
    };

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
