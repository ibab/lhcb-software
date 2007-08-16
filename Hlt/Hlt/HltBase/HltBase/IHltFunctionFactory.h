// $Id: IHltFunctionFactory.h,v 1.5 2007-08-16 09:39:21 hernando Exp $
#ifndef HLTBASE_IHLTFUNCTIONFACTORY_H 
#define HLTBASE_IHLTFUNCTIONFACTORY_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/HltEnums.h"
#include "HltBase/HltTypes.h"

static const InterfaceID IID_IHltFunctionFactory ( "IHltFunctionFactory", 1, 0 );

/** @class IHltFunctionFactory IHltFunctionFactory.h HltBase/IHltFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class IHltFunctionFactory : virtual public IAlgTool {
public:  

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltFunctionFactory; }

  // funtions/filters works with the info
  // functions check if the info is stored, otherwise it is calculated
  virtual void setSmart(bool smart) = 0;

  virtual void setTracks(std::vector<LHCb::Track*>& tracks) = 0;

  virtual void setVertices(std::vector<LHCb::RecVertex*>& vertex) = 0;
  
  virtual Hlt::TrackFunction* trackFunction(const std::string& name) = 0;

  virtual Hlt::TrackFilter* trackFilter(const std::string& name) = 0;
  
  virtual Hlt::VertexFunction* vertexFunction(const std::string& name) = 0;
  
  virtual Hlt::VertexFilter* vertexFilter(const std::string& name) = 0;

  virtual Hlt::TrackBiFunction* trackBiFunction(const std::string& name) = 0;
  

};
#endif // HLTBASE_IHLTFUNCTIONFACTORY_H
