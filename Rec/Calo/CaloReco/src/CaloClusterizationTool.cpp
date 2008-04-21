// $Id: CaloClusterizationTool.cpp,v 1.1 2008-04-21 13:52:17 vegorych Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
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
// local
#include "CaloClusterizationTool.h"
#include "TaggedCellFunctor.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloClusterizationTool
//
// 2008-04-04 : Victor Egorychev
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloClusterizationTool );

// ============================================================================
inline bool CaloClusterizationTool::isLocMax
( const LHCb::CaloDigit*                      digit ,
  const CaloClusterizationTool::DirVector&   hits  ,
  const DeCalorimeter*                  det   ) 
{

  const CaloNeighbors& ns = det->neighborCells( digit -> cellID() ) ;
  double e = digit -> e() ;
  
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloDigit*,const DeCalorimeter*> et ( det ) ;

  double eT = 0 ;

  if ( m_withET ) { eT = et ( digit ) ; }
  
  
  for ( CaloNeighbors::const_iterator iN = ns.begin() ; ns.end() != iN ; ++iN )
  {
    const CelAutoTaggedCell* cell = hits[*iN];
    if ( 0 == cell   ) { continue     ; }  
    const LHCb::CaloDigit* nd = cell->digit() ;
    if ( 0 == nd     ) { continue     ; }
    
    if ( m_withET ) { eT += et ( nd ) ; }
    
    if ( nd->e() > e ) { return false ; }
  }

  if ( m_withET && eT < m_ETcut ) { return false ; }
  
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
CaloClusterizationTool::appliRulesTagger
( CelAutoTaggedCell*             cell ,
  CaloClusterizationTool::DirVector&  hits ,
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
inline void CaloClusterizationTool::setEXYCluster
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
// Looking neigbours around cell
// ============================================================================
void CaloClusterizationTool::look_neig
( std::set<LHCb::CaloCellID> in_cells ,
  const DeCalorimeter* m_detector,
  std::set<LHCb::CaloCellID>& out_cells )
{

  for(std::set<LHCb::CaloCellID>::const_iterator icell = in_cells.begin();
      in_cells.end() != icell; ++icell){
    const CaloNeighbors& nei = m_detector->neighborCells( (*icell) );
    out_cells.insert( nei.begin(), nei.end() );
  }
  
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloClusterizationTool::CaloClusterizationTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )  
  , m_withET           ( false )
  , m_ETcut            ( -10. * Gaudi::Units::GeV)
{
  // eTcut
  declareProperty ( "withET" , m_withET ) ;
  declareProperty ( "ETcut" , m_ETcut ) ;

  declareInterface<ICaloClusterization>(this);
};

// ============================================================================
/// initialization
// ============================================================================
StatusCode CaloClusterizationTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class CaloAlgorithm",sc);}
  
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterizationTool::clusterize(std::vector<LHCb::CaloCluster*>&     clusters   ,
                                        const LHCb::CaloDigits* hits, const DeCalorimeter* m_detector, 
                                        const std::vector<LHCb::CaloCellID>&  cell_list, 
                                        const unsigned int m_neig_level)
{

  std::set<LHCb::CaloCellID> out_cells;

  // if list of "seed" is not empty
  if( !cell_list.empty() ){

    // look Cell neigbours
    std::set<LHCb::CaloCellID> cells;

    for( std::vector<LHCb::CaloCellID>::const_iterator icell = 
           cell_list.begin() ; cell_list.end() != icell ; ++icell)
    {  
      const CaloNeighbors& nei = m_detector->neighborCells( (*icell) );
      cells.insert( nei.begin(), nei.end() );
      cells.insert( (*icell) );
    }

    // look Cell neigbours for next levels
    std::set<LHCb::CaloCellID> in_cells = cells;

    for(int i = m_neig_level; i > 1; i--){      
      look_neig(in_cells, m_detector, out_cells);
      in_cells = out_cells;
    }
    
  }
  
  // z-position of cluster 
  LHCb::ClusterFunctors::ZPosition zPosition( m_detector );
  
  // Create access direct and sequential on the tagged cells  
  DirVector taggedCellsDirect( (CelAutoTaggedCell*) 0 ) ;
  SeqVector taggedCellsSeq                              ;
  typedef std::vector<CelAutoTaggedCell> _Local ;

  int local_size = 0;
  if( cell_list.empty() ){
    local_size = hits->size();
  }
  else{
    local_size = out_cells.size();
  }

  _Local local_cells ( local_size  , CelAutoTaggedCell() ) ;  
  
  if( cell_list.empty() ){
    taggedCellsDirect.reserve ( hits->size() ) ;
    taggedCellsDirect.setSize ( 14000        ) ;
    taggedCellsSeq.reserve    ( hits->size() ) ;
  }
  else{
    taggedCellsDirect.reserve ( out_cells.size() ) ;
    taggedCellsDirect.setSize ( 14000            ) ;
    taggedCellsSeq.reserve    ( out_cells.size() ) ;
  }

  if( cell_list.empty() )
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
    
  }else{//fill for HLT    
    size_t index = 0 ;

    for(std::set<LHCb::CaloCellID>::const_iterator icell = out_cells.begin();
        out_cells.end() != icell; ++icell , ++index){

      const LHCb::CaloDigit* digit   = hits->object(*icell);
      if ( 0 == digit ) { continue ; }  // CONTINUE !!! 
      
      CelAutoTaggedCell& taggedCell = *(local_cells.begin() + index ) ;
      taggedCell = digit ;

      taggedCellsDirect.addEntry ( &taggedCell , digit->cellID() ) ;
      taggedCellsSeq.push_back   ( &taggedCell                   ) ;
    }
    
  }


   // Find and mark the seeds (local maxima) 
  for( SeqVector::iterator itTag = taggedCellsSeq.begin(); taggedCellsSeq.end() != itTag ; ++itTag )
  { 
    if ( isLocMax ( (*itTag)->digit() , 
                    taggedCellsDirect , 
                    m_detector)       ) { 
      (*itTag)->setIsSeed(); 
    } 
  }
  
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
      // switch off prints
      /*      Warning ( " TAGGING NOT FULL - Remain " 
                + boost::lexical_cast<std::string> ( number  ) 
                + " not clustered cells" , StatusCode::SUCCESS ) ;*/

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

  int n_clust_stored = 0;
  
  while ( itTagSeed != itTagLastSeed ) 
  {
    LHCb::CaloCluster* cluster = new  LHCb::CaloCluster();
    const LHCb::CaloDigit* digit = (*itTagSeed)->digit() ;

    cluster->entries().push_back( LHCb::CaloClusterEntry( digit , seed ) );

    ++n_clust_stored;
  
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
    };

    setEXYCluster ( cluster, m_detector );
    
    cluster->setType( LHCb::CaloCluster::CellularAutomaton ) ;
    cluster->position().setZ( zPosition( cluster )  );
    
    //  put cluster to the output
    clusters.push_back( cluster );
  
    itTagClustered1 = itTagClustered2;
    itTagSeed++;
  }
  
  // clear local storages 
  taggedCellsSeq    .clear () ;
  taggedCellsDirect .clear () ;
  local_cells       .clear () ;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
CaloClusterizationTool::~CaloClusterizationTool() {} 

//=============================================================================
