// $Id: ROMonPublish.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonPublish.cpp,v 1.1 2008-02-01 17:41:46 frankm Exp $

// Framework includes
#include "dic.h"
#include "ROMon/ROMonPublish.h"

using namespace ROMon;


/// Standard constructor
ROMonPublish::ROMonPublish(int argc, char** argv) 
  : ROMonGblBuffer(argc,argv,ROMonPublish::help)
{
}

/// Help callback
void ROMonPublish::help() {
  ::lib_rtl_printf("romon_gbl_dump -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int ROMonPublish::monitor() {
  bool exec = true;
  while(exec)    {
    publish();
    ::lib_rtl_sleep(m_delay);
  }
  return 1;
}

