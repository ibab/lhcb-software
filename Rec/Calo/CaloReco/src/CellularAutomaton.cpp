// $Id: CellularAutomaton.cpp,v 1.3 2004-02-17 12:08:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/01/13 08:47:25  ibelyaev
//  update 'seed' and 'version' for Cluster/Clusters
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// ============================================================================ 
#define CALOCA_CELLULARAUTOMATON_CPP 1 
// ============================================================================
// STL
#include <algorithm>
// CLHEP 
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/Stat.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
// CaloEvent
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
// CaloKernel
#include "CaloKernel/CaloUtil.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
// Private              
#include "CellularAutomaton.h"
#include "TaggedCellFunctor.h"

// ============================================================================
/** @file CellularAutomaton.cpp
 *  
 *  Implementation of class CellularAutomaton
 *  
 *  CellularAutomaton : Make clusters
 *   - Input  : CaloDigit data and geometry of detector
 *   - Output : CaloCluster data
 * 
 *  @author  Nicole Brun
 *  @date    27 Feb 2001
 */ 
// ============================================================================

static const AlgFactory<CellularAutomaton>    Factory;
const IAlgFactory& CellularAutomatonFactory = Factory;

// ============================================================================
/** standard constructor
 *
 *  Algorithm parameters which can be set at run time must be declared here.
 *  @see Algorithm 
 *  @param name   name of the algorithm
 *  @param svcloc pointer to service locator 
 */
