// $Id: RichGlobalPIDDigitSel.cpp,v 1.2 2003-07-02 09:02:59 jonrob Exp $
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

  // Job options
  declareProperty( "MaxUsedPixels", m_maxUsedPixels = 8000 );
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

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
    SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
    if ( !procStat ) {
      msg << MSG::WARNING << "Failed to locate ProcStatus at " 
          << m_procStatLocation << endreq;
      return StatusCode::FAILURE;
    }
    procStat->addAlgorithmStatus(m_richGPIDName,Rich::Rec::ReachedPixelLimit);
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
