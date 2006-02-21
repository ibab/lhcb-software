// $Id: ITrackSelector.h,v 1.3 2006-02-21 08:30:19 mneedham Exp $ 
#ifndef _ITrackSelector_H
#define _ITrackSelector_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_ITrackSelector( "ITrackSelector", 0, 0 );
 
/** @class ITrackSelector
 *
 *  interface for selecting tracks....
 *
 *  @author M.Needham
 *  @date   31/05/2004
 */

namespace LHCb{ 
 class Track;
};
  
class ITrackSelector: virtual public IAlgTool{
 
public:
                                                                                
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackSelector ; }
                                                                                                   
  /// the method
  virtual bool accept(const LHCb::Track& aTrack) const = 0;
                                                                                
};
                                                                                
#endif
