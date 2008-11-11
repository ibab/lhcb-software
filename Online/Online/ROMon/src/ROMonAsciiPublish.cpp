// $Id: ROMonAsciiPublish.cpp,v 1.2 2008-11-11 15:09:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonAsciiPublish.cpp,v 1.2 2008-11-11 15:09:26 frankb Exp $

// Framework includes
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/ROMonAsciiPublish.h"

using namespace ROMon;
using namespace std;

/// Publish the collected information
void ROMonAsciiPublish::publish() { 
  ROMonData gbl(data());
  log() << "========================" << lib_rtl_timestr() << "========================" << endl;
  switch(gbl.type())  {
  case Node::TYPE:
    log() << *gbl.node << endl;
    break;
  case Nodeset::TYPE:
    log() << *gbl.nodeset << endl;
    break;
  default:
    log() << "Unknown data type -- cannot be published." << endl;
    break;
  }
}

extern "C" int romon_gbl_dump(int argc, char** argv) {
  ROMonAsciiPublish romon(argc,argv);
  return romon.monitor();
}

