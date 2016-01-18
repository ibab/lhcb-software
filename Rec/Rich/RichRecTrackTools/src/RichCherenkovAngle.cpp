
//-----------------------------------------------------------------------------
/** @file RichCherenkovAngle.cpp
 *
 *  Implementation file for tool : Rich::Rec::CherenkovAngle
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

DECLARE_TOOL_FACTORY( CherenkovAngle )

// Standard constructor
CherenkovAngle::CherenkovAngle ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : ToolBase( type, name, parent )
{
  // interface
  declareInterface<ICherenkovAngle>(this);
}

StatusCode CherenkovAngle::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace,    nullptr, true );
  acquireTool( "RichSmartIDTool",         m_smartIDTool, nullptr, true );
  acquireTool( "RichExpectedTrackSignal", m_signal       );
  acquireTool( "RichRefractiveIndex",     m_refIndex     );
  acquireTool( "RichParticleProperties",  m_richPartProp );
  acquireTool( "RichTrackEffectiveRefIndex", m_tkIndex   );
  acquireTool( "RichPhotonEmissionPoint", m_emissPoint );

  m_pidTypes = m_richPartProp->particleTypes();
  _ri_debug << "Particle types considered = " << m_pidTypes << endmsg;

  return sc;
}

double
CherenkovAngle::avgCherenkovTheta( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id,
                                   const bool useEmittedSpectrum ) const
{
  // protect against the below threshold case
  if ( id == Rich::BelowThreshold ) return 0;

  if ( !segment->averageCKTheta().dataIsValid(id) )
  {
    double angle = 0;

    // total unscattered signal
    const double unscat = ( !useEmittedSpectrum ? 
                            m_signal->nSignalPhotons ( segment, id ) :
                            m_signal->nEmittedPhotons( segment, id ) );
    if ( unscat > 0 )
    {

      // Beta for this segment
      const double beta = 
        m_richPartProp->beta( std::sqrt(segment->trackSegment().bestMomentum().mag2()), id);
      if ( beta > 0 )
      {
        // loop over energy bins
        const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & sigSpectra 
          = ( !useEmittedSpectrum ? segment->signalPhotonSpectra() : segment->emittedPhotonSpectra() );
        for ( unsigned int iEnBin = 0; iEnBin < sigSpectra.energyBins(); ++iEnBin )
        {
          const double temp = 
            beta * m_tkIndex->refractiveIndex( segment, sigSpectra.binEnergy(iEnBin) );
          angle += (sigSpectra.energyDist(id))[iEnBin] * ( temp>1 ? std::acos(1/temp) : 0 );
        }

        // normalise the angle
        angle /= unscat;

      } // beta > 0

    } // unscat > 0

    // Don't save in the segment if the emitted spectra was used
    if ( !useEmittedSpectrum ) 
      segment->setAverageCKTheta( id, 
                                  (LHCb::RichRecSegment::FloatType)(angle) );

    //if ( !useEmittedSpectrum ) 
    //{
    //  const double P = std::sqrt(segment->trackSegment().bestMomentum().mag2());
    //  if ( segment->trackSegment().rich() == Rich::Rich2 && P > 90 * Gaudi::Units::GeV )
    //  {
    //    info() << id << "  P = " << P/1000 << " GeV  CKtheta = " << angle*1000 << " mrad" << endmsg; 
    //  }
    //}

    // return the newly calculated value
    return angle;
  }

  // return the cached value
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
  const double refIn = m_refIndex->refractiveIndex( rad );
  return ( refIn > 0 ? std::acos( 1.0/refIn ) : 0 );
}

double 
CherenkovAngle::saturatedCherenkovTheta( const LHCb::RichRecSegment * segment ) const
{
  const double refIn = m_tkIndex->refractiveIndex( segment );
  return ( refIn > 0 ? std::acos( 1.0/refIn ) : 0 );
}

double CherenkovAngle::avCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                            const Rich::ParticleIDType id,
                                            const unsigned int nSamples ) const
{
  // protect against the below threshold case
  if ( id == Rich::BelowThreshold ) return 0;

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
  for ( const auto& hypo : m_pidTypes )
  {

    // Get CK theta
    const double ckTheta = avgCherenkovTheta(segment,hypo);

    // Set the value
    const double nomCK = nominalSaturatedCherenkovTheta(segment->trackSegment().radiator());
    const float C = (LHCb::RichRecSegment::FloatType)(rMax*(ckTheta/nomCK));
    segment->setAverageCKRadiusLocal( hypo, C );

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

  // ray tracing mode
  LHCb::RichTraceMode mode( LHCb::RichTraceMode::IgnoreHPDAcceptance );
  mode.setAeroRefraction ( iRad == Rich::Aerogel );

  // send off virtual photons
  double ckPhi = 0.0;
  double rSum  = 0.0;
  unsigned int nUsed = 0;
  for ( unsigned int iPhot = 0 ; iPhot < nSamples; ++iPhot, ckPhi += incPhi )
  {

    // Photon emission point is half-way between segment start and end points
    //const Gaudi::XYZPoint & emissionPt = segment->trackSegment().bestPoint();
    Gaudi::XYZPoint emissionPt;
    m_emissPoint->emissionPoint( segment, emissionPt );

    // Photon direction around loop
    const Gaudi::XYZVector photDir = segment->trackSegment().vectorAtThetaPhi( ckTheta, ckPhi );

    Gaudi::XYZPoint hitPointGlobal;
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
        rSum += std::sqrt( gsl_pow_2( hitPointLocal.x() - tkPoint.x() ) +
                           gsl_pow_2( hitPointLocal.y() - tkPoint.y() ) );
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
    const double nomCK = nominalSaturatedCherenkovTheta(segment->trackSegment().radiator());
    const float R = 
      (LHCb::RichRecSegment::FloatType)( avCKRingRadiusLocal( segment,
                                                              nomCK,
                                                              nSamples ) );
    segment->setAvSaturatedRadiusLocal( R );
  }

  return segment->avSaturatedRadiusLocal();
}
