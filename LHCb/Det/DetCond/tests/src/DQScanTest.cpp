// Local custom parsers must be defined very early in the file.
#include "GaudiKernel/Parsers.h"
#include "GaudiKernel/ParsersFactory.h"

#include "DetCond/ICondDBReader.h"

#include "boost/foreach.hpp"

namespace Gaudi {
  namespace Parsers {
    StatusCode parse(ICondDBReader::IOVList& result, const std::string& input) {
      typedef std::pair<unsigned long long, unsigned long long> iov_t;
      std::vector<iov_t> temp;
      result.clear();
      StatusCode sc = Gaudi::Parsers::parse_(temp, input);
      if (sc.isSuccess()) {
        BOOST_FOREACH(iov_t &iov, temp) {
          result.push_back(ICondDBReader::IOV(Gaudi::Time(iov.first), Gaudi::Time(iov.second)));
        }
      }
      return sc;
    }
    StatusCode parse(std::vector<std::pair<unsigned long long, unsigned long long> >& result, const std::string& input) {
      return Gaudi::Parsers::parse_(result, input);
    }
  }
}
// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "DQScanTest.h"

namespace {
  std::ostream &operator<<(std::ostream& s, const ICondDBReader::IOV& iov) {
    return s << iov.since << " -> " << iov.until;
  }
}

namespace DetCondTest {
// ----------------------------------------------------------------------------
// Implementation file for class: DQScanTest
//
// 31/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(DQScanTest)

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
                  "List of IOVs to scan.");
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

  typedef std::pair<unsigned long long, unsigned long long> iov_t;
  m_iovs.clear();
  BOOST_FOREACH(iov_t &iov, m_iovsProp) {
    m_iovs.push_back(ICondDBReader::IOV(Gaudi::Time(iov.first), Gaudi::Time(iov.second)));
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
