// $Id: IBiFunctionTool.cpp,v 1.1 2009-03-31 15:42:33 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/IBiFunctionTool.h"
#include "HltBase/ITrackBiFunctionTool.h"
// ============================================================================

// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& ITrackBiFunctionTool::interfaceID()
{
    static const InterfaceID s_ITrackBiFunctionTool{"ITrackBiFunctionTool", 2, 0};
    return s_ITrackBiFunctionTool;
}
// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& ITrackVertexBiFunctionTool::interfaceID()
{
    static const InterfaceID s_ITrackVertexBiFunctionTool{
        "ITrackVertexBiFunctionTool", 2, 0};
    return s_ITrackVertexBiFunctionTool;
}
// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& IVertexBiFunctionTool::interfaceID()
{
    static const InterfaceID s_IVertexBiFunctionTool{"IVertexBiFunctionTool", 2, 0};
    return s_IVertexBiFunctionTool;
}
// ============================================================================
// The END
// ============================================================================
