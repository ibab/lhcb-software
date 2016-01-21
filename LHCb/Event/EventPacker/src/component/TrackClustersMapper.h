#ifndef SRC_TrackClustersMapper_H
#define SRC_TrackClustersMapper_H 1

#include <string>
#include <map>

// base class
#include "MapperToolBase.h"

// Event model
#include "Event/PackedCluster.h"

class IJobOptionsSvc;

/** @class TrackClustersMapper TrackClustersMapper.h
 *
 *  Tool for mappings for Track clusters
 *
 * @author Chris Jones
 * @date 26/03/2012
 */
class TrackClustersMapper : public MapperToolBase
{

public:

  /// Standard constructor
  TrackClustersMapper( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~TrackClustersMapper();

public:

  /** Returns the correctly configured and name instance of the
   *  Clusters unpacker, for the given path
   */
  virtual Gaudi::Utils::TypeNameString algorithmForPath(const std::string &path);

public:

  /** Instruct the DataOnDemandSvc to create DataObjects for the
   *  intermediate levels of a path we can handle.
   */
  virtual std::string nodeTypeForPath(const std::string &path);

private:

  /// Add a path to the node type mappings
  void addPath( const std::string & path );

  /// Check if a given path is in the list of data locations created
  inline bool pathIsHandled( const std::string & path ) const
  {
    // See if we have an entry for this path
    const auto it = m_nodeTypeMap.find( fixPath(path) );
    return ( it != m_nodeTypeMap.end() );
  }
  
  /// Check the node mappings are updated for the given path stream
  void updateNodeTypeMap( const std::string & path );

  /// Get the location of the packed clusters for the given stream
  std::string packedClustersLocation( const std::string & stream ) const
  {
    return stream + "/" + LHCb::PackedClusterLocation::Default;
  }

private:

  /// Map to say which stream roots have been configured
  std::map<std::string,bool> m_streamsDone;

  /// Mapping between TES path and node type
  typedef std::map<std::string,std::string> NodeTypeMap;
  NodeTypeMap m_nodeTypeMap;

  /// Unpacker class type
  std::string m_unpackerType;
  
  /// Outputlevel for unpackers created
  int m_unpackersOutputLevel;

};

#endif // SRC_TrackClustersMapper_H
