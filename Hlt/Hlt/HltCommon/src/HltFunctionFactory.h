// $Id: HltFunctionFactory.h,v 1.9 2008-02-07 15:19:43 hernando Exp $
#ifndef HLTCOMMON_HLTFUNCTIONFACTORY_H 
#define HLTCOMMON_HLTFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// #include "HltBase/IPyHltFunctionFactory.h"            // Interface
#include "HltBase/IFunctionFactory.h"
#include "HltBase/IBiFunctionFactory.h"
#include "HltBase/HltTypes.h"

/** @class HltFunctionFactory HltFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-09
 */
class HltFunctionFactory : public GaudiTool {
  // virtual public IPyHltFunctionFactory {
public: 
  /// Standard constructor
  HltFunctionFactory( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);
  
  
  virtual ~HltFunctionFactory( ); ///< Destructor

  StatusCode initialize();
  
  Hlt::TrackFunction* trackFunction(const std::string& name);
  
  Hlt::TrackFilter* trackFilter(const std::string& name);
  
  Hlt::VertexFunction* vertexFunction(const std::string& name);
  
  Hlt::VertexFilter* vertexFilter(const std::string& name);

  Hlt::TrackBiFunction* trackBiFunction(const std::string& name);

protected:
  
  IFunctionFactory<LHCb::Track>* m_trackFactory;
  
  IFunctionFactory<LHCb::RecVertex>* m_vertexFactory;
  
  IBiFunctionFactory<LHCb::Track, LHCb::Track>* m_trackBiFactory;

private:

};
#endif // PYHLTFUNCTIONFACTORY_H
