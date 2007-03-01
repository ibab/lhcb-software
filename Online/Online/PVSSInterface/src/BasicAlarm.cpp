// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/BasicAlarm.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  RangeAlarm.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: BasicAlarm.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/DataPoint.h"
#include "PVSS/BasicAlarm.h"

using namespace PVSS;

/// Apply alarm settings to datapoint identified by name
void BasicAlarm::i_apply(Transaction& tr, const std::string& dp_name)    {
  tr->setValue(dp_name+".:_alert_hdl.._orig_hdl",true);
  tr->setValue(dp_name+".:_alert_hdl.._active",true);
}

/// Apply alarm settings to datapoint identified by datapoint name
void BasicAlarm::i_remove(Transaction& tr, const std::string& dp_name)   {
  try  {
    tr->setValue(dp_name+".:_alert_hdl.._active",false);
    tr->execute();
    tr->start();
    tr->setValue(dp_name+".:_alert_hdl.._type", CfgType::NOCONFIG);
    tr->execute();
    tr->start();
  }
  catch(const std::exception& e)  {
    std::cout << e.what() << std::endl;
  }
}
