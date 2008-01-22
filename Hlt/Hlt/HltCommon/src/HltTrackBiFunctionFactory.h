// $Id: HltTrackBiFunctionFactory.h,v 1.1 2008-01-22 09:56:37 hernando Exp $
#ifndef HLTCOMMON_HLTTRACKBIFUNCTIONFACTORY_H 
#define HLTCOMMON_HLTTRACKBIFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "HltBase/IBiFunctionFactory.h" 
#include "HltBase/HltBaseTool.h"

/** @class HltFunctionFactory HltFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltTrackBiFunctionFactory : public HltBaseTool, virtual public IBiFunctionFactory<LHCb::Track, LHCb::Track> {
public: 
  
  typedef IBiFunctionFactory<LHCb::Track,LHCb::Track> IBiTrackFactory;

  /// Standard constructor
  HltTrackBiFunctionFactory( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~HltTrackBiFunctionFactory( ); ///< Destructor

  StatusCode initialize();
  
  virtual bool command(const std::string& command,
                       const std::string& value = "");
  
  
  Hlt::TrackBiFunction* function(const std::string& name);
  
  Hlt::TrackBiFilter* filter(const std::string& mode);



};
#endif // HLTFUNCTIONFACTORY_H
