// $Id: BuildMCRichDigitLinks.cpp,v 1.4 2004-04-19 22:55:07 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "BuildMCRichDigitLinks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BuildMCRichDigitLinks
//
// 2003-11-09 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BuildMCRichDigitLinks>          s_factory ;
const        IAlgFactory& BuildMCRichDigitLinksFactory = s_factory ;

//=============================================================================
// Standard constructor
//=============================================================================
BuildMCRichDigitLinks::BuildMCRichDigitLinks( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) {

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
StatusCode BuildMCRichDigitLinks::initialize() {

  debug() << "Initialise" << endreq;

  // intialise base
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
};

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
StatusCode BuildMCRichDigitLinks::finalize() {

  debug() << "Finalize" << endreq;

  // finalise base
  return RichAlgBase::finalize();
}

//=============================================================================
