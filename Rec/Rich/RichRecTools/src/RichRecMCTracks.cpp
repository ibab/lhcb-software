// $Id: RichRecMCTracks.cpp,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
// Include files
#include <cmath>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "RichRecTools/RichRecMCTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecMCTracks
//
// 20/03/2002 : Christopher Rob Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecMCTracks>          s_factory ;
const        IAlgFactory& RichRecMCTracksFactory = s_factory ;


// Standard constructor, initializes variables
RichRecMCTracks::RichRecMCTracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

  // Define Job options for this algorithm
  // Location of input MCParticle in TDS.
  declareProperty( "MCParticleLocation",
                   m_mcParticleLocation = MCParticleLocation::Default );
  // Location of output RichRecTracks in TDS.
  declareProperty( "RichRecTracksLocation",
                   m_richRecTracksLocation = RichRecTrackLocation::Default );
  // Location of Geant radiator entrance and exit points in TDS.
  declareProperty( "MCRichRadiatorHitsLocation",
                   m_mcRichRadHitsLocation = MCRichRadiatorHitLocation::Default );
  // Location of output RichRecSegments in TDS
  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );

  // Selection cuts
  declareProperty( "MinimumMomentum", m_minMomentum = 1000*MeV );

}

// Destructor
RichRecMCTracks::~RichRecMCTracks() {};


// Initialisation
StatusCode RichRecMCTracks::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RichRecMCTracks::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "Execute" << endreq;

  // Obtain smart data pointer to MCParticles
  SmartDataPtr<MCParticles> mcParticles(eventSvc(),m_mcParticleLocation);
  if ( ! mcParticles ) {
    log << MSG::ERROR << "Failed to locate MCParticles at "
        << m_mcParticleLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << mcParticles->size()
        << " MCParticles at " << m_mcParticleLocation << endreq;
  }

  // Form a new container of RichRecTracks
  RichRecTracks * richRecTracks = new RichRecTracks();

  // Form a new container of RichRecSegments
  RichRecSegments * richRecSegments = new RichRecSegments();

  // Get SmartDataPointer to MC radiator hits
  SmartDataPtr<MCRichRadiatorHits> mcRichRadHits(eventSvc(),
                                                 m_mcRichRadHitsLocation);
  if ( ! mcRichRadHits ) {
    log << MSG::ERROR << "Failed to locate MCRichRadiatorHits at "
        << m_mcRichRadHitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << mcRichRadHits->size()
        << " MCRichRadiatorHits at " << m_mcRichRadHitsLocation << endreq;
  }

  // Iterate over MCParticles
  for ( MCParticles::const_iterator mcPart = mcParticles->begin();
        mcPart != mcParticles->end();
        ++mcPart) {

    // Track selection cuts
    double mcPtot = (*mcPart)->momentum().vect().mag();
    if ( mcPtot < m_minMomentum ) {
      log << MSG::VERBOSE << "Track " << (*mcPart)->index() 
          << " fails minimum momentum cut, P = " << mcPtot << " MeV/c" <<endreq;
      continue;
    }
    log << MSG::VERBOSE << "Track " << (*mcPart)->index()
        << " passess minimum momentum cut, P = " << mcPtot << " MeV/c" <<endreq;

    // Find Geant entrance and exit points in radiator volumes
    SmartRefVector<MCRichRadiatorHit> trackMCRadHits;
    for( MCRichRadiatorHits::const_iterator radHit = mcRichRadHits->begin();
         radHit != mcRichRadHits->end();
         ++radHit ) {
      if ( *mcPart == (*radHit)->mcParticle() ) {
        // Add this hit to vector of hits for this track
        trackMCRadHits.push_back(*radHit);
      }
    }

    // Reject tracks that have no passages through any rich radiator volume.
    if ( 0 == trackMCRadHits.size() ) {
      log << MSG::VERBOSE << "Track " << (*mcPart)->index() 
          << " has no radiator hits, track rejected" << endreq;
      continue;
    }
    log << MSG::VERBOSE << "Track " << (*mcPart)->index() << " has " 
        << trackMCRadHits.size() << " rich radiator hits" << endreq;

    // Make a new RichRecTrack
    RichRecTrack * newTrack = new RichRecTrack();

    // Set vertex momentum
    newTrack->setVertexMomentum( (*mcPart)->momentum().vect().mag() );

    // Set parent information
    newTrack->setParentTrack( *mcPart );
    newTrack->setParentType( Rich::RecTrack::MCParticle );

    // Form associated RichRecSegments
    for( SmartRefVector<MCRichRadiatorHit>::const_iterator radHit = 
           trackMCRadHits.begin();
         radHit != trackMCRadHits.end();
         ++radHit ) {
      
      // Form new RichRecSegment
      RichRecSegment * newSegment = new RichRecSegment();

      // Set entrance and exit points
      newSegment->trackSegment().setEntryPoint( (*radHit)->entryPoint() );
      newSegment->trackSegment().setExitPoint( (*radHit)->exitPoint() );
      newSegment->trackSegment().setMiddlePoint(0.5*((*radHit)->entryPoint()+
                                                     (*radHit)->exitPoint()) );
      
      // For time being set entrance directions equal to vector joining 
      // entrance and exit positions
      HepVector3D direction = (*radHit)->exitPoint() - (*radHit)->entryPoint();
      direction.setMag( (*radHit)->entryMomentum() );
      newSegment->trackSegment().setEntryMomentum( direction );
      newSegment->trackSegment().setExitMomentum( direction );

      // Need to set radiator and rich variables. Not done yet

      // Set associated RichRecTrack
      newSegment->setAssociatedRichRecTrack( newTrack );

      // insert this object into container
      richRecSegments->insert( newSegment );

      // Add to the SmartRefVector of RichSegments for this RichRecTrack
      newTrack->addToRadiatorSegments( newSegment );

    }

    // Insert it into container with default key
    richRecTracks->insert( newTrack );
  }

 
  StatusCode registerRichTracks =
    eventSvc()->registerObject(m_richRecTracksLocation, richRecTracks);
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

  return registerRichTracks && registerRichSegments ;
};

//  Finalize
StatusCode RichRecMCTracks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}
