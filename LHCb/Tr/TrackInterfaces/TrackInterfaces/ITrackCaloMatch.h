// $Id: ITrackCaloMatch.h,v 1.2 2006-02-21 14:49:09 mneedham Exp $ 
#ifndef _ITrackCaloMatch_H
#define _ITrackCaloMatch_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_ITrackCaloMatch( "ITrackCaloMatch", 0, 0 );
 
/** @class ITrackCaloMatch
 *
 *  interface for getting energy deposited in calos associated to track
 *  returned value is the appropriately weighted sum of ecal, hcal and preshower
 *  zero indicates no energy found 
 *
 *  @author M.Needham
 *  @date   31/05/2005
 */

namespace LHCb{
 class Track;
};
  
class ITrackCaloMatch: virtual public IAlgTool{
 
public:
                                                                                
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackCaloMatch ; }
                                                                                                   
  /// the method
  virtual double energy(const LHCb::Track& aTrack) const = 0;
                                                                                
};
                                                                                
#endif
