
//---------------------------------------------------------------------------------------------
/** @file RichDelegatedTrackCreatorFromRecoTracks.cpp
 *
 *  Implementation file for tool : RichDelegatedTrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichDelegatedTrackCreatorFromRecoTracks.cpp,v 1.2 2006-01-23 14:20:43 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------------

#include "GaudiKernel/ContainedObject.h"

// local
#include "RichDelegatedTrackCreatorFromRecoTracks.h"

// namespaces
using namespace LHCb;

//---------------------------------------------------------------------------------------------
// Declaration of the Tool Factory
static const  ToolFactory<RichDelegatedTrackCreatorFromRecoTracks>          s_factory ;
const        IToolFactory& RichDelegatedTrackCreatorFromRecoTracksFactory = s_factory ;

// Standard constructor
RichDelegatedTrackCreatorFromRecoTracks::
RichDelegatedTrackCreatorFromRecoTracks( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichTrackCreatorBase ( type, name, parent ),
    m_trTracks       ( 0     ),
    m_allDone        ( false ),
    m_tkToPtn        ( Rich::Track::NTrTypes, 0 )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "RecoTracksLocation",
                   m_trTracksLocation = TrackLocation::Default );
  // the real track tools to delegate the work to
  declareProperty( "ToolsByTrackType", m_names );

}

StatusCode RichDelegatedTrackCreatorFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichTrackCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // setup mapping between track type and tool pointer
  Rich::Map< std::string, const IRichTrackCreator * > tmpMap;
  for ( ToolList::iterator it = m_names.begin();
        it != m_names.end(); ++it )
  {
    const int slash = (*it).find_first_of( "/" );
    const std::string trackType = ( slash>0 ? (*it).substr(0,slash) : *it );
    const std::string toolType  = ( slash>0 ? (*it).substr(slash+1) : *it );
    info() << "Track type '" << trackType
           << "' will use RichTrackCreator '" << toolType << "'" << endreq;
    const Rich::Track::Type tkType = Rich::Track::type(trackType);
    if ( 0 == tmpMap[toolType] )
    {
      if ( !m_tkToPtn[tkType] ) acquireTool( toolType, m_tkToPtn[tkType] );
      tmpMap[toolType] = m_tkToPtn[tkType];
    }
    else
    {
      m_tkToPtn[tkType] = tmpMap[toolType];
    }

  }
  m_names.clear();

  return sc;
}

StatusCode RichDelegatedTrackCreatorFromRecoTracks::finalize()
{
  // Execute base class method
  return RichTrackCreatorBase::finalize();
}

const StatusCode RichDelegatedTrackCreatorFromRecoTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( recoTracks()->size() );
    for ( Tracks::const_iterator track = recoTracks()->begin();
          track != recoTracks()->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

  }

  return StatusCode::SUCCESS;
}

const long RichDelegatedTrackCreatorFromRecoTracks::nInputTracks() const
{
  return ( recoTracks() ? recoTracks()->size() : 0 );
}

const LHCb::Tracks *
RichDelegatedTrackCreatorFromRecoTracks::recoTracks() const
{
  if ( !m_trTracks )
  {
    // Obtain smart data pointer to Tracks
    m_trTracks = get<Tracks>( m_trTracksLocation );
    debug() << "located " << m_trTracks->size() << " Tracks at "
            << m_trTracksLocation << endreq;
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a Track
RichRecTrack *
RichDelegatedTrackCreatorFromRecoTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a Track ?
  const Track * trTrack = dynamic_cast<const Track*>(obj);
  if ( !trTrack ) return NULL;

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);
  if ( Rich::Track::Unknown == trType )
  {
    Warning( "Track of unknown algorithm type" );
    return NULL;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[trTrack->key()] )
  {

    // track already done
    return richTracks()->object(trTrack->key());

  }
  else
  {

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Track " << trTrack->key()
                << " type " << trType
                << " -> delegating to " << tkTool(trType)->name() << endreq;
    }

    // Add to reference map
    if ( bookKeep() ) m_trackDone[trTrack->key()] = true;

    // delegate work to appropriate tool
    return tkTool(trType)->newTrack( trTrack );

  }

  return 0;
}

void RichDelegatedTrackCreatorFromRecoTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone  = false;
  m_trTracks = 0;
}
