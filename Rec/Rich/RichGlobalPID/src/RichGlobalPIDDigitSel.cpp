// $Id: RichGlobalPIDDigitSel.cpp,v 1.8 2004-02-02 14:25:53 jonesc Exp $
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

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Max Pixels                   = " << m_maxUsedPixels << endreq;

  return StatusCode::SUCCESS;
}

// Initialise pixels
StatusCode RichGlobalPIDDigitSel::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
  if ( !procStat ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Failed to locate ProcStatus at "
        << m_procStatLocation << endreq;
    return StatusCode::FAILURE;
  } else if ( procStat->aborted() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO
        << "Processing aborted -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ProcStatAbort );
    richStatus()->setEventOK( false );
    return StatusCode::SUCCESS;
  }

  // Create all RichRecPixels
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG 
        << "Selected " << richPixels()->size() << " RichDigits" << endreq;
  }

  if ( m_maxUsedPixels < richPixels()->size() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO << "Found " << richPixels()->size()
        << " RichDigits -> RICH Global PID aborted" << endreq;
    SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
    if ( !procStat ) {
      MsgStream msg( msgSvc(), name() );
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
