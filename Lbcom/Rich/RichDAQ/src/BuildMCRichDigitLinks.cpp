// $Id: BuildMCRichDigitLinks.cpp,v 1.2 2003-11-25 14:10:12 jonrob Exp $
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialise" << endreq;

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCRichDigitLinks::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // locate MCRichDigits
  SmartDataPtr<MCRichDigits> mcDigits( eventSvc(), m_mcRichDigitsLocation );
  if ( !mcDigits ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::WARNING << "Cannot locate MCRichDigits at "
        << m_mcRichDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Successfully located " << mcDigits->size()
        << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;
  }

  // locate RichDigits
  SmartDataPtr<RichDigits> digits( eventSvc(), m_richDigitsLocation );
  if ( !digits ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::WARNING << "Cannot locate RichDigits at "
        << m_richDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Successfully located " << digits->size()
        << " RichDigits at " << m_richDigitsLocation << endreq;
  }

  MCRichDigits::iterator mcdigit = mcDigits->begin();
  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end(); ++digit, ++mcdigit ) {

    // Find MCRichDigit
    MCRichDigit * mcDigit = mcDigits->object( (*digit)->key() );
 
    // Set MCTruth
    if ( setMCTruth(*digit, mcDigit).isFailure() ) {
      MsgStream  msg( msgSvc(), name() );
      msg << MSG::WARNING << "Failed to set MCTruth for RichDigit "
          << (*digit)->key() << " MCRichDigit " << mcDigit << endreq;
    }

  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BuildMCRichDigitLinks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Finalize" << endreq;
  }

  // finalise base
  return RichAlgBase::finalize();
}

//=============================================================================
