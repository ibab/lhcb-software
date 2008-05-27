// $Id: ITrackVertexer.h,v 1.2 2008-05-27 10:51:12 wouter Exp $
#ifndef TRACKINTERFACES_ITRACKVERTEXER_H 
#define TRACKINTERFACES_ITRACKVERTEXER_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class TwoProngVertex ;
  class Track ;
  class State ;
  class RecVertex ;
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
  typedef std::vector<const LHCb::State*> StateContainer ;
  typedef std::vector<const LHCb::Track*> TrackContainer ;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackVertexer; }
  
  virtual LHCb::TwoProngVertex* fit(const LHCb::State& stateA, const LHCb::State& stateB) const = 0 ;
  virtual LHCb::RecVertex* fit(const StateContainer& states) const = 0 ;
  virtual LHCb::RecVertex* fit(const TrackContainer& tracks) const = 0 ;
};
#endif // TRACKINTERFACES_ITRACKVERTEXER_H
