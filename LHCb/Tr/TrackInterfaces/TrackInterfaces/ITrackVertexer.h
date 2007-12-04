// $Id: ITrackVertexer.h,v 1.1 2007-12-04 09:47:34 wouter Exp $
#ifndef TRACKINTERFACES_ITRACKVERTEXER_H 
#define TRACKINTERFACES_ITRACKVERTEXER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class TwoProngVertex ;
  class Track ;
  class State ;
}

static const InterfaceID IID_ITrackVertexer ( "ITrackVertexer", 1, 0 );

/** @class ITrackVertexer ITrackVertexer.h TrackInterfaces/ITrackVertexer.h
 *  
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-11-07
 */
class ITrackVertexer : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackVertexer; }
  
  virtual LHCb::TwoProngVertex* fit(const LHCb::State& stateA, const LHCb::State& stateB) const = 0 ;
};
#endif // TRACKINTERFACES_ITRACKVERTEXER_H
