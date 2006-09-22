// $Id: HltDataStore.h,v 1.1 2006-09-22 11:36:07 hernando Exp $
#ifndef HLTCOMMON_HLTDATASTORE_H 
#define HLTCOMMON_HLTDATASTORE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"
#include "HltCommon/IHltDataStore.h"            // Interface


/** @class HltDataStore HltDataStore.h HltCommon/HltDataStore.h
 *  
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
  void clear() {}
  
  const MapTracks& tracks() const {return m_mapTracks;}

  MapTracks& tracks() {return m_mapTracks;};

  const Hlt::TrackContainer& tracks(const std::string& name) const {
    if (m_mapTracks.find(name) == m_mapTracks.end())
      error() << " No  of tracks in HltDataStore named " 
              << name << endreq;
    return m_mapTracks.find(name)->second;
  }

  Hlt::TrackContainer& tracks(const std::string& name) {
    if (m_mapTracks.find(name) == m_mapTracks.end()) {
      info() << " creating track container " << name << endreq;
      m_mapTracks[name] = Hlt::TrackContainer();
      m_mapTracks[name].reserve(200);
      m_mapTracks[name].clear();  
    }
    return m_mapTracks.find(name)->second;
  }

  
  const MapVertices& vertices() const {return m_mapVertices;}

  MapVertices& vertices() {return m_mapVertices;};

  const Hlt::VertexContainer& vertices(const std::string& name) const {
    if (m_mapVertices.find(name) == m_mapVertices.end())
      error() << " No  of vertices in HltDataStore named " 
              << name << endreq;
    return m_mapVertices.find(name)->second;
  }

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
  
  MapTracks m_mapTracks;

  MapVertices m_mapVertices;
  
};
#endif // HLTCOMMON_HLTDATASTORE_H
