// $Id: RichGlobalPIDDigitSel.cpp,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
// Include files

// local
#include "RichGlobalPIDDigitSel.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDDigitSel
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDDigitSel>          s_factory ;
const        IAlgFactory& RichGlobalPIDDigitSelFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDDigitSel::RichGlobalPIDDigitSel( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  // Maximum number of pixels
  declareProperty( "MaxUsedPixels", m_maxUsedPixels = 8000 );

}

// Destructor
RichGlobalPIDDigitSel::~RichGlobalPIDDigitSel() {}

//  Initialize
StatusCode RichGlobalPIDDigitSel::initialize() {

  MsgStream msg( msgSvc(), name() );

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  // Acquire tools
  acquireTool("RichPixelCreator", m_pixelCr);

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Max Pixels                   = " << m_maxUsedPixels << endreq;

  return StatusCode::SUCCESS;
}

// Initialise pixels
StatusCode RichGlobalPIDDigitSel::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Create all RichRecPixels
  if ( !m_pixelCr->newPixels() ) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG 
        << "Selected " << richPixels()->size() << " RichDigits" << endreq;
  }

  if ( m_maxUsedPixels < richPixels()->size() ) {
    msg << MSG::WARNING << "Found " << richPixels()->size()
        << " RichDigits -> RICH Global PID aborted" << endreq;
    if ( !procStatus() ) return StatusCode::FAILURE;
    procStatus()->addAlgorithmStatus(m_richGPIDName,Rich::Rec::ReachedPixelLimit);
    richStatus()->setEventOK( false );
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDDigitSel::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
