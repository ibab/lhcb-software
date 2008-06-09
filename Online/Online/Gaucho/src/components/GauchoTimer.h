#ifndef GAUCHO_GAUCHOTIMER_H
#define GAUCHO_GAUCHOTIMER_H 1

#include "GaudiKernel/Timing.h"

/** Trivial class to measure the elapsed time between start and stop
 * @author Juan Martin Otalora Goicochea
 */

class GauchoTimer {

private:
  longlong m_startCurrentTime;
  longlong m_startCpuTime;

  longlong m_stopCurrentTime;
  longlong m_stopCpuTime;

  double m_elapsedCurrentTime;
  double m_elapsedCpuTime;

public:
  GauchoTimer() {
    m_elapsedCurrentTime = 0.;
    m_elapsedCpuTime  = 0.;
  }

  ~GauchoTimer() {};

  void start () {
    m_startCurrentTime = System::currentTime( System::milliSec );
    m_startCpuTime   = System::cpuTime( System::milliSec );
  }
  
  double stop () {
    m_stopCurrentTime = System::currentTime( System::milliSec );
    m_stopCpuTime   = System::cpuTime( System::milliSec );
  
    m_elapsedCurrentTime  =  double(m_stopCurrentTime - m_startCurrentTime );
    m_elapsedCpuTime  =  double(m_stopCpuTime - m_startCpuTime );
    
    return m_elapsedCurrentTime;
  }
  
  double elapsedCurrentTime() const { return m_elapsedCurrentTime; }
  double elapsedCpuTime() const { return m_elapsedCpuTime; }
  
  static longlong currentTime() {return System::currentTime( System::milliSec );}
  static longlong cpuTime() {return System::cpuTime( System::milliSec );}

};

#endif // GAUCHO_GAUCHOTIMER_H
