//====================================================================
//  Components_dll.cpp
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

// DllMain entry point
#include "GaudiKernel/LoadFactoryEntries.h"
LOAD_FACTORY_ENTRIES(MuonAlgs)
