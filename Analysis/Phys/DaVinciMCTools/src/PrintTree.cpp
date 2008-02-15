// $Id: PrintTree.cpp,v 1.8 2008-02-15 14:57:07 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// from DaVinci
#include "Kernel/IPrintDecayTreeTool.h"

// local
#include "PrintTree.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintTree
//
// 2004-09-10 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( PrintTree );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintTree::PrintTree( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_debug(0)
    , m_linker(0)
{
  declareProperty("PrintTruth",m_link=false );
}
//=============================================================================
// Destructor
//=============================================================================
PrintTree::~PrintTree() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrintTree::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  
  debug() << "==> Initialize" << endmsg;
  m_debug = tool<IPrintDecayTreeTool>( "PrintDecayTreeTool", this );
  if ( m_link ) {
    m_linker      = new Particle2MCLinker( this, Particle2MCMethod::Links, "");
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintTree::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  LHCb::Particle::ConstVector parts = desktop()->particles();
  LHCb::Particle::ConstVector::iterator iL;
  for ( iL = parts.begin() ; iL != parts.end() ; iL++ ) {
    debug() << "Will print a " << (*iL)->particleID().pid() << endmsg ;  
    if ( m_link) m_debug->printTree( (*iL), m_linker );
    else m_debug->printTree( (*iL) );
  }// - loop
  if (!parts.empty()) setFilterPassed(true); 
  else {
    setFilterPassed(false);  
    debug() << "No particles found to print in a tree" << endreq;
  }
  
  return StatusCode::SUCCESS;
};


//=============================================================================
