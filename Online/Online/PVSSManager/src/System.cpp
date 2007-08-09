// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/System.cpp,v 1.4 2007-08-09 20:03:47 frankm Exp $
//  ====================================================================
//  DpHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: System.cpp,v 1.4 2007-08-09 20:03:47 frankm Exp $

#include "PVSS/Internals.h"

// PVSS include files
#include "Manager.hxx"
#include "ErrHdl.hxx"

#include <cstdio>

int PVSS::systemID(const char* name)   {
  if ( name && name[0] != 0 ) {
    SystemNumType sysNum = 0;
    DpIdentificationResult res = 
      Manager::getDpIdentificationPtr()->getSystemId(name,sysNum);
    if ( 0 == res ) {
      return sysNum;
    }
  }
  return 0;
}

int PVSS::defaultSystemID()   {
  return DpIdentification::getDefaultSystem();
}

const char* PVSS::defaultSystemName()   {
  static char s_sysName[256];
  char* name="Unknown";
  s_sysName[0] = 0;
  if ( 0 == Manager::getDpIdentificationPtr()->getSystemName(DpIdentification::getDefaultSystem(),name) ) {
    ::strcpy(s_sysName,name);
    delete [] name;
  }
  return s_sysName;
}
#if 0
namespace PVSS {
  std::vector<std::string> PVSS::systemNames();
}
std::vector<std::string> PVSS::systemNames() {
  size_t count = 0;
  SystemNumType* ids = 0;
  CharString*    names = 0;
  std::vector<std::string> res;
  if ( 0 == Manager::getDpIdentificationPtr()->getAllSystems(ids,names,count) ) {
    for(size_t i=0; i<count; ++i) {
      res.push_back(names[i])
    }
    if ( count > 0 ) {
      delete [] ids;
      delete [] names;
    }
  }
  return res;
}
#endif
int PVSS::pvss_print(int severity, int type, const char* message)  {
  ErrHdl::error(ErrClass::ErrPrio(severity),
                ErrClass::ERR_CONTROL,
                type,
                message);
  return 1;
}
