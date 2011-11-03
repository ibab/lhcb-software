#include "Framework/Timing.h"
#include "RTL/rtl.h"

#include <cmath>
#include <cstring>

using namespace Framework;
using namespace std;

/// Initializing constructor
Timing::Timing(long calls, double tim, double tim2) 
: m_calls(calls), m_time(tim), m_time2(tim2) {
}

/// Default constructor
Timing::Timing() :  m_calls(0), m_time(0e0), m_time2(0e0) {
}

/// Add Timing structures
Timing& Timing::operator+=(const Timing& t) {
  m_time  += t.m_time;
  m_time2 += t.m_time2;
  m_calls += t.m_calls;
  return *this;
}

/// Subtract Timing structures
Timing& Timing::operator-=(const Timing& t) {
  m_time  -= t.m_time;
  m_time2 -= t.m_time2;
  m_calls -= t.m_calls;
  return *this;
}

/// Get the current time stamp
struct timeval Timing::now() {
  struct timezone tz;
  struct timeval  tv;
  ::gettimeofday(&tv,&tz);
  return tv;
}

/// Get timing information in string form
string Timing::toString()  const {
  char text[128];
  ::snprintf(text,sizeof(text),"%15.2f +- %10.2f seconds  [%d calls]",mean(),sigma(),int(numCalls()));
  text[sizeof(text)-1]=0;
  return text;
}

/// Make standardized printout
void Timing::print(const char* name)  const {
  ::lib_rtl_output(LIB_RTL_ALWAYS," %-36s%s",name,toString().c_str());
}

/// Statistics: collect processing time and number of calls
void Timing::initialize() {
  m_time  = m_time2 = 0e0;
  m_calls = 0;
}

/// Statistics: collect processing time and number of calls
void Timing::collect(const timeval& start, const timeval& stop) {
  long sec  = stop.tv_sec  - start.tv_sec;
  long usec = stop.tv_usec - start.tv_usec;
  double t  = double(sec*1000000) + double(usec);
  ++m_calls;
  m_time  += t;
  m_time2 += t*t;
}

/// Access mean time spent in one call
double Timing::mean()  const   {
  return m_calls > 0 ? m_time/double(m_calls) : 0e0;
}

/// Access variance of mean time spent in one call
double Timing::sigma()  const {
  return m_calls > 0 ? std::sqrt(std::fabs(m_time2 - m_time*m_time/double(m_calls))/double(m_calls)) : 0e0;
}
