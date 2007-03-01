// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/Environment.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $
//  ====================================================================
//  Environment.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Environment.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $

#include "PVSS/DevAnswer.h"
#include "PVSS/Internals.h"
#include "PVSS/Environment.h"

using namespace PVSS;

Environment::Environment()  {
}

Environment& PVSS::Environment::instance()  {
  static PVSS::Environment e;
  return e;
}

bool Environment::waitForAnswer(DevAnswer* answer)  {
  for(;;)  {
    DevAnswer::State s = answer->state();
    if ( s == DevAnswer::ERROR ) return false;
    if ( s == DevAnswer::FINISHED ) return true;
    pvss_sleep(1);
  }
  return false;
}

