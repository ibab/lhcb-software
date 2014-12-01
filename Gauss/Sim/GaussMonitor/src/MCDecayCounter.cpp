// $Id: MCDecayCounter.cpp,v 1.6 2007-01-12 15:28:40 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/MCParticle.h"

// local
#include "MCDecayCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCDecayCounter
//
// 2004-04-26 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCDecayCounter )


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
MCDecayCounter::~MCDecayCounter() {}

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
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCDecayCounter::execute() {

  debug() << "==> Execute" << endmsg;

  // Counter of events processed
  m_nEvents++;
  
  LHCb::MCParticles* kmcparts =
    get<LHCb::MCParticles>( eventSvc(), LHCb::MCParticleLocation::Default );
  
  // Find decay as described in MCDecayFinder
  const LHCb::MCParticle *mcpart = NULL;
  LHCb::MCParticle::ConstVector mcparts(kmcparts->begin(), kmcparts->end());

  while( m_mcFinder->findDecay( mcparts, mcpart ) ) {
    m_nMCFound++;
    if( m_debug ) {
      verbose() << "Signal info from " << LHCb::MCParticleLocation::Default
                << endmsg;
      verbose() << "== MCParticle id = " << mcpart->particleID().pid()
                << " , momentum = " << mcpart->momentum() 
                << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

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
