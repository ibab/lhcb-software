#ifndef SRC_TESTEVENTCOUNTER_H
#define SRC_TESTEVENTCOUNTER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IEventCounter.h"

namespace LHCbAlgsTests {

/** @class TestEventCounter TestEventCounter.h src/TestEventCounter.h
 *
 *
 * @author Marco Clemencic
 * @date 11/06/2012
 */
class TestEventCounter: public GaudiAlgorithm {
public:
  /// Standard constructor
  TestEventCounter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TestEventCounter(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
private:
  std::string m_eventCounterName;
  IEventCounter* m_eventCounter;
};

} // namespace LHCbAlgsTests

#endif // SRC_TESTEVENTCOUNTER_H
