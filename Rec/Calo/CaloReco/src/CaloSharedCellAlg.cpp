// ===========================================================================
// Include files
// from GaudiKernel
#include "GaudiKernel/MsgStream.h" 
// CaloGen 
#include "CaloKernel/CaloException.h"
//LHCb Kernel
#include "GaudiKernel/SystemOfUnits.h"
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// CaloUtils
#include "CaloUtils/Digit2ClustersConnector.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/SharedCells.h"
#include "CaloUtils/CaloAlgUtils.h"
// local
#include "CaloSharedCellAlg.h"

// ============================================================================
/** @file CaloSharedCellAlg.cpp
 * 
 *  Implementation file for class : CaloSharedCellAlg
 *
 *  @see CaloSharedCellAlg
 *  @see Digit2ClustersConnector 
 *  @see SharedCells 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 30/06/2001 
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloSharedCellAlg )

// ============================================================================
// Standard creator, initializes variables
// ============================================================================
CaloSharedCellAlg::CaloSharedCellAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_copy    ( true )
    , m_useSumEnergy  ( true  ) 
    , m_numIterations ( 5     )
    , m_useDistance   ( false )
    , m_showerSizes   ()
    , m_outputData    ()
    , m_inputData ()
    , m_detData  ()

{
  m_showerSizes.push_back( 0.1090 * 121.50 * Gaudi::Units::mm );
  m_showerSizes.push_back( 0.1326 *  60.75 * Gaudi::Units::mm );
  m_showerSizes.push_back( 0.1462 *  40.50 * Gaudi::Units::mm );

  declareProperty ( "ShareDistance"     , m_useDistance   ) ;
  declareProperty ( "ShareSumEnergy"    , m_useSumEnergy  ) ;
  declareProperty ( "Iterations"        , m_numIterations ) ;
  declareProperty ( "ShowerSizes"       , m_showerSizes   ) ;  
  declareProperty ( "InputData"         , m_inputData     = LHCb::CaloClusterLocation::Ecal) ;  
  declareProperty ( "OutputData"        , m_outputData    ) ;  
  declareProperty ( "Detector"          , m_detData       = DeCalorimeterLocation::Ecal ) ;  


  // set default data as a function of detector
  m_detData= LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  m_inputData = LHCb::CaloAlgUtils::CaloClusterLocation( name , context() );
}

// ============================================================================
// Destructor
// ============================================================================
CaloSharedCellAlg::~CaloSharedCellAlg() {}

// ============================================================================
// Initialisation. Check parameters
// ============================================================================
StatusCode CaloSharedCellAlg::initialize() 
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialise" << endmsg;
  ///
  StatusCode sc = GaudiAlgorithm::initialize();
  ///
  if( sc.isFailure() ) 
    { return Error("could not initialize base class GaudiAlgorithm!");}
  
  /// copy mode ? 
  m_copy = 
    m_outputData.empty()        ? false :
    m_outputData == m_inputData ? false : true ;
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode CaloSharedCellAlg::execute() 
{
  // avoid long names
  using namespace SharedCells     ;
  using namespace LHCb::CaloDigitStatus ;
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;
  //

  // useful typedefs
  typedef LHCb::CaloClusters        Clusters ;
  typedef const DeCalorimeter Detector ;
  
  // locate input data
  Clusters* clusters = get<Clusters>( m_inputData );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // locate geometry (if needed) 
  Detector* detector = 0;
  if( m_useDistance )
  {    
    detector = getDet<DeCalorimeter> ( m_detData );
    if( 0 == detector ){ return StatusCode::FAILURE ;}
  }
  
  Clusters* output = 0;
  if( m_copy ) ///< make a new container 
  {
    output = new Clusters();
    put( output , m_outputData );
    // make a copy 
    for( Clusters::const_iterator i = clusters->begin() ; clusters->end() != i ; ++i )
    { if( 0 != *i ) { output->insert( (*i)->clone() ) ; } }
  }
  else { output = clusters; } ///< update existing sequence
  
  /** build inverse connection table/object 
   *  keep only digits which have connections 
   *   with 2 clusters and more!
   */
  const unsigned int cutOff = 2 ;
  typedef Digit2ClustersConnector   Table;
  Table  table;
  StatusCode scc = table.load( output->begin() , output->end  () , cutOff ) ;
  if(scc.isFailure())warning()<<"Unable to load the table"<<endmsg;
  
  // sort digit by energy in order the subsequent processing is pointer address independent !
  std::vector<const LHCb::CaloDigit*> reOrder;
  for( Table::Map::iterator entry = table.map().begin() ; table.map().end() != entry ; ++entry ){
    const LHCb::CaloDigit* dig = entry->first ;
    reOrder.push_back( dig );
  }
  LHCb::CaloDataFunctor::Less_by_Energy<const LHCb::CaloDigit*> Cmp;
  std::stable_sort    ( reOrder.begin(), reOrder.end  () ,LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
  

  /// loop over all digits in the table  
  for( std::vector<const LHCb::CaloDigit*>::const_iterator idig = reOrder.begin() ; reOrder.end() != idig ; ++idig ){    
    Table::Map::iterator entry = table.map().find( *idig );
    if( entry == table.map().end() ) continue;
    const LHCb::CaloDigit* dig = entry->first ;
    /// ignore  artificial zeros  
    if( 0 == dig ) { continue; }
    
    StatusCode sc = StatusCode::SUCCESS; 
    if      ( m_useSumEnergy &&  !m_useDistance ){    
      sc = summedEnergyAlgorithm ( entry->second   , m_numIterations  ) ; // do not apply weights at this stage 
    }
    else if ( !m_useSumEnergy && !m_useDistance ){ 
      sc = seedEnergyAlgorithm( entry->second   , SeedCell ) ; 
    }
    else if ( m_useSumEnergy &&   m_useDistance ){ 
      sc = summedDistanceAlgorithm( entry->second   , 
                                    detector        ,
                                    dig->cellID()   , 
                                    m_showerSizes   ,
                                    m_numIterations  ) ; 
    }
    else if ( !m_useSumEnergy &&  m_useDistance ){ 
      sc = seedDistanceAlgorithm( entry->second   ,
                                  detector        ,
                                  SeedCell        ,
                                  dig->cellID()   ,
                                  m_showerSizes                    ) ;
    }
    else { return Error("Funny condition :-)) "); }
    ///
    if( sc.isFailure() ){ return Error("Could not redistribute the energy!"); }
  }
  

  counter ( "#Clusters from '" + m_inputData +"'") += clusters->size() ;
  
  ///
  return StatusCode::SUCCESS;
  ///  
}

StatusCode CaloSharedCellAlg::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


