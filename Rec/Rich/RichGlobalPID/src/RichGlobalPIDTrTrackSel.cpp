// $Id: RichGlobalPIDTrTrackSel.cpp,v 1.7 2003-08-06 10:02:10 jonrob Exp $
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
  declareProperty( "MaxTrTracks", m_maxTrTracks = 400 );
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );
  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );

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
      << " Max total TrStoredTracks     = " << m_maxTrTracks << endreq
      << " Max RICH selected Tracks     = " << m_maxUsedTracks << endreq
      << " Min Physics Momentum         = " << m_minPhysPtot << " MeV/c" << endreq
      << " Min LogL Momentum            = " << m_minLLPtot << " MeV/c" << endreq;
  if ( m_resetToPion ) msg << " Resetting track hypotheses to Pion" << endreq;

  return StatusCode::SUCCESS;
}

// Select tracks for analysis
StatusCode RichGlobalPIDTrTrackSel::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidTracks() || !gpidPIDs() ) return StatusCode::FAILURE;

  // Check if track processing was aborted.
  SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
  if ( !procStat ) {
    msg << MSG::WARNING << "Failed to locate ProcStatus at "
        << m_procStatLocation << endreq;
    return StatusCode::FAILURE;
  }
  if ( procStat->aborted() ) {
    msg << MSG::INFO
        << "Processing aborted -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ProcStatAbort );
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  // check number of TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    msg << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
    return StatusCode::FAILURE;
  }
  if ( tracks->size() > m_maxTrTracks ) {
    msg << MSG::WARNING
        << "Found " << tracks->size() << ">" << m_maxTrTracks
        << " max TrStoredTracks -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedTrTrackLimit ); 
    deleteEvent();
    return StatusCode::SUCCESS;
  }

  // Make sure all rich tracks are "turned off" before proceeding
  // should perhaps make a method of the track tool ?
  if ( !RichRecAlgBase::richTracks() ) return StatusCode::FAILURE;
  if ( !richTracks()->empty() ) {
    for ( RichRecTracks::iterator track = richTracks()->begin();
          track != richTracks()->end();
          ++track ) {
      (*track)->setInUse( false );
    }
  }

  // Initialise track counts
  m_nVeloTk[0] = 0;
  m_nVeloBackTk[0] = 0;
  m_nNonUniqueTk[0] = 0;
  m_nSeedTk[0] = 0;
  m_nMatchTk[0] = 0;
  m_nForwardTk[0] = 0;
  m_nUpstreamTk[0] = 0;
  m_nVeloTTTk[0] = 0;
  m_nVeloTk[1] = 0;
  m_nVeloBackTk[1] = 0;
  m_nNonUniqueTk[1] = 0;
  m_nSeedTk[1] = 0;
  m_nMatchTk[1] = 0;
  m_nForwardTk[1] = 0;
  m_nUpstreamTk[1] = 0;
  m_nVeloTTTk[1] = 0;

  // Iterate over all TrStoredTracks and choose those to use
  for ( TrStoredTracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end();
        ++iTrack) {
    TrStoredTrack * trTrack = *iTrack;
    if ( !trTrack ) continue;

    // Count total track types
    if ( !trTrack->unique()   ) ++m_nNonUniqueTk[1];
    if ( trTrack->unique() ) {
      if (  trTrack->velo()     ) ++m_nVeloTk[1];
      if (  trTrack->veloBack() ) ++m_nVeloBackTk[1];
      if (  trTrack->seed()     ) ++m_nSeedTk[1];
      if (  trTrack->match()    ) ++m_nMatchTk[1];
      if (  trTrack->forward()  ) ++m_nForwardTk[1];
      if (  trTrack->upstream() ) ++m_nUpstreamTk[1];
      if (  trTrack->veloTT()   ) ++m_nVeloTTTk[1];
    }

    // select tracks for use
    Rich::GlobalPID::TkQuality quality = trackStatus( trTrack );
    if ( Rich::GlobalPID::Unusable == quality ) continue;

    RichRecTrack * track = m_trackCr->newTrack( trTrack );
    if ( !track ) {
      continue;
    } else {
      track->setInUse( true );
    }

    // Set to pion hypothesis if configured to do so
    if ( m_resetToPion ) track->setCurrentHypothesis( Rich::Pion );

    // Make a new RichGlobalPIDTrack
    RichGlobalPIDTrack * pidTrack = new RichGlobalPIDTrack();
    m_GPIDtracks->insert( pidTrack, track->key() );

    // Make new PID result and give to track
    RichGlobalPID * newPID = new RichGlobalPID();
    m_GPIDs->insert( newPID, track->key() );

    // set track reference
    pidTrack->setGlobalPID( newPID );

    // Set TrStoredTrack reference
    newPID->setRecTrack( trTrack );

    // Store threshold information
    m_tkSignal->setThresholdInfo( track, newPID );

    // Set quality
    pidTrack->setTrQuality( quality );

    // Set its SmartRef to RichRecTrack
    pidTrack->setRichRecTrack( track );

    // Count used track types
    if ( !trTrack->unique()    ) ++m_nNonUniqueTk[0];
    if ( trTrack->unique() ) {
      if ( trTrack->velo()     ) ++m_nVeloTk[0];
      if ( trTrack->veloBack() ) ++m_nVeloBackTk[0];
      if ( trTrack->seed()     ) ++m_nSeedTk[0];
      if ( trTrack->match()    ) ++m_nMatchTk[0];
      if ( trTrack->forward()  ) ++m_nForwardTk[0];
      if ( trTrack->upstream() ) ++m_nUpstreamTk[0];
      if ( trTrack->veloTT()   ) ++m_nVeloTTTk[0];
    }

  } // track loop

  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG << "Selected " << m_GPIDtracks->size() << "/"
        << tracks->size() << " TrStoredTracks :"
        << " Velo " << m_nVeloTk[0] << "/" << m_nVeloTk[1]
        << ", VeloBck " << m_nVeloBackTk[0] << "/" << m_nVeloBackTk[1]
        << ", VTT " << m_nVeloTTTk[0] << "/" << m_nVeloTTTk[1]
        << ", Forwd " << m_nForwardTk[0] << "/" << m_nForwardTk[1]
        << ", Seed " << m_nSeedTk[0] << "/" << m_nSeedTk[1]
        << ", Match " << m_nMatchTk[0] << "/" << m_nMatchTk[1]
        << ", Upstrm " << m_nUpstreamTk[0] << "/" << m_nUpstreamTk[1]
        << ", NonUniq " << m_nNonUniqueTk[0] << "/" << m_nNonUniqueTk[1]
        << endreq;
  }
  
  if ( m_GPIDtracks->empty() ) {
    msg << MSG::INFO << "No tracks selected -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichTracks );
    deleteEvent();
  } else if ( m_maxUsedTracks < m_GPIDtracks->size() ) {
    msg << MSG::WARNING
        << "Found " << m_GPIDtracks->size() << ">" << m_maxUsedTracks
        << " max usable tracks -> RICH Global PID aborted" << endreq;
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedRichTrackLimit );
    deleteEvent();
  }

  return StatusCode::SUCCESS;
}

