// $Id: RichDigitCreator.cpp,v 1.3 2003-09-26 16:00:03 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// RichKernel
#include "RichKernel/RichSmartID.h"

// local
#include "RichDigitCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigitCreator
//
// 22/03/2002 : Christopher Rob Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitCreator>          s_factory ;
const        IAlgFactory& RichDigitCreatorFactory = s_factory ;

// Standard constructor, initializes variables
RichDigitCreator::RichDigitCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  // Define Job options for this algorithm
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLocation = RichDigitLocation::Default );
}

// Destructor
RichDigitCreator::~RichDigitCreator() {};

// Initialisation.
StatusCode RichDigitCreator::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigitCreator::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "Execute" << endreq;

  // locate MCRichDigits
  SmartDataPtr<MCRichDigits> mcDigits( eventSvc(),
                                       m_mcRichDigitsLocation );
  if ( !mcDigits ) {
    log << MSG::WARNING << "Cannot locate MCRichDigits at "
        << m_mcRichDigitsLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << mcDigits->size()
        << " MCRichDigits at "
        << m_mcRichDigitsLocation << endreq;
  }

  // Form a new container of RichDigits
  RichDigits * digits = new RichDigits();

  // Iterate over all MCRichDigits and copy to RichDigits
  if ( mcDigits ) {
    for ( MCRichDigits::const_iterator mcRichDigit = mcDigits->begin();
          mcRichDigit != mcDigits->end();
          ++mcRichDigit ) {
      
      // Make a new RichDigit
      RichDigit * newDigit = new RichDigit();
      digits->insert( newDigit, (*mcRichDigit)->key() );
      
      // Set MCTruth
      if ( setMCTruth(newDigit, *mcRichDigit).isFailure() ) {
        log << MSG::WARNING << "Failed to set MCTruth for RichDigit "
            << (*mcRichDigit)->key() << endreq;
      }
      
    }
  }
  
  // Register new container to Gaudi data store
  if ( !eventSvc()->registerObject(m_richDigitsLocation,digits) ) {
    log << MSG::ERROR << "Failed to register RichDigits at "
        << m_richDigitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully registered " << digits->size()
        << " RichDigits at " << m_richDigitsLocation << endreq;
  }

  return StatusCode::SUCCESS;
};


//  Finalize
StatusCode RichDigitCreator::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}
