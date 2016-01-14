
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

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

namespace
{
  static const double root_two_pi  = std::sqrt( 2.0 * M_PI );
  static const double maxSignal ( boost::numeric::bounds<LHCb::RichRecPhoton::FloatType>::highest()  );
  static const double minSignal ( boost::numeric::bounds<LHCb::RichRecPhoton::FloatType>::smallest() );
  static const double two_over_pi2 = 2.0 / (M_PI*M_PI);
}

//-----------------------------------------------------------------------------

// Standard constructor
PhotonSignalGaussProb::PhotonSignalGaussProb( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : ToolBase( type, name, parent )
{
  // interface
  declareInterface<IPhotonSignal>(this);
  // JOs
  declareProperty( "MinExpArg", m_minArg = -650 );
  // CRJ : Longer term this should not be a job option, but taken from the DB
  declareProperty( "Rich2PixelSignalScaleFactor", m_rich2PixelSignalScaleFactor = 4.0 );
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
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // PID types
  m_pidTypes = m_richPartProp->particleTypes();

  // Get Rich Detector elements
  const DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocations::Rich1 );
  const DeRich2 * Rich2DE = getDet<DeRich2>( DeRichLocations::Rich2 );
  m_aRichPDPanel = Rich2DE->pdPanel(Rich::left);

  // Mirror radii of curvature in mm
  m_radiusCurv[Rich::Rich1] = Rich1DE->sphMirrorRadius();
  m_radiusCurv[Rich::Rich2] = Rich2DE->sphMirrorRadius();

  m_pmtActivate = ( Rich1DE->RichPhotoDetConfig() == Rich::PMTConfig );
  m_useGrandPmtInRich2 = Rich1DE->Rich2UseGrandPmt();
  m_useMixedPmtInRich2 = Rich1DE->Rich2UseMixedPmt();

  // PD sizes
  const double xSize = ( !m_pmtActivate ?  // 0.5*mm for hpd, 2.78 mm for pmt.
                         Rich1DE->param<double>("RichHpdPixelXsize") :
                         Rich1DE->param<double>("RichPmtPixelYSize") );
  const double ySize = ( !m_pmtActivate ?  // 0.5*mm for hpd , 2.78 mm for pmt
                         Rich1DE->param<double>("RichHpdPixelYsize") :
                         Rich1DE->param<double>("RichPmtPixelYSize") );

  // for GrandPMTS large pixel size of 6 mm
  double gpmxSize = Rich1DE->param<double>("RichHpdPixelXsize");
  double gpmySize = Rich1DE->param<double>("RichHpdPixelYsize");
  if ( m_pmtActivate )
  {
    if ( Rich1DE->Rich2UseGrandPmt() &&
         Rich1DE->exists("RichGrandPmtPixelXSize") )
    {
      gpmxSize = Rich1DE->param<double>("RichGrandPmtPixelXSize");
      gpmySize = Rich1DE->param<double>("RichGrandPmtPixelYSize");
    }
    else
    {
      gpmxSize = Rich1DE->param<double>("RichPmtPixelXSize");
      gpmySize = Rich1DE->param<double>("RichPmtPixelYSize");
    }
  }

  const double demagScale = ( !m_pmtActivate ? 4.8 : 1.0 );

  // area of pixel in mm^2
  m_stdPixelArea   = demagScale*xSize    * demagScale*ySize;
  m_grandPixelArea = demagScale*gpmxSize * demagScale*gpmySize;

  m_pixelArea[Rich::Rich1] = m_stdPixelArea;
  m_pixelArea[Rich::Rich2] = ( !m_useGrandPmtInRich2 ? m_stdPixelArea : m_grandPixelArea );


  // These numbers should come from the DB eventually. Not be job options.
  if ( Rich1DE->exists("Rich2BrunelPidPixelSignalScaleFactor") )
  {
    m_rich2PixelSignalScaleFactor = Rich1DE->param<double>("Rich2BrunelPidPixelSignalScaleFactor");
  }

  m_scaleFactor[Rich::Rich1] = 4.0;
  m_scaleFactor[Rich::Rich2] = ( !m_pmtActivate ? 4.0 : 4.0 * m_rich2PixelSignalScaleFactor );

  // exp params
  m_expMinArg = _exp( m_minArg );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Informational Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() <<" RichPhotonSignalProb Pixel XY size demag scale "
            << xSize << " " << ySize
            << " " << demagScale << endmsg;
    debug() << " Mirror radii of curvature = "
            << m_radiusCurv[Rich::Rich1] << " " << m_radiusCurv[Rich::Rich2]
            << endmsg;
    debug() << " Pixel area (Rich1/Rich2) = " << m_pixelArea[Rich::Rich1]
            << " " << m_pixelArea[Rich::Rich2]
            << endmsg;
  }

  return sc;
}

