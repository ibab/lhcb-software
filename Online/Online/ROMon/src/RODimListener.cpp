// $Id: RODimListener.cpp,v 1.3 2008-06-25 22:51:31 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RODimListener.cpp,v 1.3 2008-06-25 22:51:31 frankb Exp $
#include "ROMon/ROMonInfo.h"
#include "ROMon/RODimListener.h"

using namespace ROMon;

/// Standard constructor
RODimListener::RODimListener(bool verb) 
  : m_dns(0), m_verbose(verb), m_handler(0)  {
}

/// Standard constructor
RODimListener::~RODimListener() {
  m_clients.clear();
}

/// Start listening
void RODimListener::start() {
  if ( m_dns ) delete m_dns;
  m_dns = new ROMonInfo(this);
}

/// Stop listening
void RODimListener::stop() {
  if ( m_dns ) delete m_dns;
  m_dns = 0;
}
