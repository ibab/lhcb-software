// $Id: IHltDataStore.h,v 1.1 2006-09-22 11:36:07 hernando Exp $
#ifndef HLTCOMMON_IHLTDATASTORE_H 
#define HLTCOMMON_IHLTDATASTORE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "HltCommon/HltContainers.h"

static const InterfaceID IID_IHltDataStore ( "IHltDataStore", 1, 0 );

/** @class IHltDataStore IHltDataStore.h HltCommon/IHltDataStore.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-20
 */
class IHltDataStore : virtual public IAlgTool {

public:


  typedef std::map< std::string, Hlt::TrackContainer > MapTracks;
  typedef std::map< std::string, Hlt::VertexContainer > MapVertices;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltDataStore; }
  
  virtual void clear() = 0;

  // getting tracks
  virtual const MapTracks& tracks() const = 0;
  
  virtual MapTracks& tracks() = 0;

  virtual const Hlt::TrackContainer& tracks(const std::string& name) const = 0;
  
  virtual Hlt::TrackContainer& tracks(const std::string& name) = 0;

  // getting vertices

  virtual const MapVertices& vertices() const = 0;
  
  virtual MapVertices& vertices() = 0;

  virtual const Hlt::VertexContainer& vertices(const std::string& name) const = 0;
  
  virtual Hlt::VertexContainer& vertices(const std::string& name) = 0;
  
};
#endif // HLTCOMMON_IHLTDATASTORE_H
