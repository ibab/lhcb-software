// Include files

// from Event
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"

// from LHCb
#include "MCInterfaces/IMCDecayFinder.h"

// local
#include "EvtTypeChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtTypeChecker
//
// 2004-04-26 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(EvtTypeChecker)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtTypeChecker::EvtTypeChecker( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_nEvents(0)
  , m_nMCFound(0)
  , m_setDecay(false)
  , m_evtTypeSvc(NULL)
  , m_mcFinder(NULL)
{
  declareProperty( "EvtCodeFromData",  m_fromData = true );
  declareProperty( "EvtCode",          m_evtCode  = 0 );
}

//=============================================================================
// Destructor
//=============================================================================
EvtTypeChecker::~EvtTypeChecker() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode EvtTypeChecker::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  // Retrieve the EvtTypeSvc here so that it is always done at initialization
  m_evtTypeSvc = svc<IEvtTypeSvc>( "EvtTypeSvc", true );

  // Check that EvtType code has been set with appropriate value
  // if it will not be read from data
  if( !m_fromData ) {
    if(  m_evtCode == 0 ) {
      fatal() << "With EvtCodeFromData = false you MUST set EvtCode"
              << endmsg;
      return StatusCode::FAILURE;
    }

    // Set the decay descriptor to pass to the MCDecayFinder if using evtCode
    if( (setDecayToFind( m_evtCode )).isFailure() ) {
      fatal() << " 'setDecayToFind' failed in 'initialize' "
              << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Set the decay descriptor of the MCdecayFinder tool based on the evtCode,
// after checking it exists
//=============================================================================
StatusCode EvtTypeChecker::setDecayToFind( const int evtCode ) {

  // Check if code exist
  if( !(m_evtTypeSvc->typeExists( evtCode )) ) {
    fatal() << "EvtCode " << evtCode << "is not known by the EvtTypeSvc"
            << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve tool and set decay descriptor
  m_mcFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
  std::string sdecay = m_evtTypeSvc->decayDescriptor( evtCode );
  if( (m_mcFinder->setDecay( sdecay )).isFailure() ) {
    fatal() << "Unable to set decay for EvtCode " << evtCode << endmsg;
    return StatusCode::FAILURE;
  }

  m_setDecay = true;
  m_evtCode  = evtCode;   // in case called when reading data

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EvtTypeChecker::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  // Counter of events processed
  m_nEvents++;

  // If reading event type from data retrieve event header and set decay
  // unless already done (do it only for first time)
  if( m_fromData && !m_setDecay ) {
    LHCb::GenHeader* header =
      get<LHCb::GenHeader>( evtSvc(), LHCb::GenHeaderLocation::Default );
    if( (setDecayToFind( header->evType() ).isFailure()) ) {
      fatal() << " 'setDecayToFind' failed in 'execute' "
              << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if( m_mcFinder->hasDecay() ) {
    m_nMCFound++;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EvtTypeChecker::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  std::string decayAnalyzed = "Unknown";
  if( 0 != m_mcFinder ) {
    decayAnalyzed =  m_mcFinder->decay();
  }
  double multiplicity = double(m_nMCFound)/double(m_nEvents);
  info() << endmsg
         << " EvtType analyzed = " << m_evtCode
         << endmsg
         << " Decay            = " << decayAnalyzed
         << endmsg
         << " NickName         = " << m_evtTypeSvc->nickName( m_evtCode )
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
