// $Id: SetDecayAlg.cpp,v 1.2 2005-06-24 16:32:47 gcorti Exp $
#include "SetDecayAlg.h"

#include "GaudiKernel/AlgFactory.h"

#include "Event/HepMCEvent.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "Generators/IEvtGenTool.h"

static const AlgFactory<SetDecayAlg>    Factory;
const IAlgFactory& SetDecayAlgFactory = Factory;

//=============================================================================
// Constructor
//=============================================================================
SetDecayAlg::SetDecayAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator) , m_evtTool( 0 ) { 
  declareProperty("HepMCEventLocation", m_eventLoc = HepMCEventLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
SetDecayAlg::~SetDecayAlg( ) { }

//=============================================================================
// Initialized method
//=============================================================================
StatusCode SetDecayAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ;
  
  if ( sc.isFailure() ) return Error( "Base class not initialized" ) ;

  // Retrieve EvtGen Tool
  try { m_evtTool = tool< IEvtGenTool >( "EvtGenTool" ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "   Unable to create EvtGenTool" , exc ) ;
  }
  
  debug() << "SetDecayAlg initialized" << endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute method
//=============================================================================
StatusCode SetDecayAlg::execute() {  
  HepMCEvents * hepMCptr = get< HepMCEvents >( m_eventLoc ) ;
  
  if( 0 == hepMCptr ) info() << "No HepMCptr" << endreq;
  else {
    HepMCEvents::iterator it;
    for ( it=hepMCptr->begin() ; it!=hepMCptr->end() ; ++it) {
      HepMC::GenEvent::particle_const_iterator itp ;
      HepMC::GenEvent * pEvt = (*it)->pGenEvt() ;
      for ( itp = pEvt->particles_begin(); itp != pEvt->particles_end(); 
            ++itp ) {
        //
        // This is the main loop.
        // Setting status == 888 for particles to be decayed by EvtGen
        //
        
        if( 1 == (*itp)->status() ) {
          // After PythiaAlg, all particles have StdHep 
          // PDG Id and not PythiaId anymore.
          // Check if the particle has a decay table defined
          // in EvtGen. If yes set status to 888 and decay it
          // with EvtGen in EvtDecayAlg.
          if ( m_evtTool -> isKnownToEvtGen( (*itp)->pdg_id() ) ) {
            debug() << "Seting particle with ID " 
                   << (*itp)->pdg_id() << " to be decayed by EvtGen" 
                   << endreq;
            (*itp)->set_status(888);
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}



