// $Id: MCDecayCounter.cpp,v 1.1 2004-04-29 17:19:13 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/GenMCLink.h"

// local
#include "MCDecayCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCDecayCounter
//
// 2004-04-26 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCDecayCounter>          s_factory ;
const        IAlgFactory& MCDecayCounterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCDecayCounter::MCDecayCounter( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_nEvents(0)
  , m_nMCFound(0)
  , m_mcFinder(NULL)
{
  declareProperty( "DetailedPrintOut", m_debug = false );
}
//=============================================================================
// Destructor
//=============================================================================
MCDecayCounter::~MCDecayCounter() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCDecayCounter::initialize() {

  MsgStream msg(msgSvc(), name());

  // Retrieve MCDecayfinder tool to check if signal is what looked for
  StatusCode sc = toolSvc()->retrieveTool( "MCDecayFinder", m_mcFinder, this );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to retrieve " << name() << "."
        << "MCDecayFinder"
        << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCDecayCounter::execute() {

  MsgStream  msg( msgSvc(), name() );

  // Counter of events processed
  m_nEvents++;

  // Retrieve signal from summaryInfo
  SmartDataPtr<GenMCLinks> sigLinks(evtSvc(), GenMCLinkLocation::Default);
  if( 0 == sigLinks ) {
    msg << MSG::DEBUG << "GenMCLinks not found at"
        << GenMCLinkLocation::Default
        << endreq;
    return StatusCode::FAILURE;
  }
  else {
    if( sigLinks->size() != 1 ) {
      msg << MSG::DEBUG << "More than one signal found"
          << endreq;
    }
    // Full print out with connection between MCParticle and HepMC
    if( m_debug ) {
      for( GenMCLinks::iterator aLink = sigLinks->begin();
           sigLinks->end() != aLink; ++aLink ) {

        msg << MSG::DEBUG << "Signal info from " << GenMCLinkLocation::Default
            << endreq;
        MCParticle* mcSignal = (*aLink)->signal();
        msg << MSG::DEBUG << "== Signal Process type = "
            << (*aLink)->hepMCEvent()->pGenEvt()->signal_process_id()
            << endreq;

        HepMC::GenEvent* genEvt = (*aLink)->hepMCEvent()->pGenEvt();
        HepMC::GenParticle* genP = 
          genEvt->barcode_to_particle((*aLink)->genBarCode() );
        msg << MSG::DEBUG << "== HepMC id = " 
            << genP->pdg_id() 
            << " , momentum = (" << genP->momentum().px()
            << " ," << genP->momentum().py()
            << " ," << genP->momentum().pz()
            << " ," << genP->momentum().e()
            << " )" << endreq;

        msg << MSG::DEBUG << "== MCParticle "
            << " id = "
            << mcSignal->particleID().pid()
            << " , momentum = (" << mcSignal->momentum().px() 
            << " ," << mcSignal->momentum().py() 
            << " ," << mcSignal->momentum().pz() 
            << " ," << mcSignal->momentum().e() 
            << " )" << endreq;
      }      
    }
  }  
  
  SmartDataPtr<MCParticles> kmcparts(eventSvc(), MCParticleLocation::Default );
  if( !kmcparts ) {
    msg << MSG::FATAL << "Enable to find MC particles at '"
        << MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Find decay as described in MCDecayFinder
  const MCParticle *mcpart = NULL;
  std::vector<MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());
  while( m_mcFinder->findDecay( mcparts, mcpart ) ) {
    m_nMCFound++;
    bool sigIsTheSame = false;
    for( GenMCLinks::iterator aLink = sigLinks->begin();
         sigLinks->end() != aLink; ++aLink ) {
      if( (*aLink)->signal() == mcpart ) {
        sigIsTheSame = true;
        break;
      }
      if( !sigIsTheSame ) {
        msg << MSG::WARNING << "Pointer to signal is not the same as in "
            << GenMCLinkLocation::Default
            << endreq;
      }
    }
    if( m_debug ) {
      msg << MSG::DEBUG << "Signal info from " << MCParticleLocation::Default
          << endreq;
      msg << MSG::DEBUG << "== MCParticle "
        << " id = "
        << mcpart->particleID().pid()
        << " , momentum = (" << mcpart->momentum().px() 
        << " ," << mcpart->momentum().py() 
        << " ," << mcpart->momentum().pz() 
        << " ," << mcpart->momentum().e() 
        << " )" << endreq;
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCDecayCounter::finalize() {

  MsgStream msg(msgSvc(), name());
  std::string decayAnalyzed = "Unknown";
  if( 0 != m_mcFinder ) {
    decayAnalyzed =  m_mcFinder->decay();
    toolSvc()->releaseTool(m_mcFinder);
  }  
  double multiplicity = m_nMCFound/m_nEvents;
  msg << MSG::INFO << "Number of events found for decay "
      << decayAnalyzed << " = " << m_nMCFound 
      << " ( = " << multiplicity << "/event )"
      << endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
