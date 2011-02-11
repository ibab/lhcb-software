
/** @file BuildMCRichDigitLinks.cpp
 *
 *  Implementation file for RICH DAQ algorithm : BuildMCRichDigitLinks
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "BuildMCRichDigitLinks.h"

// namespace
using namespace Rich::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BuildMCRichDigitLinks )

//=============================================================================
// Standard constructor
//=============================================================================
BuildMCRichDigitLinks::BuildMCRichDigitLinks( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Rich::AlgBase ( name, pSvcLocator )
{
  // Define Job options for this algorithm
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = LHCb::MCRichDigitLocation::Default );
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLocation   = LHCb::RichDigitLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
BuildMCRichDigitLinks::~BuildMCRichDigitLinks() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCRichDigitLinks::execute() 
{
  debug() << "Execute" << endreq;

  // locate MCRichDigits
  LHCb::MCRichDigits * mcDigits = get<LHCb::MCRichDigits>( m_mcRichDigitsLocation );
  debug() << "Successfully located " << mcDigits->size()
          << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;

  // locate RichDigits
  LHCb::RichDigits * digits = get<LHCb::RichDigits>( m_richDigitsLocation );
  debug() << "Successfully located " << digits->size()
          << " RichDigits at " << m_richDigitsLocation << endreq;

  // build the MC links
  const StatusCode sc = setMCTruth(digits,mcDigits);
  if ( sc.isFailure() ) 
  {
    return Warning("Failed to build MC links for RichDigits",sc);
  }

  return sc;
}

//=============================================================================
