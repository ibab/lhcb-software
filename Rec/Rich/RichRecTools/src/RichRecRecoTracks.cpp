// $Id: RichRecRecoTracks.cpp,v 1.1.1.1 2002-07-28 10:46:20 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichRecTools/RichRecRecoTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecRecoTracks
//
// 20/03/2002 : Christopher Rob Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecRecoTracks>          s_factory ;
const        IAlgFactory& RichRecRecoTracksFactory = s_factory ;


// Standard constructor, initializes variables
RichRecRecoTracks::RichRecRecoTracks( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

  // Define Job options for this algorithm
  // Location of input TrStoredTracks in TDS. Currently hardcoded until New code exists
  declareProperty( "RecoTracksLocation",
                   m_recoTracksLocation = TrStoredTrackLocation::Default );
  // Location of output RichRecTracks in TDS. Default location is defined in RichRecTrack.h
  declareProperty( "RichRecTracksLocation",
                   m_richRecTracksLocation = RichRecTrackLocation::Default );
  // Location of output RichRecSegments in TDS
  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );

}

// Destructor
RichRecRecoTracks::~RichRecRecoTracks() {};


// Initialisation
StatusCode RichRecRecoTracks::initialize() {

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

  // Issue health warning
  log << MSG::WARNING << "Currently only using 'forward' tracks" << endreq;

  return scDet;
};


// Main execution
StatusCode RichRecRecoTracks::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "Execute" << endreq;

  // Obtain smart data pointer to reco tracks
  SmartDataPtr<TrStoredTracks> recoTracks(eventSvc(),m_recoTracksLocation);
  if ( ! recoTracks ) {
    log << MSG::ERROR << "Failed to locate tracks at "
        << m_recoTracksLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << recoTracks->size()
        << " tracks at " << m_recoTracksLocation << endreq;
  }

  // Form a new container of RichRecTracks
  RichRecTracks * richRecTracks = new RichRecTracks();

  // Form a new container of RichRecSegments
  RichRecSegments * richRecSegments = new RichRecSegments();

  // Iterate over all reco tracks, and create new RichRecTracks
  for (TrStoredTracks::iterator recoTrack = recoTracks->begin();
       recoTrack != recoTracks->end();
       ++recoTrack) {

    // For time being only use forward tracks
    // This will change when RichDetTools can properly handle tracks
    // with 1,2 or 4 states
    if ( 0 == (*recoTrack)->forward() ) { continue; }

    // Find TrStoredTrack current state and momentum
    SmartRef<TrState> trackState = (*recoTrack)->closestState(-999999.);
    TrStateP * trackPState = dynamic_cast<TrStateP*>( (TrState*)trackState );
    if ( !trackPState ) { continue; } // Skip tracks with no Pstates

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    int Nsegs = m_richDetInterface->constructSegments( **recoTrack, segments );
    log << MSG::VERBOSE << "Found " << Nsegs << " track segment(s) for track "
        << (*recoTrack)->key() << endreq;
    if ( 0 >= Nsegs ) { continue; }

    // Make a new RichRecTrack
    RichRecTrack * newTrack = new RichRecTrack();

    for ( std::vector<RichTrackSegment>::const_iterator iSeg = segments.begin();
          iSeg != segments.end();
          ++iSeg ) {
      
      // Form new RichRecSegment
      RichRecSegment * newSegment = new RichRecSegment();

      // Set TrackSegment
      newSegment->setTrackSegment( *iSeg );

      // Set associated RichRecTrack
      newSegment->setAssociatedRichRecTrack( newTrack );

      // Add to the SmartRefVector of RichSegments for this RichRecTrack
      newTrack->addToRadiatorSegments( newSegment );

      // insert this object into container
      richRecSegments->insert( newSegment );
      
    }

    // Set vertex momentum
    if ( trackPState ) { newTrack->setVertexMomentum( trackPState->p() ); }

    // Set parent information
    newTrack->setParentTrack( *recoTrack );
    newTrack->setParentType( Rich::RecTrack::TrStoredTrack );
    
    // Insert track into container with default key
    richRecTracks->insert(newTrack);

  }

  // Register new RichRecTrack container to Gaudi data store
  StatusCode registerRichTracks =
    eventSvc()->registerObject(m_richRecTracksLocation,richRecTracks);
  if ( registerRichTracks.isFailure() ) {
    log << MSG::ERROR << "Failed to register RichRecTracks at "
        << m_richRecTracksLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully registered " << richRecTracks->size()
        << " RichRecTracks at " << m_richRecTracksLocation << endreq;
  }

  // Register new RichRecSegment container to Gaudi data store
  StatusCode registerRichSegments =
    eventSvc()->registerObject(m_richRecSegmentLocation, richRecSegments);
  if ( registerRichSegments.isFailure() ) {
    log << MSG::ERROR << "Failed to register RichRecSegments at "
        << m_richRecSegmentLocation << endreq;
  } else {
    log << MSG::DEBUG << "Successfully registered " << richRecSegments->size()
        << " RichRecSegments at " << m_richRecSegmentLocation << endreq;
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecRecoTracks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}
