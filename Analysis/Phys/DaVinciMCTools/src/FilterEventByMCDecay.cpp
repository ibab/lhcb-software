// $Id: FilterEventByMCDecay.cpp,v 1.2 2009-04-30 12:52:11 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "MCInterfaces/IMCDecayFinder.h"
#include "Event/MCParticle.h"
// local
#include "FilterEventByMCDecay.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterEventByMCDecay
//
// 2009-04-28 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterEventByMCDecay )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterEventByMCDecay::FilterEventByMCDecay( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_select(true),
  m_decayFinder(0),
  m_inputLocation(LHCb::MCParticleLocation::Default)
{
  declareProperty("Select", m_select);
}

//=============================================================================
// Destructor
//=============================================================================
FilterEventByMCDecay::~FilterEventByMCDecay() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterEventByMCDecay::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_decayFinder = tool<IMCDecayFinder>("MCDecayFinder", this);

  return (0!=m_decayFinder) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterEventByMCDecay::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::MCParticle::Container* mcParts_ = 
    get<LHCb::MCParticle::Container>(m_inputLocation);

  const LHCb::MCParticle::ConstVector mcps(mcParts_->begin(),
                                           mcParts_->end() );

  const bool foundDecay = (mcParts_ != 0 ) && ( m_decayFinder->hasDecay(mcps) );

  const bool pass = (m_select) ? foundDecay : !foundDecay ;

  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}

//=============================================================================
