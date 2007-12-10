// $Id: MicroDSTTool.cpp,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MicroDST/MicroDSTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MicroDSTTool
//
// 2007-12-04 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MicroDSTTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MicroDSTTool::MicroDSTTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : 
  MicroDSTCommon<GaudiTool> ( type, name , parent )
{
  declareInterface<MicroDSTTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MicroDSTTool::~MicroDSTTool() {} 

//=============================================================================
