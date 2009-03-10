// $Id: LHCbAlgs_dll.cpp,v 1.5 2009-03-10 07:20:05 cattanem Exp $
// Include files 
#include "Kernel/LbAppInit.h"
#include "GaudiKernel/LoadFactoryEntries.h"
#include "GaudiKernel/AlgFactory.h"

// Put LbAppInit base class factory in this library, to avoid having it in
// LHCbKernel component library
DECLARE_ALGORITHM_FACTORY( LbAppInit );

LOAD_FACTORY_ENTRIES( LHCbAlgs )

