// $Id: RichInterpCherenkovResolution.cpp,v 1.2 2003-10-13 16:32:31 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichInterpCherenkovResolution.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


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
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichCherenkovResolution>(this);

  // Define job option parameters

  declareProperty( "NAerogelResBins",    m_thebin[Rich::Aerogel] );
  declareProperty( "AerogelForwardRes",  m_theerr[Rich::Aerogel][Rich::Track::Forward] );
  declareProperty( "AerogelMatchRes",    m_theerr[Rich::Aerogel][Rich::Track::Match] );
  declareProperty( "AerogelUpStreamRes", m_theerr[Rich::Aerogel][Rich::Track::UpStream] );
  declareProperty( "AerogelSeedRes",     m_theerr[Rich::Aerogel][Rich::Track::Seed] );
  declareProperty( "AerogelVTTRes",      m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );

  declareProperty( "NC4F10ResBins",    m_thebin[Rich::C4F10] );
  declareProperty( "C4F10ForwardRes",  m_theerr[Rich::C4F10][Rich::Track::Forward] );
  declareProperty( "C4F10MatchRes",    m_theerr[Rich::C4F10][Rich::Track::Match] );
  declareProperty( "C4F10UpStreamRes", m_theerr[Rich::C4F10][Rich::Track::UpStream] );
  declareProperty( "C4F10SeedRes",     m_theerr[Rich::C4F10][Rich::Track::Seed] );
  declareProperty( "C4F10VTTRes",      m_theerr[Rich::C4F10][Rich::Track::VeloTT] );

  declareProperty( "NCF4ResBins",    m_thebin[Rich::CF4] );
  declareProperty( "CF4ForwardRes",  m_theerr[Rich::CF4][Rich::Track::Forward] );
  declareProperty( "CF4MatchRes",    m_theerr[Rich::CF4][Rich::Track::Match] );
  declareProperty( "CF4UpStreamRes", m_theerr[Rich::CF4][Rich::Track::UpStream] );
  declareProperty( "CF4SeedRes",     m_theerr[Rich::CF4][Rich::Track::Seed] );
  declareProperty( "CF4VTTRes",      m_theerr[Rich::CF4][Rich::Track::VeloTT] );

  // temporary parameters to take into acount degraded performance for robustness tests
  m_emisPntErrScale.push_back( 1 );
  m_emisPntErrScale.push_back( 1 );
  m_emisPntErrScale.push_back( 1 );
  declareProperty( "ScaleEmisPntErr", m_emisPntErrScale );

}

StatusCode RichInterpCherenkovResolution::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // initialise interpolators
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    for ( int iT = 0; iT < 5; ++iT ) {
      m_ckRes[iR][iT] = new Rich1DTabFunc( m_thebin[iR], m_theerr[iR][iT] );
      if ( !(m_ckRes[iR][iT])->valid() ) {
        msg << MSG::ERROR << "Failed to initialise interpolator for "
            << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  // Informational Printout
  msg << MSG::DEBUG
      << " Using interpolated track resolutions" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichInterpCherenkovResolution::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_ckAngle );

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
                                                  const Rich::ParticleIDType id ) {

  // Expected Cherenkov theta angle
  double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( thetaExp < 0.000001 ) return 0;

  // track and radiator type
  Rich::RadiatorType rad = segment->trackSegment().radiator();
  Rich::Track::Type type = segment->richRecTrack()->trackID().trackType();

  // compute the interpolated resolution
  double res = (m_ckRes[rad][type])->value(thetaExp);
  
  // Scale for robustness tests and return
  res *= m_emisPntErrScale[rad];
  return res;
}
