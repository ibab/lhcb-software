// $Id: CaloClustersMCTruth5Alg.cpp,v 1.2 2003-12-18 15:33:36 cattanem Exp $
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// Event 
#include "Event/MCParticle.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CasloEvent/Event
#include "Event/CaloCluster.h"
#include "Event/CaloMCTools.h"
// local
#include "CaloMCTools.h"
#include "CaloClustersMCTruth5Alg.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : CaloClustersMCTruth5Alg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloClustersMCTruth5Alg>         s_factory ;
const        IAlgFactory&CaloClustersMCTruth5AlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloClustersMCTruth5Alg::CaloClustersMCTruth5Alg
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc     ) 
  , m_threshold1  (   1 * perCent  ) 
  , m_threshold2  ( 200 * MeV      ) 
  , m_clusters    (                )
{ 
  declareProperty      ( "Threshold1"   , m_threshold1            ) ;
  declareProperty      ( "Threshold2"   , m_threshold2            ) ;

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
CaloClustersMCTruth5Alg::~CaloClustersMCTruth5Alg() {}; 
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth5Alg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const CaloClusters                               Clusters  ;
  typedef const DeCalorimeter                              Detector  ;
  typedef const MCParticles                                Particles ;
  typedef RelationWeighted1D<CaloCluster,MCParticle,float> Table     ;
  typedef std::vector<Clusters*>                           VClusters ;

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get the detector 
  Detector*   detector  = get ( detSvc   () , detData   () , detector );
  if( 0 == detector  ) { return StatusCode::FAILURE ; }
  
  VClusters vclusters;
  for( Inputs::const_iterator icont = 
         m_clusters.begin() ; m_clusters.end() != icont ; ++icont ) 
    {
      // get input clusters 
      Clusters*   clusters  = get ( eventSvc () , *icont , clusters ) ;
      if( 0 == clusters  ) { return StatusCode::FAILURE ; }  
      vclusters.push_back( clusters );
    };
  
  // scale factor for recalculation of Eactive into Etotal 
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }

  typedef MCCaloHistory<CaloCluster> MCHistory ;

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
          
          // create history map 
          CaloMCMap map1 ;
          
          // create evaluator 
          MCHistory history( &map1 ) ;
          
          // build the history 
          history( *cluster ) ;
          
          // copy history map into separate container 
          CaloMCMap map2( map1 );
          
          { // update the copy of history map 
            for ( CaloMCMap::iterator entry = map1.begin() ; 
                  map1.end() != entry ; ++entry ) 
              { 
                const MCParticle* particle = entry -> first  ;
                const double      energy   = entry -> second ;
                updateCaloMCMap( particle , energy , map2 ) ; 
              }
          }
          
          const double cut = m_threshold1 * ( (*cluster) -> e () ) ;
          
          // fill the relation table 
          for( CaloMCMap::iterator entry = map2.begin() ; 
               map2.end() != entry ; ++entry ) 
            {    
              // put relation to relation table 
              const MCParticle* particle =                 entry ->  first ;
              const double      energy   = activeToTotal * entry -> second ;
              
              if( energy                   < cut  && 
                  particle->momentum().e() < m_threshold2 ) { continue ; }
              
              // fill actual relations 
              table->relate( *cluster , particle , energy   ) ;
            }

        };
    };
  

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
