// $Id: MCDecayCounter.cpp,v 1.2 2005-08-17 16:47:29 gcorti Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator )
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

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Retrieve MCDecayfinder tool to check if signal is what looked for
  m_mcFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCDecayCounter::execute() {

  debug() << "==> Execute" << endmsg;

  // Counter of events processed
  m_nEvents++;

  // Retrieve signal from summaryInfo
  SmartDataPtr<GenMCLinks> sigLinks(evtSvc(), GenMCLinkLocation::Default);
  if( 0 == sigLinks ) {
    Warning("GenMCLinks not found at"+GenMCLinkLocation::Default);    
  }
  else {
    if( sigLinks->size() != 1 ) {
      Warning("More than one signal found");
    }
    // Full print out with connection between MCParticle and HepMC
    if( m_debug ) {
      for( GenMCLinks::iterator aLink = sigLinks->begin();
           sigLinks->end() != aLink; ++aLink ) {

        debug() << "Signal info from " << GenMCLinkLocation::Default
                  << endmsg;
        MCParticle* mcSignal = (*aLink)->signal();
        debug() << "== Signal Process type = "
                  << (*aLink)->hepMCEvent()->pGenEvt()->signal_process_id()
                  << endmsg;

        HepMC::GenEvent* genEvt = (*aLink)->hepMCEvent()->pGenEvt();
        HepMC::GenParticle* genP = 
          genEvt->barcode_to_particle((*aLink)->genBarCode() );
        verbose() << "== HepMC id = " << genP->pdg_id() 
                  << " , momentum = " << genP->momentum()
                  << endmsg;
        verbose() << "== MCParticle id = " << mcSignal->particleID().pid()
                  << " , momentum = " << mcSignal->momentum() 
                  << endmsg;
      }      
    }
  }  
  
  MCParticles* kmcparts =
    get<MCParticles>( eventSvc(), MCParticleLocation::Default );
  
  // Find decay as described in MCDecayFinder
  const MCParticle *mcpart = NULL;
  std::vector<MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());
  while( m_mcFinder->findDecay( mcparts, mcpart ) ) {
    m_nMCFound++;
    bool sigIsTheSame = false;
    if( 0 != sigLinks ) {
      for( GenMCLinks::iterator aLink = sigLinks->begin();
           sigLinks->end() != aLink; ++aLink ) {
        if( (*aLink)->signal() == mcpart ) {
          sigIsTheSame = true;
          break;
        }
        if( !sigIsTheSame ) {
          Warning("Pointer to signal is not the same as in "+
                  GenMCLinkLocation::Default);
        }
      } 
    }
    if( m_debug ) {
      verbose() << "Signal info from " << MCParticleLocation::Default
                << endmsg;
      verbose() << "== MCParticle id = " << mcpart->particleID().pid()
                << " , momentum = " << mcpart->momentum() 
                << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCDecayCounter::finalize() {

  debug() << "==> Finalize" << endmsg;

  std::string decayAnalyzed = "Unknown";
  if( 0 != m_mcFinder ) {
    decayAnalyzed =  m_mcFinder->decay();
  }  

  double multiplicity = double(m_nMCFound)/double(m_nEvents);

  info() << endmsg
         << " Decay analyzed   = " << decayAnalyzed
         << endmsg
         << "   events processed = " << format( "%8d", m_nEvents )
         << endmsg
         << "   events found     = " << format( "%8d", m_nMCFound )
         << endmsg
         << "   fraction/event   = " << format( "%8.2f", multiplicity )
         << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
  
}

//=============================================================================
