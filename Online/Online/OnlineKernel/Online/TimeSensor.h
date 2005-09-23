#ifndef ONLINE_TIMESENSOR_H
#define ONLINE_TIMESENSOR_H

#include "Online/Sensor.h"

namespace Online {
 
  class TimeSensor : public Sensor {
  protected:
    TimeSensor();
    virtual ~TimeSensor();
  public:
    TimeSensor& instance();
  };

}
#endif // ONLINE_TIMESENSOR_H