// ============================================================================
CellularAutomaton::CellularAutomaton
( const std::string& name,
  ISvcLocator* pSvcLocator )
  : CaloAlgorithm      ( name, pSvcLocator )
{
  // set the appropriate defaults for detector data 
  setInputData  ( CaloDigitLocation::     Ecal ) ;
  // set the appropriate defaults for output data 
  setOutputData ( CaloClusterLocation::   Ecal ) ;
  // set the appropriate defaults for detector data
  setDetData    ( DeCalorimeterLocation:: Ecal ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CellularAutomaton::~CellularAutomaton(){};
// ============================================================================

// ============================================================================
/** standard  initialization  method 
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CellularAutomaton::initialize() 
{
  
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class CaloAlgorithm",sc);}
  
  if ( inputData().empty() || outputData().empty() || detData().empty() ) 
    {
      return Error("Path hits, path geom, path clusters must"
                   + std::string(" be defined in job options file!") );
    }

  /// Retrieve geometry of detector
  const DeCalorimeter* detector = getDet<DeCalorimeter>( detData() );
  if( 0 == detector ) { return StatusCode::FAILURE; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard  execution  method 
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CellularAutomaton::execute() 
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Execution" << endreq;
  
  // get the detector 
  const DeCalorimeter* detector = getDet<DeCalorimeter>( detData() );
  if( 0 == detector ) { return StatusCode::FAILURE; }
  
  // z-position of cluster 
  ClusterFunctors::ZPosition zPosition( detector );
  
  // get input data (sequential and simultaneously direct access!)  
  CaloDigits* hits = get<CaloDigits>( inputData() );
  if( 0 == hits ) { return StatusCode::FAILURE ; }
  
  /** Create the container of clusters and  
   *  register it into the event data store
   */ 
  CaloClusters* output = new CaloClusters();
  // update the version number (needed for seriazalition)
  output -> setVersion( 2 ) ;

  StatusCode sc = put( output , outputData() );
  if( sc.isFailure() ) { return sc ; }
  
  // Create access direct and sequential on the tagged cells  
  DirVector taggedCellsDirect( (CelAutoTaggedCell*) 0 ) ;
  SeqVector taggedCellsSeq                              ;
  
  for( CaloDigits::const_iterator ihit = 
         hits->begin() ; hits->end() != ihit ; ++ihit )
    {
      if( 0 == *ihit ) { continue ; }                    // CONTINUE !!! 
      const CaloCellID& cellID      = (*ihit)->cellID() ;
      CelAutoTaggedCell* taggedCell = new CelAutoTaggedCell( cellID );
      taggedCellsDirect.addEntry              ( taggedCell , cellID );
      taggedCellsSeq.push_back( taggedCell );
    }
  
  // Find and mark the seeds (local maxima) 
  SeqVector::iterator  itTag;
  for( itTag = taggedCellsSeq.begin(); 
       taggedCellsSeq.end() != itTag ; ++itTag )
    {
      if ( isLocMax( *itTag, *hits , detector ) ) 
        { 
          (*itTag)->setIsSeed(); 
        }
    }
  
  /// Tag the cells which are not seeds
  SeqVector::iterator itTagLastSeed =
    std::partition( taggedCellsSeq.begin(),
                    taggedCellsSeq.end(),
                    TaggedCellFunctor::isClustered() );

  SeqVector::iterator itTagLastClustered = itTagLastSeed      ;     
  SeqVector::iterator itTagFirst         = itTagLastClustered ; 

  while ( itTagLastClustered != taggedCellsSeq.end() ) {
    
    // Apply rules tagger for all not tagged cells
    SeqVector::iterator itTag = itTagLastClustered;
    while ( itTag != taggedCellsSeq.end() ) {
      appliRulesTagger( (*itTag),  taggedCellsDirect, detector ); 
      itTag++;
    }
    
    // Valid result
    itTag = itTagFirst;   
    std::for_each ( itTag, taggedCellsSeq.end(), 
                    TaggedCellFunctor::setStatus() );
    
    itTagLastClustered = 
      std::partition( itTagFirst,
                      taggedCellsSeq.end(),
                      TaggedCellFunctor::isClusteredOrEdge() );   

    // Test if cells are tagged in this pass    
    if ( itTagLastClustered == itTagFirst ) {
      log << MSG::INFO 
          << "TAGGING NOT FULL - Remain " 
          << taggedCellsSeq.end() - itTagLastClustered
          << " not clustered cells"
          << endreq; 
      itTagLastClustered = taggedCellsSeq.end();
    }
    itTagFirst = itTagLastClustered;
  }
  log << MSG::DEBUG << "Tagging ended" << endreq;
  
  /** Partionne taggedCells 
   *  (after seeds, another clustered, shared, another case )
   */
  itTagLastClustered = 
    std::partition( itTagLastSeed,
                    taggedCellsSeq.end(),
                    TaggedCellFunctor::isClustered() );  
  
  // Create cluster data and store in output
  log << MSG::DEBUG << "Create clusters from tagged cells" << endreq; 
  
  CaloClusters* clustersSeq = output;  // V.B.!!
  SeqVector::iterator itTagSeed = taggedCellsSeq.begin();
  SeqVector::iterator itTagClustered1;
  SeqVector::iterator itTagClustered2;
  itTagClustered1 = itTagLastSeed;
  
  const CaloDigitStatus::Status seed   = 
    CaloDigitStatus::SeedCell          | CaloDigitStatus::LocalMaximum      | 
    CaloDigitStatus::UseForEnergy      | CaloDigitStatus::UseForPosition    | 
    CaloDigitStatus::UseForCovariance  ;

  const CaloDigitStatus::Status owned  = 
    CaloDigitStatus::OwnedCell         | CaloDigitStatus::UseForEnergy      | 
    CaloDigitStatus::UseForPosition    | CaloDigitStatus::UseForCovariance  ;
  
  const CaloDigitStatus::Status shared = 
    CaloDigitStatus::SharedCell        | CaloDigitStatus::UseForEnergy      | 
    CaloDigitStatus::UseForPosition    | CaloDigitStatus::UseForCovariance  ;
  
  while ( itTagSeed != itTagLastSeed ) {
    CaloCluster* cluster = new  CaloCluster();
    cluster->entries().
      push_back( CaloClusterEntry( (*hits) ( (*itTagSeed)->cellID() ) , 
                                   seed ) );
    // set seed for cluster! (I.B.)
    cluster->setSeed( (*itTagSeed)->cellID() );
    
    itTagClustered2 = 
      std::partition ( itTagClustered1,
                       itTagLastClustered,
                       TaggedCellFunctor::isWithSeed((*itTagSeed)->cellID()));
    while ( itTagClustered1 != itTagClustered2 ) {
      cluster->entries().
        push_back( CaloClusterEntry( (*hits) ( (*itTagClustered1)->cellID() ), 
                                     owned ) );
      ++itTagClustered1;
    }
    /// corrections from N.B. 9/11/2001
    SeqVector::iterator itTagFirstEdge ;
    SeqVector::iterator itTagLastEdge  ;
    itTagFirstEdge = itTagLastClustered ;
    itTagLastEdge  = 
      std::partition ( itTagLastClustered,
                       taggedCellsSeq.end(),
                       TaggedCellFunctor::isWithSeed((*itTagSeed)->cellID()));
    while ( itTagFirstEdge != itTagLastEdge ) {
      cluster->entries().
        push_back( CaloClusterEntry( (*hits) ( (*itTagFirstEdge)->cellID()),
                                     shared ) );
      ++itTagFirstEdge;
    }      
    
    
    setEXYCluster( cluster, detector );
    
    cluster->setStatus( 1 ) ;
    cluster->position().setZ( zPosition( cluster )  );
    clustersSeq->insert( cluster );
    itTagClustered1 = itTagClustered2;
    itTagSeed++;
  }
  
  /** sort the sequence to simplify the comparison 
   *  with other clusterisation techniques 
   */
  { 
    CaloDataFunctor::Less_by_Energy<const CaloCluster*> Cmp;
    std::stable_sort( clustersSeq->begin() ,
                      clustersSeq->end  () ,
                      std::not2( Cmp )     );
  }
  
  { /// delete cells
    std::for_each(taggedCellsSeq.begin() , 
                  taggedCellsSeq.end  () , 
                  CaloUtil::Eraser<CelAutoTaggedCell>() ) ;
    taggedCellsSeq    .clear();
    taggedCellsDirect .clear();
  }
  
  log << MSG::DEBUG
      << "Execution clusterization manager completed successfully" 
      << endreq;
  
  return StatusCode::SUCCESS; 
}


// IsLocMax

bool CellularAutomaton::isLocMax
( CelAutoTaggedCell*      taggedCell,
  CaloDigits&             hitsDirect,
  const DeCalorimeter* detector ) 
{
  const CaloNeighbors& neighborsCellID = 
    detector->neighborCells( taggedCell->cellID() );
  CaloNeighbors::const_iterator 
    itNeighborsCellID = neighborsCellID.begin();
  while ( itNeighborsCellID != neighborsCellID.end() ) {
    if ( ( 0 != hitsDirect( (*itNeighborsCellID) ) ) &&
         ( hitsDirect( (*itNeighborsCellID) )->e() > 
           hitsDirect( taggedCell->cellID() )->e() ) ) {
      break;
    }
    ++itNeighborsCellID ;
  }
  // If true, this cell is one seed
  return( itNeighborsCellID == neighborsCellID.end() ? true : false );
};

// Application of rules of tagging on one cell
//   - No action if no clustered neighbor 
//   - Clustered if only one clustered neighbor
//   - Edge if more then one clustered neighbor 

void 
CellularAutomaton::appliRulesTagger
( CelAutoTaggedCell*&            taggedCell        ,
  CellularAutomaton::DirVector&  taggedCellsDirect ,
  const DeCalorimeter*           detector          ) {
  
  // Find in the neighbors cells tagged before, the clustered neighbors cells 
  
  std::vector<CaloCellID> neighborsCells = 
    detector->neighborCells( taggedCell->cellID() );
  std::vector<CaloCellID>::iterator itNeighbors = neighborsCells.begin();
  while ( itNeighbors != neighborsCells.end() ) {
    if ( ( 0 != taggedCellsDirect[ (*itNeighbors) ] ) && 
         ( taggedCellsDirect[ (*itNeighbors) ]->isClustered() ) &&  
         ( ! taggedCell->
           isWithSeed( taggedCellsDirect[ (*itNeighbors) ]->
                       seedForClustered() ) ) ) {
      taggedCell->addSeed( taggedCellsDirect[ (*itNeighbors) ]->
                           seedForClustered() );    }
    itNeighbors++;
  }
  
  // Tag or or not the cell
  
  switch ( taggedCell->numberSeeds() ) {
  case 0: 
    break;
  case 1: 
    taggedCell->setClustered();
    break;
  default: 
    taggedCell->setEdge();
    break;
  } 
}


void CellularAutomaton::setEXYCluster
( CaloCluster* cluster,
  const DeCalorimeter* detector ) 
{
  ///
  double E, X, Y;
  ///
  StatusCode sc = 
    ClusterFunctors::calculateEXY( cluster->entries().begin() ,
                                   cluster->entries().end  () ,
                                   detector , E , X , Y      );
  ///
  if( sc.isSuccess() )
    {
      cluster->position().parameters()( CaloPosition::E ) = E ;
      cluster->position().parameters()( CaloPosition::X ) = X ;
      cluster->position().parameters()( CaloPosition::Y ) = Y ;
    }
  else 
    { Error( " E,X and Y of cluster could not be evaluated!",sc); }
  ///
};


