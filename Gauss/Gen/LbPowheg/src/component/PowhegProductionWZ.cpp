// ============================================================================
// Include files 
// ============================================================================
#include <unistd.h>
#include <sys/wait.h>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Local 
// ============================================================================
#include "PowhegProductionWZ.h"


// ============================================================================
/** @class PowhegProductionWZ PowhegProductionWZ.cpp
 *
 *  Interface tool to produce events with Powheg- BOX
 *
 *  @author Bartek Rachwal  bartlomiej.rachwal@ifj.edu.pl
 *  @date 2013-04-16
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProductionWZ::PowhegProductionWZ
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : PowhegProduction ( type , name , parent ) 
  {
    declareInterface< IProductionTool >( this ) ; 
  }
  
//=============================================================================
// Destructor 
//=============================================================================
  PowhegProductionWZ::~PowhegProductionWZ() {}
  
// =========================================================================
// Initialize method
//=============================================================================
StatusCode PowhegProductionWZ::initialize() {
    
  return PowhegProduction::initialize ("WZ");
  
}

//=============================================================================
//Finalize
//=============================================================================
StatusCode PowhegProductionWZ::finalize() {
  
  return PowhegProduction::finalize () ;
  
}

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProductionWZ )
// ============================================================================


// The END 
// ============================================================================
