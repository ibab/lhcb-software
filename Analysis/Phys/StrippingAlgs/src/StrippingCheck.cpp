// $Id: StrippingCheck.cpp,v 1.1 2009/10/13 13:12:34 poluekt Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Particle.h" 
//#include "GaudiAlg/GaudiTuples.h" 

// local
#include "StrippingCheck.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingCheck
//
// 2009-02-06 : Anton Polouektov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingCheck );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingCheck::StrippingCheck( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("InputLocation", m_inputLocation);
}
//=============================================================================
// Destructor
//=============================================================================
StrippingCheck::~StrippingCheck() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StrippingCheck::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingCheck::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  bool selected = numberOfCandidates(m_inputLocation) > 0;
  
  setFilterPassed( selected );

  return StatusCode::SUCCESS;
}

int StrippingCheck::numberOfCandidates(const std::string& selalgo) const {
  int num = 0;

  const LHCb::Particles* parts = 0 ;

  if (exist<LHCb::Particles>(selalgo)){
    parts = get<LHCb::Particles>(selalgo);
    if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << selalgo << " finds " << parts->size()
                                          << " candidates" << endmsg ;
    num = parts->size();
  } else if (msgLevel(MSG::VERBOSE)) verbose() <<  "Selection " 
                                               << selalgo << " has no particles" << endmsg ;

  return num;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingCheck::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
