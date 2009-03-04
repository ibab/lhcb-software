#ifndef PATALG_ITRACKSTATEINIT_H 
#define PATALG_ITRACKSTATEINIT_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/ParticleID.h"

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

  virtual StatusCode fit( LHCb::Track& track, bool celarStates = true) const = 0;
  virtual StatusCode initializeRefStates(LHCb::Track& track,
					 LHCb::ParticleID pid = 211 ) const = 0;
};
#endif
