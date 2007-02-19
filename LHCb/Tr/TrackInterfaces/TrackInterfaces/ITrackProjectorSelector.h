// $Id: ITrackProjectorSelector.h,v 1.2 2007-02-19 12:38:15 cattanem Exp $
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
};

/// Static Interface Identification
static const InterfaceID IID_ITrackProjectorSelector( "ITrackProjectorSelector" , 1 , 0 );

/** @class ITrackProjectorSelector ITrackProjectorSelector.h TrackInterfaces/ITrackProjectorSelector.h
 *
 *  Interface class to select which Projector to use.
 *
 */
 
class ITrackProjectorSelector: virtual public IAlgTool{
public:
  static const InterfaceID& interfaceID() { return IID_ITrackProjectorSelector; }

  virtual ITrackProjector* projector(const LHCb::Measurement& m) const = 0;
};

#endif // TRACKINTERFACES_ITRACKPROJECTORSELECTOR_H
