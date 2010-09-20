// $Id: BootMon.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items used to monitor the boot status
//               of the CPU farm(s)
//
//  Author     : M.Frank
//  Created    : 20/09/2010
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/BootMon.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $

#include "ROMon/BootMon.h"

using namespace ROMon;
using namespace std;

/// Standard constructor
BootNodeStatusset::BootNodeStatusset(const string& n) {
  reset();
  ::strncpy(name,n.c_str(),sizeof(name));
  name[sizeof(name)-1]=0;
}

/// Reset object structure
BootNodeStatusset* BootNodeStatusset::reset() {
  ::memset(this,0,sizeof(BootNodeStatusset));
  type = TYPE;
  return this;
}

