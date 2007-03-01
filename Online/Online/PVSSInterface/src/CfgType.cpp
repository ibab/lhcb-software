// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/CfgType.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  CfgType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: CfgType.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/CfgAttribute.h"
#include <algorithm>

using namespace PVSS;

/// Initializing constructor
CfgType::CfgType(CfgManager* mgr, int i, const std::string& nam)
: NamedIdentified(i,nam), m_manager(mgr)
{
}

/// Standard destructor
CfgType::~CfgType()   {
  std::for_each(m_attrs.begin(),m_attrs.end(),CfgManip<CfgAttribute>());
  std::for_each(m_details.begin(),m_details.end(),CfgManip<CfgDetail>());
}

/// Add detail to type
void CfgType::addDetail(CfgDetail* detail)   {
  if ( detail ) m_details.push_back(detail);
}

/// Add attribute to type
void CfgType::addAttribute(CfgAttribute* attr)  {
  if ( attr )  m_attrs.push_back(attr);
}
