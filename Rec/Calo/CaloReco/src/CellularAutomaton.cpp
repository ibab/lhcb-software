// $Id: CellularAutomaton.cpp,v 1.13 2008-09-22 01:41:23 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
#include "GaudiKernel/AlgFactory.h" 
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "CaloUtils/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "CaloKernel/CaloUtil.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CellularAutomaton.h"
#include "TaggedCellFunctor.h"
#include "boost/lexical_cast.hpp"

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
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CellularAutomaton );

// ============================================================================
inline bool CellularAutomaton::isLocMax
( const LHCb::CaloDigit*                      digit ,
  const CellularAutomaton::DirVector&   hits  ,
  const DeCalorimeter*                  det   ){
  const CaloNeighbors& ns = det->neighborCells( digit -> cellID() ) ;
  double e = digit -> e() ;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN ){
    const CelAutoTaggedCell* cell = hits[*iN];
    if ( 0 == cell   ) { continue     ; }  
    const LHCb::CaloDigit* nd = cell->digit() ;
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
CellularAutomaton::appliRulesTagger( CelAutoTaggedCell*             cell ,
                                     CellularAutomaton::DirVector&  hits ,
                                     const DeCalorimeter*           det  ){
  
  // Find in the neighbors cells tagged before, the clustered neighbors cells
  const LHCb::CaloCellID&    cellID = cell->cellID() ;
  const CaloNeighbors& ns     = det->neighborCells ( cellID ) ;
  bool hasEdgeNeighbor = false;
  bool hasClusteredNeighbor = false;
  std::vector<LHCb::CaloCellID> neighborSeeds;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN ){
    const CelAutoTaggedCell* nei = hits[ *iN ] ;
    if ( 0 == nei                 ) { continue ; }
    // 
    if( nei->isEdge() && m_release ){
      hasEdgeNeighbor = true;
      for( std::vector<LHCb::CaloCellID>::const_iterator id = nei->seeds().begin();id != nei->seeds().end();id++){
        bool ok = true;
        for(std::vector<LHCb::CaloCellID>::iterator iid = neighborSeeds.begin();iid != neighborSeeds.end();iid++){
          if( *id == *iid ){
            ok = false;
            break;
          }
        }
        if( ok )cell->addSeed( *id );
      }
    }
    if ( !nei->isClustered()      ) { continue ; }
    hasClusteredNeighbor = true;
    const LHCb::CaloCellID& seed = nei->seedForClustered() ;
    if ( cell->isWithSeed( seed ) ) { continue ; }
    cell->addSeed ( seed ) ;
  }
  
  // Tag or or not the cell
  
  switch ( cell -> numberSeeds() ) {
  case 0:
    if( !m_release)break;
    if( hasEdgeNeighbor && !hasClusteredNeighbor){
      cell->setEdge();
      for(std::vector<LHCb::CaloCellID>::iterator iid = neighborSeeds.begin();iid != neighborSeeds.end();iid++){
        cell->addSeed( *iid );
      }      
    }
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
inline void CellularAutomaton::setEXYCluster( LHCb::CaloCluster* cluster,
                                              const DeCalorimeter* detector ){
  ///
  double E, X, Y;
  ///
  StatusCode sc = LHCb::ClusterFunctors::calculateEXY( cluster->entries().begin() ,
                                                       cluster->entries().end  () ,
                                                       detector , E , X , Y      );
  ///
  if( sc.isSuccess() ){
    cluster->position().parameters()( LHCb::CaloPosition::E ) = E ;
    cluster->position().parameters()( LHCb::CaloPosition::X ) = X ;
    cluster->position().parameters()( LHCb::CaloPosition::Y ) = Y ;
  }else{
    Error( " E,X and Y of cluster could not be evaluated!",sc); 
  }
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
  : GaudiAlgorithm      ( name, pSvcLocator )
  , m_sort             ( true  )
  , m_sortByET         ( false )
{
  declareProperty("InputData" , m_inputData  = LHCb::CaloDigitLocation::Ecal);
  declareProperty("OutputData", m_outputData = LHCb::CaloClusterLocation::Ecal);  
  declareProperty("Detector"  , m_detData    = DeCalorimeterLocation::Ecal) ;
  // sort the clusters ? 
  declareProperty ( "Sort"     , m_sort     ) ;
  declareProperty ( "SortByET" , m_sortByET ) ;
  declareProperty ( "CellSelectorForEnergy" , m_used = "3x3");
    
  // set default data as a function of detector
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  std::string det = name.substr( index, 4 );
  if(det == "Ecal"){
    m_inputData=   LHCb::CaloDigitLocation::Ecal;
    m_outputData = ("HLT"==context() || "Hlt" == context()) ? 
      LHCb::CaloClusterLocation::EcalHlt : LHCb::CaloClusterLocation::Ecal;
    m_detData= DeCalorimeterLocation::Ecal;
  }
  else if( det == "Hcal"){
    m_inputData=   LHCb::CaloDigitLocation::Hcal;
    m_outputData = ("HLT"==context() || "Hlt" == context()) ? 
      LHCb::CaloClusterLocation::HcalHlt : LHCb::CaloClusterLocation::Hcal;
    m_detData= DeCalorimeterLocation::Hcal;
  }


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
  
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ){ return Error("Could not initialize base class CaloAlgorithm",sc);}
  
  if (m_inputData.empty() || m_outputData.empty() || m_detData.empty() ){
    return Error("Path hits, path geom, path clusters must"
                 + std::string(" be defined in job options file!") );
  }
  ///
  m_pass = 0.;
  m_clus = 0.;
  m_event= 0.;
  m_passMin= 999999;
  m_passMax = 0;
  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( m_detData );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }
  ///
  m_cellSelector.setDet(m_detector);
  m_cellSelector.setSelector(m_used);
  
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
  // 
  m_release = false;
  // z-position of cluster 
  LHCb::ClusterFunctors::ZPosition zPosition( m_detector );
  
  // get input data (sequential and simultaneously direct access!)  
  LHCb::CaloDigits* hits = get<LHCb::CaloDigits>( m_inputData );

  /** Create the container of clusters and  
   *  register it into the event data store
   */ 
  LHCb::CaloClusters* output = new LHCb::CaloClusters();
  // update the version number (needed for serialization)
  output -> setVersion( 2 ) ;
  put( output , m_outputData );
  
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
    for( LHCb::CaloDigits::const_iterator ihit = 
           hits->begin() ; hits->end() != ihit ; ++ihit , ++index )
    {
      const LHCb::CaloDigit* digit   = *ihit ;
      if ( 0 == digit ) { continue ; }  // CONTINUE !!! 
      CelAutoTaggedCell& taggedCell = *(local_cells.begin() + index ) ;
      taggedCell = digit ;
      taggedCellsDirect.addEntry ( &taggedCell , digit->cellID() ) ;
      taggedCellsSeq.push_back   ( &taggedCell                   ) ;
    }
  }
  
  // Find and mark the seeds (local maxima) 
  for( SeqVector::iterator itTag = taggedCellsSeq.begin(); taggedCellsSeq.end() != itTag ; ++itTag ){
    if ( isLocMax ( (*itTag)->digit() , 
                    taggedCellsDirect , 
                    m_detector          ) ) { 
      (*itTag)->setIsSeed(); } 
  }
  
  /// Tag the cells which are not seeds
  SeqVector::iterator itTagLastSeed = std::stable_partition( taggedCellsSeq.begin () ,
                                                             taggedCellsSeq.end   () ,
                                                             TaggedCellFunctor::isClustered() );
  
  SeqVector::iterator itTagLastClustered = itTagLastSeed      ;     
  SeqVector::iterator itTagFirst         = itTagLastClustered ; 

  unsigned long pass = 0;
  while ( itTagLastClustered != taggedCellsSeq.end() ) {
    
    // Apply rules tagger for all not tagged cells
    for ( SeqVector::iterator itTag = itTagLastClustered ;  taggedCellsSeq.end() != itTag ; ++itTag ){
      appliRulesTagger( (*itTag),  taggedCellsDirect , m_detector ); 
    }
    
    // Valid result
    std::for_each ( itTagFirst, taggedCellsSeq.end(), TaggedCellFunctor::setStatus() );
    
    itTagLastClustered = std::stable_partition( itTagFirst,
                                                taggedCellsSeq.end(),
                                                TaggedCellFunctor::isClusteredOrEdge() );   
    
    // Test if cells are tagged in this pass    
    if ( itTagLastClustered == itTagFirst && m_release ){
      itTagLastClustered = taggedCellsSeq.end(); // stop iteration and report the problem
      const long number = taggedCellsSeq.end() - itTagLastClustered ;
      Warning ( " TAGGING NOT FULL - Remain " 
                + boost::lexical_cast<std::string> ( number  ) 
                + " not clustered cells" , StatusCode::SUCCESS ) ;
    }
    if( itTagLastClustered == itTagFirst )m_release = true; // try additional passes releasing appliRulesTagger criteria


    itTagFirst = itTagLastClustered;
    pass++;
  }


  
  // Create cluster data and store in output
  LHCb::CaloClusters* clustersSeq = output;
  const LHCb::CaloDigitStatus::Status used   = LHCb::CaloDigitStatus::UseForEnergy  | LHCb::CaloDigitStatus::UseForPosition |
    LHCb::CaloDigitStatus::UseForCovariance  ;
  const LHCb::CaloDigitStatus::Status seed   = LHCb::CaloDigitStatus::SeedCell | LHCb::CaloDigitStatus::LocalMaximum | used ;
  
  
  itTagLastClustered = std::stable_partition( itTagLastSeed                    ,
                                              taggedCellsSeq.end()             ,
                                              TaggedCellFunctor::isClustered() ) ;  
  SeqVector::iterator itTagSeed = taggedCellsSeq.begin();
  SeqVector::iterator itTagClustered1 = itTagLastSeed;
  SeqVector::iterator itTagClustered2;
  while ( itTagSeed != itTagLastSeed ){
    LHCb::CaloCellID seedID = (*itTagSeed)->cellID();
    LHCb::CaloCluster* cluster = new  LHCb::CaloCluster();
    const LHCb::CaloDigit* digit = (*itTagSeed)->digit() ;
    cluster->entries().push_back( LHCb::CaloClusterEntry( digit , seed ) );
    cluster->setSeed( digit->cellID() );    
    // Owned cells
    itTagClustered2 = std::stable_partition( itTagClustered1                                        ,
                                             itTagLastClustered                                     ,
                                             TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID()) ) ;
    for (  ; itTagClustered1 != itTagClustered2 ; ++itTagClustered1 ){
      LHCb::CaloCellID  cellID = (*itTagClustered1)->cellID();
      const LHCb::CaloDigit* digit = (*itTagClustered1)->digit() ;      
      LHCb::CaloDigitStatus::Status owned  = LHCb::CaloDigitStatus::OwnedCell ;
      if( m_cellSelector( seedID, cellID) > 0.)owned |= used;
      cluster->entries().push_back( LHCb::CaloClusterEntry( digit , owned ) );
    }
    // Shared cells
    SeqVector::iterator itTagFirstEdge = itTagLastClustered ;
    SeqVector::iterator itTagLastEdge  = std::stable_partition( itTagLastClustered                                       ,
                                                                taggedCellsSeq.end()                                     ,
                                                                TaggedCellFunctor::isWithSeed ( (*itTagSeed)->cellID() ) ) ;
    for(  ; itTagFirstEdge != itTagLastEdge ; ++itTagFirstEdge  ){
      const LHCb::CaloDigit* digit = (*itTagFirstEdge)->digit() ;
      LHCb::CaloCellID  cellID = (*itTagFirstEdge)->cellID();
      LHCb::CaloDigitStatus::Status shared = LHCb::CaloDigitStatus::SharedCell;
      if( m_cellSelector( seedID, cellID) > 0.)shared |= used;     
      cluster->entries().push_back( LHCb::CaloClusterEntry( digit , shared ) );
    }
    
    setEXYCluster ( cluster, m_detector );
    
    cluster->setType( LHCb::CaloCluster::CellularAutomaton ) ;
    cluster->position().setZ( zPosition( cluster )  );
    clustersSeq->insert( cluster );
    itTagClustered1 = itTagClustered2;
    itTagSeed++;
  }
  
  /** sort the sequence to simplify the comparison 
   *  with other clusterisation techniques 
   */
  if ( m_sort )  {
    if ( !m_sortByET ){
      // sorting criteria: Energy
      LHCb::CaloDataFunctor::Less_by_Energy<const LHCb::CaloCluster*> Cmp;
      // perform the sorting 
      std::stable_sort( clustersSeq->begin()            ,
                        clustersSeq->end  ()            ,
                        LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
    } else{
      // sorting criteria : Transverse Energy
      LHCb::CaloDataFunctor::Less_by_TransverseEnergy<const LHCb::CaloCluster*, const DeCalorimeter*> Cmp ( m_detector ) ;
      // perform the sorting 
      std::stable_sort  ( clustersSeq->begin()            ,
                          clustersSeq->end  ()            ,
                          LHCb::CaloDataFunctor::inverse( Cmp ) ) ;    
    }
  };
  
  // clear local storages 
  taggedCellsSeq    .clear () ;
  taggedCellsDirect .clear () ;
  local_cells       .clear () ;

  

  if ( msgLevel( MSG::DEBUG) ){
    debug() << "Built " << clustersSeq->size() <<" cellular automaton clusters  with " << pass << " iterations" <<endreq;
    debug() << " ----------------------- Cluster List : " << endreq;
    for(LHCb::CaloClusters::iterator ic = clustersSeq->begin();ic!=clustersSeq->end();ic++){
      debug() << " Cluster seed " << (*ic)->seed() 
              << " energy " << (*ic)->e() 
              << " #entries " << (*ic)->entries().size() 
              << endreq;
    }    
  }
  m_event += 1.;
  m_clus += (double) clustersSeq->size();
  m_pass  += (double) pass;
  if(pass < m_passMin)m_passMin = pass;
  if(pass > m_passMax)m_passMax = pass;
  

  return StatusCode::SUCCESS; 
  }
  

// ==========================================================================
StatusCode CellularAutomaton::finalize() {

  debug() << "==> Finalize" << endmsg;

  double avePass = 0.;
  double aveClus = 0.;
  if(m_event>0){
    avePass = m_pass/m_event;
    aveClus = m_clus/m_event;
  }
  info() << "Built " << aveClus <<" cellular automaton clusters/event  with" 
         << avePass << " iterations (min,max)=(" << m_passMin << "," << m_passMax << ") on average " << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

