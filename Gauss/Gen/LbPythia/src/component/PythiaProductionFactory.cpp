// Include files
#include "GaudiKernel/ToolFactory.h"
#include "LbPythia/PythiaProduction.h"

// It must be present otherwise the class cannot be instanciated, since the
// Factory is in the linker library since it can (and is) specialized by 
// other production tools based on Pythia
DECLARE_TOOL_FACTORY( PythiaProduction )

