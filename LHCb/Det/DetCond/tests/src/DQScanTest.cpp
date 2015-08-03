// Local custom parsers must be defined very early in the file.
#include "GaudiKernel/ParsersFactory.h"

namespace Gaudi {
  namespace Parsers {
    // Note: to be kept in sync with the property in DetCondTest::DQScanTest
    StatusCode parse(std::vector<std::pair<unsigned int, unsigned int> >& result, const std::string& input) {
      return Gaudi::Parsers::parse_(result, input);
    }
  }
}

// Include files

#include "DetCond/ICondDBReader.h"

// local
#include "DQScanTest.h"

#include "boost/foreach.hpp"

namespace {
  inline long long s2ns(unsigned int s) {
    return static_cast<long long>(s) * 1000000000;
  }
}

// ----------------------------------------------------------------------------
// Implementation file for class: DQScanTest
//
// 31/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DetCondTest, DQScanTest)

namespace DetCondTest {
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
DQScanTest::DQScanTest(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator), m_scanner(0)
{
  declareProperty("DQScanner",
                  m_DQScannerName = "CondDBDQScanner",
                  "Type/name of the IDQScanner instance to use.");
  declareProperty("IOVs",
                  m_iovsProp,
                  "List of IOVs (specified in seconds) to scan.");
}

// ============================================================================
// Destructor
// ============================================================================
DQScanTest::~DQScanTest() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode DQScanTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_scanner = tool<IDQScanner>(m_DQScannerName);

  m_iovs.clear();
  BOOST_FOREACH(IOVPropType &iov, m_iovsProp) {
    m_iovs.push_back(ICondDBReader::IOV(Gaudi::Time(s2ns(iov.first)), Gaudi::Time(s2ns(iov.second))));
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode DQScanTest::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  info() << "Execute" << endmsg;

  BOOST_FOREACH(ICondDBReader::IOV &iov, m_iovs) {
    always() << "Process IOV " << iov.since << " -> " << iov.until << endmsg;
    IDQFilter::FlagsType result = m_scanner->scan(iov.since, iov.until);
    always() << "-> Flags: " << result << endmsg;
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode DQScanTest::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if (release(m_scanner).isFailure()) {
    warning() << "Failed to release tool " << m_DQScannerName << endmsg;
  }
  m_scanner = 0;

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================
} // namespace DetCondTest
