// $Id: CPUMonOstream.h,v 1.4 2008-11-11 15:09:25 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMonOstream.h,v 1.4 2008-11-11 15:09:25 frankb Exp $
#ifndef ROMON_CPUMONOSTREAM_H
#define ROMON_CPUMONOSTREAM_H 1

#include <iomanip>
#include <iostream>

// Framework include files
#include "ROMon/CPUMon.h"
namespace ROMon  {
  std::ostream& operator<<(std::ostream& os, const CPU::Stat& s);
  std::ostream& operator<<(std::ostream& os, const CPU& n);
  std::ostream& operator<<(std::ostream& os, const CPUset& n);
  std::ostream& operator<<(std::ostream& os, const CPUfarm& n);

  std::ostream& operator<<(std::ostream& os, const Process& n);
  std::ostream& operator<<(std::ostream& os, const Procset& n);
  std::ostream& operator<<(std::ostream& os, const ProcFarm& n);

  std::ostream& operator<<(std::ostream& os, const Memory& n);
  std::ostream& operator<<(std::ostream& os, const NodeStats& n);
}
#endif /* ROMON_CPUMONOSTREAM_H */

