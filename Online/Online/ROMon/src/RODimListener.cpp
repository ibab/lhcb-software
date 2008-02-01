// $Id: RODimListener.cpp,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RODimListener.cpp,v 1.1 2008-02-01 17:41:45 frankm Exp $
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimListener.h"

using namespace ROMon;

/// Standard constructor
RODimListener::RODimListener(bool verb) : m_verbose(verb) {
  m_dns = new ROMonInfo(this);
}

/// Standard constructor
RODimListener::~RODimListener() {
  m_clients.clear();
}
