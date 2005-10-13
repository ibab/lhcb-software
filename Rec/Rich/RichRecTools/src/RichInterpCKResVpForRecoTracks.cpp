
//---------------------------------------------------------------------------------------
/** @file RichInterpCKResVpForRecoTracks.cpp
 *
 *  Implementation file for tool : RichInterpCKResVpForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVpForRecoTracks.cpp,v 1.1 2005-10-13 16:01:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichInterpCKResVpForRecoTracks.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichInterpCKResVpForRecoTracks>          s_factory ;
const        IToolFactory& RichInterpCKResVpForRecoTracksFactory = s_factory ;

// Standard constructor
RichInterpCKResVpForRecoTracks::
RichInterpCKResVpForRecoTracks ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : RichRecToolBase( type, name, parent )
{

  declareInterface<IRichCherenkovResolution>(this);

  // Define job option parameters

  declareProperty( "NAerogelResBins",    m_thebin[Rich::Aerogel] );
  declareProperty( "AerogelForwardRes",  m_theerr[Rich::Aerogel][Rich::Track::Forward] );
  declareProperty( "AerogelMatchRes",    m_theerr[Rich::Aerogel][Rich::Track::Match] );
  declareProperty( "AerogelFollowRes",   m_theerr[Rich::Aerogel][Rich::Track::Follow] );
  declareProperty( "AerogelKsTrackRes",  m_theerr[Rich::Aerogel][Rich::Track::KsTrack] );
  declareProperty( "AerogelSeedRes",     m_theerr[Rich::Aerogel][Rich::Track::Seed] );
  declareProperty( "AerogelVTTRes",      m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );
  declareProperty( "AerogelVeloRes",     m_theerr[Rich::Aerogel][Rich::Track::Velo] );

  declareProperty( "NC4F10ResBins",    m_thebin[Rich::C4F10] );
  declareProperty( "C4F10ForwardRes",  m_theerr[Rich::C4F10][Rich::Track::Forward] );
  declareProperty( "C4F10MatchRes",    m_theerr[Rich::C4F10][Rich::Track::Match] );
  declareProperty( "C4F10FollowRes",   m_theerr[Rich::C4F10][Rich::Track::Follow] );
  declareProperty( "C4F10KsTrackRes",  m_theerr[Rich::C4F10][Rich::Track::KsTrack] );
  declareProperty( "C4F10SeedRes",     m_theerr[Rich::C4F10][Rich::Track::Seed] );
  declareProperty( "C4F10VTTRes",      m_theerr[Rich::C4F10][Rich::Track::VeloTT] );
  declareProperty( "C4F10VeloRes",     m_theerr[Rich::C4F10][Rich::Track::Velo] );

  declareProperty( "NCF4ResBins",    m_thebin[Rich::CF4] );
  declareProperty( "CF4ForwardRes",  m_theerr[Rich::CF4][Rich::Track::Forward] );
  declareProperty( "CF4MatchRes",    m_theerr[Rich::CF4][Rich::Track::Match] );
  declareProperty( "CF4FollowRes",   m_theerr[Rich::CF4][Rich::Track::Follow] );
  declareProperty( "CF4KsTrackRes",  m_theerr[Rich::CF4][Rich::Track::KsTrack] );
  declareProperty( "CF4SeedRes",     m_theerr[Rich::CF4][Rich::Track::Seed] );
  declareProperty( "CF4VTTRes",      m_theerr[Rich::CF4][Rich::Track::VeloTT] );
  declareProperty( "CF4VeloRes",     m_theerr[Rich::CF4][Rich::Track::Velo] );

  // set interpolator pointers to NULL
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT ) { m_ckRes[iR][iT] = 0; }
  }

}

StatusCode RichInterpCKResVpForRecoTracks::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // initialise interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT ) {
      m_ckRes[iR][iT] = new Rich1DTabFunc( m_thebin[iR], m_theerr[iR][iT] );
      if ( !(m_ckRes[iR][iT])->valid() ) {
        err() << "Failed to initialise interpolator for "
              << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RichInterpCKResVpForRecoTracks::finalize()
{
  // clean up interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT ) {
      if ( m_ckRes[iR][iT] ) { delete m_ckRes[iR][iT]; m_ckRes[iR][iT] = 0; }
    }
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichInterpCKResVpForRecoTracks::ckThetaResolution( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // Check track parent type is Track or TrStoredTrack
    if ( Rich::TrackParent::Track         != tkID.parentType() &&
         Rich::TrackParent::TrStoredTrack != tkID.parentType() )
    {
      Exception( "Track parent type is not Track or TrStoredTrack" );
    }

    // momentum for this segment
    const double ptot = segment->trackSegment().bestMomentum().mag();

    // track type
    const Rich::Track::Type type = segment->richRecTrack()->trackID().trackType();

    // which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // fill the resolution
    segment->setCKThetaResolution( id, (m_ckRes[rad][type])->value(ptot) );

  }

  return segment->ckThetaResolution( id );
}
