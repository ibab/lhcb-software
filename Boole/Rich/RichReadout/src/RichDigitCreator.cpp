// $Id: RichDigitCreator.cpp,v 1.7 2004-03-16 13:51:42 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "Kernel/RichSmartID.h"

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
  : RichAlgBase ( name, pSvcLocator ) {

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

  debug() << "Initialize" << endreq;

  // Initialize base class
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigitCreator::execute() {

  debug() << "Execute" << endreq;

  // locate MCRichDigits
  MCRichDigits * mcDigits = get<MCRichDigits>( m_mcRichDigitsLocation );

  // Form a new container of RichDigits
  RichDigits * digits = new RichDigits();

  // Iterate over all MCRichDigits and copy to RichDigits
  if ( mcDigits ) {
    for (MCRichDigits::const_iterator mcRichDigit = mcDigits->begin();
         mcRichDigit != mcDigits->end();
         ++mcRichDigit) {

      // Make a new RichDigit
      RichDigit * newDigit = new RichDigit();
      digits->insert( newDigit, (*mcRichDigit)->key() );

      // Set MCTruth
      if ( setMCTruth(newDigit, *mcRichDigit).isFailure() ) {
        warning() << "Failed to set MCTruth for RichDigit "
                  << (*mcRichDigit)->key() << endreq;
      }

    }
  }

  // Register new container to Gaudi data store
  put( digits, m_richDigitsLocation );

  return StatusCode::SUCCESS;
};


//  Finalize
StatusCode RichDigitCreator::finalize() {

  debug() << "Finalize" << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}
