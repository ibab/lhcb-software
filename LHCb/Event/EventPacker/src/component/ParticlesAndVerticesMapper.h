#ifndef SRC_ParticlesAndVerticesMapper_H
#define SRC_ParticlesAndVerticesMapper_H 1

#include <string>
#include <map>

// base class
#include "MapperToolBase.h"

// Implemented interfaces
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Event model
#include "Event/PackedTrack.h"
#include "Event/PackedProtoParticle.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedParticle.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"
#include "Event/PackedFlavourTag.h"
#include "Event/PackedPartToRelatedInfoRelation.h"

#include "Event/StandardPacker.h"

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
class ParticlesAndVerticesMapper : public extends1< MapperToolBase,
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

private:

  /// Mapping between TES path and node type
  typedef std::map<std::string,std::string> NodeTypeMap;
  NodeTypeMap m_nodeTypeMap;

  /// Map to say which stream roots have been processed each event
  std::map<std::string,bool> m_streamsDone;

  StandardPacker m_pack; ///< Standard Packer

  /// Unpacker class type
  std::string m_unpackerType;
  
  /// Outputlevel for unpackers created
  int m_unpackersOutputLevel;

};

#endif // SRC_ParticlesAndVerticesMapper_H
