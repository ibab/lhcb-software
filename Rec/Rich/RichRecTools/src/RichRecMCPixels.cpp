// $Id: RichRecMCPixels.cpp,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichRecTools/RichRecMCPixels.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecMCPixels
//
// 20/03/2002 : Christopher Rob Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecMCPixels>          s_factory ;
const        IAlgFactory& RichRecMCPixelsFactory = s_factory ;


// Standard constructor, initializes variables
RichRecMCPixels::RichRecMCPixels( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

  // Define Job options for this algorithm
  // Location of input RichDigits in TES. Default location defined in RichDigit.h
  declareProperty( "MCHitsLocation",
                   m_mcHitsLocation = MCHitLocation::RiHits );
  // Location of output RichRecDigits in TES. Default location defined in RichRecPixel.h
  declareProperty( "RichRecPixelsLocation",
                   m_richRecPixelsLocation = RichRecPixelLocation::Default );

}

// Destructor
RichRecMCPixels::~RichRecMCPixels() {};


// Initialisation.
StatusCode RichRecMCPixels::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endreq;

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RichRecMCPixels::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::INFO << "Execute" << endreq;

  // Obtain smart data pointer to MC Hits
  SmartDataPtr<MCHits> mcHits(eventSvc(),m_mcHitsLocation);
  if ( ! mcHits ) {
    log << MSG::ERROR << "Failed to locate MC hits at "
        << m_mcHitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << mcHits->size()
        << " MCHits at " << m_mcHitsLocation << endreq;
  }

  // Form a new container of RichRecPixels
  RichRecPixels * richRecPixels = new RichRecPixels();

  // Iterate over all digits and form RichRecPixels
  for (MCHits::iterator mcHit = mcHits->begin();
       mcHit != mcHits->end();
       ++mcHit) {

    // Make a new RichRecTrack
    RichRecPixel * newPixel = new RichRecPixel();

    // Set parent information
    newPixel->setParentPixel( *mcHit );
    newPixel->setParentType( Rich::RecPixel::MCHit );

    // Hit position
    // CRJ :  New to review what position to use
    newPixel->setPosition( (*mcHit)->entry() );

    // Insert it into container with default key
    richRecPixels->insert(newPixel);

  }

  // Register new container to Gaudi data store
  StatusCode registerRichPixels =
    eventSvc()->registerObject(m_richRecPixelsLocation,richRecPixels);
  if ( registerRichPixels.isFailure() ) {
    log << MSG::ERROR << "Failed to register RichRecPixels at "
        << m_richRecPixelsLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully registered " << richRecPixels->size()
        << " RichRecPixels at " << m_richRecPixelsLocation << endreq;
  }

  return StatusCode::SUCCESS;
};


//  Finalize
StatusCode RichRecMCPixels::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}
