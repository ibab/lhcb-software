// $Id: RunDb.cpp,v 1.1 2006-07-04 17:04:38 frankb Exp $
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
LOAD_FACTORY_ENTRIES(DimRunDb)

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES(DimRunDb)  {
  DECLARE_NAMESPACE_SERVICE(   LHCb,RunDatabase);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,RunDbDataWriter);
  DECLARE_NAMESPACE_ALGORITHM( LHCb,RunCreator);
}
