//====================================================================
// Components_load.cpp
//--------------------------------------------------------------------
//
//	Package   : Components
//
//	Author    : Markus Frank
//  History   :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 21/07/99| Initial version                              | MF
// +---------+----------------------------------------------+---------
//====================================================================
#define COMPONENTS_LOAD_CPP  1

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonAlgs) {
 DECLARE_ALGORITHM(MuonDigitization)     
//  DECLARE_ALGORITHM(MuonTest)
//  DECLARE_ALGORITHM(AccessGeoAlgorithm)  
//  DECLARE_ALGORITHM(AccessMuonGeo) 

}
