 //  $Id: LbPythia_entries.cpp,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
//====================================================================
//  LbPythia_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Gen/LbPythia
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( LbPythia ) {
  DECLARE_ALGORITHM( PythiaAlg );
  DECLARE_ALGORITHM( HiggsPythiaAlg );
  DECLARE_ALGORITHM( TTbarPythiaAlg );
}
  
