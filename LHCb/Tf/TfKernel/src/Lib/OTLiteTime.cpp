#include "TfKernel/OTLiteTime.h"

namespace LHCb 
{
  float OTLiteTime::m_nsPerTdc = 1 ;

  std::ostream& OTLiteTime::fillStream(std::ostream& s) const
  {
    return s << "OTLiteTime { " << channel() << " nsPerTdc=" << m_nsPerTdc << " }";
  }

}
