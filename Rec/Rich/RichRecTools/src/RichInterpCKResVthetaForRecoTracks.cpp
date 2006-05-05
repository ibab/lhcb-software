
//-----------------------------------------------------------------------------
/** @file RichInterpCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichInterpCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVthetaForRecoTracks.cpp,v 1.3 2006-05-05 11:01:40 jonrob Exp $
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

  declareProperty( "NRich1GasResBins",    m_thebin[Rich::Rich1Gas] );
  declareProperty( "Rich1GasForwardRes",  m_theerr[Rich::Rich1Gas][Rich::Track::Forward] );
  declareProperty( "Rich1GasMatchRes",    m_theerr[Rich::Rich1Gas][Rich::Track::Match] );
  declareProperty( "Rich1GasFollowRes",   m_theerr[Rich::Rich1Gas][Rich::Track::Follow] );
  declareProperty( "Rich1GasKsTrackRes",  m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack] );
  declareProperty( "Rich1GasSeedRes",     m_theerr[Rich::Rich1Gas][Rich::Track::Seed] );
  declareProperty( "Rich1GasVTTRes",      m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT] );
  declareProperty( "Rich1GasVeloRes",     m_theerr[Rich::Rich1Gas][Rich::Track::Velo] );

  declareProperty( "NRich2GasResBins",    m_thebin[Rich::Rich2Gas] );
  declareProperty( "Rich2GasForwardRes",  m_theerr[Rich::Rich2Gas][Rich::Track::Forward] );
  declareProperty( "Rich2GasMatchRes",    m_theerr[Rich::Rich2Gas][Rich::Track::Match] );
  declareProperty( "Rich2GasFollowRes",   m_theerr[Rich::Rich2Gas][Rich::Track::Follow] );
  declareProperty( "Rich2GasKsTrackRes",  m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack] );
  declareProperty( "Rich2GasSeedRes",     m_theerr[Rich::Rich2Gas][Rich::Track::Seed] );
  declareProperty( "Rich2GasVTTRes",      m_theerr[Rich::Rich2Gas][Rich::Track::VeloTT] );
  declareProperty( "Rich2GasVeloRes",     m_theerr[Rich::Rich2Gas][Rich::Track::Velo] );

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
