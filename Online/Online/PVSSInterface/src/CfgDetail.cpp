// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/CfgDetail.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: CfgDetail.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/CfgDetail.h"

using namespace PVSS;

/// Initializing constructor
CfgDetail::CfgDetail(CfgType* cfg, int i, const std::string& nam)
: NamedIdentified(i,nam), m_config(cfg)
{
}

/// Standard destructor
CfgDetail::~CfgDetail()   {
}

/// Access to the configuration manager
CfgManager* CfgDetail::manager() const  {
  return m_config->manager();
}

