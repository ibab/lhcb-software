
/** @file BuildMCRichDigitLinks.cpp
 *
 *  Implementation file for RICH DAQ algorithm : BuildMCRichDigitLinks
 *
 *  CVS Log :-
 *  $Id: BuildMCRichDigitLinks.cpp,v 1.2 2006-01-23 13:52:07 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "BuildMCRichDigitLinks.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BuildMCRichDigitLinks>          s_factory ;
const        IAlgFactory& BuildMCRichDigitLinksFactory = s_factory ;

//=============================================================================
// Standard constructor
//=============================================================================
BuildMCRichDigitLinks::BuildMCRichDigitLinks( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) 
{
  // Define Job options for this algorithm
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLocation = RichDigitLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
BuildMCRichDigitLinks::~BuildMCRichDigitLinks() {};

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode BuildMCRichDigitLinks::initialize() 
{
  // intialise base
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCRichDigitLinks::execute() {

  debug() << "Execute" << endreq;

  // locate MCRichDigits
  MCRichDigits * mcDigits = get<MCRichDigits>( m_mcRichDigitsLocation );
  debug() << "Successfully located " << mcDigits->size()
          << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;

  // locate RichDigits
  RichDigits * digits = get<RichDigits>( m_richDigitsLocation );
  debug() << "Successfully located " << digits->size()
          << " RichDigits at " << m_richDigitsLocation << endreq;

  // build the MC links
  if ( setMCTruth(digits,mcDigits).isFailure() ) {
    return Warning("Failed to build MC links for RichDigits");
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BuildMCRichDigitLinks::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}

//=============================================================================
