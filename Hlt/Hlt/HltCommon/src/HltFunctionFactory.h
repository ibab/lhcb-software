// $Id: HltFunctionFactory.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTFUNCTIONFACTORY_H 
#define HLTFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltFunctionFactory.h"            // Interface


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

  void setVertices(std::vector<LHCb::RecVertex*>& vertices) 
  {m_vertices = &vertices;}

  void setTracks(std::vector<LHCb::Track*>& tracks) 
  {m_tracks = &tracks;}
  
  Hlt::TrackFunction* trackFunction(int id);
  
  Hlt::TrackFilter* trackFilter(const Hlt::TrackFunction& fun, 
                                const std::string& mode,
                                double x0, double xf = 1e6);  
  
  Hlt::TrackFilter* trackFilter(int id, const std::string& mode, 
                           double x0, double xf = 1e6);  
  
  Hlt::VertexFunction* vertexFunction(int id);

  Hlt::VertexFilter* vertexFilter(const Hlt::VertexFunction& fun, 
                                  const std::string& mode,
                                  double x0, double xf = 1e6);

  Hlt::VertexFilter* vertexFilter(int id, const std::string& mode, 
                                  double x0, double xf = 1e6);

  virtual Hlt::TrackBiFunction* trackBiFunction(int id);

protected:
  
  bool m_smart;
  
  std::vector<LHCb::RecVertex*>* m_vertices;

  std::vector<LHCb::Track*>* m_tracks;

};
#endif // HLTFUNCTIONFACTORY_H
