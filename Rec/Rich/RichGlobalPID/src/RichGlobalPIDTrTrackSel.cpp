
/** @file RichGlobalPIDTrTrackSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDTrTrackSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrTrackSel.cpp,v 1.16 2004-07-27 10:56:37 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

// local
#include "RichGlobalPIDTrTrackSel.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDTrTrackSel>          s_factory ;
const        IAlgFactory& RichGlobalPIDTrTrackSelFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDTrTrackSel::RichGlobalPIDTrTrackSel( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ),
    m_tkSignal       ( 0 ),
    m_minPhysPtot    ( 0 ),
    m_minLLPtot      ( 0 ),
    m_resetToPion    ( false ),
    m_maxUsedTracks  ( 0 ),
    m_maxInputTracks ( 0 )
{

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
StatusCode RichGlobalPIDTrTrackSel::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

// Select tracks for analysis
StatusCode RichGlobalPIDTrTrackSel::execute() {

  debug() << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidTracks() || !gpidPIDs() ) return StatusCode::FAILURE;

  // Check if track processing was aborted.
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );
  if ( procStat->aborted() ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ProcStatAbort );
    richStatus()->setEventOK( false );  // set event status false
    deleteEvent();
    return Warning("Processing aborted -> Abort",StatusCode::SUCCESS);
  }

  // Check the number of input raw tracks
  if ( trackCreator()->nInputTracks() > m_maxInputTracks ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedTrTrackLimit );
    deleteEvent();
    return Warning("Max. number of input tracks exceeded -> Abort",StatusCode::SUCCESS);
  }

  // Make sure RichRecTracks are available
  if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  if ( richTracks()->empty() ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichTracks );
    deleteEvent();
    return Warning("No tracks selected -> Abort",StatusCode::SUCCESS);
  } else if ( richTracks()->size() > m_maxUsedTracks ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedRichTrackLimit );
    deleteEvent();
    return Warning("Max. number of RICH tracks exceeded -> Abort",StatusCode::SUCCESS);
  }

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
    verbose() << "Track " << (*track)->key() << " has initial hypothesis "
              << (*track)->currentHypothesis() << endreq;

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

  } // track loop

  if ( m_GPIDtracks->empty() ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichTracks );
    deleteEvent();
    return Warning("No tracks selected -> Abort",StatusCode::SUCCESS);
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
  const double pTot = track->vertexMomentum();

  // If below minimum momentum for use in LL, return false
  if ( pTot < m_minLLPtot ) return Rich::GlobalPID::Unusable;

  // Likelihood use only ?
  if ( pTot >= m_minLLPtot && pTot < m_minPhysPtot ) {
    quality = Rich::GlobalPID::LikelihoodOnly;
  }

  return quality;
}

//  Finalize
StatusCode RichGlobalPIDTrTrackSel::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
