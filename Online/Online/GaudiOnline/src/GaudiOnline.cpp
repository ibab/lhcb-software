// $Id: GaudiOnline.cpp,v 1.15 2006-11-27 13:46:37 frankb Exp $
//====================================================================
//  GaudiOnline_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : 
//
//====================================================================
// Include Files
#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(GaudiOnline)

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GaudiOnline)  {
  DECLARE_NAMESPACE_SERVICE(   LHCb,DimMessageSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,DimErrorLogger);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MBMCnvSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPManager);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPRxSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPHolderSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPConverterSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,OnlineRunable);
  DECLARE_NAMESPACE_SERVICE(   LHCb,OnlineMessageSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,OnlineEvtSelector);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPRxSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MemMonitorSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MBMMonitorSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,DimMonitoringEngine);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MemMonitoringEngine);
  DECLARE_NAMESPACE_SERVICE(   LHCb,SharedMemMonitoringEngine);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,DecisionSetterAlg);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MDFSender);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MDFReceiver);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,DelaySleepAlg);
}
