// $Id: RichInterpCherenkovResolution.cpp,v 1.6 2004-03-16 13:45:03 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichInterpCherenkovResolution.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichInterpCherenkovResolution
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichInterpCherenkovResolution>          s_factory ;
const        IToolFactory& RichInterpCherenkovResolutionFactory = s_factory ;

// Standard constructor
RichInterpCherenkovResolution::RichInterpCherenkovResolution ( const std::string& type,
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
  declareProperty( "AerogelFollowRes", m_theerr[Rich::Aerogel][Rich::Track::Follow] );
  declareProperty( "AerogelKsTrackRes", m_theerr[Rich::Aerogel][Rich::Track::KsTrack] );
  declareProperty( "AerogelSeedRes",     m_theerr[Rich::Aerogel][Rich::Track::Seed] );
  declareProperty( "AerogelVTTRes",      m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );

  declareProperty( "NC4F10ResBins",    m_thebin[Rich::C4F10] );
  declareProperty( "C4F10ForwardRes",  m_theerr[Rich::C4F10][Rich::Track::Forward] );
  declareProperty( "C4F10MatchRes",    m_theerr[Rich::C4F10][Rich::Track::Match] );
  declareProperty( "C4F10FollowRes", m_theerr[Rich::C4F10][Rich::Track::Follow] );
  declareProperty( "C4F10KsTrackRes", m_theerr[Rich::C4F10][Rich::Track::KsTrack] );
  declareProperty( "C4F10SeedRes",     m_theerr[Rich::C4F10][Rich::Track::Seed] );
  declareProperty( "C4F10VTTRes",      m_theerr[Rich::C4F10][Rich::Track::VeloTT] );

  declareProperty( "NCF4ResBins",    m_thebin[Rich::CF4] );
  declareProperty( "CF4ForwardRes",  m_theerr[Rich::CF4][Rich::Track::Forward] );
  declareProperty( "CF4MatchRes",    m_theerr[Rich::CF4][Rich::Track::Match] );
  declareProperty( "CF4FollowRes", m_theerr[Rich::CF4][Rich::Track::Follow] );
  declareProperty( "CF4KsTrackRes", m_theerr[Rich::CF4][Rich::Track::KsTrack] );
  declareProperty( "CF4SeedRes",     m_theerr[Rich::CF4][Rich::Track::Seed] );
  declareProperty( "CF4VTTRes",      m_theerr[Rich::CF4][Rich::Track::VeloTT] );

}

StatusCode RichInterpCherenkovResolution::initialize() {

  debug() << "Initialize" << endreq;

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // initialise interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( int iT = 0; iT < 5; ++iT ) {
      m_ckRes[iR][iT] = new Rich1DTabFunc( m_thebin[iR], m_theerr[iR][iT] );
      if ( !(m_ckRes[iR][iT])->valid() ) {
        err() << "Failed to initialise interpolator for "
              << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  // Informational Printout
  debug() << " Using interpolated track resolutions" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichInterpCherenkovResolution::finalize() {

  debug() << "Finalize" << endreq;

  // clean up interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( int iT = 0; iT < 5; ++iT ) {
      if ( m_ckRes[iR][iT] ) { delete m_ckRes[iR][iT]; m_ckRes[iR][iT] = 0; }
    }
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichInterpCherenkovResolution::ckThetaResolution( RichRecSegment * segment,
                                                  const Rich::ParticleIDType id ) const {

  // Expected Cherenkov theta angle
  const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( thetaExp < 0.000001 ) return 0;

  // track and radiator type
  const Rich::RadiatorType rad = segment->trackSegment().radiator();
  const Rich::Track::Type type = segment->richRecTrack()->trackID().trackType();

  // compute the interpolated resolution
  return (m_ckRes[rad][type])->value(thetaExp);
}
