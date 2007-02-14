// $Id: IHltDataStore.h,v 1.3 2007-02-14 17:23:51 cattanem Exp $
#ifndef HLTBASE_IHLTDATASTORE_H 
#define HLTBASE_IHLTDATASTORE_H 1

// Include files
// from STL
#include <string>
#include <map>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "HltBase/HltContainers.h"

static const InterfaceID IID_IHltDataStore ( "IHltDataStore", 1, 0 );

/** @class IHltDataStore IHltDataStore.h HltBase/IHltDataStore.h
 *  
 *  functionality:
 *        A tool to share collections of tracks/vertices between Hlt algorithms
 *        It containes collections of tracks and vertices
 *                     these collections are accesible via a name
 *                     collections are created by Hlt algorithms
 *
 *  Note: Tracks and vertices are not owner by the collections
 *        but by an external container (i.e PatDataStore)
 *        we call them logical collections (or conainers)
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-20
 */
class IHltDataStore : virtual public IAlgTool {

public:

  // typdef for tracks and vertices containers
  typedef std::map< std::string, Hlt::TrackContainer > MapTracks;
  typedef std::map< std::string, Hlt::VertexContainer > MapVertices;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltDataStore; }

  // clear of the containers
  virtual void clear() = 0;

  // getting the map of all collection of tracks by name
  virtual const MapTracks& tracks() const = 0;

  // getting the map of all collection of tracks by name
  virtual MapTracks& tracks() = 0;

  // getting the collection of tracks with a given name
  virtual const Hlt::TrackContainer& tracks(const std::string& name) const = 0;
  
  // getting the collection of tracks with a given name
  virtual Hlt::TrackContainer& tracks(const std::string& name) = 0;

  // getting the map of all collection of vertices by name
  virtual const MapVertices& vertices() const = 0;
  
  // getting the map of all collection of vertices by name
  virtual MapVertices& vertices() = 0;

  // getting the collection of vertices with a given name
  virtual const Hlt::VertexContainer& vertices(const std::string& name) const = 0;
  // getting the collection of vertices with a given name
  virtual Hlt::VertexContainer& vertices(const std::string& name) = 0;
  
};
#endif // HLTBASE_IHLTDATASTORE_H
