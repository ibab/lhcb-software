// $Id: MinimumBias.cpp,v 1.2 2005-11-17 15:57:08 robbep Exp $
// Include files 

// local
#include "MinimumBias.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Generators/IProductionTool.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimumBias
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MinimumBias>          s_factory ;
const        IToolFactory& MinimumBiasFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MinimumBias::MinimumBias( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : ExternalGenerator( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
MinimumBias::~MinimumBias( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode MinimumBias::initialize( ) {
  info() << "Generating Minimum Bias events." << endmsg ;
  return ExternalGenerator::initialize( ) ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool MinimumBias::generate( const unsigned int nPileUp , 
                            EventVector & theEventVector , 
                            HardVector  & theHardVector ) {
  StatusCode sc ;
  HardInfo * theHardInfo( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEventVector, theHardVector, theGenEvent, 
                        theHardInfo ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;
  } 
  return true ;
}