Rich::GlobalPID::TkQuality
RichGlobalPIDTrTrackSel::trackStatus( TrStoredTrack * trTrack ) {

  // Only use requested track types
  if ( !m_trSelector.trackSelected(trTrack) ) return Rich::GlobalPID::Unusable;

  // Find TrStoredTrack current state and momentum
  SmartRef<TrState> trackState = trTrack->closestState(-999999.);
  TrStateP * trackPState = dynamic_cast<TrStateP*>( (TrState*)trackState );
  if ( !trackPState ) {
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::VERBOSE << "Track has no TrStateP" << endreq;
    }
    return Rich::GlobalPID::Unusable;
  }
  float pTot = trackPState->p();

  // Set default quality to be good
  Rich::GlobalPID::TkQuality quality = Rich::GlobalPID::Physics;

  // If below minimum momentum for use in LL, return false
  if ( pTot < m_minLLPtot ) return Rich::GlobalPID::Unusable;

  // Likelihood use only ?
  if ( pTot >= m_minLLPtot && pTot < m_minPhysPtot ) {
    quality = Rich::GlobalPID::LikelihoodOnly;
  }

  return quality;
}

void RichGlobalPIDTrTrackSel::deleteEvent() {

  // set event status false
  richStatus()->setEventOK( false );

  // Tidy up incase of event abort
  if ( m_GPIDtracks && !m_GPIDtracks->empty() ) m_GPIDtracks->clear();
  if ( m_GPIDs      && !m_GPIDs->empty()      ) m_GPIDs->clear();

}

//  Finalize
StatusCode RichGlobalPIDTrTrackSel::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_trackCr );
  releaseTool( m_tkSignal );

  // Execute base class method
  return RichRecAlgBase::finalize();
}
