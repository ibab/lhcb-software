#include "Framework/EventContext.h"

using namespace Framework;

/// Default constructor
EventContext::EventContext(long id) : _id(id), _start(now())  {
}

/// Initializing constructor
EventContext::EventContext(long id,const IOMask& m) : _id(id), _start(now()) {
  _ioMask.mask_or(m);
}

/// Get the current time stamp
struct timeval EventContext::now() const {
  struct timezone tz;
  struct timeval  tv;
  ::gettimeofday(&tv,&tz);
  return tv;
}

struct timeval EventContext::execTime() const {
  struct timeval n = now();
  if (n.tv_usec < _start.tv_usec ) {
    n.tv_sec   -= (_start.tv_sec + 1);
    n.tv_usec  += (1000000 - _start.tv_usec);
  }
  else {
    n.tv_sec  -= _start.tv_sec;
    n.tv_usec -= _start.tv_usec;
  }
  return n;
}


