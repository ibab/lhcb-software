// $Id: HltDataStore.h,v 1.3 2007-02-05 09:09:08 hernando Exp $
#ifndef HLTCOMMON_HLTDATASTORE_H 
#define HLTCOMMON_HLTDATASTORE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"
#include "HltBase/IHltDataStore.h"            // Interface


/** @class HltDataStore HltDataStore.h
 *  
 * functionality:
 *      store of collections of tracks and vertices by key string
 *      tool to share these collections between HltAlgorithms
 *      the pointers tracks and vertices are owned by an external container
 *           (i.e PatDataStore)
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-19
 */
class HltDataStore : public GaudiTool, virtual public IHltDataStore {
public:

  /// Standard constructor
  HltDataStore( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~HltDataStore( ); ///< Destructor

  //TODO
  // clear of the containers
  void clear() {}

  // getting the map of all collection of tracks by name  
  const MapTracks& tracks() const {return m_mapTracks;}

  // getting the map of all collection of tracks by name
  MapTracks& tracks() {return m_mapTracks;};

  // getting the collection of tracks with a given name
  const Hlt::TrackContainer& tracks(const std::string& name) const {
    if (m_mapTracks.find(name) == m_mapTracks.end())
      error() << " No  of tracks in HltDataStore named " 
              << name << endreq;
    return m_mapTracks.find(name)->second;
  }

  // getting the collection of tracks with a given name
  Hlt::TrackContainer& tracks(const std::string& name) {
    if (m_mapTracks.find(name) == m_mapTracks.end()) {
      info() << " creating track container " << name << endreq;
      m_mapTracks[name] = Hlt::TrackContainer();
      m_mapTracks[name].reserve(200);
      m_mapTracks[name].clear();  
    }
    return m_mapTracks.find(name)->second;
  }

  // getting the map of all collection of vertices by name  
  const MapVertices& vertices() const {return m_mapVertices;}

  // getting the map of all collection of vertices by name
  MapVertices& vertices() {return m_mapVertices;};

  // getting the collection of vertices with a given name
  const Hlt::VertexContainer& vertices(const std::string& name) const {
    if (m_mapVertices.find(name) == m_mapVertices.end())
      error() << " No  of vertices in HltDataStore named " 
              << name << endreq;
    return m_mapVertices.find(name)->second;
  }

  // getting the collection of vertices with a given name
  Hlt::VertexContainer& vertices(const std::string& name) {
    if (m_mapVertices.find(name) == m_mapVertices.end()) {
      info() << " creating vertex container " << name << endreq;
      m_mapVertices[name] = Hlt::VertexContainer();
      m_mapVertices[name].reserve(100);
      m_mapVertices[name].clear();
    } 
    return m_mapVertices.find(name)->second;
  }


protected:

  // map of collection of tracks by name
  MapTracks m_mapTracks;

  // map of collection of vertices by name
  MapVertices m_mapVertices;
  
};
#endif // HLTCOMMON_HLTDATASTORE_H
