// Include files
#include "GaudiKernel/ToolFactory.h"
#include "LbPythia/PythiaProduction.h"
#include "LbPythia/ReadLHE.h"

// It must be present otherwise the class cannot be instanciated, since the
// Factory is in the linker library since it can (and is) specialized by 
// other production tools based on Pythia
DECLARE_TOOL_FACTORY( PythiaProduction )
DECLARE_NAMESPACE_TOOL_FACTORY(LbPythia,ReadLHE)

