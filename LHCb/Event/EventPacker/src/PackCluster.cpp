// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "PackCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackCluster
//
// 2012-03-05 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackCluster )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackCluster::PackCluster( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedClusterLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
PackCluster::~PackCluster() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PackCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCluster::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::Tracks>(m_inputName) ) return StatusCode::SUCCESS;
  LHCb::Tracks* tracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_inputName );
  LHCb::PackedClusters* out = new LHCb::PackedClusters();
  put( out, m_outputName );


  std::vector<LHCb::LHCbID> allIds;
  allIds.reserve( tracks->size() * 40 );
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
    LHCb::Track* track = *itT;
    for ( std::vector<LHCb::LHCbID>::const_iterator itI = track->lhcbIDs().begin();
          track->lhcbIDs().end() != itI; ++itI ) {
      if ( (*itI).isOT() ) continue;
      allIds.push_back( *itI );
    }
  }
  // Sort and remove duplicate LHCbIDs
  std::sort( allIds.begin(), allIds.end() );
  std::vector<LHCb::LHCbID>::iterator itEnd = std::unique( allIds.begin(), allIds.end() );
  allIds.resize( itEnd - allIds.begin() );
  
  LHCb::VeloClusters* vClus = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default );
  LHCb::STClusters*  ttClus = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters );
  LHCb::STClusters*  itClus = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters );
  
  for ( std::vector<LHCb::LHCbID>::const_iterator itI = allIds.begin();
        allIds.end() != itI; ++itI ) {
    if ( (*itI).isVelo() ) {
      LHCb::VeloCluster* cl = vClus->object( (*itI).veloID() );
      if ( NULL != cl ) {
        out->addVeloCluster( cl );
      } else {
        info() << "Unknown Velo cluster " << *itI << endmsg;
      }
    } else if ( (*itI).isTT() ) {
      LHCb::STCluster* cl = ttClus->object( (*itI).stID() );
      if ( NULL != cl ) {
        out->addTTCluster( cl );
      } else {
        info() << "Unknown TT cluster " << *itI << endmsg;
      }
    } else if ( (*itI).isIT() ) {
      LHCb::STCluster* cl = itClus->object( (*itI).stID() );
      if ( NULL != cl ) {
        out->addITCluster( cl );
      } else {
        info() << "Unknown TT cluster " << *itI << endmsg;
      }
    } else {
      info() << "Unknown LHCb ID on track : " << *itI << endmsg;
    }
  }
  
  debug() << "Stored " << out->clusters().size() << " clusters" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PackCluster::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
