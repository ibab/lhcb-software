// $Id: GaudiUPI.cpp,v 1.1 2006-04-24 13:45:56 frankb Exp $
//====================================================================
//  GaudiUPI_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done for each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : 
//
//====================================================================
// Include Files
#include "GaudiKernel/LoadFactoryEntries.h"
LOAD_FACTORY_ENTRIES(GaudiUPI)
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GaudiUPI)  {
  DECLARE_NAMESPACE_SERVICE(Gaudi,UPIService);
}
