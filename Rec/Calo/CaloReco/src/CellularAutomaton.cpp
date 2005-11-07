// $Id: CellularAutomaton.cpp,v 1.7 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/10/22 19:08:03  ibelyaev
//  small technical improvements in matching tool base
//
// Revision 1.5  2004/10/22 16:33:53  ibelyaev
//  update CellularAutomaton and remove obsolete algs
//
// Revision 1.4  2004/09/05 20:23:48  ibelyaev
//  trivial modification to obtain 20-25% CPU gain
//
// ============================================================================ 
#define CALOCA_CELLULARAUTOMATON_CPP 1 
// ============================================================================
// STL
// ============================================================================
#include <algorithm>
// ============================================================================
// CLHEP 
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
// ============================================================================
// Gaudi 
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/Chrono.h"
// ============================================================================
// DetDesc
// ============================================================================
#include "DetDesc/IGeometryInfo.h"
// ============================================================================
// CaloEvent
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
// ============================================================================
// Private              
// ============================================================================
#include "CellularAutomaton.h"
#include "TaggedCellFunctor.h"

// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

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
inline bool CellularAutomaton::isLocMax
( const CaloDigit*                      digit ,
  const CellularAutomaton::DirVector&   hits  ,
  const DeCalorimeter*                  det   ) 
{
  const CaloNeighbors& ns = det->neighborCells( digit -> cellID() ) ;
  double e = digit -> e() ;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN )
  {
    const CelAutoTaggedCell* cell = hits[*iN];
    if ( 0 == cell   ) { continue     ; }  
    const CaloDigit* nd = cell->digit() ;
    if ( 0 == nd     ) { continue     ; }
    if ( nd->e() > e ) { return false ; }
  }   
  return true ;
};
// ============================================================================

// ============================================================================
/** Application of rules of tagging on one cell
 *  - No action if no clustered neighbor 
 *   - Clustered if only one clustered neighbor
 *   - Edge if more then one clustered neighbor 
 */
// ============================================================================
inline void 
CellularAutomaton::appliRulesTagger
( CelAutoTaggedCell*             cell ,
  CellularAutomaton::DirVector&  hits ,
  const DeCalorimeter*           det  ) 
{
  
  // Find in the neighbors cells tagged before, the clustered neighbors cells
  const CaloCellID&    cellID = cell->cellID() ;
  const CaloNeighbors& ns     = det->neighborCells ( cellID ) ;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN ) 
  {
    const CelAutoTaggedCell* nei = hits[ *iN ] ;
    if ( 0 == nei                 ) { continue ; }
    if ( !nei->isClustered()      ) { continue ; }
    const CaloCellID& seed = nei->seedForClustered() ;
    if ( cell->isWithSeed( seed ) ) { continue ; }
    cell->addSeed ( seed ) ;
  }
  
  // Tag or or not the cell
  
  switch ( cell -> numberSeeds() ) {
  case 0:
    break;
  case 1: 
    cell->setClustered();
    break;
  default: 
    cell->setEdge();
    break;
  } 
};
// ============================================================================

