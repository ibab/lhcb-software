// $Id: CellularAutomatonAlg.cpp,v 1.1 2008-04-21 13:53:18 vegorych Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "DetDesc/IGeometryInfo.h"
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "CaloKernel/CaloUtil.h"
// ============================================================================
#include "CaloUtils/ClusterFunctors.h"

// local
#include "CellularAutomatonAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CellularAutomatonAlg
//
// 2008-04-03 : Victor Egorychev
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CellularAutomatonAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CellularAutomatonAlg::CellularAutomatonAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_sort             ( true  )
    , m_sortByET         ( false )
    , m_tool(0)
{
  declareProperty("InputData" , m_inputData = LHCb::CaloDigitLocation::Ecal);
  declareProperty("Detector"  , m_detData   = DeCalorimeterLocation::Ecal ) ;
  declareProperty("Neig_level", m_neig_level = 0) ;
  declareProperty("OutputData", m_outputData=  LHCb::CaloClusterLocation::Ecal);  
  // sort the clusters ? 
  declareProperty ( "Sort"     , m_sort     ) ;
  declareProperty ( "SortByET" , m_sortByET ) ;
  // Tool name
  declareProperty("Tool"      , m_toolName = "CaloClusterizationTool");
}
//=============================================================================
// Destructor
//=============================================================================
CellularAutomatonAlg::~CellularAutomatonAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CellularAutomatonAlg::initialize() {
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

//=============================================================================
// Main execution
//=============================================================================
StatusCode CellularAutomatonAlg::execute() {

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
  StatusCode sc = m_tool->clusterize(clusters, digits, m_detector);

  if ( sc.isFailure() ) 
  {
    return Error(" Failure from the tool, no clusterization!");
  }
  
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
      std::stable_sort
        ( clusters.begin()            ,
          clusters.end  ()            ,
          LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
    }
    else 
    {
      // sorting criteria : Transverse Energy
      LHCb::CaloDataFunctor::Less_by_TransverseEnergy<const LHCb::CaloCluster*,
        const DeCalorimeter*> Cmp ( m_detector ) ;
      // perform the sorting 
      std::stable_sort
        ( clusters.begin()            ,
          clusters.end  ()            ,
          LHCb::CaloDataFunctor::inverse( Cmp ) ) ;    
    }
    
  }

  // put to the container of clusters  

  for ( std::vector<LHCb::CaloCluster*>::const_iterator icluster = clusters.begin();
        clusters.end() != icluster; ++icluster)
  {
    output -> insert ( *icluster ) ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CellularAutomatonAlg::finalize() {

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
