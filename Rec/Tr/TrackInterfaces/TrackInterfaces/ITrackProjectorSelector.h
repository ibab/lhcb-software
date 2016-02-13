#ifndef TRACKINTERFACES_ITRACKPROJECTORSELECTOR_H
#define TRACKINTERFACES_ITRACKPROJECTORSELECTOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward Declaration - from TrackInterfaces
class ITrackProjector;
namespace LHCb {
   class Measurement;
}

/** @class ITrackProjectorSelector ITrackProjectorSelector.h TrackInterfaces/ITrackProjectorSelector.h
 *
 *  Interface class to select which Projector to use.
 *
 */
 
class ITrackProjectorSelector: public extend_interfaces<IAlgTool> {
public:
  DeclareInterfaceID( ITrackProjectorSelector , 2 , 0 );
  virtual ITrackProjector* projector(const LHCb::Measurement& m) const = 0;
};

#endif // TRACKINTERFACES_ITRACKPROJECTORSELECTOR_H
