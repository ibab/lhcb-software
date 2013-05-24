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
#include "PowhegProductionZZ.h"


// ============================================================================
/** @class PowhegProductionZZ PowhegProductionZZ.cpp
 *
 *  Interface tool to produce events with Powheg- BOX
 *
 *  @author Bartek Rachwal  bartlomiej.rachwal@ifj.edu.pl
 *  @date 2013-04-16
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProductionZZ::PowhegProductionZZ
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
  PowhegProductionZZ::~PowhegProductionZZ() {}
  
// =========================================================================
// Initialize method
//=============================================================================
StatusCode PowhegProductionZZ::initialize() {
    
  return PowhegProduction::initialize ("ZZ");
  
}

//=============================================================================
//Finalize
//=============================================================================
StatusCode PowhegProductionZZ::finalize() {
  
  return PowhegProduction::finalize () ;
  
}

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProductionZZ )
// ============================================================================


// The END 
// ============================================================================
