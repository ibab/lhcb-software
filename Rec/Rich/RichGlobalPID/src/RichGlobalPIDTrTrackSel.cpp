// $Id: RichGlobalPIDTrTrackSel.cpp,v 1.10 2003-11-25 13:51:24 jonesc Exp $
// Include files

// local
#include "RichGlobalPIDTrTrackSel.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDTrTrackSel
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDTrTrackSel>          s_factory ;
const        IAlgFactory& RichGlobalPIDTrTrackSelFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDTrTrackSel::RichGlobalPIDTrTrackSel( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  // Selection cuts
  declareProperty( "MinimumPhysicsMomentum",  m_minPhysPtot = 2.0*GeV );
  declareProperty( "MinimumLikelihoodMomentum", m_minLLPtot = 1.0*GeV );
  declareProperty( "ResetTracksToPion", m_resetToPion = false );
  declareProperty( "MaxUsedTracks", m_maxUsedTracks = 250 );
  declareProperty( "MaxInputTracks", m_maxInputTracks = 350 );
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichGlobalPIDTrTrackSel::~RichGlobalPIDTrTrackSel() {}

//  Initialize
StatusCode RichGlobalPIDTrTrackSel::initialize() {

  MsgStream msg(msgSvc(), name());

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  // Acquire tools
  acquireTool( "RichTrackCreator",        m_trackCr  );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Track types selected         = " << m_trSelector.selectedTrackTypes() << endreq
      << " Max RICH selected Tracks     = " << m_maxUsedTracks << endreq
      << " Min Physics Momentum         = " << m_minPhysPtot << " MeV/c" << endreq
      << " Min LogL Momentum            = " << m_minLLPtot << " MeV/c" << endreq;
  if ( m_resetToPion ) msg << " Resetting track hypotheses to Pion" << endreq;

  return StatusCode::SUCCESS;
}

// Select tracks for analysis
StatusCode RichGlobalPIDTrTrackSel::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidTracks() || !gpidPIDs() ) return StatusCode::FAILURE;

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
    richStatus()->setEventOK( false );  // set event status false
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  // Check the number of input raw tracks
  if ( m_trackCr->nInputTracks() > m_maxInputTracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO
        << "Found " << m_trackCr->nInputTracks() << ">" << m_maxInputTracks
        << " max input tracks -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedTrTrackLimit ); 
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  // Make sure RichRecTracks are available
  if ( !m_trackCr->newTracks() ) return StatusCode::FAILURE;
  if ( richTracks()->empty() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO << "No tracks selected -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichTracks );
    deleteEvent();
    return StatusCode::SUCCESS;
  } else if ( richTracks()->size() > m_maxUsedTracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO
        << "Found " << richTracks()->size() << ">" << m_maxUsedTracks
        << " max usable tracks -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedRichTrackLimit );
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  // Initialise track count
  std::vector<int> nTracks(Rich::Track::NTrTypes,0);

  // Iterate over all RichRecTracks and choose those to use
  for ( RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ) {

    // select tracks for use
    Rich::GlobalPID::TkQuality quality = trackStatus( *track );
    if ( Rich::GlobalPID::Unusable == quality ) {
      (*track)->setInUse( false );
      continue;
    } else {
      (*track)->setInUse( true );
    }

    // Set to pion hypothesis if configured to do so
    if ( m_resetToPion ) (*track)->setCurrentHypothesis( Rich::Pion );
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::VERBOSE
          << "Track " << (*track)->key() << " has initial hypothesis " 
          << (*track)->currentHypothesis() << endreq;
    }

    // Make a new RichGlobalPIDTrack
    RichGlobalPIDTrack * pidTrack = new RichGlobalPIDTrack();
    m_GPIDtracks->insert( pidTrack, (*track)->key() );

    // Make new PID result and give to track
    RichGlobalPID * newPID = new RichGlobalPID();
    m_GPIDs->insert( newPID, (*track)->key() );

    // set track reference
    pidTrack->setGlobalPID( newPID );

    // Set TrStoredTrack reference
    newPID->setRecTrack(dynamic_cast<TrStoredTrack*>((*track)->parentTrack()));

    // Store threshold information
    m_tkSignal->setThresholdInfo( (*track), newPID );

    // Set quality
    pidTrack->setTrQuality( quality );

    // Set its SmartRef to RichRecTrack
    pidTrack->setRichRecTrack( *track );

    // Count used track types
    ++nTracks[(*track)->trackID().trackType()];

  } // track loop

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG 
        << "Selected " << m_GPIDtracks->size() << " Tracks :"
        << " VTT=" << nTracks[Rich::Track::VeloTT]
        << ", Forwd=" << nTracks[Rich::Track::Forward]
        << ", Seed=" << nTracks[Rich::Track::Seed]
        << ", Match=" << nTracks[Rich::Track::Match]
        << ", Upstrm=" << nTracks[Rich::Track::UpStream]
        << endreq;
  }

  if ( m_GPIDtracks->empty() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::INFO 
        << "No tracks selected -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichTracks );
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

Rich::GlobalPID::TkQuality
RichGlobalPIDTrTrackSel::trackStatus( RichRecTrack * track ) {

  // Set default quality to be good
  Rich::GlobalPID::TkQuality quality = Rich::GlobalPID::Physics;

  // Only use requested track types
  if ( !m_trSelector.trackSelected(track) ) quality = Rich::GlobalPID::Unusable;

  // Momentum
  double pTot = track->vertexMomentum();

  // If below minimum momentum for use in LL, return false
  if ( pTot < m_minLLPtot ) return Rich::GlobalPID::Unusable;

  // Likelihood use only ?
  if ( pTot >= m_minLLPtot && pTot < m_minPhysPtot ) {
    quality = Rich::GlobalPID::LikelihoodOnly;
  }

  return quality;
}

void RichGlobalPIDTrTrackSel::deleteEvent() 
{
  // Tidy up incase of event abort
  if ( m_GPIDtracks && !m_GPIDtracks->empty() ) m_GPIDtracks->clear();
  if ( m_GPIDs      && !m_GPIDs->empty()      ) m_GPIDs->clear();
}

//  Finalize
StatusCode RichGlobalPIDTrTrackSel::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_trackCr  );
  releaseTool( m_tkSignal );

  // Execute base class method
  return RichRecAlgBase::finalize();
}
