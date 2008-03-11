// $Id: ROMonOstream.h,v 1.3 2008-03-11 12:37:52 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonOstream.h,v 1.3 2008-03-11 12:37:52 frankb Exp $
#ifndef ROMON_ROMONOSTREAM_H
#define ROMON_ROMONOSTREAM_H 1

#include <iomanip>
#include <iostream>

// Framework include files
#include "ROMon/ROMon.h"

std::ostream& operator<<(std::ostream& os, const CONTROL& c);
std::ostream& operator<<(std::ostream& os, const ROMon::MBMClient& c);
std::ostream& operator<<(std::ostream& os, const ROMon::MBMBuffer& b);
std::ostream& operator<<(std::ostream& os, const ROMon::FSMTask& t);
std::ostream& operator<<(std::ostream& os, const ROMon::Node& n);
std::ostream& operator<<(std::ostream& os, const ROMon::Nodeset& n);

#endif /* ROMON_ROMONOSTREAM_H */

