
//-----------------------------------------------------------------------------
/** @file RichCherenkovAngle.cpp
 *
 *  Implementation file for tool : RichCherenkovAngle
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngle.cpp,v 1.20 2006-08-09 11:12:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichCherenkovAngle.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichCherenkovAngle>          s_factory ;
const        IToolFactory& RichCherenkovAngleFactory = s_factory ;

// Standard constructor
RichCherenkovAngle::RichCherenkovAngle ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_signal        ( 0 ),
    m_richPartProp  ( 0 ),
    m_refIndex      ( 0 ),
    m_smartIDTool   ( 0 ),
    m_rayTrace      ( 0 )
{
  // interface
  declareInterface<IRichCherenkovAngle>(this);
}

StatusCode RichCherenkovAngle::initialize()
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

  const double refAero   = m_refIndex->refractiveIndex( Rich::Aerogel );
  if ( !(refAero>0) )  return Error( "Aerogel nominal refractive index < 0" );
  m_nomCK[Rich::Aerogel] = acos(1.0/refAero);

  const double refRich1Gas  = m_refIndex->refractiveIndex( Rich::Rich1Gas   );
  if ( !(refRich1Gas>0) ) return Error( "Rich1Gas nominal refractive index < 0"   );
  m_nomCK[Rich::Rich1Gas]   = acos(1.0/refRich1Gas);

  const double refRich2Gas    = m_refIndex->refractiveIndex( Rich::Rich2Gas     );
  if ( !(refRich2Gas>0) )   return Error( "Rich2Gas nominal refractive index < 0"     );
  m_nomCK[Rich::Rich2Gas]     = acos(1.0/refRich2Gas);

  return sc;
}

StatusCode RichCherenkovAngle::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichCherenkovAngle::avgCherenkovTheta( RichRecSegment * segment,
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
      RichPhotonSpectra<RichRecSegment::FloatType> & sigSpectra = segment->signalPhotonSpectra();
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

double RichCherenkovAngle::avgCherenkovTheta( RichRecSegment * segment ) const
{
  return avgCherenkovTheta( segment, segment->richRecTrack()->currentHypothesis() );
}

double RichCherenkovAngle::nominalSaturatedCherenkovTheta( const Rich::RadiatorType rad ) const
{
  return m_nomCK[rad];
}

double RichCherenkovAngle::avCKRingRadiusLocal( RichRecSegment * segment,
                                                const Rich::ParticleIDType id,
                                                const unsigned int nSamples ) const
{
  if ( !segment->averageCKRadiusLocal().dataIsValid(id) )
  {
    computeRadii(segment,nSamples);
  }
  return segment->averageCKRadiusLocal( id );
}

void RichCherenkovAngle::computeRadii( RichRecSegment * segment,
                                       const unsigned int nSamples ) const
{

  // radius for saturated rings
  const double rMax = satCKRingRadiusLocal( segment, nSamples );

  // Loop over all particle codes
  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
  {
    const Rich::ParticleIDType id = static_cast<Rich::ParticleIDType>(iHypo);

    // Get CK theta
    const double ckTheta = avgCherenkovTheta(segment,id);

    // Set the value
    segment->setAverageCKRadiusLocal( id,
                                      rMax * (ckTheta/m_nomCK[segment->trackSegment().radiator()]) );

  }

}

double RichCherenkovAngle::avCKRingRadiusLocal( RichRecSegment * segment,
                                                const double ckTheta,
                                                const unsigned int nSamples ) const
{

  // Calculate increment in phi
  const double incPhi = twopi / static_cast<double>(nSamples) ;

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
    const RichTraceMode mode( RichTraceMode::IgnoreHPDAcceptance );
    if ( m_rayTrace->traceToDetector( segment->trackSegment().rich(),
                                      emissionPt,
                                      photDir,
                                      hitPointGlobal, 
                                      mode ) )
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

double RichCherenkovAngle::satCKRingRadiusLocal( RichRecSegment * segment,
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