void PhotonSignalGaussProb::handle ( const Incident& )
{
  // only subscribe to one sort of incident, so no need to check type
  m_prefillDone = false;
}

void PhotonSignalGaussProb::prefillPredictedPixelSignal() const
{
  if ( !m_prefillDone )
  {
    // Loop over all photons
    for ( LHCb::RichRecPhoton * photon : *(richPhotons()) )
    {
      // Compute the ID independent term
      const double A = _predictedPixelSignal(photon);
      // Loop over the mass hypos and compute and fill each value
      for ( const auto& id : m_pidTypes )
      {
        // If not alrady done, fill this hypo
        if ( !photon->expPixelSignalPhots().dataIsValid(id) )
        {
          photon->setExpPixelSignalPhots( id, A * _predictedPixelSignal(photon,id) );
        }
      }
    }
    m_prefillDone = true;
  }
}

double
PhotonSignalGaussProb::predictedPixelSignal( LHCb::RichRecPhoton * photon,
                                             const Rich::ParticleIDType id ) const
{
  // Protect against below threshold hypothesis
  if ( id == Rich::BelowThreshold ) return 0;

  if ( !photon->expPixelSignalPhots().dataIsValid(id) )
  {
    // Compute the expected pixel contribution
    // See note LHCB/98-040 page 10 equation 16 for the details of where this comes from

    const LHCb::RichRecPhoton::FloatType pixelSignal =
      _predictedPixelSignal(photon) * _predictedPixelSignal(photon,id);

    // // check for (over/under)flows
    // if      ( pixelSignal > maxSignal ) { pixelSignal = maxSignal; }
    // else if ( pixelSignal < minSignal ) { pixelSignal = minSignal; }

    // save final result
    photon->setExpPixelSignalPhots( id, pixelSignal );
  }

  return photon->expPixelSignalPhots( id );
}

double PhotonSignalGaussProb::signalProbFunc( const double thetaDiff,
                                              const double thetaExpRes ) const
{
  // See note LHCB/98-040 page 11 equation 18
  const double expArg = -0.5 * gsl_pow_2(thetaDiff/thetaExpRes);
  return ( ( expArg>m_minArg ? _exp(expArg) : m_expMinArg ) /
           ( root_two_pi*thetaExpRes ) );
}

double
PhotonSignalGaussProb::signalProb( LHCb::RichRecPhoton * photon,
                                   const Rich::ParticleIDType id ) const
{
  // Protect against below threshold hypothesis
  if ( id == Rich::BelowThreshold ) return 0.0;

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
  if ( id == Rich::BelowThreshold ) return 0.0;

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
      fbkg = ( _exp(17.0*thetaRec) - 1.0 ) / ( _exp(17.0*thetaExp) - 1.0 );
    }
    else if ( thetaRec < 0.5*M_PI + thetaExp - 0.04 )
    {
      fbkg = gsl_pow_2( std::cos( thetaRec - thetaExp + 0.04 ) ) / 0.9984;
    }
    else
    {
      return 0.0;
    }

    // return the probability
    return two_over_pi2 * fbkg;
  }

  return 0.0;
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonSignalGaussProb )

//-----------------------------------------------------------------------------
