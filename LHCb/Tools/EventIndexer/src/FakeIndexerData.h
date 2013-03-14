#ifndef SRC_FAKEINDEXERDATA_H
#define SRC_FAKEINDEXERDATA_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentSvc.h"

namespace EventIndexerTest {
/** @class FakeIndexerData FakeIndexerData.h src/FakeIndexerData.h
  *
  * Generate fake data to test the EventIndexer algorithm.
  *
  * @author Marco Clemencic
  * @date 14/03/2013
  */
class FakeIndexerData: public GaudiAlgorithm {
public:
  /// Standard constructor
  FakeIndexerData(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FakeIndexerData(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
protected:
private:

  SmartIF<IIncidentSvc> m_incSvc;
  long long m_eventNumber;
};

}

#endif // SRC_FAKEINDEXERDATA_H
