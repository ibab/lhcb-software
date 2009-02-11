// $Id: TupleToolSelResults.cpp,v 1.1 2009-02-11 18:02:35 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolSelResults.h"
#include "Kernel/ICheckSelResults.h"            // Interface

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolSelResults
//
// 2009-02-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolSelResults );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolSelResults::TupleToolSelResults( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("Selections", m_selections, "List of algorithm names");
  declareProperty("Head", m_head = "", "This will be appended before any tuple entry");
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolSelResults::~TupleToolSelResults() {} 

//=============================================================================
// init
//=============================================================================
StatusCode TupleToolSelResults::initialize() {
  StatusCode sc = GaudiTupleTool::initialize();
  m_selTool = tool<ICheckSelResults>("CheckSelResultsTool",this);
  return sc ;
} 

//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolSelResults::fill( Tuples::Tuple& tup) {
  bool test = true;
  for ( std::vector<std::string>::const_iterator s = m_selections.begin() ; s!= m_selections.end() ; ++s){
    test &= tup->column(m_head+(*s),m_selTool->isSelected(*s));
    if (!test) {
      err() << "Cannot fill variable name " << m_head+(*s) << endmsg ;
      break;
    }
  }
  return StatusCode(test) ;
} 
