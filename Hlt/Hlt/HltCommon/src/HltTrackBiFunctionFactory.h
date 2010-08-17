// $Id: HltTrackBiFunctionFactory.h,v 1.4 2010-08-17 08:47:19 graven Exp $
#ifndef HLTCOMMON_HLTTRACKBIFUNCTIONFACTORY_H 
#define HLTCOMMON_HLTTRACKBIFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "HltBase/IBiFunctionFactory.h" 
#include "HltTypes.h"
#include "HltBase/HltBaseTool.h"

/** @class HltFunctionFactory HltFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltTrackBiFunctionFactory : public HltBaseTool, public virtual IBiFunctionFactory<LHCb::Track, LHCb::Track> {
public: 
  /// Standard constructor
  HltTrackBiFunctionFactory( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~HltTrackBiFunctionFactory( ); ///< Destructor

  virtual bool command(const std::string& command,
                       const std::string& value = "");
  
  
  Hlt::TrackBiFunction* function(const std::string& name);
  
  Hlt::TrackBiFilter* filter(const std::string& mode);

};
#endif // HLTFUNCTIONFACTORY_H
