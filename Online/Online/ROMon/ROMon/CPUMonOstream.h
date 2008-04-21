// $Id: CPUMonOstream.h,v 1.2 2008-04-21 17:36:02 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMonOstream.h,v 1.2 2008-04-21 17:36:02 frankm Exp $
#ifndef ROMON_CPUMONOSTREAM_H
#define ROMON_CPUMONOSTREAM_H 1

#include <iomanip>
#include <iostream>

// Framework include files
#include "ROMon/CPUMon.h"

std::ostream& operator<<(std::ostream& os, const ROMon::CPU& n);
std::ostream& operator<<(std::ostream& os, const ROMon::CPUset& n);
std::ostream& operator<<(std::ostream& os, const ROMon::CPUfarm& n);

std::ostream& operator<<(std::ostream& os, const ROMon::Process& n);
std::ostream& operator<<(std::ostream& os, const ROMon::Procset& n);

#endif /* ROMON_CPUMONOSTREAM_H */

