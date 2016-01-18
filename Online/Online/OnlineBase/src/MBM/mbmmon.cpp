// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII BUFFER MANAGER DISPLAY                         */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include "RTL/rtl.h"
#include "MBM/Monitor.h"
#include "CPP/AsciiDisplay.h"

extern "C" int mbm_mon(int argc , char** argv) {
  MBM::Monitor mon(argc, argv, new AsciiDisplay(""));//Buffer Manager Monitor on "+RTL::nodeName()));
  ::lib_rtl_install_printer(MBM::Monitor::print, &mon);
  return mon.monitor();
}

extern "C" int mbm_ascii(int /* argc */, char** /* argv */) {
  for ( unsigned char i=0; i < 255; ++i)  {
    printf("%3d  %03X   \"%c\"\n", i,i,i);
  }
  return 1;
}

