
//-----------------------------------------------------------------------------
/** @file OTLiteTime.h
 *
 *  Header file for track find class LHCb::OTLiteTime
 *
 *  CVS Log :-
 *  $Id: OTLiteTime.h,v 1.1 2007-09-07 13:19:20 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef OTDAQ_OTLITETIME_HH__
#define OTDAQ_OTLITETIME_HH__ 1

#include <vector>
#include "Kernel/OTChannelID.h"
#include "LoKi/Range.h"

namespace LHCb
{

  /** @class OTLiteTime OTDAQ/OTLiteTime.h
   *  Simple class to extend the OTChannelID with calibrated raw time information
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class OTLiteTime
  {

  public:

    /// Constructor from an OTChannelID
    OTLiteTime(LHCb::OTChannelID channelid) : m_channelid(channelid) {}

    /// Access the OTChannelID for this OTLiteTime
    const LHCb::OTChannelID& channel() const { return m_channelid ; }

    /// Access the raw OT time
    float rawTime() const { return m_channelid.tdcTime()*m_nsPerTdcCount ; }

    /// Static method to set the ns per TDS conversion factor
    static void setNsPerTdcCount(const float c) { m_nsPerTdcCount = c ; }

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

    LHCb::OTChannelID m_channelid ;       ///< The OTChannelID
    static float m_nsPerTdcCount ;  ///< The ns per TDC conversion factor

  } ;

  /// Type for a container of OTLiteTime objects
  typedef std::vector<OTLiteTime> OTLiteTimeContainer ;

  /// Range object for OTLiteTimeContainer
  typedef LoKi::Range_<OTLiteTimeContainer> OTLiteTimeRange ;

}

#endif // OTDAQ_OTLITETIME_HH__
