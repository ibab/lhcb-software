//====================================================================
//	LHCbOutputStreamAgent.cpp
//--------------------------------------------------------------------
//
//	Package    :  (The LHCb PersistencySvc service)
//
//  Description: Implementation of the LHCbOutputStream Agent
//
//	Author     : M.Frank
//  Created    : 13/1/99
//	Changes    : 
//
//====================================================================
#define PERSISTENCYSVC_OUTPUTSTREAMAGENT_CPP

// Framework includes
#include "GaudiKernel/IDataManagerSvc.h"
#include "LHCbOutputStream.h"
#include "LHCbOutputStreamAgent.h"

/// Standard Constructor
LHCbOutputStreamAgent::LHCbOutputStreamAgent(LHCbOutputStream* OutputStream)
: m_OutputStream(OutputStream)  
{
}

/// Standard Destructor
LHCbOutputStreamAgent::~LHCbOutputStreamAgent()  {
}

/// Analysis callback
bool LHCbOutputStreamAgent::analyse(IRegistry* pRegistry, int level)   {
  return m_OutputStream->collect(pRegistry, level);
}
