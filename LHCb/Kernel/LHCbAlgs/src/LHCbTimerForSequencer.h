// $Id: LHCbTimerForSequencer.h,v 1.5 2004/11/25 13:26:26 mato Exp $
#ifndef LHCBTIMERFORSEQUENCER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Timing.h"

/** Auxilliary class. Measure the time between start and stop, and compute
 *  average, min and max. In fact, measure the cpu time, and the elapsed time
 *  but givesmin/max only for elapsed.
 *
 * @author O.Callot
 */

class LHCbTimerForSequencer
{

public:

  /** Constructor. Specify the name, for later printing. */
  LHCbTimerForSequencer( const std::string& name,
                     const unsigned int size,
                     const double factor )
    : m_name     ( name ),
      m_size     ( size ),
      m_factor   ( factor ),
      m_startClock ( 0LL  ),
      m_startCpu   ( 0LL  ),
      m_num      ( 0LL  ),
      m_lastTime ( 0.   ),
      m_lastCpu  ( 0.   ),
      m_min      ( 0.   ),
      m_max      ( 0.   ),
      m_sum      ( 0.   ),
      m_sumCpu   ( 0.   )
  { }

  ~LHCbTimerForSequencer() {}

  /** Start a time measurement */
  inline void start ()
  {
    m_startClock = System::currentTime( System::microSec );
    m_startCpu   = System::cpuTime    ( System::microSec );
  }

  /** Stop time measurement and return the last elapsed time.
      @return Measured time in ms
  */
  double stop();

  /** returns the name **/
  inline const std::string& name() const { return m_name; }

  /** returns the last measured time **/
  inline double lastTime()   const { return m_lastTime; }

  /** returns the last measured time **/
  inline double lastCpu()   const { return m_lastCpu; }

  /** returns the total elapsed time */
  inline double elapsedTotal() const { return m_sum; }

  /** returns the total cpu time */
  inline double cpuTotal() const { return m_sumCpu; }

  /** Returns the number run count*/
  inline double count() const { return (double)m_num; }

  /** Returns the min*/
  inline double min() const { return (double)m_min; }

  /** Returns the max*/
  inline double max() const { return (double)m_max; }

  /** Write measured time into the message stream. */
  MsgStream & fillStream(MsgStream & s) const;

  /** header matching the previous format **/
  static std::string header( std::string::size_type size );

private:

  std::string m_name;
  unsigned int m_size;
  double m_factor;
  long long m_startClock;
  long long m_startCpu;

  long long m_num;
  double m_lastTime;
  double m_lastCpu;
  double m_min;
  double m_max;
  double m_sum;
  double m_sumCpu;
};

inline MsgStream& operator<<(MsgStream& ms, const LHCbTimerForSequencer& count)
{
  return count.fillStream( ms );
}

#endif // LHCBTIMERFORSEQUENCER_H
