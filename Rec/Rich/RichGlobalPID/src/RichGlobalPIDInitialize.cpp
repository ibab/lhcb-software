// $Id: RichGlobalPIDInitialize.cpp,v 1.3 2003-11-25 13:51:23 jonesc Exp $
// Include files

// local
#include "RichGlobalPIDInitialize.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDInitialize
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDInitialize>          s_factory ;
const        IAlgFactory& RichGlobalPIDInitializeFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDInitialize::RichGlobalPIDInitialize( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {}

// Destructor
RichGlobalPIDInitialize::~RichGlobalPIDInitialize() {}

//  Initialize
StatusCode RichGlobalPIDInitialize::initialize() {

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;
  
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDInitialize::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Set event status to OK for start of GlobalPID processing
  richStatus()->setEventOK( true );

  // Summary object
  RichGlobalPIDSummary * GPIDsummary = new RichGlobalPIDSummary();
  if ( !eventSvc()->registerObject( m_richGPIDSummaryLocation,
                                    GPIDsummary ) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register RichGlobalPIDSummary at "
        << m_richGPIDSummaryLocation << endreq;
    return StatusCode::FAILURE;
  }

  // RichGlobalPIDTrack container
  RichGlobalPIDTracks * GPIDtracks = new RichGlobalPIDTracks();
  if ( !eventSvc()->registerObject( m_richGPIDTrackLocation, GPIDtracks ) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register RichGlobalTracks at "
        << m_richGPIDTrackLocation << endreq;
    return StatusCode::FAILURE;
  }

  // RichGlobalPID container
  RichGlobalPIDs * globalPIDs = new RichGlobalPIDs();
  if ( !eventSvc()->registerObject( m_richGPIDLocation, globalPIDs ) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register RichGlobalPIDs at "
        << m_richGPIDLocation << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDInitialize::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
