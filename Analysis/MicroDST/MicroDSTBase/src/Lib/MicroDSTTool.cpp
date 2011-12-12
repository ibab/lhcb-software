// $Id: MicroDSTTool.cpp,v 1.5 2009-07-30 06:02:37 jpalac Exp $
// Include files
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
