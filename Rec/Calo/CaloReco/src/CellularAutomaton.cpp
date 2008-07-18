// $Id: CellularAutomaton.cpp,v 1.12 2008-07-18 00:33:10 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
#include "DetDesc/IGeometryInfo.h"
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "CaloKernel/CaloUtil.h"
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
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CellularAutomaton );

// ============================================================================
inline bool CellularAutomaton::isLocMax
( const LHCb::CaloDigit*                      digit ,
  const CellularAutomaton::DirVector&   hits  ,
  const DeCalorimeter*                  det   ) 
{
  const CaloNeighbors& ns = det->neighborCells( digit -> cellID() ) ;
  double e = digit -> e() ;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN )
  {
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
CellularAutomaton::appliRulesTagger
( CelAutoTaggedCell*             cell ,
  CellularAutomaton::DirVector&  hits ,
  const DeCalorimeter*           det  ) 
{
  
  // Find in the neighbors cells tagged before, the clustered neighbors cells
  const LHCb::CaloCellID&    cellID = cell->cellID() ;
  const CaloNeighbors& ns     = det->neighborCells ( cellID ) ;
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN ) 
  {
    const CelAutoTaggedCell* nei = hits[ *iN ] ;
    if ( 0 == nei                 ) { continue ; }
    if ( !nei->isClustered()      ) { continue ; }
    const LHCb::CaloCellID& seed = nei->seedForClustered() ;
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
( LHCb::CaloCluster* cluster,
  const DeCalorimeter* detector ) 
{
  ///
  double E, X, Y;
  ///
  StatusCode sc = LHCb::ClusterFunctors::calculateEXY
    ( cluster->entries().begin() ,
      cluster->entries().end  () ,
      detector , E , X , Y      );
  ///
  if( sc.isSuccess() )
  {
    cluster->position().parameters()( LHCb::CaloPosition::E ) = E ;
    cluster->position().parameters()( LHCb::CaloPosition::X ) = X ;
    cluster->position().parameters()( LHCb::CaloPosition::Y ) = Y ;
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
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class CaloAlgorithm",sc);}
  
  if (m_inputData.empty() || m_outputData.empty() || m_detData.empty() ) 
    {
      return Error("Path hits, path geom, path clusters must"
                   + std::string(" be defined in job options file!") );
    }

  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( m_detData );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }
  
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
  for( SeqVector::iterator itTag = taggedCellsSeq.begin(); taggedCellsSeq.end() != itTag ; ++itTag )
  { if ( isLocMax ( (*itTag)->digit() , 
                    taggedCellsDirect , 
                    m_detector          ) ) { (*itTag)->setIsSeed(); } }
  
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
    { appliRulesTagger( (*itTag),  taggedCellsDirect , m_detector ); }
    
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
  
  LHCb::CaloClusters* clustersSeq = output;  // V.B.!!
  SeqVector::iterator itTagSeed = taggedCellsSeq.begin();
  SeqVector::iterator itTagClustered1;
  SeqVector::iterator itTagClustered2;
  itTagClustered1 = itTagLastSeed;
  
  const LHCb::CaloDigitStatus::Status seed   = 
    LHCb::CaloDigitStatus::SeedCell          | LHCb::CaloDigitStatus::LocalMaximum      | 
    LHCb::CaloDigitStatus::UseForEnergy      | LHCb::CaloDigitStatus::UseForPosition    | 
    LHCb::CaloDigitStatus::UseForCovariance  ;

  const LHCb::CaloDigitStatus::Status owned  = 
    LHCb::CaloDigitStatus::OwnedCell         | LHCb::CaloDigitStatus::UseForEnergy      | 
    LHCb::CaloDigitStatus::UseForPosition    | LHCb::CaloDigitStatus::UseForCovariance  ;
  
  const LHCb::CaloDigitStatus::Status shared = 
    LHCb::CaloDigitStatus::SharedCell        | LHCb::CaloDigitStatus::UseForEnergy      | 
    LHCb::CaloDigitStatus::UseForPosition    | LHCb::CaloDigitStatus::UseForCovariance  ;
  
  while ( itTagSeed != itTagLastSeed ) 
  {
    LHCb::CaloCluster* cluster = new  LHCb::CaloCluster();
    const LHCb::CaloDigit* digit = (*itTagSeed)->digit() ;
    cluster->entries().push_back( LHCb::CaloClusterEntry( digit , seed ) );
    // set seed for cluster! (I.B.)
    cluster->setSeed( digit->cellID() );
    
    itTagClustered2 = std::stable_partition 
      ( itTagClustered1                                        ,
        itTagLastClustered                                     ,
        TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID()) ) ;
    for (  ; itTagClustered1 != itTagClustered2 ; ++itTagClustered1 ) 
    {
      const LHCb::CaloDigit* digit = (*itTagClustered1)->digit() ;
      cluster->entries().push_back( LHCb::CaloClusterEntry( digit , owned ) );
    }
    /// corrections from N.B. 9/11/2001
    SeqVector::iterator itTagFirstEdge = itTagLastClustered ;
    SeqVector::iterator itTagLastEdge  = std::stable_partition 
      ( itTagLastClustered                                       ,
        taggedCellsSeq.end()                                     ,
        TaggedCellFunctor::isWithSeed ( (*itTagSeed)->cellID() ) ) ;
    for(  ; itTagFirstEdge != itTagLastEdge ; ++itTagFirstEdge  ) 
    {
      const LHCb::CaloDigit* digit = (*itTagFirstEdge)->digit() ;
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
  if ( m_sort )
  { 

    if ( !m_sortByET ) 
    {
      // sorting criteria: Energy
      LHCb::CaloDataFunctor::Less_by_Energy<const LHCb::CaloCluster*> Cmp;
      // perform the sorting 
      std::stable_sort
        ( clustersSeq->begin()            ,
          clustersSeq->end  ()            ,
          LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
    }
    else 
    {
      // sorting criteria : Transverse Energy
      LHCb::CaloDataFunctor::Less_by_TransverseEnergy<const LHCb::CaloCluster*,
        const DeCalorimeter*> Cmp ( m_detector ) ;
      // perform the sorting 
      std::stable_sort
        ( clustersSeq->begin()            ,
          clustersSeq->end  ()            ,
          LHCb::CaloDataFunctor::inverse( Cmp ) ) ;    
    }
    
  };
  
  // clear local storages 
  taggedCellsSeq    .clear () ;
  taggedCellsDirect .clear () ;
  local_cells       .clear () ;
  
  return StatusCode::SUCCESS; 
  }
  

// ==========================================================================
StatusCode CellularAutomaton::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
