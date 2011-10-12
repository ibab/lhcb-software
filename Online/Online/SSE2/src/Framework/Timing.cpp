#include "Framework/Timing.h"
#include "RTL/rtl.h"

#include <cmath>

using namespace Framework;

/// Default constructor
Timing::Timing() :  m_calls(0), m_time(0e0), m_time2(0e0) {
}

/// Get the current time stamp
struct timeval Timing::now() {
  struct timezone tz;
  struct timeval  tv;
  ::gettimeofday(&tv,&tz);
  return tv;
}

/// Make standardized printout
void Timing::print(const char* name)  const {
  ::lib_rtl_output(LIB_RTL_ALWAYS," %-36s%15.2f +- %10.2f seconds  [%d calls]\n",
		   name, mean(), sigma(), int(numCalls()));
}

/// Statistics: collect processing time and number of calls
void Timing::initialize() {
  m_time  = 0e0;
  m_time2 = 0e0;
  m_calls = 0;
}

/// Statistics: collect processing time and number of calls
void Timing::collect(const timeval& start, const timeval& stop) {
  struct timeval n = stop;
  if (n.tv_usec < start.tv_usec ) {
    n.tv_sec   -= (start.tv_sec + 1);
    n.tv_usec  += (1000000 - start.tv_usec);
  }
  else {
    n.tv_sec  -= start.tv_sec;
    n.tv_usec -= start.tv_usec;
  }
  double t = double(n.tv_sec*1000000) + double(n.tv_usec);
  ++m_calls;
  m_time  += t;
  m_time2 += t*t;
}

/// Access mean time spent in one call
double Timing::mean()  const   {
  if ( m_calls > 0 ) {
    return m_time/double(m_calls);
  }
  return 0e0;
}

/// Access variance of mean time spent in one call
double Timing::sigma()  const {
  if ( m_calls > 0 ) {
    double v = std::sqrt(std::fabs(m_time2 - m_time*m_time/double(m_calls))/double(m_calls));
    return v;
  }
  return 0e0;
}
