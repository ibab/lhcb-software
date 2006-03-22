// $Id: Special.cpp,v 1.5 2006-03-22 22:58:06 robbep Exp $
// Include files 

// local
#include "Special.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/GenCounters.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Special
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<Special>          s_factory ;
const        IToolFactory& SpecialFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Special::Special( const std::string & type , const std::string & name ,
                  const IInterface * parent )
  : ExternalGenerator( type, name , parent ) ,
    m_nEventsBeforeCut( 0 ) ,
    m_nEventsAfterCut( 0 ) { }

//=============================================================================
// Destructor
//=============================================================================
Special::~Special( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Special::initialize( ) {
  info() << "Generating Special events." << endmsg ;
  return ExternalGenerator::initialize( ) ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Special::generate( const unsigned int /* nPileUp */ , 
                        LHCb::HepMCEvents * theEvents , 
                        LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  bool result = false ;
  
  // For the moment no pile-up for this type of event
  for ( unsigned int i = 0 ; i < 1 ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;
    
    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    ParticleVector theParticleList ;
    m_nEventsBeforeCut++ ;
    bool passCut = true ;
    if ( 0 != m_cutTool ) 
      passCut = m_cutTool -> applyCut( theParticleList , theGenEvent , 
                                       theGenCollision ) ;
    
    if ( passCut ) {
      m_nEventsAfterCut++ ;
      result = true ;
      theGenCollision -> setIsSignal( true ) ;
    }
  } 
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Special::printCounters( ) const {
  using namespace GenCounters ;
  printEfficiency( info() , "generator level cut" , m_nEventsAfterCut , 
                   m_nEventsBeforeCut ) ;
}
