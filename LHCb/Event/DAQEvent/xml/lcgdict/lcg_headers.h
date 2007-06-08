// begin include files
#include <vector>
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/SmartRef.h"
#include "Event/ODIN.h"
#include "Event/RawEvent.h"
// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    SmartRef<LHCb::ODIN>               m_SmartRef_LHCb__ODIN;
    SmartRefVector<LHCb::ODIN>         m_SmartRefVector_LHCb__ODIN;
    std::vector<SmartRef<LHCb::ODIN> > m_std_vector_SmartRef_LHCb__ODIN;
    // end instantiations
  };
}

