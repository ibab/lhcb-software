// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/System.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  DpHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#include "PVSS/Internals.h"

// PVSS include files
#include "Manager.hxx"

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
