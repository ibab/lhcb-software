#include "CaloInterfaces/ICaloClusterization.h"
#include "L0Calo2CaloTool.h"
#include "CaloKernel/CaloVector.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/L0DUBase.h"

#include <boost/functional/hash.hpp>

/** @class L0Calo2CaloTool L0Calo2CaloTool.cpp
 *
 * Tool to get a list of CaloClusters (owned by TES) in the vicinity of
 * the input L0CaloCandidate(s), if necessary invoking decoding and clusterization.
 *
 *  @author Dmitry Goloubkov
 *  @date   2009-07-29
 */

DECLARE_TOOL_FACTORY( L0Calo2CaloTool );
  
/// Standard constructor
L0Calo2CaloTool::L0Calo2CaloTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
   : GaudiTool ( type, name , parent )
{
  declareInterface<IL0Calo2Calo>(this);

  declareProperty("CaloClusterizationTool", m_clusterizationToolName = "CaloClusterizationTool");
  declareProperty("CaloDataProviderTool",   m_dataProviderToolName   = "CaloDataProvider");
  declareProperty("CaloDigitLocation",      m_digitLocation          = LHCb::CaloDigitLocation::Hlt1Ecal);
  declareProperty("CaloClusterLocation",    m_clusterLocation        = LHCb::CaloClusterLocation::EcalHlt1);  
  declareProperty("NeighbourLevel",         m_neighbourLevel         = 0,
                  "Level parameter for the CaloClusterizationTool, search clusters in (2+Level)x(2+Level) region around the seed cell");
  declareProperty("Sort",                   m_sort                   = false,
                  "sort the clusters due to energy");
  declareProperty("SortET",                 m_sortET                 = false,
                  "if Sort: sort the clusters due to transverse energy");
  declareProperty("DecodeFullEcal",         m_decodeFullEcal         = false,
                  "false = decode only the Tell1s around the L0CaloCandidate cellID");
  declareProperty("DuplicateClustersOnTES", m_duplicateClustersOnTES = false,
                  "allow duplicated clusters on TES in case of multiple calls on the same zone");
}
// ==========================================================================
StatusCode L0Calo2CaloTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_calo               = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_clusterizationTool = tool< ICaloClusterization >(m_clusterizationToolName);
  m_dataProviderTool   = tool< ICaloDataProvider >(m_dataProviderToolName, "EcalDataProvider", this, true);
  m_ecalCaloNum        = CaloCellCode::CaloNumFromName("Ecal");

  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode L0Calo2CaloTool::clusterize
