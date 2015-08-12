// Include files 

// from Gaudi
#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

// local
#include "VeloUpdateHighThreshold.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloUpdateHighThreshold
//
// 2009-08-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloUpdateHighThreshold )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloUpdateHighThreshold::VeloUpdateHighThreshold( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "HighThreshold", m_highThreshold = 80. );
}
//=============================================================================
// Destructor
//=============================================================================
VeloUpdateHighThreshold::~VeloUpdateHighThreshold() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloUpdateHighThreshold::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloUpdateHighThreshold::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //== Get lite clusters
  LHCb::VeloLiteCluster::FastContainer* lite = 
    get<LHCb::VeloLiteCluster::FastContainer>( LHCb::VeloLiteClusterLocation::Default );
  
  //== Get the clusters
  LHCb::VeloClusters* clust = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default );

  if ( lite->size() !=  clust->size() ) {
    info() << "Lite " << lite->size() << " clusters, full " << clust->size() << " clusters." << endmsg;
    return StatusCode::FAILURE;
  }
  
  LHCb::VeloClusters::const_iterator itC = clust->begin();
  for ( LHCb::VeloLiteCluster::FastContainer::iterator itL = lite->begin();
        lite->end() != itL; ++itL, ++itC ) {
    LHCb::VeloChannelID lID = (*itL).channelID();
    LHCb::VeloChannelID cID = (*itC)->channelID();
    if ( cID != lID ) {
      info() << "Mismatch in clusters: lite " << lID << " cluster " << cID << endmsg;
      return StatusCode::FAILURE;
    }
    double fracStrip     = (*itL).interStripFraction();
    unsigned int size    = (*itL).pseudoSize();
    bool secondThreshold = ( (*itC)->totalCharge() > m_highThreshold );
    LHCb::VeloLiteCluster newLite( lID, fracStrip, size, secondThreshold );
    (*itL) = newLite;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloUpdateHighThreshold::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
