#include "Event/OTLiteTime.h"

namespace LHCb
{
  float OTLiteTime::m_nsPerTdcCount = 1 ;

  std::ostream& OTLiteTime::fillStream(std::ostream& s) const
  {
    return s << "OTLiteTime { " << channel() << " nsPerTdcCount=" << m_nsPerTdcCount << " }";
  }

}
