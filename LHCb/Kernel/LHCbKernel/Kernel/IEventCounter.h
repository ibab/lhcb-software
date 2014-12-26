#ifndef IEVENTCOUNTER_H
#define IEVENTCOUNTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IEventCounter IEventCounter.h
 *
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-18
 *  @author Marco CLEMENCIC
 *  @date   2012-06-12
 */
class IEventCounter : virtual public IAlgTool {
public:
  /// InterfaceID
  DeclareInterfaceID(IEventCounter, 2, 0);
  virtual ~IEventCounter() {}

  virtual long getEventCounter() const = 0;   ///< Get Event Counter

  virtual void incrementEventCounter() = 0;   ///< Increment Event Counter

  /// Set the event counter overriding the current value.
  virtual void setEventCounter(long n) = 0;
};
#endif // IEVENTCOUNTER_H
