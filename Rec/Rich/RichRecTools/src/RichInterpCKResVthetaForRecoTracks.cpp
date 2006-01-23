
//-----------------------------------------------------------------------------
/** @file RichInterpCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichInterpCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVthetaForRecoTracks.cpp,v 1.2 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichInterpCKResVthetaForRecoTracks.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichInterpCKResVthetaForRecoTracks>          s_factory ;
const        IToolFactory& RichInterpCKResVthetaForRecoTracksFactory = s_factory ;

// Standard constructor
RichInterpCKResVthetaForRecoTracks::
RichInterpCKResVthetaForRecoTracks ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_ckAngle ( 0 )
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
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR )
  {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT ) { m_ckRes[iR][iT] = 0; }
  }

}

StatusCode RichInterpCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // initialise interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR )
  {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT )
    {
      m_ckRes[iR][iT] = new Rich1DTabFunc( m_thebin[iR], m_theerr[iR][iT] );
      if ( !(m_ckRes[iR][iT])->valid() )
      {
        err() << "Failed to initialise interpolator for "
              << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  return sc;
}

StatusCode RichInterpCKResVthetaForRecoTracks::finalize()
{
  // clean up interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR )
  {
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT )
    {
      if ( m_ckRes[iR][iT] ) { delete m_ckRes[iR][iT]; m_ckRes[iR][iT] = 0; }
    }
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichInterpCKResVthetaForRecoTracks::ckThetaResolution( RichRecSegment * segment,
                                                       const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // Check track parent type is Track
    if ( Rich::TrackParent::Track != tkID.parentType() )
    {
      Exception( "Track parent type is not Track or TrStoredTrack" );
    }

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( thetaExp < 0.000001 ) return 0;

    // track type
    const Rich::Track::Type type = tkID.trackType();

    // which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // compute the interpolated resolution
    segment->setCKThetaResolution( id, (m_ckRes[rad][type])->value(thetaExp) );

  }

  return segment->ckThetaResolution( id );

}
