// $Id: ITrackManipulator.h,v 1.2 2007-05-24 06:55:42 mneedham Exp $ 
#ifndef _ITrackManipulator_H
#define _ITrackManipulator_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_ITrackManipulator( "ITrackManipulator", 0, 0 );
 
/** @class ITrackManipulator
 *
 *  interface for setting the track reference parameters from the measurement info
 *
 *  @author M.Needham
 *  @date   16/06/2006
 */

namespace LHCb{
 class Track;
};
  
class ITrackManipulator: virtual public IAlgTool{
 
public:
                                                                                
  /** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_ITrackManipulator ; }                                                                                                
  /** Add the reference information */
  virtual StatusCode execute(LHCb::Track& aTrack) const = 0;
                                                                                
};
                                                                                
#endif
