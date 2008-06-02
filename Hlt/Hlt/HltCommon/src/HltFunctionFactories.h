// $Id: HltFunctionFactories.h,v 1.2 2008-06-02 19:57:57 graven Exp $
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

  typedef Hlt::FunctionCreator<LHCb::Track>                   TCreator;
  typedef Hlt::TFunctionCreator<LHCb::Track,LHCb::Track>     TTCreator;
  typedef Hlt::TFunctionCreator<LHCb::Track,LHCb::RecVertex> TVCreator;
  

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

  typedef Hlt::FunctionCreator<LHCb::RecVertex>                   VCreator;
  typedef Hlt::TFunctionCreator<LHCb::RecVertex,LHCb::Track>     VTCreator;
  typedef Hlt::TFunctionCreator<LHCb::RecVertex,LHCb::RecVertex> VVCreator;
  

  /// Standard constructor
  HltVertexFunctionFactory( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);
  
  virtual ~HltVertexFunctionFactory( ); ///< Destructor
  
  virtual StatusCode initialize();
  

};

#endif // HLTFUNCTIONFACTORIES_H
