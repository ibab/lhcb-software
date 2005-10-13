
//-----------------------------------------------------------------------------
/** @file RichBinnedCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichBinnedCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForRecoTracks.cpp,v 1.1 2005-10-13 16:01:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichBinnedCKResVthetaForRecoTracks.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichBinnedCKResVthetaForRecoTracks>          s_factory ;
const        IToolFactory& RichBinnedCKResVthetaForRecoTracksFactory = s_factory ;

// Standard constructor
RichBinnedCKResVthetaForRecoTracks::
RichBinnedCKResVthetaForRecoTracks ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_ckAngle ( 0 )
{

  declareInterface<IRichCherenkovResolution>(this);

  // Define job option parameters

  (m_binEdges[Rich::Aerogel]).push_back( 0.2 );
  (m_binEdges[Rich::Aerogel]).push_back( 0.25 );
  declareProperty( "NAerogelResBins", m_binEdges[Rich::Aerogel] );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00320 );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00242 );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00221 );
  declareProperty( "AerogelForwardRes", m_theerr[Rich::Aerogel][Rich::Track::Forward] );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00323 );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00262 );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00221 );
  declareProperty( "AerogelMatchRes", m_theerr[Rich::Aerogel][Rich::Track::Match] );
  (m_theerr[Rich::Aerogel][Rich::Track::Follow]).push_back( 0.00300 );
  (m_theerr[Rich::Aerogel][Rich::Track::Follow]).push_back( 0.00272 );
  (m_theerr[Rich::Aerogel][Rich::Track::Follow]).push_back( 0.00230 );
  declareProperty( "AerogelFollowRes", m_theerr[Rich::Aerogel][Rich::Track::Follow] );
  (m_theerr[Rich::Aerogel][Rich::Track::KsTrack]).push_back( 0.00300 );
  (m_theerr[Rich::Aerogel][Rich::Track::KsTrack]).push_back( 0.00272 );
  (m_theerr[Rich::Aerogel][Rich::Track::KsTrack]).push_back( 0.00230 );
  declareProperty( "AerogelKsTrackRes", m_theerr[Rich::Aerogel][Rich::Track::KsTrack] );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  declareProperty( "AerogelSeedRes", m_theerr[Rich::Aerogel][Rich::Track::Seed] );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.01 );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.00478 );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.00273 );
  declareProperty( "AerogelVTTRes", m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );

  (m_binEdges[Rich::C4F10]).push_back( 0.03 );
  (m_binEdges[Rich::C4F10]).push_back( 0.05 );
  declareProperty( "NC4F10ResBins", m_binEdges[Rich::C4F10] );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00357 );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00226 );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00150 );
  declareProperty( "C4F10ForwardRes", m_theerr[Rich::C4F10][Rich::Track::Forward] );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00385 );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00231 );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00180 );
  declareProperty( "C4F10MatchRes", m_theerr[Rich::C4F10][Rich::Track::Match] );
  (m_theerr[Rich::C4F10][Rich::Track::Follow]).push_back( 0.00393 );
  (m_theerr[Rich::C4F10][Rich::Track::Follow]).push_back( 0.00242 );
  (m_theerr[Rich::C4F10][Rich::Track::Follow]).push_back( 0.00242 );
  declareProperty( "C4F10FollowRes", m_theerr[Rich::C4F10][Rich::Track::Follow] );
  (m_theerr[Rich::C4F10][Rich::Track::KsTrack]).push_back( 0.00393 );
  (m_theerr[Rich::C4F10][Rich::Track::KsTrack]).push_back( 0.00242 );
  (m_theerr[Rich::C4F10][Rich::Track::KsTrack]).push_back( 0.00242 );
  declareProperty( "C4F10KsTrackRes", m_theerr[Rich::C4F10][Rich::Track::KsTrack] );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  declareProperty( "C4F10SeedRes", m_theerr[Rich::C4F10][Rich::Track::Seed] );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00806 );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00307 );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00211 );
  declareProperty( "C4F10VTTRes", m_theerr[Rich::C4F10][Rich::Track::VeloTT] );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );

  (m_binEdges[Rich::CF4]).push_back( 0.03 );
  (m_binEdges[Rich::CF4]).push_back( 0.05 );
  declareProperty( "NCF4ResBins", m_binEdges[Rich::CF4] );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00119 );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00122 );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00083 );
  declareProperty( "CF4ForwardRes", m_theerr[Rich::CF4][Rich::Track::Forward] );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00119 );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00132 );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00091 );
  declareProperty( "CF4MatchRes", m_theerr[Rich::CF4][Rich::Track::Match] );
  (m_theerr[Rich::CF4][Rich::Track::Follow]).push_back( 0.00133 );
  (m_theerr[Rich::CF4][Rich::Track::Follow]).push_back( 0.00131 );
  (m_theerr[Rich::CF4][Rich::Track::Follow]).push_back( 0.00109 );
  declareProperty( "CF4FollowRes", m_theerr[Rich::CF4][Rich::Track::Follow] );
  (m_theerr[Rich::CF4][Rich::Track::KsTrack]).push_back( 0.00133 );
  (m_theerr[Rich::CF4][Rich::Track::KsTrack]).push_back( 0.00131 );
  (m_theerr[Rich::CF4][Rich::Track::KsTrack]).push_back( 0.00109 );
  declareProperty( "CF4KsTrackRes", m_theerr[Rich::CF4][Rich::Track::KsTrack] );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00138 );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00117 );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00099 );
  declareProperty( "CF4SeedRes", m_theerr[Rich::CF4][Rich::Track::Seed] );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  declareProperty( "CF4VTTRes", m_theerr[Rich::CF4][Rich::Track::VeloTT] );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );

}

StatusCode RichBinnedCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // Informational Printout
  debug() << " Using binned track resolutions for TrStoredTracks" << endreq;
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR )
  {
    debug() << " " << (Rich::RadiatorType)iR << " Resolution bins = " << m_binEdges[iR] << endreq;
    for ( unsigned iT = 0; iT < Rich::Track::NTrTypes; ++iT )
    {
      debug() << " " << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT
              << " Cherenkov Resolution = " << m_theerr[iR][iT] << endreq;
    }
  }

  return sc;
}

StatusCode RichBinnedCKResVthetaForRecoTracks::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichBinnedCKResVthetaForRecoTracks::ckThetaResolution( RichRecSegment * segment,
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

    double res = 0;

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( thetaExp > 1e-6 )
    {

      const Rich::RadiatorType rad = segment->trackSegment().radiator();
      const Rich::Track::Type type = tkID.trackType();

      if ( thetaExp > 0. &&  thetaExp < (m_binEdges[rad])[0] )
      {
        res = (m_theerr[rad][type])[0];
      }
      else if ( thetaExp > (m_binEdges[rad])[0] &&
                thetaExp < (m_binEdges[rad])[1] )
      {
        res = (m_theerr[rad][type])[1];
      }
      else if ( thetaExp > (m_binEdges[rad])[1] )
      {
        res = (m_theerr[rad][type])[2];
      }

    }

    segment->setCKThetaResolution( id, res );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Segment " << segment->key() << " : " << id << " ckRes " << res << endreq;
    }

  }

  return segment->ckThetaResolution( id );
}
