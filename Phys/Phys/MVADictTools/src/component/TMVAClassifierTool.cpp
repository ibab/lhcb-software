
#include "TMVAClassifierTool.h"

#include "LoKi/DictTransform.h"
#include "GaudiKernel/ToolFactory.h"

////==============================================================================
typedef LoKi::Hybrid::DictTransform<TMVATransform> TMVAClassifierTool;
DECLARE_TOOL_FACTORY(TMVAClassifierTool)
//==============================================================================
