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
#include "PowhegProductionWbb.h"


// ============================================================================
/** @class PowhegProductionWbb PowhegProductionWbb.cpp
 *
 *  Interface tool to produce events with Powheg- BOX
 *
 *  @author Bartek Rachwal  bartlomiej.rachwal@ifj.edu.pl
 *  @date 2013-04-16
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProductionWbb::PowhegProductionWbb
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : PowhegProduction ( type , name , parent ) 
  {
    declareInterface< IProductionTool >( this ) ;
    //InputFile() = "pwgevents.lhe";
  }
  
//=============================================================================
// Destructor 
//=============================================================================
  PowhegProductionWbb::~PowhegProductionWbb() {}
  
// =========================================================================
// Initialize method
//=============================================================================
StatusCode PowhegProductionWbb::initialize() {
    
  
  
  return PowhegProduction::initialize ("Wbb");
  
}

//=============================================================================
//Finalize
//=============================================================================
StatusCode PowhegProductionWbb::finalize() {
  
  return PowhegProduction::finalize () ;
  
}

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProductionWbb )
// ============================================================================


// The END 
// ============================================================================
