//====================================================================
//--------------------------------------------------------------------
//
//  Package    : Event/GenEvent 
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GenEvent) {
    DECLARE_GENERIC_CONVERTER( HepMCEvent );
}


