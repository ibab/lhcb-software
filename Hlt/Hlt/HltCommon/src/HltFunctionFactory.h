// $Id: HltFunctionFactory.h,v 1.6 2007-12-04 16:58:58 hernando Exp $
#ifndef HLTFUNCTIONFACTORY_H 
#define HLTFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltFunctionFactory.h"            // Interface
#include "HltBase/HltTypes.h"
#include "HltBase/HltFunctions.h"

/** @class HltFunctionFactory HltFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltFunctionFactory : public GaudiTool, virtual public IHltFunctionFactory {
public: 
  /// Standard constructor
  HltFunctionFactory( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~HltFunctionFactory( ); ///< Destructor

  StatusCode initialize();

  virtual void setSmart(bool smart) 
  { m_smart = smart;}

  void setVertices(std::vector<LHCb::RecVertex*>& vertices) 
  {m_vertices = &vertices;}

  void setTracks(std::vector<LHCb::Track*>& tracks) 
  {m_tracks = &tracks;}
  
  Hlt::TrackFunction* trackFunction(const std::string& name);
  
  Hlt::TrackFilter* trackFilter(const std::string& mode);  
  
  Hlt::VertexFunction* vertexFunction(const std::string& name);
  
  Hlt::VertexFilter* vertexFilter(const std::string& name);

  Hlt::TrackBiFunction* trackBiFunction(const std::string& name);

protected:

  template <class T>
  Estd::filter<T>* makeFilter(const Estd::function<T>& fun,
                              const std::string& mode, 
                              float x0, float xf) {
    if (mode == "<") return (fun < x0).clone();
    else if (mode == ">") return (fun > x0).clone();
    else if (mode == "=") return (fun == x0).clone();
    else if (mode == "[]") return ((fun > x0) && (fun < xf)).clone();
    else if (mode == "||>") {
      Hlt::AbsFun<T> afun(fun);
      return (afun > x0).clone();
    } else if ( mode == "||<") {
      Hlt::AbsFun<T> afun(fun);
      return (afun < x0).clone();
    } else if (mode == "||[]") {
      Hlt::AbsFun<T> afun(fun);
      return ((afun > x0) && (afun < xf)).clone();
    } 
    fatal() << " not mode " << mode << endreq;
    return NULL;
  }

protected:


  std::string m_dataSummaryLocation;
  Hlt::Configuration* m_conf;
  
  bool m_smart;
  
  std::vector<LHCb::RecVertex*>* m_vertices;

  std::vector<LHCb::Track*>* m_tracks;

};
#endif // HLTFUNCTIONFACTORY_H
