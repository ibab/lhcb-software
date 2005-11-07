// $Id: CaloClusterCorrect3x3Position.cpp,v 1.3 2005-11-07 12:12:41 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// STD & STL 
#include <numeric>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h" 
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Stat.h"
// Kernel
#include "Kernel/CaloHypotheses.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h" 
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CaloPosition.h"
// Kernel
#include "CaloKernel/CaloCellID.h"
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

// ============================================================================
/** Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterCorrect3x3Position>         s_Factory ;
const        IAlgFactory&CaloClusterCorrect3x3PositionFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloClusterCorrect3x3Position::CaloClusterCorrect3x3Position
( const std::string& name    ,
  ISvcLocator*       svcloc  )
  : CaloAlgorithm ( name , svcloc ) 
{
 // set the appropriate defaults for input    data 
  setInputData    ( CaloClusterLocation::   Ecal       ) ;
  // set the appropriate defaults for output   data 
  setOutputData   ( CaloHypoLocation::      MergedPi0s ) ;
  // set the appropriate defaults for detector data 
  setDetData      ( DeCalorimeterLocation:: Ecal       ) ;
};

// ============================================================================
/** destructor
 */
// ============================================================================
CaloClusterCorrect3x3Position::~CaloClusterCorrect3x3Position() {};


// ============================================================================
/** standard algorithm initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::initialize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class!",sc);}
  return StatusCode::SUCCESS;

};


// ============================================================================
/** standard algorithm finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::finalize() 
{  
  /// finalize the base class 
  return CaloAlgorithm::finalize();
};

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
( const CaloCluster*             cluster ,
  const CaloDigitStatus::Status& status  ) const 
{
  /// check arguments 
  if( 0 == cluster ) 
  { Error(" numberOfDigits! CaloCluster* points to NULL!"); return -1;}
  ///
  long num = 0 ;
  for( CaloCluster::Entries::const_iterator entry = 
         cluster->entries().begin() ;
       cluster->entries().end() != entry ; ++entry )
  {
    if( entry->status() & status ) { ++num ; }
  }
  ///
  return num ;
};

// ============================================================================
/** standard algorithm execution 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterCorrect3x3Position::execute() 
{
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  /// avoid long names
  using namespace  CaloDataFunctor;
  typedef CaloClusters              Clusters;
  typedef Clusters::iterator        Iterator;
  
  /// load data 
  SmartDataPtr<DeCalorimeter> detector( detSvc() , detData() );
  if( !detector )
    { return Error("could not locate calorimeter '"+detData()+"'");}

  const DeCalorimeter* calo =  getDet<DeCalorimeter>( detData () ) ;
  m_cell3x3.setDet ( calo ) ;
  m_neighbour.setDet ( calo );
  SmartDataPtr<Clusters>  clusters( eventSvc() , inputData() );
  if( 0 == clusters )
    { return Error("Could not load input data ='"+inputData()+"'");}

  //StatusCode sc = StatusCode::SUCCESS ;


  /// loop over all clusters 
  for( Iterator icluster = clusters->begin() ;
       clusters->end() != icluster ; ++icluster )
    {
      CaloCluster* cluster = *icluster ;
      if( 0 == cluster )                { continue ; }

      double Energy=0;
      double PosX=0;
      double PosY=0;
      /// Find Cluster Seed 
      const CaloCluster::Digits::const_iterator iSeed = 
	clusterLocateDigit( cluster->entries().begin() ,
			    cluster->entries().end  () ,
			    CaloDigitStatus::SeedCell     );
      const CaloDigit* seed = iSeed->digit() ;
      if( 0 == seed) { continue ; }   
      const CaloCellID&  seedID = seed->cellID() ;
      // Loop over 3x3 digits
      const CaloCluster::Entries& entries = cluster->entries();
      for(CaloCluster::Entries::const_iterator ient = entries.begin();
	  ient != entries.end() ; ient++){
	const CaloDigit*  cell = ient->digit();
	const CaloCellID&  cellID = cell->cellID() ;
	if( 0 != m_cell3x3( seedID , cellID) && m_neighbour(seedID,cellID) ){
    
    // commented by I.B to suppress compiler warnings 2004-07-21
	  // const HepPoint3D& centerMtrx =             
    //       detector->cellCenter ( seedID ) ;
	  // const HepPoint3D& centerCell =             
    //       detector->cellCenter ( cellID ) ;
	  // const double      sizeMtrx   =  
    //       0.5  * 3 * detector->cellSize   ( seedID ) ;
	  // const double      sizeCell   =  
    //       0.5  *     detector->cellSize   ( cellID ) ;
	  
	  Energy += cell->e() * ient->fraction();
	  PosX += detector->cellX(cellID) * cell->e() * ient->fraction();
	  PosY += detector->cellY(cellID) * cell->e() * ient->fraction();
	}
      }
      PosX=PosX/Energy;
      PosY=PosY/Energy;
      CaloPosition pos = cluster->position();
      HepVector params(3);
      params(CaloPosition::X)=PosX;
      params(CaloPosition::Y)=PosY;
      params(CaloPosition::E)=Energy;
      pos.setParameters( params );
      cluster->setPosition( pos );
    }
  return StatusCode::SUCCESS;
};

// ============================================================================
// The End 
// ============================================================================
