// $Id: CPUMonOstream.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMonOstream.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
#ifndef ROMON_CPUMONOSTREAM_H
#define ROMON_CPUMONOSTREAM_H 1

#include <iomanip>
#include <iostream>

// Framework include files
#include "ROMon/CPUMon.h"

std::ostream& operator<<(std::ostream& os, const ROMon::CPU& n);
std::ostream& operator<<(std::ostream& os, const ROMon::CPUset& n);
std::ostream& operator<<(std::ostream& os, const ROMon::CPUfarm& n);

#endif /* ROMON_CPUMONOSTREAM_H */

