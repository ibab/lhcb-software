#ifndef __TF_OTLITETIME_HH__
#define __TF_OTLITETIME_HH__

#include <vector>
#include "Kernel/OTChannelID.h"
#include "LoKi/Range.h"

namespace LHCb
{
  class OTLiteTime
  {
  public:
    OTLiteTime(OTChannelID channelid) : m_channelid(channelid) {} 
    const OTChannelID& channel() const { return m_channelid ; }
    float rawTime() const { return m_channelid.tdcTime()*m_nsPerTdc ; }
    static void setNsPerTdc(float c) { m_nsPerTdc = c ; }
  public:
    /// Print this OTLiteTime in a human readable way
    std::ostream& fillStream(std::ostream& s) const;
    /// Implement ostream << method
    friend inline std::ostream& operator << ( std::ostream& s,
                                              const OTLiteTime& time )
    {
      return time.fillStream(s);
    }
  private:
    OTChannelID m_channelid ;
    static float m_nsPerTdc ;
  } ;

  typedef std::vector<OTLiteTime> OTLiteTimeContainer ;
  typedef LoKi::Range_<OTLiteTimeContainer> OTLiteTimeRange ;

}

#endif
