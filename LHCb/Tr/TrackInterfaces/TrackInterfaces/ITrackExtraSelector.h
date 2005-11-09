// $Id: ITrackExtraSelector.h,v 1.1 2005-11-09 14:30:04 erodrigu Exp $
#ifndef TRACKINTERFACES_ITRACKEXTRASELECTOR_H
#define TRACKINTERFACES_ITRACKEXTRASELECTOR_H 1


// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class ITrackExtrapolator;

/// Static Interface Identification
static const InterfaceID IID_ITrackExtraSelector( "ITrackExtraSelector" , 1 , 0 );

/** @class ITrExtraSelector ITrExtraSelector "TrExtrapolator/ITrExtraSelector"
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
