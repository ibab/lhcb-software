// $Id: MinimumBias.cpp,v 1.1 2005-10-03 10:29:04 robbep Exp $
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
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    HepMCEvent * theHepMCEvent = new HepMCEvent( m_productionTool -> name() ,
                                                 1 , 1 ) ;
    HardInfo * theHardInfo = new HardInfo( ) ;

    HepMC::GenEvent * theGenEvent = theHepMCEvent -> pGenEvt() ;
    theHardInfo -> setEvent( theHepMCEvent ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    theEventVector.push_back( theHepMCEvent ) ;
    theHardVector .push_back( theHardInfo   ) ;
  } 
  return true ;
}
