// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/System.cpp,v 1.3 2007-04-26 18:24:30 frankb Exp $
//  ====================================================================
//  DpHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: System.cpp,v 1.3 2007-04-26 18:24:30 frankb Exp $

#include "PVSS/Internals.h"

// PVSS include files
#include "Manager.hxx"
#include "ErrHdl.hxx"

int PVSS::systemID(const char* name)   {
  return DpIdentification::getDefaultSystem();
}

int PVSS::defaultSystemID()   {
  return DpIdentification::getDefaultSystem();
}

const char* PVSS::defaultSystemName()   {
  char* name="Unknown";
  Manager::getDpIdentificationPtr()->getSystemName(DpIdentification::getDefaultSystem(),name);
  return name;
}

int PVSS::pvss_print(int severity, int type, const char* message)  {
  ErrHdl::error(ErrClass::ErrPrio(severity),
                ErrClass::ERR_CONTROL,
                type,
                message);
  return 1;
}