// $Id: MCUtilityTool.cpp,v 1.1.1.1 2001-10-19 17:06:04 gcorti Exp $
// Framework include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"

// Tool example
#include "MCUtilityTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCUtilityTool
//
// 14/10/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static ToolFactory<MCUtilityTool>          Factory;
const IToolFactory& MCUtilityToolFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
MCUtilityTool::MCUtilityTool( const std::string& type,
                              const std::string& name, 
                              const IInterface* parent )
  : AlgTool( type, name, parent ) {
}

//=============================================================================
// Standard destructor
//=============================================================================

MCUtilityTool::~MCUtilityTool( ) {
}


//=============================================================================
// Query the interface
//=============================================================================
StatusCode MCUtilityTool::queryInterface(const IID& /* riid */, 
                                         void** /* ppvInterface */){
  return NO_INTERFACE;
}

//=============================================================================
// Print decay tree for a given particle
//=============================================================================

//=============================================================================
// Match decay tree for a given particle
//=============================================================================

