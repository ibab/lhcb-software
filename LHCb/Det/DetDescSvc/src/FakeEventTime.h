#ifndef FAKEEVENTTIME_H 
#define FAKEEVENTTIME_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"            // Interface

class IDataProviderSvc;

/** @class FakeEventTime FakeEventTime.h
 *  
 *  Basic implementation of an EventTimeDecoder.
 *  It just provides fake event times.
 *
 *  @author Marco Clemencic
 *  @date   2006-09-21
 */
class FakeEventTime : public AlgTool, virtual public IEventTimeDecoder {
public: 
  /// Standard constructor
  FakeEventTime( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~FakeEventTime( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize();

  /// Finalization
  virtual StatusCode finalize();  

  // --- implementation of IEventTimeDecoder ---

  /// Return the time of current event.
  Gaudi::Time getTime() const;

private:
  
  // ---------- data members ----------

  /// First event time (when simluating them).
  /// Set by the option StartTime (default = 0)
  long long m_startTime;
  /// Event time increment (when simluating event times).
  /// Set by the option TimeStep (default = 0, which means no simulation)
  long long m_timeStep;

};
#endif // FAKEEVENTTIME_H
