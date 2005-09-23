#include "Online/TimeSensor.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

Online::TimeSensor::TimeSensor()
: Sensor( WT_FACILITY_TIMER1, "TimeSensor" )
{
}

Online::TimeSensor::~TimeSensor()  {
}

Online::TimeSensor& Online::TimeSensor::instance()  {
  static TimeSensor sensor;
  return sensor;
}
