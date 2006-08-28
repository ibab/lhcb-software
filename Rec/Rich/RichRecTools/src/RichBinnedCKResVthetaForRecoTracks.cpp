
//-----------------------------------------------------------------------------
/** @file RichBinnedCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichBinnedCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForRecoTracks.cpp,v 1.4 2006-08-28 11:34:41 jonrob Exp $
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
#include "Kernel/PhysicalConstants.h"

// namespaces
using namespace LHCb;

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

  (m_binEdges[Rich::Rich1Gas]).push_back( 0.03 );
  (m_binEdges[Rich::Rich1Gas]).push_back( 0.05 );
  declareProperty( "NRich1GasResBins", m_binEdges[Rich::Rich1Gas] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Forward]).push_back( 0.00357 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Forward]).push_back( 0.00226 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Forward]).push_back( 0.00150 );
  declareProperty( "Rich1GasForwardRes", m_theerr[Rich::Rich1Gas][Rich::Track::Forward] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Match]).push_back( 0.00385 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Match]).push_back( 0.00231 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Match]).push_back( 0.00180 );
  declareProperty( "Rich1GasMatchRes", m_theerr[Rich::Rich1Gas][Rich::Track::Match] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Follow]).push_back( 0.00393 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Follow]).push_back( 0.00242 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Follow]).push_back( 0.00242 );
  declareProperty( "Rich1GasFollowRes", m_theerr[Rich::Rich1Gas][Rich::Track::Follow] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack]).push_back( 0.00393 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack]).push_back( 0.00242 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack]).push_back( 0.00242 );
  declareProperty( "Rich1GasKsTrackRes", m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Seed]).push_back( 999 );
  declareProperty( "Rich1GasSeedRes", m_theerr[Rich::Rich1Gas][Rich::Track::Seed] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT]).push_back( 0.00806 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT]).push_back( 0.00307 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT]).push_back( 0.00211 );
  declareProperty( "Rich1GasVTTRes", m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT] );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Rich1Gas][Rich::Track::Velo]).push_back( 999 );

  (m_binEdges[Rich::Rich2Gas]).push_back( 0.03 );
  (m_binEdges[Rich::Rich2Gas]).push_back( 0.05 );
  declareProperty( "NRich2GasResBins", m_binEdges[Rich::Rich2Gas] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Forward]).push_back( 0.00119 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Forward]).push_back( 0.00122 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Forward]).push_back( 0.00083 );
  declareProperty( "Rich2GasForwardRes", m_theerr[Rich::Rich2Gas][Rich::Track::Forward] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Match]).push_back( 0.00119 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Match]).push_back( 0.00132 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Match]).push_back( 0.00091 );
  declareProperty( "Rich2GasMatchRes", m_theerr[Rich::Rich2Gas][Rich::Track::Match] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Follow]).push_back( 0.00133 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Follow]).push_back( 0.00131 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Follow]).push_back( 0.00109 );
  declareProperty( "Rich2GasFollowRes", m_theerr[Rich::Rich2Gas][Rich::Track::Follow] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack]).push_back( 0.00133 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack]).push_back( 0.00131 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack]).push_back( 0.00109 );
  declareProperty( "Rich2GasKsTrackRes", m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Seed]).push_back( 0.00138 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Seed]).push_back( 0.00117 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Seed]).push_back( 0.00099 );
  declareProperty( "Rich2GasSeedRes", m_theerr[Rich::Rich2Gas][Rich::Track::Seed] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::VeloTT]).push_back( 999 );
  declareProperty( "Rich2GasVTTRes", m_theerr[Rich::Rich2Gas][Rich::Track::VeloTT] );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Rich2Gas][Rich::Track::Velo]).push_back( 999 );

}

StatusCode RichBinnedCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // Informational Printout
  info() << "Using binned Cherenkov theta resolution" << endreq;
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

    // Check track parent type is Track
    if ( Rich::TrackParent::Track         != tkID.parentType() )
    {
      Exception( "Track parent type is not 'Track'" );
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
