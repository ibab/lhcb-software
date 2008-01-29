/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  UPI BUFFER MANAGER MONITOR                           */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include "MBM/Monitor.h"
#include "UPI/UpiSensor.h"
#include "UPI/UpiDisplay.h"

static const int TERM_HEIGHT    =  65;
static const int TERM_WIDTH     = 126;

extern "C" int run_mbm_upi(int argc , char** argv) {
  UpiSensor::instance();
  MBM::Monitor mon(argc, argv, new UPI::UpiDisplay(TERM_WIDTH,TERM_HEIGHT));
  return mon.monitor();
}

extern "C" MBM::Monitor* mbm_upi(int argc , char** argv) {
  MBM::Monitor* mon = new MBM::Monitor(argc, argv, new UPI::UpiDisplay(TERM_WIDTH,TERM_HEIGHT));
  return mon;
}
