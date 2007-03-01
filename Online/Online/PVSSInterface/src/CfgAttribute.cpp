// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/CfgAttribute.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  CfgAttribute.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: CfgAttribute.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/CfgAttribute.h"

using namespace PVSS;

/// Initializing constructor
CfgAttribute::CfgAttribute(CfgType* cfg, int i, const std::string& nam)
: NamedIdentified(i,nam), m_config(cfg)
{
}

/// Standard destructor
CfgAttribute::~CfgAttribute()   {
}

/// Access to the configuration manager
CfgManager* CfgAttribute::manager() const  {
  return m_config->manager();
}

