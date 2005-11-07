// $Id: CaloSharedCellAlg.cpp,v 1.3 2005-11-07 12:12:43 odescham Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 12:08:10  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// ============================================================================
#define  CALOALGS_CALOSHAREDECELLALG_CPP 1 
// ============================================================================
// Include files
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/SmartRef.h" 
// CaloGen 
#include "CaloKernel/CaloException.h"
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// CaloUtils
#include "CaloUtils/Digit2ClustersConnector.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/SharedCells.h"
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

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloSharedCellAlg>          s_Factory ;
const        IAlgFactory& CaloSharedCellAlgFactory = s_Factory ; 

// ============================================================================
// Standard creator, initializes variables
// ============================================================================
CaloSharedCellAlg::CaloSharedCellAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator )
  , m_copy    ( true )
  , m_useSumEnergy  ( true  ) 
  , m_numIterations ( 5     )
  , m_useDistance   ( false )
  , m_showerSizes   ()
{
  m_showerSizes.push_back( 0.1090 * 121.50 * mm );
  m_showerSizes.push_back( 0.1326 *  60.75 * mm );
  m_showerSizes.push_back( 0.1462 *  40.50 * mm );

  declareProperty ( "ShareDistance"     , m_useDistance   ) ;
  declareProperty ( "ShareSumEnergy"    , m_useSumEnergy  ) ;
  declareProperty ( "Iterations"        , m_numIterations ) ;
  declareProperty ( "ShowerSizes"       , m_showerSizes   ) ;  

  // set the appropriate default for input data  
  setInputData    ( CaloClusterLocation::   Ecal ) ;
  // set the appropriate default for input data  
  setDetData      ( DeCalorimeterLocation:: Ecal ) ;
};

// ============================================================================
// Destructor
// ============================================================================
CaloSharedCellAlg::~CaloSharedCellAlg() {};

// ============================================================================
// Initialisation. Check parameters
// ============================================================================
StatusCode CaloSharedCellAlg::initialize() 
{
  MsgStream log( msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  ///
  StatusCode sc = CaloAlgorithm::initialize();
  ///
  if( sc.isFailure() ) 
    { return Error("could not initialize base class CaloAlgorithm!");}
  
  /// copy mode ? 
  m_copy = 
    outputData().empty()        ? false :
    outputData() == inputData() ? false : true ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
// Main execution
// ============================================================================
StatusCode CaloSharedCellAlg::execute() 
{
  // avoid long names 
  using namespace SharedCells     ;
  using namespace CaloDigitStatus ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  //

  // useful typedefs
  typedef CaloClusters        Clusters ;
  typedef const DeCalorimeter Detector ;
  
  // locate input data
  Clusters* clusters = get<Clusters>( inputData() );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // locate geometry (if needed) 
  Detector* detector = 0;
  if( m_useDistance )
  {    
    detector = getDet<DeCalorimeter> ( detData() );
    if( 0 == detector ){ return StatusCode::FAILURE ;}
  }
  
  Clusters* output = 0;
  if( m_copy ) ///< make a new container 
  {
    output = new Clusters();
    StatusCode sc = put( output , outputData() );
    if( sc.isFailure() ) { return StatusCode::FAILURE ; }
    // make a copy 
    for( Clusters::const_iterator i = clusters->begin() ;
         clusters->end() != i ; ++i )
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
  table.load( output->begin() , output->end  () , cutOff ) ;
  
  /// loop over all digits in the table  
  for( Table::Map::iterator entry = table.map().begin() ; 
       table.map().end() != entry ; ++entry ) 
  {
    const CaloDigit* dig = entry->first ;
    /// ignore  artificial zeros  
    if( 0 == dig ) { continue; }
    
    StatusCode sc = StatusCode::SUCCESS; 
    if      ( m_useSumEnergy &&  !m_useDistance )
    {    
      sc = summedEnergyAlgorithm   
        ( entry->second   , m_numIterations ) ; 
    }
    else if ( !m_useSumEnergy && !m_useDistance )
    { 
      sc = seedEnergyAlgorithm     
        ( entry->second   , SeedCell        ) ; 
    }
    else if ( m_useSumEnergy &&   m_useDistance )
    { 
      sc = summedDistanceAlgorithm 
        ( entry->second   , 
          detector        ,
          dig->cellID()   , 
          m_showerSizes   ,
          m_numIterations                   ) ; 
    }
    else if ( !m_useSumEnergy &&  m_useDistance )
    { 
      sc = seedDistanceAlgorithm   
        ( entry->second   ,
          detector        ,
          SeedCell        ,
          dig->cellID()   ,
          m_showerSizes                     ) ;
    }
    else { return Error("Funny condition :-)) "); }
    ///
    if( sc.isFailure() )
    { return Error("Could not redistribute the energy!"); }
  }
  ///
  return StatusCode::SUCCESS;
  ///  
};

// ============================================================================
// The End 
// ============================================================================
