
// Include files
#include <iterator>
// local
#include "TrackClustersMapper.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ParticlesAndVerticesMapper
//
// 17/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
TrackClustersMapper::TrackClustersMapper( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : MapperToolBase( type, name, parent )
{
  declareProperty( "UnpackerType", m_unpackerType = "UnpackCluster" );
  declareProperty( "UnpackerOutputLevel", m_unpackersOutputLevel = -1 );
  //setProperty( "OutputLevel", 1 );
}

// ============================================================================

TrackClustersMapper::~TrackClustersMapper() {}

// ============================================================================

Gaudi::Utils::TypeNameString
TrackClustersMapper::algorithmForPath( const std::string & path )
{
  LOG_VERBOSE << "TrackClustersMapper::algorithmForPath '"
              << path << "'" << endmsg;

  updateNodeTypeMap(path);

  if ( pathIsHandled(path) )
  {
    // Choose a unique name for the algorithm instance
    const std::string algName = streamName(path) + "_TkClusUnpack";

    // Set RootInTES for this unpacker. Controls where packed data is loaded
    // from and where the output goes.
    joSvc()->addPropertyToCatalogue( algName,
                                     StringProperty("RootInTES",streamRoot(path)) );
    if ( m_unpackersOutputLevel > 0 )
    {
      std::stringstream lvl;
      lvl << m_unpackersOutputLevel;
      joSvc()->addPropertyToCatalogue( algName,
                                       StringProperty("OutputLevel",lvl.str()));
    }

    // Return the algorithm type/name.
    LOG_VERBOSE << " -> Use algorithm type '" << m_unpackerType << "'"
                << " name '" << algName << "'"
                << endmsg;
    return Gaudi::Utils::TypeNameString(algName,m_unpackerType);
  }

  return "";
}

// ============================================================================

std::string
TrackClustersMapper::nodeTypeForPath( const std::string & path )
{
  updateNodeTypeMap(path);

  NodeTypeMap::const_iterator it = m_nodeTypeMap.find( fixPath(path) );

  const std::string& retS = ( it != m_nodeTypeMap.end() ? it->second : "" );

  LOG_VERBOSE << "TrackClustersMapper::nodeTypeForPath '"
              << path << "' NodeType '" << retS << "'" << endmsg;

  return retS;
}

// ============================================================================

void TrackClustersMapper::updateNodeTypeMap( const std::string & path )
{
  // The stream TES root
  const std::string streamR = streamRoot(path);

  LOG_VERBOSE << "TrackClustersMapper::updateNodeTypeMap Running for " 
              << streamR << endmsg;

  // See if the packed clusters object exists for this stream
  if ( !m_streamsDone[streamR] )
  {
    m_streamsDone[streamR] = true;
    const std::string packedLoc = packedClustersLocation(streamR);
    LOG_VERBOSE << "TrackClustersMapper::updateNodeTypeMap Looking for " 
                << packedLoc << endmsg;
    if ( exist<LHCb::PackedClusters*>(packedLoc) )
    {
      // Update the node type paths handled
      addPath( streamR + "/" + LHCb::VeloClusterLocation::Default  );
      addPath( streamR + "/" + LHCb::STClusterLocation::TTClusters );
      addPath( streamR + "/" + LHCb::STClusterLocation::UTClusters );
      addPath( streamR + "/" + LHCb::STClusterLocation::ITClusters );
    }
    else
    {
      LOG_VERBOSE << " -> Missing" << endmsg;
    }
  }
}

// ============================================================================

void TrackClustersMapper::addPath( const std::string & path )
{
  // Make sure paths start with /Event/
  const std::string npath = fixPath(path);

  // if not already there, add.
  if ( m_nodeTypeMap.find(npath) == m_nodeTypeMap.end() )
  {
    LOG_VERBOSE << " -> Path " << npath << endmsg;

    // Main path
    m_nodeTypeMap[npath] = "";

    // Data Node paths ...
    std::string tmp = npath;
    std::string::size_type slash = tmp.find_last_of("/");
    while ( !tmp.empty() && slash != std::string::npos )
    {
      tmp = tmp.substr(0,slash);
      if ( !tmp.empty() )
      {
        LOG_VERBOSE << "  -> Node " << tmp << endmsg;
        m_nodeTypeMap[tmp] = "DataObject";
      }
      slash = tmp.find_last_of("/");
    }
  }

}

// ============================================================================

DECLARE_TOOL_FACTORY(TrackClustersMapper)

// ============================================================================