( std::vector<LHCb::CaloCluster*>&      clusters, 
  const LHCb::L0CaloCandidate*          candidate,
  const unsigned int                    level     ) const
{
  if ( ! isUsable( candidate ) ) return StatusCode::FAILURE;

  const LHCb::CaloCellID& cellID = candidate->id();
  if ( ! isUsable( cellID ) ) return StatusCode::FAILURE;

  // Invoke the CaloDataProvider tool
  const LHCb::CaloDigits* digits = makeDigits( cellID, level );
  if ( 0 == digits ) return Error(" Failure to (re)make CaloDigits!", StatusCode::FAILURE );

  // Invoke the CaloClusterizationTool
  StatusCode sc = makeClusters( clusters, digits, cellID, level );
  if ( sc.isFailure() ) return Error(" Failure from the CaloClusterizationTool!", sc );

  return putClustersOnTES( clusters );
}
// ==========================================================================
StatusCode L0Calo2CaloTool::clusterize
( std::vector<LHCb::CaloCluster*>&      clusters, 
  const LHCb::L0CaloCandidate*          candidates ) const
{
  return clusterize( clusters, candidates, m_neighbourLevel );
}
// ==========================================================================
StatusCode L0Calo2CaloTool::makeClusters
( std::vector<LHCb::CaloCluster*>&      clusters, 
  const LHCb::CaloDigits*               digits,
  const LHCb::CaloCellID&               cellID,
  const unsigned int                    level    ) const
{
  if ( NULL == digits ) return Error("CaloDigits == NULL (should never happen here)", StatusCode::FAILURE);
  clusters.clear();

  StatusCode sc;
  if ( level > 0 ){
    std::vector<LHCb::CaloCellID> seeds;
    seeds.clear();
    seeds.push_back( cellID );
    sc = m_clusterizationTool->clusterize(clusters, digits, m_calo, seeds, level);
  }
  else {
    sc = m_clusterizationTool->clusterize(clusters, digits, m_calo);
  }
  
  // TODO : use level with list of seeds
  if ( sc.isFailure() ) return Error(" Failure from the CaloClusterizationTool!" , sc);

   /** sort the sequence to simplify the comparison 
   *  with other clusterisation techniques 
   */
  if ( m_sort ){ 
    error() << " FEATURE NOT implemented! (neither Sort nor SortET)" << endmsg;
/*
    if ( !m_sortET ) 
    {
      // sorting criteria: Energy
      LHCb::CaloDataFunctor::Less_by_Energy<const LHCb::CaloCluster*> Cmp;
      // perform the sorting 
      std::stable_sort    ( clusters.begin()            ,
                            clusters.end  ()            ,
                            LHCb::CaloDataFunctor::inverse( Cmp ) ) ;
    }
    else
    {
      // sorting criteria : Transverse Energy
      LHCb::CaloDataFunctor::Less_by_TransverseEnergy<const LHCb::CaloCluster*,
        const DeCalorimeter*> Cmp ( m_detector ) ;
      // perform the sorting 
      std::stable_sort   ( clusters.begin()            ,
                           clusters.end  ()            ,
                           LHCb::CaloDataFunctor::inverse( Cmp ) ) ;    
    }
*/
  }

  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode L0Calo2CaloTool::putClustersOnTES( std::vector<LHCb::CaloCluster*>& clusters ) const
{
  LHCb::CaloClusters* output = getOrCreate<LHCb::CaloClusters, LHCb::CaloClusters>( m_clusterLocation );
  output -> setVersion( 2 ); // update the version number (needed for serialization)
  if ( msgLevel(MSG::DEBUG) )
    debug() << "0: LHCb::CaloClusters at " << m_clusterLocation << " size = " << ( output ? (long) output->size() : -1 )
            << " v=" << (int) output->version() << " newly found clusters.size() = " << clusters.size() << endmsg; ///////

  // populate the CaloClusters TS container 
  int cnt(0);
  for ( std::vector<LHCb::CaloCluster*>::iterator icluster = clusters.begin();
        clusters.end() != icluster; ++icluster){
    LHCb::CaloCluster* cl  = *icluster;
    if ( 0 == cl ) continue;

    if ( m_duplicateClustersOnTES ){ // store everything on TES (allow duplications in case of multiple calls)
      output -> insert ( cl ) ;
      continue;
    }

    // look for a cluster with the same seed cell (cover most of the cases)
    int key = static_cast<int>( cl->seed().index() ); // alternatively use m_calo->cellIndex( cl->seed() )
    const LHCb::CaloCluster* fnd = output->object( key );

    // when found, check if the cluster seems to be the same (e.g. same energy)
    static const double zerro = 1e-15;
    if ( fnd && (fabs(fnd->e() - cl->e()) > zerro) ){
      // Now look for a cluster with the same seed cell AND energy
      // the hKey typically is a very large number, much more than cellID.index().
      // Using the unique cellID.index() number as a key to most of the clusters,
      // reduce the clashes possible in case of using only the hKey for everything.
      size_t hKey = key;
      boost::hash_combine<double>(hKey, cl->e());
      key = static_cast<int>( hKey );
      fnd = output->object( key );
    }
    if ( msgLevel(MSG::DEBUG) ) debug() << " CaloCluster #" << cnt ++ << " seed =" << (LHCb::CaloCellID)cl->seed() << " idx =" << cl->seed().index()
                                        << " CaloCluster key = " << cl->key() << " Hash key=" << key << "  e =" << cl->e() << endmsg;
    if ( fnd ){ // if already in TES - substitute the pointer in the output container with that from TES
      delete cl;
      cl = const_cast<LHCb::CaloCluster *>( fnd );
      continue;
    }
    output -> insert ( cl, key ) ;
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "1: LHCb::CaloClusters at " << m_clusterLocation << " size = " << ( output ? long( output->size() ) : -1 ) << endmsg; ////////////
 
  return StatusCode::SUCCESS;
}
// ==========================================================================
bool L0Calo2CaloTool::isUsable( const LHCb::CaloCellID& cellID ) const
{
  if ( cellID.calo() != m_ecalCaloNum ){
    if ( msgLevel(MSG::DEBUG) ) debug() << "skip non-Ecal cellID = " << cellID << endmsg;
    return false;
  }

  if ( !m_calo->valid( cellID ) ){
    if ( msgLevel(MSG::DEBUG) ) debug() << "skip ivnalid cellID = " << cellID << endmsg;
    return false;
  }
 
  return true;
}
// ==========================================================================
bool L0Calo2CaloTool::isUsable( const LHCb::L0CaloCandidate* candidate ) const
{
  if ( ! candidate ) return false;

  int type = candidate->type();
  if (   type == L0DUBase::CaloType::Electron
      || type == L0DUBase::CaloType::Photon  ) return true;

  return false;
}
// ==========================================================================
const LHCb::CaloDigits* L0Calo2CaloTool::makeDigits
( const LHCb::CaloCellID&  cellID,
  const unsigned int       level  ) const
{
  bool read = exist<LHCb::CaloDigits>( m_digitLocation );
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "0: data at " << m_digitLocation << " does" << ( read ? "" : " NOT") << " exist" << endmsg;
  if ( !read ){
    m_dataProviderTool->getBanks();
    m_decodedSources.clear();
  }

  LHCb::CaloDigits* digits = getOrCreate<LHCb::CaloDigits, LHCb::CaloDigits>( m_digitLocation ) ;
  if ( msgLevel(MSG::DEBUG) )
     debug() << "1: data at " << m_digitLocation << " does" << ( exist<LHCb::CaloDigits>( m_digitLocation ) ? "" : " NOT") << " exist"
             << " digits->size() = " << ( digits ? long( digits->size() ) : -1 ) << endmsg;

  if ( m_decodedSources.find( -1 ) != m_decodedSources.end() ){
    if ( msgLevel(MSG::DEBUG) ) debug() << " full Ecal (source =-1) has been already decoded" << endmsg;
    return digits;
  }


  std::set<int> tell1s;
  if ( m_decodeFullEcal )
     tell1s.insert( -1 ); // single source = -1: decode everything
  else
     (void) collectTell1s( tell1s, cellID, level ); // loop over neighbour cells, collect corresponding tell1s
 

  std::vector<int> sources;
  for (std::set<int>::const_iterator it = tell1s.begin(); it != tell1s.end(); ++ it){
    int source = *it;
    if ( m_decodedSources.find( source ) == m_decodedSources.end() ){
      sources.push_back( source );
      m_decodedSources.insert( source );
    }else
      if ( msgLevel(MSG::DEBUG) ) debug() << " source " << *it << " has been already decoded" << endmsg;
  }

  if ( sources.empty() ){
    if ( msgLevel(MSG::DEBUG) ) debug() << " all sources have been already decoded" << endmsg;
    return digits;
  }


  CaloVector<LHCb::CaloDigit>& dgs = m_dataProviderTool->digits( sources );


  // populate the CaloDigits TS container 
  for (CaloVector<LHCb::CaloDigit>::const_iterator it = dgs.begin(); it != dgs.end(); ++ it){
    const LHCb::CaloDigit* found = digits->object ( it->cellID() ) ;
    if ( 0 != found ) { continue ; }	 	
    digits->insert( new LHCb::CaloDigit(*it) );
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "2: digits->size() = " << ( digits ? (long) digits->size() : -1 ) << endmsg;

  return digits;
}
// ==========================================================================
StatusCode L0Calo2CaloTool::finalize()
{  
  return GaudiTool::finalize ();
}
// ==========================================================================
// Looking neigbours around cell (borrowed from CaloClusterizationTool)
// ============================================================================
void L0Calo2CaloTool::look_neig
( const std::set<LHCb::CaloCellID>& in_cells,
  std::set<LHCb::CaloCellID>&       out_cells ) const
{
  for(std::set<LHCb::CaloCellID>::const_iterator icell = in_cells.begin();
      in_cells.end() != icell; ++icell){
    const CaloNeighbors& nei = m_calo->neighborCells( (*icell) );
    out_cells.insert( nei.begin(), nei.end() );
  }
}
// ============================================================================
void L0Calo2CaloTool::collectTell1s
( std::set<int>&                    tell1s,
  const LHCb::CaloCellID&           cellID,
  const unsigned int                level  ) const
{
  std::set<LHCb::CaloCellID> cell_list;
  std::set<int> cards;

  if ( !m_calo->valid(cellID) ) return;

  // incert the cells corresponding to the L0CaloCandidate
  cell_list.insert( cellID );

  std::set<LHCb::CaloCellID> out_cells = cell_list;
  if ( level > 0 ){
    unsigned int area = cellID.area();
    unsigned int row  = cellID.row();
    unsigned int col  = cellID.col();
    // LHCb::CaloCellID(unsigned int Calo, unsigned int Area, unsigned int Row, unsigned int Column)
    LHCb::CaloCellID ul(m_ecalCaloNum, area, row+1, col  );
    LHCb::CaloCellID ur(m_ecalCaloNum, area, row+1, col+1);
    LHCb::CaloCellID dr(m_ecalCaloNum, area, row  , col+1);

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "seed=" << cellID << ":" << m_calo->cellCenter( cellID )
                << " ul =" << ul << ":" << m_calo->cellCenter( ul )
                << " ur =" << ur << ":" << m_calo->cellCenter( ur )
                << " dr =" << dr << ":" << m_calo->cellCenter( dr ) << endmsg;
  
    if ( m_calo->valid(ul) ) cell_list.insert( ul );
    else error() << "ul cell " << ul << " invalid" << endmsg;

    if ( m_calo->valid(ur) ) cell_list.insert( ur );
    else error() << "ur cell " << ur << " invalid" << endmsg;

    if ( m_calo->valid(dr) ) cell_list.insert( dr );
    else error() << "dr cell " << dr << " invalid" << endmsg;

    // ------------------------------------------------------------------------
    // look Cell neigbours -- code adapted from the CaloClusterization tool
    out_cells = cell_list;
    std::set<LHCb::CaloCellID> cells;

    for (std::set<LHCb::CaloCellID>::const_iterator icell = 
         cell_list.begin() ; cell_list.end() != icell ; ++icell){  
      const CaloNeighbors& nei = m_calo->neighborCells( (*icell) );
      cells.insert( nei.begin(), nei.end() );
      cells.insert( (*icell) );
    }

    // look Cell neigbours for next levels
    std::set<LHCb::CaloCellID>& in_cells  = cells;
    out_cells = cells;

    for (int ii = level; ii > 1; ii--){
      look_neig(in_cells, out_cells);
      in_cells = out_cells;
    }
    // ------------------------------------------------------------------------
  }


  for (std::set<LHCb::CaloCellID>::const_iterator it = out_cells.begin();
       it != out_cells.end(); ++ it){
    if ( ! m_calo->valid( *it ) ){
      warning() << "one of the neighbour cells, " << *it << " turned out to be invalid" << endmsg;
      continue;
    }

    int card = m_calo->cardNumber( *it );
    if ( card < 0 ){
      warning() << "valid cell " << *it << " corresponds to no FE card" << endmsg;
      continue;
    }
    if ( msgLevel(MSG::DEBUG) ) debug() << "  cell " << (*it) << " card = " << card << endmsg;
    cards.insert( card );
  }

  for (std::set<int>::const_iterator it = cards.begin(); it != cards.end(); ++ it){
    int source = m_calo->cardToTell1( *it );
    if ( msgLevel(MSG::DEBUG) ) debug() << " card = " << *it << " tell1 ID = " << source << endmsg;
    if ( source < 0 ){
      if ( msgLevel(MSG::ERROR) ) warning() <<  "FE card = " << *it << " corresponds to no Tell1" << endmsg;
      continue;
    }
    tell1s.insert( source );
  }

  return;
}
// ============================================================================
