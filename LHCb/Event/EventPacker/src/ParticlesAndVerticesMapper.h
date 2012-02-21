#ifndef SRC_ParticlesAndVerticesMapper_H
#define SRC_ParticlesAndVerticesMapper_H 1

#include <string>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Implemented interfaces
#include "GaudiKernel/IDODAlgMapper.h"
#include "GaudiKernel/IDODNodeMapper.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "Event/PackedParticle.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"
#include "Event/PackedParticle2Ints.h"

#include "Kernel/StandardPacker.h"

class IJobOptionsSvc;

/** @class ParticlesAndVerticesMapper ParticlesAndVerticesMapper.h
 *
 *  Tool for automatic conversions in the transient store.
 *
 *  Bespoke mapping tool for the UnpackParticlesAndVertices unpacker
 *
 * @author Chris Jones
 * @date 06/02/2012
 */
class ParticlesAndVerticesMapper : public extends3< GaudiTool,
                                                    IDODAlgMapper,
                                                    IDODNodeMapper,
                                                    IIncidentListener >
{

public:

  /// Standard constructor
  ParticlesAndVerticesMapper( const std::string& type,
                              const std::string& name,
                              const IInterface* parent );

  /// Destructor
  virtual ~ParticlesAndVerticesMapper();

  /// Initialize the tool instance.
  virtual StatusCode initialize();

  /// Initialize the tool instance.
  virtual StatusCode finalize();

public:

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public:

  /** Returns the correctly configured and name instance of the
   *  Particles and Vertices unpacker, for the given path
   */
  virtual Gaudi::Utils::TypeNameString algorithmForPath(const std::string &path);

public:

  /** Instruct the DataOnDemandSvc to create DataObjects for the
   *  intermediate levels of a path we can handle.
   */
  virtual std::string nodeTypeForPath(const std::string &path);

private:

  /// Get the Stream name from a data path
  std::string streamName( const std::string & path ) const;

  /// Get the stream root from a data path
  inline std::string streamRoot( const std::string & path ) const
  {
    return "/Event/" + streamName(path);
  }

  /// Load the packed data and update the mappings of paths to Node Type
  void updateNodeTypeMap( const std::string & path );

  /// Add a path to the node type mappings
  void addPath( const std::string & path );

  /// Check if a given path is in the list of data locations created
  inline bool pathIsHandled( const std::string & path ) const
  {
    // See if we have an entry for this path
    NodeTypeMap::const_iterator it = m_nodeTypeMap.find( fixPath(path) );
    return ( it != m_nodeTypeMap.end() );
  }

  /// Make sure a path starts with /Event/
  inline std::string fixPath( const std::string & path ) const
  {
    std::string tmp = path;
    if ( tmp.substr(0,7) != "/Event/" ) { tmp = "/Event/"+tmp; }
    return tmp;
  }

private:

  /// Reference to the JobOptionsSvc.
  /// It is needed to configure the Algorithm instances.
  SmartIF<IJobOptionsSvc> m_jos;

  /// Mapping between TES path and node type
  typedef std::map<std::string,std::string> NodeTypeMap;
  NodeTypeMap m_nodeTypeMap;

  /// Map to say which stream roots have been processed each event
  std::map<std::string,bool> m_streamsDone;

  /// Unpacker class type
  std::string m_unpackerType;

  /// Outputlevel for unpackers created
  int m_unpackersOutputLevel;

  StandardPacker m_pack; ///< Standard Packer

};

#endif // SRC_ParticlesAndVerticesMapper_H