// ============================================================================
inline void CellularAutomaton::setEXYCluster
( CaloCluster* cluster,
  const DeCalorimeter* detector ) 
{
  ///
  double E, X, Y;
  ///
  StatusCode sc = ClusterFunctors::calculateEXY
    ( cluster->entries().begin() ,
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
// ============================================================================


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
  , m_sort             ( true  )
  , m_sortByET         ( false )
{
  // set the appropriate defaults for detector data 
  setInputData  ( CaloDigitLocation::     Ecal ) ;
  // set the appropriate defaults for output data 
  setOutputData ( CaloClusterLocation::   Ecal ) ;
  // set the appropriate defaults for detector data
  setDetData    ( DeCalorimeterLocation:: Ecal ) ;
  // sort the clusters ? 
  declareProperty ( "Sort"     , m_sort     ) ;
  // using ET sort versus E sort 
  declareProperty ( "SortByET" , m_sortByET ) ;
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
  
  if ( sc.isFailure() ) { return sc ; }
  
  // Create access direct and sequential on the tagged cells  
  DirVector taggedCellsDirect( (CelAutoTaggedCell*) 0 ) ;
  SeqVector taggedCellsSeq                              ;
  typedef std::vector<CelAutoTaggedCell> _Local ;
  _Local local_cells ( hits->size()  , CelAutoTaggedCell() ) ;
  
  taggedCellsDirect.reserve ( hits->size() ) ;
  taggedCellsDirect.setSize ( 14000        ) ;
  taggedCellsSeq.reserve    ( hits->size() ) ;
  
  { // fill with the data
    size_t index = 0 ;
    for( CaloDigits::const_iterator ihit = 
           hits->begin() ; hits->end() != ihit ; ++ihit , ++index )
    {
      const CaloDigit* digit   = *ihit ;
      if ( 0 == digit ) { continue ; }  // CONTINUE !!! 
      CelAutoTaggedCell& taggedCell = *(local_cells.begin() + index ) ;
      taggedCell = digit ;
      taggedCellsDirect.addEntry ( &taggedCell , digit->cellID() ) ;
      taggedCellsSeq.push_back   ( &taggedCell                   ) ;
    }
  }
  
  // Find and mark the seeds (local maxima) 
  for( SeqVector::iterator itTag = taggedCellsSeq.begin(); taggedCellsSeq.end() != itTag ; ++itTag )
  { if ( isLocMax ( (*itTag)->digit() , 
                    taggedCellsDirect , 
                    detector          ) ) { (*itTag)->setIsSeed(); } }
  
  /// Tag the cells which are not seeds
  SeqVector::iterator itTagLastSeed = std::stable_partition 
    ( taggedCellsSeq.begin () ,
      taggedCellsSeq.end   () ,
      TaggedCellFunctor::isClustered() );
  
  SeqVector::iterator itTagLastClustered = itTagLastSeed      ;     
  SeqVector::iterator itTagFirst         = itTagLastClustered ; 
  
  while ( itTagLastClustered != taggedCellsSeq.end() ) {
    
    // Apply rules tagger for all not tagged cells
    for ( SeqVector::iterator itTag = itTagLastClustered ; 
          taggedCellsSeq.end() != itTag ; ++itTag ) 
    { appliRulesTagger( (*itTag),  taggedCellsDirect , detector ); }
    
    // Valid result
    std::for_each ( itTagFirst, taggedCellsSeq.end(), TaggedCellFunctor::setStatus() );
    
    itTagLastClustered = std::stable_partition
      ( itTagFirst,
        taggedCellsSeq.end(),
        TaggedCellFunctor::isClusteredOrEdge() );   
    
    // Test if cells are tagged in this pass    
    if ( itTagLastClustered == itTagFirst ) 
    {
      const long number = taggedCellsSeq.end() - itTagLastClustered ;
      Warning ( " TAGGING NOT FULL - Remain " 
                + boost::lexical_cast<std::string> ( number  ) 
                + " not clustered cells" , StatusCode::SUCCESS ) ;
      itTagLastClustered = taggedCellsSeq.end();
    }
    itTagFirst = itTagLastClustered;
  }
  
  /** Partionne taggedCells 
   *  (after seeds, another clustered, shared, another case )
   */
  itTagLastClustered = std::stable_partition 
    ( itTagLastSeed                    ,
      taggedCellsSeq.end()             ,
      TaggedCellFunctor::isClustered() ) ;  
  
  // Create cluster data and store in output
  
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
  
  while ( itTagSeed != itTagLastSeed ) 
  {
    CaloCluster* cluster = new  CaloCluster();
    const CaloDigit* digit = (*itTagSeed)->digit() ;
    cluster->entries(). push_back( CaloClusterEntry( digit , seed ) );
    // set seed for cluster! (I.B.)
    cluster->setSeed( digit->cellID() );
    
    itTagClustered2 = std::stable_partition 
      ( itTagClustered1                                        ,
        itTagLastClustered                                     ,
        TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID()) ) ;
    for (  ; itTagClustered1 != itTagClustered2 ; ++itTagClustered1 ) 
    {
      const CaloDigit* digit = (*itTagClustered1)->digit() ;
      cluster->entries().push_back( CaloClusterEntry( digit , owned ) );
    }
    /// corrections from N.B. 9/11/2001
    SeqVector::iterator itTagFirstEdge = itTagLastClustered ;
    SeqVector::iterator itTagLastEdge  = std::stable_partition 
      ( itTagLastClustered                                       ,
        taggedCellsSeq.end()                                     ,
        TaggedCellFunctor::isWithSeed ( (*itTagSeed)->cellID() ) ) ;
    for(  ; itTagFirstEdge != itTagLastEdge ; ++itTagFirstEdge  ) 
    {
      const CaloDigit* digit = (*itTagFirstEdge)->digit() ;
      cluster->entries().push_back( CaloClusterEntry( digit , shared ) );
    }
    
    setEXYCluster ( cluster, detector );
    
    cluster->setStatus( 1 ) ;
    cluster->position().setZ( zPosition( cluster )  );
    clustersSeq->insert( cluster );
    itTagClustered1 = itTagClustered2;
    itTagSeed++;
  }
  
  /** sort the sequence to simplify the comparison 
   *  with other clusterisation techniques 
   */
  if ( m_sort )
  { 

    if ( !m_sortByET ) 
    {
      // sorting criteria: Energy
      CaloDataFunctor::Less_by_Energy<const CaloCluster*> Cmp;
      // perform the sorting 
      std::stable_sort
        ( clustersSeq->begin()            ,
          clustersSeq->end  ()            ,
          CaloDataFunctor::inverse( Cmp ) ) ;
    }
    else 
    {
      // sorting criteria : Transverse Energy
      CaloDataFunctor::Less_by_TransverseEnergy<const CaloCluster*,
        const DeCalorimeter*> Cmp ( detector ) ;
      // perform the sorting 
      std::stable_sort
        ( clustersSeq->begin()            ,
          clustersSeq->end  ()            ,
          CaloDataFunctor::inverse( Cmp ) ) ;    
    }
    
  };
  
  // clear local storages 
  taggedCellsSeq    .clear () ;
  taggedCellsDirect .clear () ;
  local_cells       .clear () ;
  
  return StatusCode::SUCCESS; 
  }
  

// ==========================================================================
