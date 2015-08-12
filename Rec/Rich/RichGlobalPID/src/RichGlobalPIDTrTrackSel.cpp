
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSel
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDTrTrackSel.h"

// From GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSel )

// Standard constructor, initializes variables
TrackSel::TrackSel( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : AlgBase ( name, pSvcLocator ),
    m_trSelector     ( NULL  ),
    m_frozenTrSel    ( NULL  ),
    m_gtkCreator     ( NULL  ),
    m_minPhysPtot    ( 0     ),
    m_minLLPtot      ( 0     ),
    m_resetToPion    ( false ),
    m_maxUsedTracks  ( 0     ),
    m_maxInputTracks ( 0     )
{
  // Selection cuts
  declareProperty( "MinimumPhysicsMomentum",  m_minPhysPtot = 0.0*Gaudi::Units::GeV );
  declareProperty( "MinimumLikelihoodMomentum", m_minLLPtot = 0.0*Gaudi::Units::GeV );
  declareProperty( "ResetTracksToPion", m_resetToPion = false );
  declareProperty( "MaxUsedTracks", m_maxUsedTracks = 400 );
  declareProperty( "MaxInputTracks", m_maxInputTracks = 99999 );
  declareProperty( "FreezeSelectedTracks", m_freezeTracks = false );
  declareProperty( "FrozenTrackMassHypo",  m_frozenType = (int)Rich::Pion );
}

// Destructor
TrackSel::~TrackSel() {}

//  Initialize
StatusCode TrackSel::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "GPIDTrackCreator", m_gtkCreator );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  trackCreator();
  segmentCreator();

  if ( m_freezeTracks )
  {
    info() << "Will freeze selected tracks to mass hypothesis "
           << (Rich::ParticleIDType)m_frozenType << endmsg;
    acquireTool( "TrackSelector", "FrozenTrackSelector", m_frozenTrSel, this );
  }
  
  return sc;
}

StatusCode TrackSel::eventInit()
{ 
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  if ( procStatus()->aborted() ) 
  {
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    return Warning("Processing aborted -> Abort",StatusCode::SUCCESS,0);
  }

  // Check the number of input raw tracks
  if ( 0 == trackCreator()->nInputTracks() )
  {
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    return Warning("No raw input tracks -> Stopped",StatusCode::SUCCESS,0);
  }
  if ( trackCreator()->nInputTracks() > m_maxInputTracks ) 
  {
    procStatus()->addAlgorithmStatus( gpidName(), "RICH", "ReachedTrTrackLimit",
                                      Rich::Rec::ReachedTrTrackLimit, true );
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    std::ostringstream mess;
    mess << "Number of input tracks exceeds maximum of "
         << m_maxInputTracks << " -> Abort";
    return Warning(mess.str(),StatusCode::SUCCESS,0);
  }

  // Make sure RichRecTracks are available
  if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  if ( richTracks()->empty() ) 
  {
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    return Warning("No tracks selected for processing -> Stopped",StatusCode::SUCCESS,0);
  }
  else if ( richTracks()->size() > m_maxUsedTracks )
  {
    procStatus()->addAlgorithmStatus( gpidName(), "RICH", "ReachedRichTrackLimit",
                                      Rich::Rec::ReachedRichTrackLimit, true );
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    std::ostringstream mess;
    mess << "Number of RICH tracks exceeds maximum of "
         << m_maxUsedTracks << " -> Abort";
    return Warning(mess.str(),StatusCode::SUCCESS,0);
  }

  return StatusCode::SUCCESS;
}

// Select tracks for analysis
StatusCode TrackSel::execute()
{
  // event initisation
  const StatusCode sc = eventInit();
  if ( sc.isFailure() ) return sc;

  // Iterate over all RichRecTracks and choose those to use
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ) 
  {

    // select tracks for use
    Rich::Rec::GlobalPID::TkQuality quality = trackStatus( *track );
    (*track)->setInUse( Rich::Rec::GlobalPID::Unusable != quality );
    if ( !(*track)->inUse() ) continue;

    // Set to pion hypothesis if configured to do so
    if ( m_resetToPion ) (*track)->setCurrentHypothesis( Rich::Pion );
    if ( msgLevel(MSG::VERBOSE) )
    { verbose() << "Track " << (*track)->key() << " has initial hypothesis "
                << (*track)->currentHypothesis() << endmsg; }
    
    // Make a new RichGlobalPIDTrack
    LHCb::RichGlobalPIDTrack * pidTrack = m_gtkCreator->createTrack(*track);

    // Frozen track ?
    if ( m_freezeTracks )
    {
      // is this track selected as one to freeze ?
      if ( m_frozenTrSel->trackSelected(*track) )
      {
        // freeze the track
        pidTrack->setFrozenHypo(true);
        // Set mass hypo to frozen type
        (*track)->setCurrentHypothesis( (Rich::ParticleIDType)m_frozenType );
        // label track as one not to have a final PID object 
        quality = Rich::Rec::GlobalPID::LikelihoodOnly;
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " -> Track is selected to be frozen to type " 
                    << (Rich::ParticleIDType)m_frozenType << endmsg;
        }
      }
    }

    // Set quality
    pidTrack->setTrQuality( quality );

  } // track loop

  if ( gpidTracks()->empty() ) 
  {
    procStatus()->addAlgorithmStatus( name(), "RICH", "NoRichTracks",
                                      Rich::Rec::NoRichTracks, false );
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    return Warning("No tracks selected -> Abort",StatusCode::SUCCESS,0);
  }

  return sc;
}

Rich::Rec::GlobalPID::TkQuality
TrackSel::trackStatus( LHCb::RichRecTrack * track ) 
{
  // Set default quality to be good
  Rich::Rec::GlobalPID::TkQuality quality = Rich::Rec::GlobalPID::Physics;

  // Momentum
  const double pTot = track->vertexMomentum();

  // If below minimum momentum for use in LL, return false
  if ( pTot < m_minLLPtot ) 
  {
    quality = Rich::Rec::GlobalPID::Unusable;
  }
  // Likelihood use only ?
  else if ( pTot >= m_minLLPtot && pTot < m_minPhysPtot ) 
  {
    quality = Rich::Rec::GlobalPID::LikelihoodOnly;
  }

  // Only use requested track types
  if ( !trackSelection(track) ) quality = Rich::Rec::GlobalPID::Unusable;

  return quality;
}

bool TrackSel::trackSelection( LHCb::RichRecTrack * track ) const
{
  return m_trSelector->trackSelected(track);
}
