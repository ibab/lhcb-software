// $Id: RichRecRecoPixels.cpp,v 1.1.1.1 2002-07-28 10:46:20 jonesc Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "RichRecTools/RichRecRecoPixels.h"

// RichKernel
#include "RichKernel/RichSmartID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecRecoPixels
//
// 20/03/2002 : Christopher Rob Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecRecoPixels>          s_factory ;
const        IAlgFactory& RichRecRecoPixelsFactory = s_factory ; 


// Standard constructor, initializes variables
RichRecRecoPixels::RichRecRecoPixels( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

  // Define Job options for this algorithm
  // Location of input RichDigits in TDS. Default location is defined in RichDigit.h
  declareProperty( "RecoDigitsLocation", 
                   m_recoDigitsLocation = RichDigitLocation::Default );
  // Location of output RichRecDigits in TDS. Default location is defined in RichRecPixel.h
  declareProperty( "RichRecPixelsLocation", 
                   m_richRecPixelsLocation = RichRecPixelLocation::Default );

}

// Destructor
RichRecRecoPixels::~RichRecRecoPixels() {}; 


// Initialisation.
StatusCode RichRecRecoPixels::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  // Get pointer to Rich Detector Tool
  StatusCode scDet = toolSvc()->retrieveTool( "RichDetInterface",
                                              m_richDetInterface );
  if ( scDet.isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve RichDetInterface" << endreq;
  } else {
    log << MSG::DEBUG << "Successfully retrieved RichDetInterface" << endreq;
  }

  // Get pointer to PixelFinder Tool (temporary for SICB data)
  StatusCode scPixelF = toolSvc()->retrieveTool( "PixelFinder",
                                                 m_pixelFinder );
  if ( scPixelF.isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve PixelFinder" << endreq;
  } else {
    log << MSG::DEBUG << "Successfully retrieved PixelFinder" << endreq;
  }

  return (scDet && scPixelF);
};


// Main execution
StatusCode RichRecRecoPixels::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "Execute" << endreq;

  // Obtain smart data pointer to reco pixels
  SmartDataPtr<RichDigits> recoDigits(eventSvc(),m_recoDigitsLocation);
  if ( ! recoDigits ) { 
    log << MSG::ERROR << "Failed to locate digits at "
        << m_recoDigitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << recoDigits->size()
        << " digits at " << m_recoDigitsLocation << endreq;
  }

  // Form a new container of RichRecPixels
  RichRecPixels * richRecPixels = new RichRecPixels();

  // Iterate over all digits and form RichRecPixels
  for ( RichDigits::iterator recoDigit = recoDigits->begin();
        recoDigit != recoDigits->end();
        ++recoDigit ) {

    // Make a new RichRecTrack
    RichRecPixel * newPixel = new RichRecPixel();

    // set pixel position
    RichSmartID id = (*recoDigit)->key();
    if ( id ) { 
      // For the moment use SICB geometry and not Xml
      // HepPoint3D xmlPosition;
      // m_richDetInterface->globalPosition( id, xmlPosition);
      // Scale by 10 to convert to mm
      newPixel->setPosition( 10.0*m_pixelFinder->globalPosition(id) );
    } else {
      log << MSG::WARNING << "RichDigit has null RichSmartID" << endreq;
    }

    // Set parent information
    newPixel->setParentPixel( *recoDigit );
    newPixel->setParentType( Rich::RecPixel::Digit );

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
StatusCode RichRecRecoPixels::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}
