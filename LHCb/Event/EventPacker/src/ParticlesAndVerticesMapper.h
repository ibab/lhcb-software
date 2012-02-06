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

class IJobOptionsSvc;

/** Tool for automatic conversions in the transient store.
 *
 *  Bespoke tool for the UnpackParticlesAndVertices unpacker
 *  
 * @author Chris Jones
 * @date 06/02/2012
 */
class ParticlesAndVerticesMapper: public extends3< GaudiTool, 
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

  virtual Gaudi::Utils::TypeNameString algorithmForPath(const std::string &path);

public:

  virtual std::string nodeTypeForPath(const std::string &path);

private:

  /// Get the stream root from a data path
  std::string streamRoot( const std::string & path ) const;

  /// Load the packed data and update the mappings of paths to Node Type
  void updateNodeTypeMap( const std::string & path );

  /// Add a path to the node type mappings
  void addPath( const std::string & path );

  /// Check if a given path is in the list of data locations created
  bool pathIsHandled( const std::string & path ) const;

private:

  /// Reference to the JobOptionsSvc.
  /// It is needed to configure the Algorithm instances.
  SmartIF<IJobOptionsSvc> m_jos;

  /// Mapping between TES path and node type
  typedef std::map<std::string,std::string> NodeTypeMap;
  NodeTypeMap m_nodeTypeMap;

  /// Map to say which stream roots have been processed each event
  std::map<std::string,bool> m_streamsDone;

};

#endif // SRC_ParticlesAndVerticesMapper_H
