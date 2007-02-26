// $Id: LHCbAlgs_dll.cpp,v 1.2 2007-02-26 12:02:06 cattanem Exp $
// Include files 
#include "GaudiKernel/LoadFactoryEntries.h"
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/LbAppInit.h"

// Put LbAppInit base class factory in this library, to avoid having it in
// LHCbKernel component library
DECLARE_ALGORITHM_FACTORY( LbAppInit );

LOAD_FACTORY_ENTRIES( LHCbAlgs )

