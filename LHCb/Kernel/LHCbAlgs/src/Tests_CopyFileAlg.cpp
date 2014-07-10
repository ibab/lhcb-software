#include "GaudiAlg/GaudiAlgorithm.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace LHCbAlgsTests {
  /// Test algorithm that copies a file when executed.
  class CopyFileAlg: public GaudiAlgorithm {
  public:
    CopyFileAlg(const std::string& name, ISvcLocator* pSvcLocator):
      GaudiAlgorithm(name, pSvcLocator) {
      declareProperty("Source", m_source="");
      declareProperty("Destination", m_destination="");
    }

    StatusCode execute() {
      path src = m_source;
      path dst = m_destination;

      if (msgLevel(MSG::DEBUG))
        debug() << "Copying " << m_source << " -> " << m_destination << endmsg;

      copy_file(src, dst, copy_option::overwrite_if_exists);

      return StatusCode::SUCCESS;
    }

  private:
    std::string m_source, m_destination;
  };

  DECLARE_COMPONENT(CopyFileAlg)
}
