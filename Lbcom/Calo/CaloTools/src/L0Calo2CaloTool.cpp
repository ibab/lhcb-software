#include "CaloInterfaces/ICaloClusterization.h"
#include "L0Calo2CaloTool.h"
#include "CaloKernel/CaloVector.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CaloNeighbours.h"
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

DECLARE_TOOL_FACTORY( L0Calo2CaloTool )
  
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
  declareProperty("NeighbourLevel",         m_neighbourLevel         = 2,
                  "Level parameter for the CaloClusterizationTool, search clusters in (1+2*Level)x(1+2*Level) region around the seed cell");
  declareProperty("Sort",                   m_sort                   = false,
                  "sort the clusters due to energy");
  declareProperty("SortET",                 m_sortET                 = false,
                  "if Sort: sort the clusters due to transverse energy");
  declareProperty("DecodeFullEcal",         m_decodeFullEcal         = false,
                  "false = decode only the Tell1s around the L0CaloCandidate cellID");

  declareProperty("ClusterOnTES",m_clusOnTES=false);
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

  info() << "Clusterizing Calo around L0 candidates : NeighbourLevel  set to " << m_neighbourLevel << endmsg;

  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode L0Calo2CaloTool::clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters ,
    const LHCb::CaloCellID&               cellID,
    const unsigned int                    level     ) const
{
  if ( ! isUsable( cellID ) ) return StatusCode::FAILURE;

  // Invoke the CaloDataProvider tool
  const LHCb::CaloDigits* digits = makeDigits( cellID, level );
  if ( 0 == digits ) return Error(" Failure to (re)make CaloDigits!", StatusCode::FAILURE );

  // Select the right seed :
  LHCb::CaloCellID seedID = cellID;
  unsigned int area = cellID.area();
  unsigned int row  = cellID.row();
  unsigned int col  = cellID.col();
  double dig = m_dataProviderTool->digit( cellID );
  LHCb::CaloCellID ul(m_ecalCaloNum, area, row+1, col  );
  if( m_dataProviderTool->digit( ul ) > dig ){ dig = m_dataProviderTool->digit( ul ); seedID = ul;}
  LHCb::CaloCellID ur(m_ecalCaloNum, area, row+1, col+1);
  if( m_dataProviderTool->digit( ur ) > dig ){ dig = m_dataProviderTool->digit( ur ); seedID = ur;}
  LHCb::CaloCellID dr(m_ecalCaloNum, area, row  , col+1);
  if( m_dataProviderTool->digit( dr ) > dig ){ dig = m_dataProviderTool->digit( dr ); seedID = dr;}

  // Invoke the CaloClusterizationTool
  StatusCode sc = makeClusters( clusters, digits, seedID, level );
  if ( sc.isFailure() ) return Error(" Failure from the CaloClusterizationTool!", sc );

  return putClustersOnTES( clusters );
}
// ==========================================================================                                                                         
StatusCode L0Calo2CaloTool::clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters,
    const LHCb::CaloCellID&               cellID   ) const
{
  return clusterize( clusters, cellID, m_neighbourLevel );
}

// ==========================================================================
StatusCode L0Calo2CaloTool::clusterize
( std::vector<LHCb::CaloCluster*>&      clusters, 
  const LHCb::L0CaloCandidate*          candidate,
  const unsigned int                    level     ) const
{
  if ( ! isUsable( candidate ) ) return StatusCode::FAILURE;

  const LHCb::CaloCellID& cellID = candidate->id();
  return clusterize(clusters, cellID, level);
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
  std::vector<LHCb::CaloCellID> seeds;
  seeds.clear();
  seeds.push_back( cellID );
  sc = m_clusterizationTool->clusterize(clusters, digits, m_calo, seeds, level);
  
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


  if( !m_clusOnTES )return StatusCode::SUCCESS;
  

  LHCb::CaloClusters* output = getOrCreate<LHCb::CaloClusters, LHCb::CaloClusters>( m_clusterLocation );
  output -> setVersion( 2 ); // update the version number (needed for serialization)
  if ( msgLevel(MSG::DEBUG) )
    debug() << "0: LHCb::CaloClusters at " << m_clusterLocation << " size = " << ( output ? (long) output->size() : -1 )
            << " v=" << (int) output->version() << " newly found clusters.size() = " << clusters.size() << endmsg; ///////

  // populate the CaloClusters TS container 
  for ( std::vector<LHCb::CaloCluster*>::iterator icluster = clusters.begin();
        clusters.end() != icluster; ++icluster){
    LHCb::CaloCluster* cl  = *icluster;
    if ( 0 == cl ) continue;

    // check that cluster is not yet owned by some container
    if ( 0 != cl->parent() ) continue;

    // store everything on TES (allow duplications in case of multiple calls)
    output -> insert ( cl ) ;
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
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "0: data at " << m_digitLocation 
                                          << " does" << ( read ? "" : " NOT") << " exist" << endmsg;
  if ( !read ){
    //m_dataProviderTool->getBanks();
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


  const CaloVector<LHCb::CaloDigit>& dgs = m_dataProviderTool->digits( sources );


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
// ============================================================================
void L0Calo2CaloTool::collectTell1s
( std::set<int>&                    tell1s,
  const LHCb::CaloCellID&           cellID,
  const unsigned int                level  ) const
{
  LHCb::CaloCellID::Set out_cells;
  std::set<int> cards;

  if ( !m_calo->valid(cellID) ) return;

  // incert the cells corresponding to the L0CaloCandidate
  out_cells.insert( cellID );

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
  
    if ( m_calo->valid(ul) ) out_cells.insert( ul );
    else
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "ul cell " << ul << " invalid" << endmsg;

    if ( m_calo->valid(ur) ) out_cells.insert( ur );
    else
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "ur cell " << ur << " invalid" << endmsg;

    if ( m_calo->valid(dr) ) out_cells.insert( dr );
    else
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "dr cell " << dr << " invalid" << endmsg;


    /** find all neighbours for the given set of cells for the givel level
     *  @param cells    (UPDATE) list of cells
     *  @param level    (INPUT)  level
     *  @param detector (INPUT) the detector
     *  @return true if neighbours are added
     */
    LHCb::CaloFunctors::neighbours(out_cells, level, m_calo);
  }


  for (LHCb::CaloCellID::Set::const_iterator it = out_cells.begin();
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
