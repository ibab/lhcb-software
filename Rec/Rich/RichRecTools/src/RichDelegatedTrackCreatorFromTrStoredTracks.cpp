
/** @file RichDelegatedTrackCreatorFromTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichDelegatedTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichDelegatedTrackCreatorFromTrStoredTracks.cpp,v 1.5 2005-01-13 14:34:26 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/07/27 20:15:29  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichDelegatedTrackCreatorFromTrStoredTracks.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichDelegatedTrackCreatorFromTrStoredTracks>          s_factory ;
const        IToolFactory& RichDelegatedTrackCreatorFromTrStoredTracksFactory = s_factory ;

// Standard constructor
RichDelegatedTrackCreatorFromTrStoredTracks::
RichDelegatedTrackCreatorFromTrStoredTracks( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_tracks         ( 0     ),
    m_trTracks       ( 0     ),
    m_allDone        ( false )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecTrackLocation",
                   m_richRecTrackLocation = RichRecTrackLocation::Default );
  // the real track tools to delegate the work to
  declareProperty( "ToolsByTrackType", m_names );

}

StatusCode RichDelegatedTrackCreatorFromTrStoredTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // setup mapping between track type and tool pointer
  m_nameToPnt.clear();
  m_trackToTool.clear();
  for ( ToolList::iterator it = m_names.begin();
        it != m_names.end(); ++it ) {
    const int slash = (*it).find_first_of( "/" );
    const std::string trackType = ( slash>0 ? (*it).substr(0,slash) : *it );
    const std::string toolType  = ( slash>0 ? (*it).substr(slash+1) : *it );
    debug() << " Tracktype '" << trackType
            << "' will use tool '" << toolType << "'" << endreq;
    m_trackToTool[trackType] = toolType;
  }
  m_names.clear();

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichDelegatedTrackCreatorFromTrStoredTracks::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichDelegatedTrackCreatorFromTrStoredTracks::handle ( const Incident & incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

const StatusCode RichDelegatedTrackCreatorFromTrStoredTracks::newTracks() const {

  if ( ! m_allDone ) {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( trStoredTracks()->size() );
    for ( TrStoredTracks::const_iterator track = trStoredTracks()->begin();
          track != trStoredTracks()->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

  }

  return StatusCode::SUCCESS;
}

const long RichDelegatedTrackCreatorFromTrStoredTracks::nInputTracks() const
{
  return ( trStoredTracks() ? trStoredTracks()->size() : 0 );
}

const TrStoredTracks *
RichDelegatedTrackCreatorFromTrStoredTracks::trStoredTracks() const
{
  if ( !m_trTracks ) {
    // Obtain smart data pointer to TrStoredTracks
    m_trTracks = get<TrStoredTracks>( m_trTracksLocation );
    debug() << "located " << m_trTracks->size() << " TrStoredTracks at "
            << m_trTracksLocation << endreq;
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichDelegatedTrackCreatorFromTrStoredTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a TrStoredTrack ?
  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) return NULL;

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);
  if ( Rich::Track::Unknown == trType ) {
    Warning( "TrStoredTrack of unknown algorithm type");
    return NULL;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // See if this RichRecTrack already exists
  if ( m_trackDone[trTrack->key()] ) {

    // track already done
    return richTracks()->object(trTrack->key());

  } else {

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "TrStoredTrack " << trTrack->key()
                << " type " << Rich::Track::type(trTrack) 
                << " -> delegating to " << toolName(trTrack) << endreq;
    }
    
    // delegate work to appropriate tool
    return tkTool(trTrack)->newTrack( trTrack );
    
  }
  
  return 0;
}

RichRecTracks * RichDelegatedTrackCreatorFromTrStoredTracks::richTracks() const
{
  if ( !m_tracks ) {
    SmartDataPtr<RichRecTracks> tdsTracks( evtSvc(), m_richRecTrackLocation );
    if ( !tdsTracks ) {

      // Reinitialise the track Container
      m_tracks = new RichRecTracks();

      // Register new RichRecTrack container to Gaudi data store
      put( m_tracks, m_richRecTrackLocation );

    } else {

      debug() << "Found " << tdsTracks->size() << " pre-existing RichRecTracks in TES at "
              << m_richRecTrackLocation << endreq;

      // Set smartref to TES track container
      m_tracks = tdsTracks;

      // Remake local track reference map
      for ( RichRecTracks::const_iterator iTrack = tdsTracks->begin();
            iTrack != tdsTracks->end();
            ++iTrack ) {
        m_trackDone[(*iTrack)->key()] = true;
      }

    }
  }
  return m_tracks;
}
