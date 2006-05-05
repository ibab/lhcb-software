
//---------------------------------------------------------------------------------------
/** @file RichInterpCKResVpForRecoTracks.cpp
 *
 *  Implementation file for tool : RichInterpCKResVpForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVpForRecoTracks.cpp,v 1.3 2006-05-05 11:01:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichInterpCKResVpForRecoTracks.h"

// namespaces
using namespace LHCb;

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
    if ( Rich::TrackParent::Track != tkID.parentType() )
    {
      Exception( "Track parent type is not Track or TrStoredTrack" );
    }

    // momentum for this segment
    const double ptot = sqrt(segment->trackSegment().bestMomentum().Mag2());

    // track type
    const Rich::Track::Type type = segment->richRecTrack()->trackID().trackType();

    // which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // fill the resolution
    segment->setCKThetaResolution( id, (m_ckRes[rad][type])->value(ptot) );

  }

  return segment->ckThetaResolution( id );
}
