// $Id: RichRecAlgBase.cpp,v 1.1.1.1 2003-04-01 13:21:55 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

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

  // track selection
  m_trNames.push_back( "unique" );
  m_trNames.push_back( "seed" );
  m_trNames.push_back( "match" );
  m_trNames.push_back( "forward" );
  m_trNames.push_back( "upstream" );
  m_trNames.push_back( "veloTT" );
  declareProperty( "TrackSelection", m_trNames );

}

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  MsgStream msg(msgSvc(), name());

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to Rich Pixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool ) ) {
    msg << MSG::ERROR << "Unable to retrieve RichRecPixelTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Track Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool ) ) {
    msg << MSG::ERROR << "Unable to retrieve RichRecTrackTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Photon Tool
  if ( !toolSvc()->retrieveTool( "RichRecPhotonTool", m_richRecPhotonTool ) ) {
    msg << MSG::ERROR << "Unable to retrieve RichRecPhotonTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Segment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ){
    msg << MSG::ERROR << "Unable to retrieve RichRecSegmentTool" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to Rich Detector Tool
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "Unable to retrieve RichDetInterface" << endreq;
    sc = StatusCode::FAILURE;
  }

  // ChronoStat timing
  if ( !serviceLocator()->service( "ChronoStatSvc", m_chrono, true ) ) {
    msg << MSG::ERROR << "ChronoStatSvc not found" << endreq;
    m_timing = false;
    sc = StatusCode::FAILURE;
  }

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;

  // Initialise track bit selection
  m_trBits = 0;
  m_uniqueTrOnly = false;
  for ( std::vector<string>::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end();
        iName++ ) {
    if      ( *iName == "unique"   ) { m_uniqueTrOnly = true; }
    else if ( *iName == "velo"     ) { m_trBits += 2;   } 
    else if ( *iName == "seed"     ) { m_trBits += 4;   } 
    else if ( *iName == "match"    ) { m_trBits += 8;   } 
    else if ( *iName == "forward"  ) { m_trBits += 16;  } 
    else if ( *iName == "upstream" ) { m_trBits += 32;  } 
    else if ( *iName == "veloTT"   ) { m_trBits += 64;  }
    else if ( *iName == "veloBack" ) { m_trBits += 128; }
    else { 
      msg << MSG::ERROR << "Unknown track type " << *iName << endreq;
      sc = StatusCode::FAILURE;
    }
  }

  return sc;
};

// Execute
StatusCode RichRecAlgBase::execute() { return StatusCode::SUCCESS; }

StatusCode RichRecAlgBase::richTracks() {

  MsgStream msg(msgSvc(), name());

  // Update RichRecTracks pointer
  m_richTracks = m_richRecTrackTool->richTracks();
  if ( !m_richTracks ) {
    msg << MSG::WARNING << "Failed to locate RichRecTracks" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richPixels() {

  MsgStream msg(msgSvc(), name());

  // Update RichRecPixels pointer
  m_richPixels = m_richRecPixelTool->richPixels();
  if ( !m_richPixels ) {
    msg << MSG::WARNING << "Failed to locate RichRecPixels" << endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richSegments() {

  MsgStream msg(msgSvc(), name());

  // Update RichRecSegments pointer
  m_richSegments = m_richRecSegmentTool->richSegments();
  if ( !m_richSegments ) {
    msg << MSG::WARNING << "Failed to locate RichRecSegments" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::richPhotons() {

  MsgStream msg(msgSvc(), name());

  // Update RichRecPhotons pointer
  m_richPhotons = m_richRecPhotonTool->richPhotons();
  if ( !m_richPhotons ) {
    msg << MSG::WARNING << "Failed to locate RichRecPhotons" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecAlgBase::trTracks() {

  MsgStream msg(msgSvc(), name());

  // Obtain smart data pointer to TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    msg << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
    return StatusCode::FAILURE;
  }
  m_trTracks = tracks;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  // Release all tools
  if ( m_richRecPhotonTool  ) toolSvc()->releaseTool( m_richRecPhotonTool );
  if ( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if ( m_richRecPixelTool   ) toolSvc()->releaseTool( m_richRecPixelTool );
  if ( m_richRecTrackTool   ) toolSvc()->releaseTool( m_richRecTrackTool );
  if ( m_richDetInterface   ) toolSvc()->releaseTool( m_richDetInterface );

  return StatusCode::SUCCESS;
}
