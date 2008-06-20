// $Id: MicroDSTTool.cpp,v 1.4 2008-06-20 07:59:35 jpalac Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/ToolFactory.h" 

// local
#include "MicroDST/MicroDSTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MicroDSTTool
//
// 2007-12-04 : Juan PALACIOS
//-----------------------------------------------------------------------------

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
  sc = MicroDST::synchroniseProperty(this->parent(), this, "OutputPrefix");
  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
MicroDSTTool::~MicroDSTTool() {} 

//=============================================================================
