
//-----------------------------------------------------------------------------
/** @file OTLiteTime.h
 *
 *  Header file for track find class LHCb::OTLiteTime
 *
 *  CVS Log :-
 *  $Id: OTLiteTime.h,v 1.2 2007-08-16 14:37:26 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_OTLITETIME_HH__
#define TFKERNEL_OTLITETIME_HH__ 1

#include <vector>
#include "Kernel/OTChannelID.h"
#include "LoKi/Range.h"

namespace LHCb
{

  /** @class OTLiteTime TfKernel/OTLiteTime.h
   *  Simple class to extend the OTChannelID with calibrated raw time information
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class OTLiteTime
  {

  public:

    /// Constructor from an OTChannelID
    OTLiteTime(OTChannelID channelid) : m_channelid(channelid) {}

    /// Access the OTChannelID for this OTLiteTime
    const OTChannelID& channel() const { return m_channelid ; }

    /// Access the raw OT time
    float rawTime() const { return m_channelid.tdcTime()*m_nsPerTdc ; }

    /// Static method to set the ns per TDS conversion factor
    static void setNsPerTdc(const float c) { m_nsPerTdc = c ; }

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

    OTChannelID m_channelid ;  ///< The OTChannelID
    static float m_nsPerTdc ;  ///< The ns per TDC conversion factor

  } ;

  /// Type for a container of OTLiteTime objects
  typedef std::vector<OTLiteTime> OTLiteTimeContainer ;

  /// Range object for OTLiteTimeContainer
  typedef LoKi::Range_<OTLiteTimeContainer> OTLiteTimeRange ;

}

#endif // TFKERNEL_OTLITETIME_HH__
