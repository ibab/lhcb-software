// $Id: FixedNInteractions.cpp,v 1.1 2005-10-03 10:18:38 robbep Exp $
// Include files 

// local
#include "FixedNInteractions.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedNInteractions
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<FixedNInteractions>          s_factory ;
const        IToolFactory& FixedNInteractionsFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedNInteractions::FixedNInteractions( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "NInteractions" , m_nInteractions = 1 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedNInteractions::~FixedNInteractions( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedNInteractions::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  if ( 1 == m_nInteractions ) 
    info() << "Single Interaction Mode" << endmsg ;
  else info() << "Fixed Number of Interactions per Event = : "
              << m_nInteractions << endmsg ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedNInteractions::numberOfPileUp( double & currentLuminosity ) {
  currentLuminosity = 0. ;
  return m_nInteractions ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedNInteractions::printPileUpCounters( ) { } ;

