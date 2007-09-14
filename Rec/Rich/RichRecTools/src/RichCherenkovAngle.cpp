
//-----------------------------------------------------------------------------
/** @file RichCherenkovAngle.cpp
 *
 *  Implementation file for tool : Rich::Rec::CherenkovAngle
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngle.cpp,v 1.24 2007-09-14 13:39:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichCherenkovAngle.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CherenkovAngle );

// Standard constructor
CherenkovAngle::CherenkovAngle ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_signal        ( NULL ),
    m_richPartProp  ( NULL ),
    m_refIndex      ( NULL ),
    m_smartIDTool   ( NULL ),
    m_rayTrace      ( NULL )
{
  // interface
  declareInterface<ICherenkovAngle>(this);
}

StatusCode CherenkovAngle::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace     );
  acquireTool( "RichSmartIDTool",         m_smartIDTool, 0, true  );
  acquireTool( "RichExpectedTrackSignal", m_signal       );
  acquireTool( "RichRefractiveIndex",     m_refIndex     );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // Get the nominal refractive index for the given radiator

  const double refAero      = m_refIndex->refractiveIndex( Rich::Aerogel );
  if ( !(refAero>0) )       return Error( "Aerogel nominal refractive index < 0"  );
  m_nomCK[Rich::Aerogel]    = acos(1.0/refAero);

  const double refRich1Gas  = m_refIndex->refractiveIndex( Rich::Rich1Gas   );
  if ( !(refRich1Gas>0) )   return Error( "Rich1Gas nominal refractive index < 0" );
  m_nomCK[Rich::Rich1Gas]   = acos(1.0/refRich1Gas);

  const double refRich2Gas  = m_refIndex->refractiveIndex( Rich::Rich2Gas     );
  if ( !(refRich2Gas>0) )   return Error( "Rich2Gas nominal refractive index < 0" );
  m_nomCK[Rich::Rich2Gas]   = acos(1.0/refRich2Gas);

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endreq;

  return sc;
}

double
CherenkovAngle::avgCherenkovTheta( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) const
{

  if ( !segment->averageCKTheta().dataIsValid(id) )
  {
    double angle = 0;

    // total unscattered signal
    const double unscat = m_signal->nSignalPhotons( segment, id );

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " calculating avgCK theta : unscat = " << unscat << endreq;
    }

    if ( unscat > 0 )
    {

      // which radiator
      const Rich::RadiatorType rad = segment->trackSegment().radiator();

      // Beta for this segment
      const double beta = m_richPartProp->beta( sqrt(segment->trackSegment().bestMomentum().mag2()), id);

      // loop over energy bins
      Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & sigSpectra = segment->signalPhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < sigSpectra.energyBins(); ++iEnBin )
      {
        const double temp = beta *
          m_refIndex->refractiveIndex( rad, sigSpectra.binEnergy(iEnBin) );
        angle += (sigSpectra.energyDist(id))[iEnBin] * ( temp>1 ? acos(1/temp) : 0 );
      }

      // normalise the angle
      angle /= unscat;

    }

    segment->setAverageCKTheta( id, angle );
  }

  return segment->averageCKTheta( id );
}

double
CherenkovAngle::avgCherenkovTheta( LHCb::RichRecSegment * segment ) const
{
  return avgCherenkovTheta( segment, segment->richRecTrack()->currentHypothesis() );
}

double
CherenkovAngle::nominalSaturatedCherenkovTheta( const Rich::RadiatorType rad ) const
{
  return m_nomCK[rad];
}

double CherenkovAngle::avCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                            const Rich::ParticleIDType id,
                                            const unsigned int nSamples ) const
{
  if ( !segment->averageCKRadiusLocal().dataIsValid(id) )
  {
    computeRadii(segment,nSamples);
  }
  return segment->averageCKRadiusLocal( id );
}

void CherenkovAngle::computeRadii( LHCb::RichRecSegment * segment,
                                   const unsigned int nSamples ) const
{

  // radius for saturated rings
  const double rMax = satCKRingRadiusLocal( segment, nSamples );

  // Loop over all particle codes
  for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
        hypo != m_pidTypes.end(); ++hypo )
  {

    // Get CK theta
    const double ckTheta = avgCherenkovTheta(segment,*hypo);

    // Set the value
    segment->setAverageCKRadiusLocal( *hypo,
                                      rMax * (ckTheta/m_nomCK[segment->trackSegment().radiator()]) );

  }

}

double CherenkovAngle::avCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                            const double ckTheta,
                                            const unsigned int nSamples ) const
{

  // Calculate increment in phi
  const double incPhi = Gaudi::Units::twopi / static_cast<double>(nSamples) ;

  // Track impact point on HPD panel
  const Gaudi::XYZPoint & tkPoint = segment->pdPanelHitPointLocal();

  // radiator
  const Rich::RadiatorType iRad = segment->trackSegment().radiator();

  // send off virtual photons
  double ckPhi = 0.0;
  double rSum  = 0.0;
  unsigned int nUsed = 0;
  for ( unsigned int iPhot = 0 ; iPhot < nSamples; ++iPhot, ckPhi+=incPhi )
  {

    // Photon emission point is half-way between segment start and end points
    const Gaudi::XYZPoint & emissionPt = segment->trackSegment().bestPoint();

    // Photon direction around loop
    const Gaudi::XYZVector photDir = segment->trackSegment().vectorAtThetaPhi( ckTheta, ckPhi );

    Gaudi::XYZPoint hitPointGlobal;
    LHCb::RichTraceMode mode( LHCb::RichTraceMode::IgnoreHPDAcceptance );
    mode.setAeroRefraction ( iRad == Rich::Aerogel ); 
    const LHCb::RichTraceMode::RayTraceResult result = 
      m_rayTrace->traceToDetector( segment->trackSegment().rich(),
                                   emissionPt,
                                   photDir,
                                   hitPointGlobal,
                                   mode );
    if ( mode.traceWasOK(result) )
    {

      // Hit in local coordinates
      const Gaudi::XYZPoint hitPointLocal = m_smartIDTool->globalToPDPanel(hitPointGlobal);

      // Only use photons that end up on the same side of the RICH as the track
      if ( ( iRad != Rich::Rich2Gas && hitPointLocal.y()*tkPoint.y() > 0. ) ||
           ( iRad == Rich::Rich2Gas && hitPointLocal.x()*tkPoint.x() > 0. ) )
      {
        rSum += sqrt( gsl_pow_2(hitPointLocal.x() - tkPoint.x()) +
                      gsl_pow_2(hitPointLocal.y() - tkPoint.y()) );
        ++nUsed;
      }

    }

  } // end loop over vitual photons

  // return average radius
  return ( nUsed > 0 ? rSum / static_cast<double>(nUsed) : 0 );
}

double CherenkovAngle::satCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                             const unsigned int nSamples ) const
{
  if ( segment->avSaturatedRadiusLocal() < 0 )
  {
    // Get radius for saturated angle
    segment->setAvSaturatedRadiusLocal( avCKRingRadiusLocal( segment,
                                                             m_nomCK[segment->trackSegment().radiator()],
                                                             nSamples ) );
  }

  return segment->avSaturatedRadiusLocal();
}
