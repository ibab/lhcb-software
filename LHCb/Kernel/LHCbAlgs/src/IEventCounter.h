// $Id: IEventCounter.h,v 1.1 2007-03-02 08:49:26 cattanem Exp $
#ifndef IEVENTCOUNTER_H 
#define IEVENTCOUNTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IEventCounter ( "IEventCounter", 1, 0 );

/** @class IEventCounter IEventCounter.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-18
 */
class IEventCounter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IEventCounter; }

  /// Standard constructor
  virtual long getEventCounter() = 0;   ///< Get Event Counter

  virtual void incrementEventCounter() = 0;   ///< Increment Event Counter
  
};
#endif // IEVENTCOUNTER_H
