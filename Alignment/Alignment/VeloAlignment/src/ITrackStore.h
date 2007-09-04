#ifndef VELOALIGNMENT_ITRACKSTORE_H 
#define VELOALIGNMENT_ITRACKSTORE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Track.h"
#include "VeloTrack.h"

//from DetDesc
#include "VeloDet/DeVelo.h"

/** @class ITrackStore ITrackStore.h VeloAlignment/ITrackStore.h
 * 
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */

//--------------------------------------------------------------
// This tool is used to create a track store for Velo Alignment
//--------------------------------------------------------------

static const InterfaceID IID_ITrackStore ( "ITrackStore", 1, 0 );

class ITrackStore : virtual public IAlgTool {
public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackStore; }

  virtual StatusCode TransformTrack(LHCb::Track* ftrack, VeloTrack& atrack, double Map_VELO[]) = 0;
};
#endif // VELOALIGNMENT_ITRACKSTORE_H
