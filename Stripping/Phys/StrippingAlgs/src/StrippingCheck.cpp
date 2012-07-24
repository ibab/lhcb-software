// $Id: StrippingCheck.cpp,v 1.1 2009/10/13 13:12:34 poluekt Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Particle.h"

// local
#include "StrippingCheck.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingCheck
//
// 2009-02-06 : Anton Polouektov
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingCheck::StrippingCheck( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation", m_inputLocation);
}

//=============================================================================
// Destructor
//=============================================================================
StrippingCheck::~StrippingCheck() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingCheck::execute()
{
  const bool selected = ( numberOfCandidates(m_inputLocation) > 0 );

  setFilterPassed( selected );

  return StatusCode::SUCCESS;
}

unsigned int 
StrippingCheck::numberOfCandidates(const std::string& selalgo) const
{
  const LHCb::Particle::Range parts = getIfExists<LHCb::Particle::Range>(selalgo);
  const unsigned int num = parts.size();
  
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Selection " << selalgo << " finds " << num
              << " candidates" << endmsg ;

  return num;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingCheck )

//=============================================================================

