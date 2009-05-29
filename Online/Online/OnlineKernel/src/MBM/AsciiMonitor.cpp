/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII BUFFER MANAGER DISPLAY                         */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdlib>
#include "MBM/Monitor.h"
#include "CPP/AsciiDisplay.h"
#include "SCR/ScrDisplay.h"

extern "C" int mbm_scr(int argc , char** argv) {
  MBM::Monitor mon(argc, argv, new ScrDisplay());
  return mon.monitor();
}

