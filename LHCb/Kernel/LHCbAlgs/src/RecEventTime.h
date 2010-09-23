#ifndef RECEVENTTIME_H
#define RECEVENTTIME_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"            // Interface
/** Simple implementation of IEventTimeDecoder that takes the event
 *  time from LHCb::RecHeader.
 *
 *  @author Marco Clemencic
 *  @date   2010-09-23
 */
class RecEventTime: public extends1<GaudiTool, IEventTimeDecoder> {
public:
  /// Standard constructor
  RecEventTime(const std::string& type, const std::string& name,
               const IInterface* parent);

  virtual ~RecEventTime(); ///< Destructor

  // --- implementation of IEventTimeDecoder ---
  /// Retrieve the event time from RecHeader (if present).
  /// @return The time of current event.
  virtual Gaudi::Time getTime() const;

private:
  /// Location of the RecHeader in the transient store.
  std::string m_recHeaderLoc;
};

#endif /* RECEVENTTIME_H */
