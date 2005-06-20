// $Id: SignalDecayAlg.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
#include "SignalDecayAlg.h"

//From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

// From Event
#include "Event/HepMCEvent.h"

// From HepMC
#include "HepMC/GenEvent.h"

// From EvtGen
#include "EvtGenBase/EvtPDL.hh"

// Local
#include "Generators/IEvtGenTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalDecayAlg
//
// 2003-10-15 : Patrick Robbe
//-----------------------------------------------------------------------------

// Gaudi RandomNumberSvc
// Common random engine to all Gauss algorithms
extern Rndm::Numbers randgaudi;

static const AlgFactory<SignalDecayAlg>    Factory;
const IAlgFactory& SignalDecayAlgFactory = Factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalDecayAlg::SignalDecayAlg(const std::string& name, 
                               ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator) , m_evtTool( 0 ) { }

//=============================================================================
// Destructor
//=============================================================================
SignalDecayAlg::~SignalDecayAlg( ) { }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode SignalDecayAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ;
  
  // Initialization of Gaudi Rndm::Flat
  // if it is not already initialized
  if ( ! ( randgaudi ) ) {
    sc=randgaudi.initialize(randSvc(),Rndm::Flat(0,1));
    if( ! sc.isSuccess() ) return Error( "Could not initialize Rndm::Flat" ) ;
  }
  
  // Retrieve the shared EvtGen tool
  try { m_evtTool = tool< IEvtGenTool >( "EvtGenTool" ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "  Unable to create EvtGenTool" , exc ) ;
  }

  debug() << "SignalDecayAlg initialized" << endreq ;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute method
//=============================================================================
StatusCode SignalDecayAlg::execute() {
  // For the moment only consider only one signal hadron
  // or two CP conjugate signal hadrons 
  if ( ! ( m_evtTool->isSignalDecay( ) ) ) return StatusCode::SUCCESS ;
  
  // Skip SignalDecayAlg if no forced fragmentation is requested
  if ( ! ( m_evtTool->forcedFragmentation( ) ) ) return StatusCode::SUCCESS ;
  
  EvtId decayId ;
  
  if ( m_evtTool -> doCPMixture () ) {
    // If one has to generate both B and Bbar hadrons
    
    double flavour = m_evtTool -> randomFromEngine () ;
    
    // Decide flavour to decay 
    // if < 0.5, b flavour
    // if > 0.5, anti-b flavour
    
    // Enable possibility to change the B flavour inside EvtGen
    // This is used for CP modes
    m_evtTool->enableFlip() ;
    
    if ( flavour < 0.5 ) 
      // Particle to decay is B signal hadron
      decayId = m_evtTool->getbId() ;
    else 
      // Particle to decay is Bbar signal hadron
      decayId = m_evtTool->getbbarId() ;
  }
  else {
    // If only one B hadron is generated, disable possibility to flip
    // B flavour in EvtGen
    m_evtTool -> disableFlip () ;

    if ( * (m_evtTool -> getBHadronsList().begin()) > 0 ) 
      // If PID is > 0 , decay B hadron
      decayId = m_evtTool->getbId();
    else 
      // PID < 0 then decay Bbar hadron
      decayId = m_evtTool->getbbarId() ;
  }
  
  // Create simple HepMC event to store signal decay tree
  HepMCEvent * mcevt = new HepMCEvent( name() , 1 , 1 ) ;
  HepMC::GenEvent * hepMCevt = mcevt -> pGenEvt() ;

  // Generate the decay of the decayId particle which has
  // been determined above and put it in the HepMCEvent
  int temp ;
  m_evtTool->generateDecayAtRest( hepMCevt , decayId , temp , 0 ) ;

  // Check if signal HepMCEvents already exist  
  if ( exist< HepMCEvents> ( "/Event/Gen/SignalDecay" ) ) 
    return Error( "SignalDecay events already exist" ) ;
  
  // Create HepMCEvents and add to transient store
  HepMCEvents * hepVect = new HepMCEvents ;
  hepVect -> insert ( mcevt ) ;
  StatusCode sc = put( hepVect , "/Event/Gen/SignalDecay" ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not register SignalDecay events" ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode SignalDecayAlg::finalize() {
  debug() << "SignalDecayAlg finalized" << endreq;
  // Finalize random engine
  randgaudi.finalize( ) ;
  return GaudiAlgorithm::finalize( ) ;
}

