
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracksWithForcedIneffic.cpp
 *
 *  Implementation file for tool : Rich::Rec::TrackCreatorFromRecoTracksWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracksWithForcedIneffic.cpp,v 1.3 2009-07-30 11:25:33 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromRecoTracksWithForcedIneffic.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-------------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackCreatorFromRecoTracksWithForcedIneffic );

// Standard constructor
TrackCreatorFromRecoTracksWithForcedIneffic::
TrackCreatorFromRecoTracksWithForcedIneffic( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
  : TrackCreatorFromRecoTracks ( type, name, parent )
{
  declareProperty( "RejectFraction", m_rejFrac = 0.1 );
}

StatusCode TrackCreatorFromRecoTracksWithForcedIneffic::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TrackCreatorFromRecoTracks::initialize();
  if ( sc.isFailure() ) { return sc; }

  // random number generator
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_rndm.initialize( randSvc, Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  warning() << "Will randomly reject " << m_rejFrac*100 << " % of tracks" << endmsg;

  return sc;
}

StatusCode TrackCreatorFromRecoTracksWithForcedIneffic::finalize()
{
  // finalize random number generator
  m_rndm.finalize();
  // Execute base class method
  return TrackCreatorFromRecoTracks::finalize();
}

// Forms a new RichRecTrack object from a Track
LHCb::RichRecTrack *
TrackCreatorFromRecoTracksWithForcedIneffic::newTrack ( const ContainedObject * obj ) const
{
  return ( m_rndm() < m_rejFrac ? NULL :
           TrackCreatorFromRecoTracks::newTrack(obj) );
}
