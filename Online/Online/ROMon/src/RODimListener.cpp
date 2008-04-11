// $Id: RODimListener.cpp,v 1.2 2008-04-11 12:11:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RODimListener.cpp,v 1.2 2008-04-11 12:11:22 frankb Exp $
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimListener.h"

using namespace ROMon;

/// Standard constructor
RODimListener::RODimListener(bool verb) 
: m_verbose(verb), m_handler(0)  {
  m_dns = new ROMonInfo(this);
}

/// Standard constructor
RODimListener::~RODimListener() {
  m_clients.clear();
}
