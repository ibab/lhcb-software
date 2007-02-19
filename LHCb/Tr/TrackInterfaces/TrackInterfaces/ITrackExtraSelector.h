// $Id: ITrackExtraSelector.h,v 1.3 2007-02-19 12:38:15 cattanem Exp $
#ifndef TRACKINTERFACES_ITRACKEXTRASELECTOR_H
#define TRACKINTERFACES_ITRACKEXTRASELECTOR_H 1


// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward Declaration - from TrackInterfaces
class ITrackExtrapolator;

/// Static Interface Identification
static const InterfaceID IID_ITrackExtraSelector( "ITrackExtraSelector" , 1 , 0 );

/** @class ITrackExtraSelector ITrackExtraSelector.h TrackInterfaces/ITrackExtraSelector
 *
 *  Interface class to select which extrapolator to use.
 *
 */
 
class ITrackExtraSelector: virtual public IAlgTool{
public:

  static const InterfaceID& interfaceID() {return IID_ITrackExtraSelector;}

  virtual ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const = 0;

};

#endif // TRACKINTERFACES_ITRACKEXTRASELECTOR_H
