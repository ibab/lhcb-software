
// Include files
#include <iterator>
// local
#include "ChargedProtoParticleMapper.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ParticlesAndVerticesMapper
//
// 17/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
ChargedProtoParticleMapper::ChargedProtoParticleMapper( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : MapperToolBase( type, name, parent )
{
  declareProperty( "UnpackerOutputLevel", m_unpackersOutputLevel = -1 );
  {
    const std::vector<std::string> tks = 
      boost::assign::list_of("Long")("Downstream")("Upstream");
    declareProperty( "TrackTypes", m_tkTypes = tks );
  }
  { 
    const std::vector<std::string> pids =
      boost::assign::list_of("Electron")("Muon")("Pion")("Kaon")("Proton")("Ghost");
    declareProperty( "PIDTypes", m_pidTypes = pids );
  }
  declareProperty( "ANNPIDTune", m_pidTune = "" );
  declareProperty( "VersionRegex", m_regex = "" );
}

// ============================================================================

ChargedProtoParticleMapper::~ChargedProtoParticleMapper() { }

// ============================================================================

Gaudi::Utils::TypeNameString
ChargedProtoParticleMapper::algorithmForPath( const std::string & path )
{
  LOG_VERBOSE << "ChargedProtoParticleMapper::algorithmForPath '"
              << path << "'" << endmsg;

  updateNodeTypeMap(path);

  if ( pathIsHandled(path) )
  {
    // Choose a unique name
    const std::string baseName = streamName(path) + "_ChargedPP";

    // Use a sequencer as the main type
    const std::string unpackerType = "GaudiSequencer";
    const std::string seqName      = baseName + "_Seq";

    // List of algorithms to run in the sequencer
    std::vector<std::string> algs;

    // Add the basic unpacker
    const std::string unpackName = baseName + "_Unpack";
    algs.push_back( "UnpackProtoParticle/" + unpackName );
    joSvc()->addPropertyToCatalogue( unpackName,
                                     StringProperty("InputName",
                                                    packedProtoLocation(streamRoot(path))) );
    joSvc()->addPropertyToCatalogue( unpackName,
                                     StringProperty("OutputName",
                                                    protoLocation(streamRoot(path))) );

    // PID recalibration
    if ( !m_pidTune.empty() )
    {
      if ( !m_regex.empty() )
      {
        const std::string name = "Reco14Filter_" + baseName;
        joSvc()->addPropertyToCatalogue( name, StringProperty("HeaderLocation","Rec/Header") );
        joSvc()->addPropertyToCatalogue( name, StringProperty("VersionRegex",m_regex) );
        algs.push_back( "ApplicationVersionFilter/" + name );
      }
      for ( const auto& tkType : m_tkTypes )
      {
        for ( const auto& pidType : m_pidTypes )
        {
          const std::string name = "ANNPID" + tkType + pidType + "_" + baseName;
          joSvc()->addPropertyToCatalogue( name, StringProperty("TrackType",tkType) );
          joSvc()->addPropertyToCatalogue( name, StringProperty("PIDType",pidType) );
          joSvc()->addPropertyToCatalogue( name, StringProperty("NetworkVersion",m_pidTune) );
          joSvc()->addPropertyToCatalogue( name, StringProperty("ProtoParticleLocation",
                                                                protoLocation(streamRoot(path))) );
          algs.push_back( "ANNGlobalPID::ChargedProtoANNPIDAlg/" + name );
        }
      }
    }
    
    // Set the sequencer alg list
    joSvc()->addPropertyToCatalogue( seqName, StringArrayProperty("Members",algs) );
    
    // Set output levels, if required
    if ( m_unpackersOutputLevel > 0 )
    {
      std::stringstream lvl;
      lvl << m_unpackersOutputLevel;
      joSvc()->addPropertyToCatalogue( seqName,
                                       StringProperty("OutputLevel",lvl.str()));
      for ( std::vector<std::string>::const_iterator iName = algs.begin();
            iName != algs.end(); ++iName )
      {
        const std::string::size_type slash = (*iName).find_last_of("/");
        const std::string N = ( slash != std::string::npos ?
                                (*iName).substr(slash+1) : *iName );
        joSvc()->addPropertyToCatalogue( N, StringProperty("OutputLevel",lvl.str()) );
      }
    }

    // Return the algorithm type/name.
    LOG_VERBOSE << " -> Use algorithm type '" << unpackerType << "'"
                << " name '" << seqName << "'"
                << endmsg;
    return Gaudi::Utils::TypeNameString(seqName,unpackerType);
  }

  return "";
}

// ============================================================================

std::string
ChargedProtoParticleMapper::nodeTypeForPath( const std::string & path )
{
  updateNodeTypeMap(path);

  NodeTypeMap::const_iterator it = m_nodeTypeMap.find( fixPath(path) );

  const std::string& retS = ( it != m_nodeTypeMap.end() ? it->second : "" );

  LOG_VERBOSE << "ChargedProtoParticleMapper::nodeTypeForPath '"
              << path << "' NodeType '" << retS << "'" << endmsg;

  return retS;
}

// ============================================================================

void ChargedProtoParticleMapper::updateNodeTypeMap( const std::string & path )
{
  // The stream TES root
  const std::string streamR = streamRoot(path);

  LOG_VERBOSE << "ChargedProtoParticleMapper::updateNodeTypeMap Running for "
              << streamR << endmsg;

  // See if the packed clusters object exists for this stream
  if ( !m_streamsDone[streamR] )
  {
    m_streamsDone[streamR] = true;
    const std::string packedLoc = packedProtoLocation(streamR);
    LOG_VERBOSE << "ChargedProtoParticleMapper::updateNodeTypeMap Looking for "
                << packedLoc << endmsg;
    if ( exist<LHCb::PackedProtoParticles*>(packedLoc) )
    {
      // Update the node type paths handled
      addPath( protoLocation(streamR) );
    }
    else
    {
      LOG_VERBOSE << " -> Missing" << endmsg;
    }
  }
}

// ============================================================================

void ChargedProtoParticleMapper::addPath( const std::string & path )
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

DECLARE_TOOL_FACTORY(ChargedProtoParticleMapper)

// ============================================================================
