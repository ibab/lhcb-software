/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII BUFFER MANAGER DISPLAY                         */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdlib>
#include "MBM/Monitor.h"
#include "CPP/AsciiDisplay.h"

extern "C" int mbm_mon(int argc , char** argv) {
  MBM::Monitor mon(argc, argv, new AsciiDisplay());
  return mon.monitor();
}

extern "C" int mbm_ascii(int /* argc */, char** /* argv */) {
  for ( unsigned char i=0; i < 255; ++i)  {
    printf("%3d  %03X   \"%c\"\n", i,i,i);
  }
  return 1;
}

#include "SCR/ScrDisplay.h"
extern "C" int mbm_scr(int argc , char** argv) {
  MBM::Monitor mon(argc, argv, new ScrDisplay());
  return mon.monitor();
}

