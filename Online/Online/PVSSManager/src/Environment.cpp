// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/Environment.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

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

