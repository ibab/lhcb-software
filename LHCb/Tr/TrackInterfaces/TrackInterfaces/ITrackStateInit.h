#ifndef PATALG_ITRACKSTATEINIT_H 
#define PATALG_ITRACKSTATEINIT_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/ParticleID.h"

namespace LHCb {
  class Track;
}

/** @class ITrackStateInit ITrackStateInit.h 
 *
 * An interface to the TrackStateInitTool
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-02
 */

static const InterfaceID IID_ITrackStateInit ( "ITrackStateInit", 1, 0 );

class ITrackStateInit : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackStateInit; }
  /**
   * remove all states on the track, apply a simple fit only based on LHCbIds
   * and reinitalize a list of states. clearStates = false leaves the tracks
   * untouched for the moment , more features will be added here later.
   */   
  virtual StatusCode fit( LHCb::Track& track, bool clearStates = true) const = 0;
  /**
   * check if track states are initalized at a given list of z reference positions.
   * if list of state not complete, add additional states based on extrapolation of
   * states given on the track
   */
  virtual StatusCode initializeRefStates(LHCb::Track& track,
					 LHCb::ParticleID pid = LHCb::ParticleID(211) ) const = 0;
};
#endif
