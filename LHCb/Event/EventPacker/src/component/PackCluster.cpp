
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

  // Check to see if packed output already exists. If it does print a warning and return
  LHCb::PackedClusters * out = getIfExists<LHCb::PackedClusters>(m_outputName);
  if ( out )
  {
    return Warning( "Packed Clusters already exist at '"+m_outputName+"' -> Abort",
                    StatusCode::SUCCESS, 3 );
  }

  // Create and save the output container
  out = new LHCb::PackedClusters();
  put( out, m_outputName );
  
  // Load the input. If not existing just return
  const LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>( m_inputName );
  if ( !tracks ) return StatusCode::SUCCESS;

  // Select LHCbIDs from the input tracks
  std::vector<LHCb::LHCbID> allIds;
  allIds.reserve( tracks->size() * 40 );
  for ( const auto* tk : *tracks )
  {
    for ( const auto& id : tk->lhcbIDs() )
    {
      // Skip OT hits ...
      if ( !id.isOT() ) { allIds.push_back(id); }
    }
  }
 
  // Sort and remove duplicate LHCbIDs
  std::sort( allIds.begin(), allIds.end() );
  allIds.erase( std::unique(allIds.begin(),allIds.end()), allIds.end() );

  // Cluster pointers. Only loaded when really needed.
  const LHCb::VeloClusters* vClus = NULL;
  const LHCb::STClusters*  ttClus = NULL;
  const LHCb::STClusters*  utClus = NULL;
  const LHCb::STClusters*  itClus = NULL;

  // pack the clusters
  for ( const auto& id : allIds )
  {
    if ( msgLevel(MSG::VERBOSE) ) { verbose() << "Packing " << id << endmsg; }

    // Pack by type
    if ( id.isVelo() )
    {
      if ( !vClus ) { vClus = getIfExists<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default); }
      const LHCb::VeloCluster* cl = ( vClus ? vClus->object(id.veloID()) : NULL );
      if ( cl ) { out->addVeloCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown Velo cluster : " << id;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( id.isTT() )
    {
      if ( !ttClus ) { ttClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters); }
      const LHCb::STCluster* cl = ( ttClus ? ttClus->object(id.stID()) : NULL );
      if ( cl ) { out->addTTCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown TT cluster : " << id;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( id.isUT() )
    {
      if ( !utClus ) { utClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::UTClusters); }
      const LHCb::STCluster* cl = ( utClus ? utClus->object(id.stID()) : NULL );
      if ( cl ) { out->addUTCluster( cl ); }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown UT cluster : " << id;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( id.isIT() ) 
    {
      if ( !itClus ) { itClus = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters); }
      const LHCb::STCluster* cl = ( itClus ? itClus->object(id.stID()) : NULL );
      if ( cl ) { out->addITCluster( cl ); }
      else
      {
        std::ostringstream mess;
        mess << "Unknown IT cluster : " << id;
        Warning( mess.str() ).ignore();
      }
    } 
    else 
    {
      std::ostringstream mess;
      mess << "Unknown LHCbID type : " << id;
      Warning( mess.str() ).ignore();
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Stored " << out->clusters().size() << " clusters" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
