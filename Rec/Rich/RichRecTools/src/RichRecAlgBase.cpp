// $Id: RichRecAlgBase.cpp,v 1.1 2002-11-14 13:54:23 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "RichRecTools/RichRecAlgBase.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : Algorithm ( name, pSvcLocator ) {

  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "ChronoTiming", m_timing = false );

}

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  MsgStream log(msgSvc(), name());

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to Rich Pixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool ) ) {
    log << MSG::ERROR << "Unable to retrieve RichRecPixelTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Track Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool ) ) {
    log << MSG::ERROR << "Unable to retrieve RichRecTrackTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Photon Tool
  if ( !toolSvc()->retrieveTool( "RichRecPhotonTool", m_richRecPhotonTool ) ) {
    log << MSG::ERROR << "Unable to retrieve RichRecPhotonTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Segment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ){
    log << MSG::ERROR << "Unable to retrieve RichRecSegmentTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Detector Tool
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    log << MSG::ERROR << "Unable to retrieve RichDetInterface" << endreq;
    sc = StatusCode::FAILURE;
  }

  // ChronoStat timing
  if ( !serviceLocator()->service( "ChronoStatSvc", m_chrono, true ) ) {
    log << MSG::ERROR << "ChronoStatSvc not found" << endreq;
    m_timing = false;
    sc = StatusCode::FAILURE;
  }

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;

  return sc;
};

// Execute
StatusCode RichRecAlgBase::execute() { return StatusCode::SUCCESS; }

StatusCode RichRecAlgBase::richTracks() {

  MsgStream log(msgSvc(), name());

  // Update RichRecTracks pointer
  m_richTracks = m_richRecTrackTool->richTracks();
  if ( !m_richTracks ) {
    log << MSG::WARNING << "Failed to locate RichRecTracks" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << m_richTracks->size()
        << " RichRecTracks" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richPixels() {

  MsgStream log(msgSvc(), name());

  // Update RichRecPixels pointer
  m_richPixels = m_richRecPixelTool->richPixels();
  if ( !m_richPixels ) {
    log << MSG::WARNING << "Failed to locate RichRecPixels" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << m_richPixels->size()
        << " RichRecPixels" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richSegments() {

  MsgStream log(msgSvc(), name());

  // Update RichRecSegments pointer
  m_richSegments = m_richRecSegmentTool->richSegments();
  if ( !m_richSegments ) {
    log << MSG::WARNING << "Failed to locate RichRecSegments" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << m_richSegments->size()
        << " RichRecSegments" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richPhotons() {

  MsgStream log(msgSvc(), name());

  // Update RichRecPhotons pointer
  m_richPhotons = m_richRecPhotonTool->richPhotons();
  if ( !m_richPhotons ) {
    log << MSG::WARNING << "Failed to locate RichRecPhotons" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << m_richPhotons->size()
        << " RichRecPhotons" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::trTracks() {

  MsgStream log(msgSvc(), name());

  // Obtain smart data pointer to TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    log << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << tracks->size()
        << " TrStoredTracks at " << m_trTracksLocation << endreq;
    m_trTracks = tracks;
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  return StatusCode::SUCCESS;
}
