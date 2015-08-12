// $Id: CellularAutomatonAlg.cpp,v 1.8 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// Include files 
// ============================================================================
// DetDesc
// ============================================================================
#include "DetDesc/IGeometryInfo.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloUtil.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// local
// ============================================================================
#include "CellularAutomatonAlg.h"
// ============================================================================
/** @file 
 *  Implementation file for class : CellularAutomatonAlg
 * 
 *  @date 2008-04-03 
 *  @author Victor Egorychev
 */
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( CellularAutomatonAlg )
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
CellularAutomatonAlg::CellularAutomatonAlg
( const std::string& name        ,
  ISvcLocator*       pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_sort             ( true  )
  , m_sortByET         ( false )
  , m_tool(0)
{
  
  declareProperty ( "InputData" , m_inputData = LHCb::CaloDigitLocation::Ecal);
  declareProperty ( "Detector"  , m_detData   = DeCalorimeterLocation::Ecal ) ;
  declareProperty ( "Level"     , m_neig_level = 0) ;
  declareProperty ( "OutputData", m_outputData=  LHCb::CaloClusterLocation::Ecal);  
  // sort the clusters ? 
  declareProperty ( "Sort"     , m_sort     ) ;
  declareProperty ( "SortByET" , m_sortByET ) ;
  // Tool name
  declareProperty("Tool"      , m_toolName = "CaloClusterizationTool");    
  // set default data as a function of detector
  m_detData    = LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  m_inputData  = LHCb::CaloAlgUtils::CaloDigitLocation( name , context() );
  m_outputData = LHCb::CaloAlgUtils::CaloClusterLocation( name , context() );
  // init counters
  m_pass = 0.;
  m_clus = 0.;
  m_event= 0.;
  m_passMin= 999999;
  m_passMax = 0;
}
// ============================================================================
// Destructor
// ============================================================================
CellularAutomatonAlg::~CellularAutomatonAlg() {}
// ============================================================================
// Initialization
// ============================================================================
StatusCode CellularAutomatonAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if (m_inputData.empty() || m_detData.empty() || m_outputData.empty() ) 
  {
    return Error("Path hits, path geom, path clusters must"
                 + std::string(" be defined in job options file!") );
  }
  
  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( m_detData );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }
  
  // Tool Interface
  m_tool = tool<ICaloClusterization>(m_toolName, this);
  
  return StatusCode::SUCCESS;
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode CellularAutomatonAlg::execute() 
{
  
  // get input data (sequential and simultaneously direct access!)  
  LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( m_inputData );
  
  /** Create the container of clusters and  
   *  register it into the event data store
   */ 
  LHCb::CaloCluster::Container* output = new LHCb::CaloCluster::Container();
  // update the version number (needed for serialization)
  output -> setVersion( 2 ) ;
  put ( output , m_outputData  ) ;
  
  // create vector of pointers for CaloCluster    
  std::vector<LHCb::CaloCluster*> clusters;
  
  // clusterization tool which return the vector of pointers for CaloClusters

  StatusCode sc;
  if( m_neig_level> 0){
    std::vector<LHCb::CaloCellID> seeds;
    seeds.clear();
    sc= m_tool->clusterize(clusters, digits, m_detector, seeds, m_neig_level) ;
  }
  else{
    sc = m_tool->clusterize(clusters, digits, m_detector) ;
  }
  
  
  if ( sc.isFailure() )
  { return Error(" Failure from the tool, no clusterization!" , sc ); }
  
  // put to the container of clusters
  for ( std::vector<LHCb::CaloCluster*>::const_iterator icluster = clusters.begin();
        clusters.end() != icluster; ++icluster)
  { output -> insert ( *icluster ) ; }
  
  /** sort the sequence to simplify the comparison 
   *  with other clusterisation techniques 
   */
  if ( m_sort )  
  { 
    if ( !m_sortByET ) 
    {
      // sorting criteria: Energy
      LHCb::CaloDataFunctor::Less_by_Energy<const LHCb::CaloCluster*> Cmp;
      // perform the sorting 
      std::stable_sort    ( clusters.begin()            ,
                            clusters.end  ()            ,
                            LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
    }
    else
    {
      // sorting criteria : Transverse Energy
      LHCb::CaloDataFunctor::Less_by_TransverseEnergy<const LHCb::CaloCluster*,
        const DeCalorimeter*> Cmp ( m_detector ) ;
      // perform the sorting 
      std::stable_sort   ( clusters.begin()            ,
                           clusters.end  ()            ,
                           LHCb::CaloDataFunctor::inverse( Cmp ) ) ;    
    }
  }
  
  // statistics
  m_pass += (double) m_tool->iterations();
  m_clus += (double) output->size();
  m_event += 1;
  if(m_tool->iterations() < m_passMin)m_passMin = m_tool->iterations();
  if(m_tool->iterations() > m_passMax)m_passMax = m_tool->iterations();
  
  counter ( "#clusters => '" + m_outputData + "'" ) += output->size() ;
  counter ( "Clusterisation pass") += m_tool->iterations();
 
  if (UNLIKELY( msgLevel( MSG::DEBUG) )){
    debug() << "Built " << clusters.size() <<" cellular automaton clusters  with " 
            << m_tool->iterations() << " iterations" <<endmsg;
    debug() << " ----------------------- Cluster List : " << endmsg;
    for(std::vector<LHCb::CaloCluster*>::iterator ic = clusters.begin();ic!=clusters.end();ic++){
      debug() << " Cluster seed " << (*ic)->seed() 
              << " energy " << (*ic)->e() 
              << " #entries " << (*ic)->entries().size() 
              << endmsg;
    }    
  }
  
  return StatusCode::SUCCESS;
}
// ============================================================================
//  Finalize
// ============================================================================
StatusCode CellularAutomatonAlg::finalize() 
{
  
  double avePass = 0.;
  double aveClus = 0.;
  if(m_event>0)
  {
    avePass = m_pass/m_event;
    aveClus = m_clus/m_event;
  }
  info() << "Built <" << aveClus 
         <<"> cellular automaton clusters/event  with <" 
         << avePass  << "> iterations (min,max)=(" << m_passMin << "," << m_passMax << ") on average " << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
// =============================================================================
// the END 
// =============================================================================
