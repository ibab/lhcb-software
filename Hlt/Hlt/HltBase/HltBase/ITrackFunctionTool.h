// $Id: ITrackFunctionTool.h,v 1.1 2007-12-06 16:07:36 hernando Exp $
#ifndef HLTBASE_ITRACKFUNCTIONTOOL_H 
#define HLTBASE_ITRACKFUNCTIONTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class Track;
};

static const InterfaceID IID_ITrackFunctionTool ( "ITrackFunctionTool", 1, 0 );

/** @class ITrackFunctionTool ITrackFunctionTool.h HltBase/ITrackFunctionTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-06
 */
class ITrackFunctionTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackFunctionTool; }

  virtual double function(const LHCb::Track& track) = 0;
  
private:

};
#endif // HLTBASE_ITRACKFUNCTION_H
