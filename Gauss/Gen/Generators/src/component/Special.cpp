// $Id: Special.cpp,v 1.11 2008-01-25 11:42:45 robbep Exp $
// Include files 

// local
#include "Special.h"
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

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

DECLARE_TOOL_FACTORY( Special );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Special::Special( const std::string & type , const std::string & name ,
                  const IInterface * parent )
  : ExternalGenerator( type, name , parent ) ,
    m_nEventsBeforeCut    (  0 ) ,
    m_nEventsAfterCut     (  0 ) ,
    m_maxInteractions     ( 30 ) ,
    m_pileUpProductionTool(  0 ) { 
  declareProperty( "PileUpProductionTool" , 
                   m_pileUpProductionToolName = 
                   "PythiaProduction/MinimumBiasPythiaProduction" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
Special::~Special( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Special::initialize( ) {
  info() << "Generating Special events." << endmsg ;

  if ( "" != m_pileUpProductionToolName ) 
    m_pileUpProductionTool = 
      tool< IProductionTool >( m_pileUpProductionToolName , this ) ;

  m_pileUpEventsVector.clear() ;
  m_pileUpCollisionsVector.clear() ;

  return ExternalGenerator::initialize( ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode Special::finalize( ) {
  std::vector< HepMC::GenEvent * >::iterator iter ;
  for ( iter = m_pileUpEventsVector.begin() ; 
        iter != m_pileUpEventsVector.end() ; ++iter ) 
    delete (*iter) ;
  m_pileUpEventsVector.clear() ;
  
  std::vector< LHCb::GenCollision * >::iterator iterC ;
  for ( iterC = m_pileUpCollisionsVector.begin() ; 
        iterC != m_pileUpCollisionsVector.end() ; ++iterC ) 
    delete (*iterC) ;
  m_pileUpCollisionsVector.clear() ;

  if ( 0 != m_pileUpProductionTool ) release( m_pileUpProductionTool ) ;  

  return ExternalGenerator::finalize() ;
}


//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Special::generate( const unsigned int nPileUp , 
                        LHCb::HepMCEvents * theEvents , 
                        LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  bool result = false ;
  
  // For the moment no pile-up for this type of event
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;
    
    // First interaction is always "signal"
    
    if ( 0 == i ) {
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
      } else return false ;
    } else {
      // if event passed generator level cut
      // look if there are still enough pile-up events
      if ( m_pileUpEventsVector.empty() ) generatePileUp() ;
      
      // retrieve now pile-up events
      HepMC::GenEvent * pileUpEvent = m_pileUpEventsVector.back() ;
      (*theGenEvent) =  ( * pileUpEvent ) ;

      m_pileUpEventsVector.pop_back() ;
      delete pileUpEvent ;

      // and collision
      LHCb::GenCollision * pileUpCollision = m_pileUpCollisionsVector.back() ;
      copyCollision( pileUpCollision , theGenCollision ) ;
      m_pileUpCollisionsVector.pop_back() ;
      delete pileUpCollision ;
    }
  } 
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Special::printCounters( ) const {
  using namespace GenCounters ;

  info() << "************* Special generation counters   ***************"
         << std::endl ;

  printEfficiency( info() , "generator level cut" , m_nEventsAfterCut , 
                   m_nEventsBeforeCut ) ;
  info() << endmsg ;
}

//=============================================================================
// Generate PileUp Minimum Bias interactions
//=============================================================================
void Special::generatePileUp() {
  // initialize the production tool for pile up generation
  m_pileUpProductionTool -> initializeGenerator() ;
  
  // generate given number of events
  for ( unsigned int i = 0 ; i < m_maxInteractions ; ++i ) {
    HepMC::GenEvent * theEvent = new HepMC::GenEvent ;
    LHCb::GenCollision * theCollision = new LHCb::GenCollision ;
    m_pileUpProductionTool -> generateEvent( theEvent , 
                                             theCollision ) ;
    
    m_pileUpEventsVector.push_back( theEvent ) ;
    m_pileUpCollisionsVector.push_back( theCollision ) ;
  }

  // now initialize the production tool for the "signal"
  m_productionTool -> initializeGenerator() ;
  //TODO : add finalize function to delete remaining events
}

//=============================================================================
// Copy collisions
//=============================================================================
void Special::copyCollision( const LHCb::GenCollision * FROM , 
                             LHCb::GenCollision * TO ) const {
  TO -> setIsSignal( FROM -> isSignal() ) ;
  TO -> setProcessType( FROM -> processType() ) ;
  TO -> setSHat( FROM -> sHat() ) ;
  TO -> setTHat( FROM -> tHat() ) ;
  TO -> setUHat( FROM -> uHat() ) ;
  TO -> setPtHat( FROM -> ptHat() ) ;
  TO -> setX1Bjorken( FROM -> x1Bjorken() ) ;
  TO -> setX2Bjorken( FROM -> x2Bjorken() ) ;
}


