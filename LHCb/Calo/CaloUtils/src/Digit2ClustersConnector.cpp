// ===========================================================================
#define CALOUTILS_DIGIT2CLUSTERSCONNECTOR_CPP 1 
// ===========================================================================
#include "CaloKernel/CaloException.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
// local 
#include "CaloUtils/Digit2ClustersConnector.h"

// ============================================================================
/** @file Digit2ClustersConnector.cpp 
 *
 * Implementation file for class : Digit2ClusterConnector
 * 
 * @date 01/07/2001 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** Constructor
 *   @param   Clus container of CaloCluster objects  
 *   @param   CutOff for "shared" digits 
 */
Digit2ClustersConnector::Digit2ClustersConnector( Clusters*    clus         ,
                                                  const unsigned int CutOff ) 
  : m_map()
  , m_cut( CutOff )
{
  if( 0 != clus ) 
    { 
      StatusCode sc = load( clus->begin() , 
                            clus->end  () , 
                            CutOff        ); 
      if( sc.isFailure() )
        { throw CaloException( "Digit2ClustersConnector! corrupted data") ; }
    };  
}
// ============================================================================

// ============================================================================
/// addCluster method 
StatusCode Digit2ClustersConnector::addCluster( LHCb::CaloCluster* cluster )
{
  // check arguments 
  if( 0 == cluster ) { return StatusCode::SUCCESS ;}
  // loop over cluster entries 
  for( EntryPointer entry = cluster->entries().begin();
       cluster->entries().end() != entry ; ++entry )
    {
      const LHCb::CaloDigit* digit = entry->digit();
      if( 0 == digit ) { continue; }              ///< CONTINUE!
      // update map  
      m_map[digit].push_back( ClusterPair( cluster, entry ) );
    }
  ///   
  return StatusCode::SUCCESS;
  ///
}
// ============================================================================

// ============================================================================
/// addCluster method 
StatusCode Digit2ClustersConnector::applyCutOff( const unsigned int CutOff )
{
  // check the value of cut-off 
  const unsigned int cut = ( 0 != CutOff ) ? CutOff : m_cut ;
  if( 0 == cut ) { return StatusCode::SUCCESS ; }
  // loop over the map and erase some entries 
  Map::iterator it = m_map.begin();
  while( it != m_map.end() )
    {
      if( it->second.size() >= cut ) {        ++it  ; }
      else { m_map.erase( it ) ; it = m_map.begin() ; }
    }
  ///
  return StatusCode::SUCCESS;
  ///
}
//  ===========================================================================

// ============================================================================
///  destructor
Digit2ClustersConnector::~Digit2ClustersConnector() { m_map.clear(); }
// ============================================================================

