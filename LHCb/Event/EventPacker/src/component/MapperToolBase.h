#ifndef MAPPERTOOLBASE_H
#define MAPPERTOOLBASE_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// interfaces
#include "GaudiKernel/IDODAlgMapper.h"
#include "GaudiKernel/IDODNodeMapper.h"

// Helpers for printing
#define ON_VERBOSE  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
#define ON_DEBUG    if (UNLIKELY(msgLevel(MSG::DEBUG)))
#define LOG_VERBOSE ON_VERBOSE verbose()
#define LOG_DEBUG   ON_DEBUG   debug()

/** @class MapperToolBase MapperToolBase.h
 *
 *  Base class for mapper tools
 *
 *  @author Chris Jones
 *  @date   2012-03-26
 */
class MapperToolBase : public extends2< GaudiTool,
                                        IDODAlgMapper,
                                        IDODNodeMapper >
{

public:

  /// Standard constructor
  MapperToolBase( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

  virtual ~MapperToolBase( ); ///< Destructor

  /// Initialize the tool instance.
  virtual StatusCode initialize();

  /// Initialize the tool instance.
  virtual StatusCode finalize();

protected:

  /// Access the Job Option Service
  SmartIF<IJobOptionsSvc>& joSvc() const;

  /// Get the Stream name from a data path
  std::string streamName( const std::string & path ) const;

  /// Get the stream root from a data path
  inline std::string streamRoot( const std::string & path ) const
  {
    return "/Event/" + streamName(path);
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
  mutable SmartIF<IJobOptionsSvc> m_jos;

};

#endif // MAPPERTOOLBASE_H
