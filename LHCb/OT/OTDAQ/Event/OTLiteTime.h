
//-----------------------------------------------------------------------------
/** @file OTLiteTime.h
 *
 *  Header file for track find class LHCb::OTLiteTime
 *
 *  CVS Log :-
 *  $Id: OTLiteTime.h,v 1.6 2008-05-29 10:53:06 smenzeme Exp $
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

  /** @class OTLiteTime OTLiteTime.h OTDAQ/OTLiteTime.h
   *  Simple class to hold an OT hit, including a 'calibrated' drift time. This
   *  is the 'light' version of an OTTime. (It is light because it is not a
   *  contained object.)
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class OTLiteTime
  {

  public:

    /// Default constructor does nothing
    OTLiteTime() : m_channelid(0), m_calibratedTime(0) {}
    
    /// Constructor from an OTChannelID
    OTLiteTime(LHCb::OTChannelID channelid, double calibratedtime) 
      : m_channelid(channelid), m_calibratedTime(calibratedtime) {}

    /// Access the OTChannelID for this OTLiteTime
    const LHCb::OTChannelID& channel() const { return m_channelid ; }

    /// Access to the calibrated time
    double calibratedTime() const { return m_calibratedTime ; }

  public:

    /// Print this OTLiteTime in a human readable way
    std::ostream& fillStream(std::ostream& s) const
    {
      return s << "[ channel=" << m_channelid << ", " << m_calibratedTime << "]" ;
    }

    /// Implement ostream << method
    friend inline std::ostream& operator << ( std::ostream& s,
                                              const OTLiteTime& time )
    {
      return time.fillStream(s);
    }

  private:
    LHCb::OTChannelID m_channelid ; ///< The OTChannelID
    double m_calibratedTime ;        ///< T0 corrected timet0 corrected; calibratedTime=propagationTime+driftTime 
  } ;

  /// Type for a container of OTLiteTime objects
  typedef std::vector<OTLiteTime> OTLiteTimeContainer ;

  /// Range object for OTLiteTimeContainer
  typedef LoKi::Range_<OTLiteTimeContainer> OTLiteTimeRange ;

}

#endif // OTDAQ_OTLITETIME_HH__
