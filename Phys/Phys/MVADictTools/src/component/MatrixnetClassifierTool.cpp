
#include "MatrixnetClassifierTool.h"

#include "LoKi/DictTransform.h"
#include "GaudiKernel/ToolFactory.h"

////==============================================================================
typedef LoKi::Hybrid::DictTransform<MatrixnetTransform> MatrixnetClassifierTool;
DECLARE_TOOL_FACTORY(MatrixnetClassifierTool)
//==============================================================================
