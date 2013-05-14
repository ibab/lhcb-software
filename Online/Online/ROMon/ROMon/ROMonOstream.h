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
namespace ROMon  {
  std::ostream& operator<<(std::ostream& os, const MBMClient& c);
  std::ostream& operator<<(std::ostream& os, const MBMBuffer& b);
  std::ostream& operator<<(std::ostream& os, const FSMTask& t);
  std::ostream& operator<<(std::ostream& os, const Node& n);
  std::ostream& operator<<(std::ostream& os, const Nodeset& n);
}
#endif /* ROMON_ROMONOSTREAM_H */

