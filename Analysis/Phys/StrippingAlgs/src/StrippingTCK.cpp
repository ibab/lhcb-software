#include "GaudiKernel/AlgFactory.h" 
#include "StrippingTCK.h"

#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingTCK
// 
// A simple algorithm that sets the TCK field in the stripping DecReports 
// structure
// 
// 2010-09-20 : Anton Poluektov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingTCK );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingTCK::StrippingTCK( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("HDRLocation", m_hdrLocation = "Strip/Phys/DecReports");
  declareProperty("TCK", m_tck = 0);
}
//=============================================================================
// Destructor
//=============================================================================
StrippingTCK::~StrippingTCK() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StrippingTCK::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingTCK::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::HltDecReports* reports = get<LHCb::HltDecReports>(m_hdrLocation);
  
  if (reports) { 
    reports->setConfiguredTCK(m_tck);
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Set stripping TCK = " << m_tck << 
                                  "in " << m_hdrLocation << endmsg;
  } else {
    warning() << "No stripping DecReports found at " << m_hdrLocation << endmsg;
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingTCK::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
