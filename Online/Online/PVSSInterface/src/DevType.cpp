// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DevType.cpp,v 1.3 2007-03-02 12:19:02 frankb Exp $
//  ====================================================================
//  DevType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DevType.cpp,v 1.3 2007-03-02 12:19:02 frankb Exp $

// Framework include files
#include "PVSS/DevType.h"
#include "PVSS/DevTypeElement.h"
#include <algorithm>

using namespace PVSS;


/// Initializing constructor
DevType::DevType(DevTypeManager* mgr, int i, const std::string& nam, int typ)
: DevTypeElement(0,i,nam,typ), m_manager(mgr)
{
  m_type = this;
}

/// Standard destructor
DevType::~DevType()   {
}
