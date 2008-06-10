#ifndef SLEEP_H_
#define SLEEP_H_

#include "boost/thread/xtime.hpp"
#include "boost/thread/thread.hpp"

namespace {
  /// Simple sleep function taken from COOL.
  /// @author Marco Clemencic
  void Sleep(int sec){
    using namespace boost;
    xtime t;
    if ( xtime_get(&t,TIME_UTC) == TIME_UTC ) {
      t.sec += sec;
      thread::sleep(t);
    }
    // TODO: (MCl) do something if cennot get the time.
  }

  /// Small variation on the sleep function for nanosecons sleep.
  /// @author Marco Clemencic
  void NanoSleep(long long nsec){
    using namespace boost;
    xtime t;
    if ( xtime_get(&t,TIME_UTC) == TIME_UTC ) {
      t.sec  += nsec / 1000000000;
      t.nsec += nsec % 1000000000;
      thread::sleep(t);
    }
    // TODO: (MCl) do something if cennot get the time.
  }

}

#endif /*SLEEP_H_*/
