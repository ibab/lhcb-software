// $Id: ITrackBiFunctionTool.h,v 1.1 2007-12-06 16:07:36 hernando Exp $
#ifndef HLTBASE_ITRACKBIFUNCTIONTOOL_H 
#define HLTBASE_ITRACKBIFUNCTIONTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
};

static const InterfaceID IID_ITrackBiFunctionTool ( "ITrackBiFunctionTool", 1, 0 );

/** @class ITrackBiFunctionTool ITrackBiFunctionTool.h HltBase/ITrackBiFunctionTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-06
 */
class ITrackBiFunctionTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackBiFunctionTool; }


  double function(const LHCb::Track& track1, const LHCb::Track& track2);
  

protected:

private:

};
#endif // HLTBASE_ITRACKBIFUNCTION_H
