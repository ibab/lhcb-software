
//-------------------------------------------------------------------------------
/** @file RichTrackCreatorFromMCRichTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromMCRichTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromMCRichTracks.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackCreatorFromMCRichTracks )

// Standard constructor
TrackCreatorFromMCRichTracks::
TrackCreatorFromMCRichTracks( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : RichTrackCreatorBase   ( type, name, parent ),
    m_mcrTracks            ( 0 ),
    m_massHypoRings        ( 0 ),
    m_segMaker             ( 0 ),
    m_signal               ( 0 ),
    m_mcrTracksLocation    ( LHCb::MCRichTrackLocation::Default ),
    m_trSegToolNickName    ( "RichTrSegMakerFromMCRichTracks"   ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false ),
    m_fakeRecoTracks       ( true  ),
    m_fakeTrLoc            ( "Rec/Rich/MCFakedTracks" ),
    m_fakeTracks           ( 0 )
{
  // declare interface for this tool
  declareInterface<ITrackCreator>(this);

  // job options
  declareProperty( "TracksLocation",           m_mcrTracksLocation  );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings     );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName  );
  declareProperty( "FakeRecoTracks",           m_fakeRecoTracks     );
  declareProperty( "TrackLocation",            m_fakeTrLoc          );
}

StatusCode TrackCreatorFromMCRichTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TrackCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( m_trSegToolNickName,       m_segMaker    );
  if ( m_buildHypoRings ) acquireTool( "RichMassHypoRings", m_massHypoRings );

  return sc;
}

StatusCode TrackCreatorFromMCRichTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( nInputTracks() );
    const LHCb::MCRichTracks * tracks = trTracks();
    for ( LHCb::MCRichTracks::const_iterator track = tracks->begin();
          track != tracks->end(); ++track )
    {
      newTrack( *track );
    }

  }

  return StatusCode::SUCCESS;
}

unsigned int TrackCreatorFromMCRichTracks::nInputTracks() const
{
  const LHCb::MCRichTracks * tracks = trTracks();
  return ( tracks ? tracks->size() : 0 );
}

const LHCb::MCRichTracks *
TrackCreatorFromMCRichTracks::trTracks() const
{
  if ( !m_mcrTracks )
  {
    // Obtain smart data pointer to Tracks
    m_mcrTracks = get<LHCb::MCRichTracks>( m_mcrTracksLocation );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "located " << m_mcrTracks->size() << " MCRichTracks at "
              << m_mcrTracksLocation << endmsg;
    }

    if ( m_fakeRecoTracks )
    {

      // create MC linker
      TrackToMCP linker( evtSvc(), msgSvc(), m_fakeTrLoc );
      linker.setDecreasingWeight();

      // clear map
      m_mcToFakeMap.clear();

      // loop over input tracks
      for ( LHCb::MCRichTracks::const_iterator track = m_mcrTracks->begin();
            track != m_mcrTracks->end(); ++track )
      {
        verbose() << "Trying MCRichTrack " << (*track)->key() << endmsg;

        // Pointer to underlying MCParticle
        const LHCb::MCParticle * mcPart = (*track)->mcParticle();
        if ( !mcPart )
        {
          Warning( "MCRichTrack has null MCParticle reference" );
          continue;
        }

        // track type
        //const Rich::Rec::Track::Type trType = getTrType( *track );
        // momentum and charge
        const double ptot   = mcPart->p();
        const int    charge = ( mcPart->particleID().threeCharge()>0 ? 1 : -1 ); 

        // new fake Track
        LHCb::Track * newFake = new LHCb::Track();

        // set momentum and charge info
        LHCb::State fakeState;
        fakeState.setQOverP( ptot>0 ? (double)charge/ptot : 0 );
        newFake->addToStates( fakeState );

        // set track types
        //newFake->setFlag    ( LHCb::Track::Clone, false ); // Unique by default
        newFake->setType    ( LHCb::Track::Long         );
        newFake->setHistory ( LHCb::Track::TrackIdealPR ); // closest to 'MC' history

        // Track selection
        //if ( !trackSelector().trackSelected(newFake) ) { delete newFake; continue; }
        verbose() << " -> Selected for fake reco track" << endmsg;

        // add to container
        fakedTracks()->insert( newFake );

        // store MC link
        linker.link( newFake, mcPart );

        // add to map
        m_mcToFakeMap[*track] = newFake;

      } // loop over tracks

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Created " << fakedTracks()->size() << " fake Tracks for MCRichTracks" << endmsg;
      }

    } // build fake if

  } // first time for each event if

  return m_mcrTracks;
}

// Forms a new RichRecTrack object from a Track
LHCb::RichRecTrack *
TrackCreatorFromMCRichTracks::newTrack ( const ContainedObject * obj ) const
{
  // make sure fake tracks are available if requested
  trTracks();

  // Is this a Track ?
  const LHCb::MCRichTrack * mcrTrack = dynamic_cast<const LHCb::MCRichTrack*>(obj);
  if ( !mcrTrack )
  {
    Warning( "Input data object is not of type 'MCRichTrack'" );
    return NULL;
  }

  // Pointer to underlying MCParticle
  const LHCb::MCParticle * mcPart = mcrTrack->mcParticle();
  if ( !mcPart )
  {
    Warning( "MCRichTrack has null MCParticle reference" );
    return NULL;
  }

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // track type
  const Rich::Rec::Track::Type trType = getTrType( mcrTrack );

  // unique ( by definition for MC ... )
  const bool trUnique = true;

  // momentum and charge
  const double ptot   = mcPart->p();
  const int    charge = ( mcPart->particleID().threeCharge()>0 ? 1 : -1 );

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying MCRichTrack " << mcrTrack->key()
              << " ptot = " << ptot/Gaudi::Units::GeV << " GeV/c, charge = " << charge
              << endmsg;
  }

  // Is track a usable type
  if ( !Rich::Rec::Track::isUsable(trType) ) return NULL;

  // Get reference to track stats object
  TrackCount & tkCount = trackStats().trackStats(trType,trUnique);

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[mcrTrack->key()] )
  {
    return richTracks()->object(mcrTrack->key());
  }
  else
  {

    // count tried tracks
    ++tkCount.triedTracks;

    // Track selection
    //if ( !trackSelector().trackSelected(trType,ptot,charge,trUnique) ) return NULL;

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Track selected" << endmsg;
    }

    // New track object pointer
    LHCb::RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<LHCb::RichTrackSegment*> segments;
    const int Nsegs = m_segMaker->constructSegments( mcrTrack, segments );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Found " << Nsegs << " radiator segments" << endmsg;
    }
    if ( 0 < Nsegs )
    {

      // Form a new RichRecTrack
      newTrack = new LHCb::RichRecTrack();

      // Configure TrackID for this Track
      newTrack->trackID().initialiseFor( mcrTrack );

      bool keepTrack = false;
      for ( std::vector<LHCb::RichTrackSegment*>::iterator iSeg = segments.begin();
            iSeg != segments.end(); ++iSeg )
      {
        if ( !(*iSeg) ) { Error( "Null RichTrackSegment pointer" ); continue; }

        // make a new RichRecSegment from this RichTrackSegment
        // takes ownership of RichTrackSegment* : (*iSeg) is responsible for deletion
        LHCb::RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

        // Get PD panel impact point
        if ( rayTraceHPDPanelPoints(**iSeg,newSegment) )
        {
          // test if this segment has valid information
          if ( m_signal->hasRichInfo(newSegment) )
          {

            if ( msgLevel(MSG::VERBOSE) )
              verbose() << " TrackSegment in " << (*iSeg)->radiator() << " selected" << endmsg;

            // keep track
            keepTrack = true;

            // Save this segment
            segmentCreator()->saveSegment( newSegment );

            // Add to the SmartRefVector of RichSegments for this RichRecTrack
            newTrack->addToRichRecSegments( newSegment );

            // set radiator info
            setDetInfo( newTrack, (*iSeg)->radiator() );

            // make RichRecRings for the mass hypotheses if requested
            if ( m_buildHypoRings ) m_massHypoRings->massHypoRings( newSegment );

            // Count radiator segments
            tkCount.countRadiator( (*iSeg)->radiator() );

          }
          else
          {
            if ( msgLevel(MSG::VERBOSE) )
            {
              verbose() << " TrackSegment in " << (*iSeg)->radiator()
                        << " rejected : Has no RICH info" << endmsg;
            }
            delete newSegment;
            newSegment = NULL;
          }

        }
        else
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << " TrackSegment in " << (*iSeg)->radiator()
                      << " rejected : Failed to project to detector plane" << endmsg;
          }
          delete newSegment;
          newSegment = NULL;
        }

      } // end loop over RichTrackSegments

      if ( keepTrack )
      {

        // give to container
        try
        {
          richTracks()->insert( newTrack, mcrTrack->key() );
        }
        catch ( ... )
        {
          Warning( "Failed to insert MCRichTrack based RichRecTrack with original key", StatusCode::SUCCESS ).ignore();
          richTracks()->insert( newTrack );
        }

        // Set momentum and pt
        newTrack->setVertexMomentum( static_cast<LHCb::RichRecTrack::FloatType>(ptot) );
        newTrack->setVertexPt( static_cast<LHCb::RichRecTrack::FloatType>(mcPart->pt()) );

        // track charge
        newTrack->setCharge( charge );

        // Count selected tracks
        ++tkCount.selectedTracks;

        // Set parent information
        if ( m_fakeRecoTracks )
        {
          const LHCb::Track * faketrack = m_mcToFakeMap[mcrTrack];
          if (!faketrack) { Warning( "MCRichTrack has no fake Track !" ); }
          else            { newTrack->setParentTrack( faketrack ); }
          //newTrack->trackID().initialiseFor( faketrack );
        }
        else
        {
          newTrack->setParentTrack( mcrTrack );
        }

      }
      else
      {
        delete newTrack;
        newTrack = NULL;
      }

    }
    else if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  --> No TrackSegments created" << endmsg;
    } // end segments if

    // Add to reference map
    if ( bookKeep() ) m_trackDone[mcrTrack->key()] = true;

    return newTrack;
  }

}

void TrackCreatorFromMCRichTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone    = false;
  m_mcrTracks  = 0;
  m_fakeTracks = 0;
}

void TrackCreatorFromMCRichTracks::FinishEvent()
{
  RichTrackCreatorBase::FinishEvent();
  if ( m_fakeTracks )
  {
    debug() << "Created " << m_fakeTracks->size() << " fake Tracks at "
            << m_fakeTrLoc << endmsg;
  }
}

LHCb::Tracks *
TrackCreatorFromMCRichTracks::fakedTracks() const
{
  if ( !m_fakeTracks )
  {
    m_fakeTracks = new LHCb::Tracks();
    put ( m_fakeTracks, m_fakeTrLoc );
  }
  return m_fakeTracks;
}
