// $Id: ITrackExtraSelector.h,v 1.4 2010-04-07 20:58:29 wouter Exp $
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

  virtual const ITrackExtrapolator* select( const double zStart,
					    const double zEnd ) const = 0;
  
};

#endif // TRACKINTERFACES_ITRACKEXTRASELECTOR_H
