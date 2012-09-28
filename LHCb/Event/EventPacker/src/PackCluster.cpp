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
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
PackCluster::~PackCluster() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCluster::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::Tracks>(m_inputName) ) return StatusCode::SUCCESS;

  // Create and save the output container
  LHCb::PackedClusters* out = new LHCb::PackedClusters();
  put( out, m_outputName );

  // Load the input. If not existing just return
  const LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>( m_inputName );
  if ( !tracks ) return StatusCode::SUCCESS;

  // Select LHCbIDs from the input tracks
  std::vector<LHCb::LHCbID> allIds;
  allIds.reserve( tracks->size() * 40 );
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) 
  {
    for ( std::vector<LHCb::LHCbID>::const_iterator itI = (*itT)->lhcbIDs().begin();
          (*itT)->lhcbIDs().end() != itI; ++itI )
    {
      if ( (*itI).isOT() ) continue;
      allIds.push_back( *itI );
    }
  }
 
  // Sort and remove duplicate LHCbIDs
  std::sort( allIds.begin(), allIds.end() );
  allIds.erase( std::unique(allIds.begin(),allIds.end()), allIds.end() );

  // Load the clusters
  const LHCb::VeloClusters* vClus = getIfExists<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
  const LHCb::STClusters*  ttClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
  const LHCb::STClusters*  utClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::UTClusters);
  const LHCb::STClusters*  itClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);

  // pack the clusters
  for ( std::vector<LHCb::LHCbID>::const_iterator itI = allIds.begin();
        allIds.end() != itI; ++itI ) 
  {
    if ( msgLevel(MSG::VERBOSE) ) { verbose() << "Packing " << *itI << endmsg; }

    // Pack by type
    if ( (*itI).isVelo() )
    {
      const LHCb::VeloCluster* cl = ( vClus ? vClus->object((*itI).veloID()) : NULL );
      if ( cl ) { out->addVeloCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown Velo cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( (*itI).isTT() )
    {
      const LHCb::STCluster* cl = ( ttClus ? ttClus->object((*itI).stID()) : NULL );
      if ( cl ) { out->addTTCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown TT cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( (*itI).isUT() )
    {
      const LHCb::STCluster* cl = ( utClus ? utClus->object((*itI).stID()) : NULL );
      if ( cl ) { out->addUTCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown UT cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( (*itI).isIT() ) 
    {
      const LHCb::STCluster* cl = ( itClus ? itClus->object((*itI).stID()) : NULL );
      if ( cl ) { out->addITCluster( cl ); }
      else
      {
        std::ostringstream mess;
        mess << "Unknown IT cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else 
    {
      std::ostringstream mess;
      mess << "Unknown LHCbID type : " << *itI;
      Warning( mess.str() ).ignore();
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Stored " << out->clusters().size() << " clusters" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
