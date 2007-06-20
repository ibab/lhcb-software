// $Id: IHltFunctionFactory.h,v 1.1 2007-06-20 12:08:40 hernando Exp $
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

  virtual void setTracks(std::vector<LHCb::Track*>& tracks) = 0;

  virtual void setVertices(std::vector<LHCb::RecVertex*>& vertex) = 0;
  
  virtual Hlt::TrackFunction* trackFunction(int id) = 0;

  virtual Hlt::TrackFilter* trackFilter(int id, const std::string& mode,  
                                   double x0, double xf = 1e6) = 0;

  virtual Hlt::TrackFilter* trackFilter(const Hlt::TrackFunction& fun, 
                                   const std::string& mode,
                                   double x0, double xf = 1e6) = 0;
  
  virtual Hlt::VertexFunction* vertexFunction(int id) = 0;

  virtual Hlt::VertexFilter* vertexFilter(int id, const std::string& mode,
                                          double x0, double xf = 1e6) = 0;

  virtual Hlt::VertexFilter* vertexFilter(const Hlt::VertexFunction& fun, 
                                          const std::string& mode,
                                          double x0, double xf = 1e6) = 0;

  virtual Hlt::TrackBiFunction* trackBiFunction(int id) = 0;

};
#endif // HLTBASE_IHLTFUNCTIONFACTORY_H
