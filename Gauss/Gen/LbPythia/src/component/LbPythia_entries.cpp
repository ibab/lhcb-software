 //  $Id: LbPythia_entries.cpp,v 1.2 2005-10-03 10:34:52 robbep Exp $
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
  DECLARE_TOOL( PythiaProduction ) ;
}
  
