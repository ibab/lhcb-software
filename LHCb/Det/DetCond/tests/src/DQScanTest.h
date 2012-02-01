#ifndef SRC_DQSCANTEST_H
#define SRC_DQSCANTEST_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IDQScanner.h"

#include "DetCond/ICondDBReader.h"

namespace DetCondTest {

/** @class DQScanTest DQScanTest.h src/DQScanTest.h
  *
  * Algorithm to test the behavior of an IDQScanner implementation.
  *
  * @author Marco Clemencic
  * @date 31/01/2012
  */
class DQScanTest: public GaudiAlgorithm {
public:
  typedef std::pair<unsigned int, unsigned int> IOVPropType;
  typedef std::vector<IOVPropType> IOVListPropType;

  /// Standard constructor
  DQScanTest(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DQScanTest(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
protected:
private:

  /// Type/name of the IDQScanner instance.
  /// (property DQScanner)
  std::string m_DQScannerName;

  /// List of IOVs (with time specified in seconds) to try to retrieve (property).
  IOVListPropType m_iovsProp;

  /// List of IOVs to try to retrieve.
  ICondDBReader::IOVList m_iovs;

  /// Pointer to the IDQScanner instance.
  IDQScanner *m_scanner;
};

}

#endif // SRC_DQSCANTEST_H
