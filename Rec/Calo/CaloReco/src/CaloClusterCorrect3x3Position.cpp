// ============================================================================
// Include files
// STD & STL 
#include <numeric>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/SmartDataPtr.h" 
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h" 
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
// CaloUtils
#include "CaloUtils/CaloDataFunctor.h"
#include "CaloUtils/CaloAlgUtils.h"
// Kernel
#include "Kernel/CaloCellID.h"
// local
#include "CaloClusterCorrect3x3Position.h"

// ============================================================================
/** @file CaloClusterCorrect3x3Position.cpp
 * 
 *  Implementation file for class : CaloClusterCorrect3x3Position
 * 
 *  @author Olivier Deschamps 
 *  @date 10/05/2002
 *
 *  Revision 1.2 2004/09/02 20:46:40  odescham 
 * - Transv. Shape parameters in option file 
 *
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloClusterCorrect3x3Position )

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloClusterCorrect3x3Position::CaloClusterCorrect3x3Position
( const std::string& name    ,
  ISvcLocator*       svcloc  )
  : GaudiAlgorithm ( name , svcloc ) 
  ,m_inputData    ( ) 
  ,m_detData      ( ) 
{
  declareProperty ( "InputData"         , m_inputData    = LHCb::CaloClusterLocation::Ecal) ;  
  declareProperty ( "Detector"          , m_detData      = DeCalorimeterLocation::Ecal ) ;  


  // set default data as a function of detector & context
  m_detData= LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  m_inputData = LHCb::CaloAlgUtils::CaloClusterLocation( name , context() );

}

// ============================================================================
/** destructor
 */
// ============================================================================
CaloClusterCorrect3x3Position::~CaloClusterCorrect3x3Position() {}


// ============================================================================
/** standard algorithm initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::initialize() 
{
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class!",sc);}
  return StatusCode::SUCCESS;

}


// ============================================================================
/** standard algorithm finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::finalize() 
{  
  /// finalize the base class 
  return GaudiAlgorithm::finalize();
}

// ============================================================================
/** helper function to calculate number of digits 
 *  in the cluster with given status 
 *  @param cluster pointet to the cluster object
 *  @param status  digit statsu to be checked
 *  @return number of digits with given status.
 *       In the case of errors it returns -1
 */
// ============================================================================
long CaloClusterCorrect3x3Position::numberOfDigits 
( const LHCb::CaloCluster*             cluster ,
  const LHCb::CaloDigitStatus::Status& status  ) const 
{
  /// check arguments 
  if( 0 == cluster ) 
  { Error(" numberOfDigits! CaloCluster* points to NULL!"); return -1;}
  ///
  long num = 0 ;
  for( LHCb::CaloCluster::Entries::const_iterator entry = 
         cluster->entries().begin() ;
       cluster->entries().end() != entry ; ++entry )
  {
    if( entry->status() & status ) { ++num ; }
  }
  ///
  return num ;
}

// ============================================================================
/** standard algorithm execution 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::execute() 
{
  

  /// avoid long names
  using namespace  LHCb::CaloDataFunctor;
  typedef LHCb::CaloClusters              Clusters;
  typedef Clusters::iterator        Iterator;
  
  /// load data 
  SmartDataPtr<DeCalorimeter> detector( detSvc() , m_detData );
  if( !detector )
    { return Error("could not locate calorimeter '"+m_detData+"'");}

  const DeCalorimeter* calo =  getDet<DeCalorimeter>( m_detData ) ;
  m_cell3x3.setDet ( calo ) ;
  m_neighbour.setDet ( calo );
  SmartDataPtr<Clusters>  clusters( eventSvc() , m_inputData );
  if( 0 == clusters )
    { return Error("Could not load input data ='"+m_inputData+"'");}

  //StatusCode sc = StatusCode::SUCCESS ;


  /// loop over all clusters 
  for( Iterator icluster = clusters->begin() ;
       clusters->end() != icluster ; ++icluster )
    {
      LHCb::CaloCluster* cluster = *icluster ;
      if( 0 == cluster )                { continue ; }

      double Energy=0;
      double PosX=0;
      double PosY=0;
      /// Find Cluster Seed 
      const LHCb::CaloCluster::Digits::const_iterator iSeed = 
        clusterLocateDigit( cluster->entries().begin() ,
                            cluster->entries().end  () ,
                            LHCb::CaloDigitStatus::SeedCell     );
      const LHCb::CaloDigit* seed = iSeed->digit() ;
      if( 0 == seed) { continue ; }  
      const LHCb::CaloCellID&  seedID = seed->cellID() ;
      // Loop over 3x3 digits
      const LHCb::CaloCluster::Entries& entries = cluster->entries();
      for(LHCb::CaloCluster::Entries::const_iterator ient = entries.begin();
          ient != entries.end() ; ient++){
        const LHCb::CaloDigit*  cell = ient->digit();
        const LHCb::CaloCellID&  cellID = cell->cellID() ;
        if( 0 != m_cell3x3( seedID , cellID) && m_neighbour(seedID,cellID) ){
          
          
          Energy += cell->e() * ient->fraction();
          PosX += detector->cellX(cellID) * cell->e() * ient->fraction();
          PosY += detector->cellY(cellID) * cell->e() * ient->fraction();
        }
      }
      PosX=PosX/Energy;
      PosY=PosY/Energy;
      LHCb::CaloPosition pos = cluster->position();
      LHCb::CaloPosition::Parameters  params;
      

      params(LHCb::CaloPosition::X)=PosX;
      params(LHCb::CaloPosition::Y)=PosY;
      params(LHCb::CaloPosition::E)=Energy;
      pos.setParameters( params );
      cluster->setPosition( pos );
    }
  return StatusCode::SUCCESS;
}

// ============================================================================
// The End 
// ============================================================================
