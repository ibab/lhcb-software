#include "Event/OTLiteTime.h"

namespace LHCb
{
  float OTLiteTime::m_nsPerTdcCount = 1 ;

  std::ostream& OTLiteTime::fillStream(std::ostream& s) const
  {
    return s << "OTLiteTime { " << channel() << " nsPerTdcCount=" << m_nsPerTdcCount << " }";
  }

    /// Access the raw OT time
  float OTLiteTime::rawTime() const { return m_channelid.tdcTime()*m_nsPerTdcCount ; }

    /// Static method to set the ns per TDS conversion factor
  void OTLiteTime::setNsPerTdcCount(const float c) { m_nsPerTdcCount = c ; }

}
