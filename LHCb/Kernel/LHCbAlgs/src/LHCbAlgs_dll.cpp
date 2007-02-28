// $Id: LHCbAlgs_dll.cpp,v 1.4 2007-02-28 17:44:39 cattanem Exp $
// Include files 
#include "GaudiKernel/LoadFactoryEntries.h"
#include "Kernel/LbAppInit.h"
#include "GaudiKernel/AlgFactory.h"

// Put LbAppInit base class factory in this library, to avoid having it in
// LHCbKernel component library
DECLARE_ALGORITHM_FACTORY( LbAppInit );

LOAD_FACTORY_ENTRIES( LHCbAlgs )

