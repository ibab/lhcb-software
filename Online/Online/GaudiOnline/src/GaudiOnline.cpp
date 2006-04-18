// $Id: GaudiOnline.cpp,v 1.8 2006-04-18 14:44:06 frankb Exp $
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
  DECLARE_SERVICE(             OnlineMessageSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MBMCnvSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPCnvSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPManager);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPRxSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPHolderSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPConverterSvc);
  DECLARE_NAMESPACE_SERVICE(   LHCb,OnlineRunable);
  DECLARE_NAMESPACE_SERVICE(   LHCb,OnlineEvtSelector);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,DecisionSetterAlg);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MDFNetworkSendAlg);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MDFNetworkReceiveAlg);
}
