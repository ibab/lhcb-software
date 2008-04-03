// $Id: MicroDSTTool.cpp,v 1.2 2008-04-03 14:35:58 jpalac Exp $
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
StatusCode MicroDSTTool::initialize() 
{
  StatusCode sc = MicroDSTCommon<GaudiTool>::initialize();
  if (sc.isFailure()) return sc;
  sc = MicroDST::synchroniseProperty(this->parent(), this, "InputPrefix");
  sc = MicroDST::synchroniseProperty(this->parent(), this, "OutputPrefix");
  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
MicroDSTTool::~MicroDSTTool() {} 

//=============================================================================
