// $Id: LHCbAlgs_dll.cpp,v 1.3 2007-02-27 16:07:58 cattanem Exp $
// Include files 
#include "Kernel/LbAppInit.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/LoadFactoryEntries.h" // Put this include last, to please winwows

// Put LbAppInit base class factory in this library, to avoid having it in
// LHCbKernel component library
DECLARE_ALGORITHM_FACTORY( LbAppInit );

LOAD_FACTORY_ENTRIES( LHCbAlgs )

