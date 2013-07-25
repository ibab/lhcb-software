//-----------------------------------------------------------------------------
/** @file RichPhotonSignalGaussProb.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonSignalGaussProb
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonSignalGaussProb.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

namespace
{
  static const double root_two_pi = std::sqrt( 2.0 * M_PI );
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonSignalGaussProb )

// Standard constructor
PhotonSignalGaussProb::PhotonSignalGaussProb( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
: ToolBase ( type, name, parent )
{
  // interface
  declareInterface<IPhotonSignal>(this);
  // JOs
  declareProperty( "MinExpArg", m_minArg = -650 );
}

StatusCode PhotonSignalGaussProb::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle );
  acquireTool( "RichExpectedTrackSignal", m_signal  );
  acquireTool( "RichCherenkovResolution", m_ckRes   );

  // Get Rich Detector elements
  const DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocations::Rich1 );
  const DeRich2 * Rich2DE = getDet<DeRich2>( DeRichLocations::Rich2 );

  // Mirror radii of curvature in mm
  m_radiusCurv[Rich::Rich1] = Rich1DE->sphMirrorRadius();
  m_radiusCurv[Rich::Rich2] = Rich2DE->sphMirrorRadius();

  // HPDs or PMTs
  const bool PmtActivate= ( Rich1DE -> RichPhotoDetConfig() == Rich::PMTConfig );

  // area of pixel in mm^2
  const double xSize      = ( !PmtActivate ?
                              Rich1DE->param<double>("RichHpdPixelXsize") :
                              Rich1DE->param<double>("RichPmtPixelYSize") );
  const double ySize      = ( !PmtActivate ?
                              Rich1DE->param<double>("RichHpdPixelYsize") :
                              Rich1DE->param<double>("RichPmtPixelYSize") );
  //const double demagScale       = Rich1DE->param<double>("HPDDemagScaleFactor"); // 4.8
  const double demagScale       =  ( !PmtActivate ? 
                                     Rich1DE->param<double>("HPDDemagScaleFactor") : 1.0 );
  m_pixelArea = demagScale*xSize * demagScale*ySize;

  // exp params
  m_expMinArg = vdt::fast_exp( m_minArg );

  // Informational Printout
  debug() << " Mirror radii of curvature    = "
          << m_radiusCurv[Rich::Rich1] << " " << m_radiusCurv[Rich::Rich2] << endmsg
          << " Pixel area                   = " << m_pixelArea << endmsg;

  return sc;
}

double
PhotonSignalGaussProb::predictedPixelSignal( LHCb::RichRecPhoton * photon,
                                             const Rich::ParticleIDType id ) const
{
  // Protect against below threshold hypothesis
  if ( id == Rich::BelowThreshold ) return 0;

  if ( !photon->expPixelSignalPhots().dataIsValid(id) )
  {
    using namespace boost::numeric;

    // static variables
    static const double maxSignal ( bounds<LHCb::RichRecPhoton::FloatType>::highest()  );
    static const double minSignal ( bounds<LHCb::RichRecPhoton::FloatType>::smallest() );

    // Which detector
    const Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();

    // Reconstructed Cherenkov theta angle
    const double thetaReco = photon->geomPhoton().CherenkovTheta();

    // Compute the expected pixel contribution
    // See note LHCB/98-040 page 10 equation 16 for the details of where this comes from
    double pixelSignal = photon->geomPhoton().activeSegmentFraction() *
      ( ( signalProb(photon, id) *
          m_signal->nSignalPhotons(photon->richRecSegment(),id) ) +
        ( scatterProb(photon, id) *
          m_signal->nScatteredPhotons(photon->richRecSegment(),id) ) ) *
      4.0 * m_pixelArea / ( m_radiusCurv[det] * m_radiusCurv[det] *
                            (thetaReco>1e-10 ? thetaReco : 1e-10) );

    // check for (over/under)flows
    if      ( pixelSignal > maxSignal ) { pixelSignal = maxSignal; }
    else if ( pixelSignal < minSignal ) { pixelSignal = minSignal; }

    // save final result
    photon->setExpPixelSignalPhots( id, (LHCb::RichRecPhoton::FloatType)(pixelSignal) );

  }

  return photon->expPixelSignalPhots( id );
}

double PhotonSignalGaussProb::signalProbFunc( const double thetaDiff,
                                              const double thetaExpRes ) const
{
  // See note LHCB/98-040 page 11 equation 18
  const double expArg = -0.5*thetaDiff*thetaDiff/(thetaExpRes*thetaExpRes);
  return ( ( expArg>m_minArg ? vdt::fast_exp(expArg) : m_expMinArg ) /
           ( root_two_pi*thetaExpRes ) );
}

double
PhotonSignalGaussProb::signalProb( LHCb::RichRecPhoton * photon,
                                   const Rich::ParticleIDType id ) const
{
  // Protect against below threshold hypothesis
  if ( id == Rich::BelowThreshold ) return 0;

  // Expected Cherenkov theta angle
  const double thetaExp = m_ckAngle->avgCherenkovTheta(photon->richRecSegment(),id);
  if ( thetaExp < 0.000001 ) return 0.0;

  // Expected Cherenkov theta angle resolution
  const double thetaExpRes = m_ckRes->ckThetaResolution(photon->richRecSegment(),id);

  // The difference between reco and expected
  const double thetaDiff = photon->geomPhoton().CherenkovTheta() - thetaExp;
  //if ( fabs(thetaDiff) > 30.0*thetaExpRes ) return 0.0;

  // return the expected signal contribution
  return this->signalProbFunc(thetaDiff,thetaExpRes) / (2.0*M_PI);
}

double
PhotonSignalGaussProb::scatterProb( LHCb::RichRecPhoton * photon,
                                    const Rich::ParticleIDType id ) const
{
  // Protect against below threshold hypothesis
  if ( id == Rich::BelowThreshold ) return 0;

  if ( Rich::Aerogel == photon->richRecSegment()->trackSegment().radiator() )
  {

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( photon->richRecSegment(), id );
    if ( thetaExp < 0.000001 ) return 0.0;

    // Reconstructed Cherenkov theta angle
    const double thetaRec = photon->geomPhoton().CherenkovTheta();

    // Compute the scattering
    double fbkg = 0.0;
    if ( thetaRec < thetaExp )
    {
      fbkg = ( std::exp(17.0*thetaRec) - 1.0 ) / ( std::exp(17.0*thetaExp) - 1.0 );
    }
    else if ( thetaRec < 0.5*M_PI + thetaExp - 0.04 )
    {
      fbkg = std::cos( thetaRec - thetaExp + 0.04 );
      fbkg = fbkg*fbkg/0.9984;
    }
    else
    {
      return 0.0;
    }

    // return the probability
    return 2.0 * fbkg / (M_PI*M_PI);
  }

  return 0.;
}
