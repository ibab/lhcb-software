// $Id: HltFunctionFactories.h,v 1.3 2009-07-01 21:00:38 graven Exp $
#ifndef HLTFUNCTIONFACTORIES_H 
#define HLTFUNCTIONFACTORIES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltTFunctionFactory.h"            // Interface


/** @class HltFunctionFactories HltFunctionFactories.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-04
 */
class HltTrackFunctionFactory : public HltTFunctionFactory<LHCb::Track> {
public: 

  /// Standard constructor
  HltTrackFunctionFactory( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~HltTrackFunctionFactory( ); ///< Destructor
  
  virtual StatusCode initialize();
  

};

/** @class HltFunctionFactories HltFunctionFactories.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-04
 */
class HltVertexFunctionFactory : public HltTFunctionFactory<LHCb::RecVertex>{
public: 

  /// Standard constructor
  HltVertexFunctionFactory( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);
  
  virtual ~HltVertexFunctionFactory( ); ///< Destructor
  
  virtual StatusCode initialize();
  

};

#endif // HLTFUNCTIONFACTORIES_H
