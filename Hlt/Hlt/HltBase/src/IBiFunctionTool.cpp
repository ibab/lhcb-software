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
  static const InterfaceID 
    s_ITrackBiFunctionTool { "ITrackBiFunctionTool", 2 , 0 };
  return s_ITrackBiFunctionTool;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
ITrackBiFunctionTool::~ITrackBiFunctionTool() {} 
// ============================================================================


// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& ITrackVertexBiFunctionTool::interfaceID() 
{ 
  static const InterfaceID 
    s_ITrackVertexBiFunctionTool { "ITrackVertexBiFunctionTool", 2 , 0 };
  return s_ITrackVertexBiFunctionTool;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
ITrackVertexBiFunctionTool::~ITrackVertexBiFunctionTool() {} 
// ============================================================================


// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& IVertexBiFunctionTool::interfaceID() 
{ 
  static const InterfaceID 
    s_IVertexBiFunctionTool { "IVertexBiFunctionTool", 2 , 0 };
  return s_IVertexBiFunctionTool;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
IVertexBiFunctionTool::~IVertexBiFunctionTool() {} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
