// $Id: MDF.cpp,v 1.7 2006-06-29 15:58:35 frankb Exp $
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

LOAD_FACTORY_ENTRIES(MDF)

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MDF)  {
  DECLARE_NAMESPACE_SERVICE(   LHCb,MDFSelector  );
  DECLARE_NAMESPACE_SERVICE(   LHCb,RawDataCnvSvc );
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MDFWriter);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MIFWriter);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MEPWriter);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,RawDataWriter);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,MEPDump);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,RawEventTestDump);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,RawEventTestCreator);
} 
