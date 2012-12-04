// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Generators
#include "Generators/IProductionTool.h"

// from LbPythia8
#include "LbPythia8/LbLHAupFortran.h"

// local
#include "LbLHAup.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbLHAup
//
// 2012-12-02 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LbLHAup )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LbLHAup::LbLHAup( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_productionTool( 0 ) 
{
  declareInterface<ILHAupFortranTool>(this);
  declareProperty( "UserProcess" , m_productionToolName ) ;
}

//=============================================================================
// Destructor
//=============================================================================
LbLHAup::~LbLHAup() {} 

//=============================================================================
// Initialize
//=============================================================================
StatusCode LbLHAup::initialize( ) {
  StatusCode sc = GaudiTool::initialize() ;
  m_productionTool = tool< IProductionTool >( m_productionToolName , this ) ;
  m_lhaPtr = new LbLHAupFortran( ) ;
  m_lhaPtr -> setProductionTool( m_productionTool ) ;
  return sc ;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode LbLHAup::finalize( ) {
  delete m_lhaPtr ;
  m_lhaPtr = 0 ;
  return GaudiTool::finalize() ;
}
