
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResVpForRecoTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResVpForRecoTracks
 *
 *  $Id: RichFunctionalCKResVpForRecoTracks.cpp,v 1.1 2005-10-13 16:01:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichFunctionalCKResVpForRecoTracks.h"

//----------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalCKResVpForRecoTracks>          s_factory ;
const        IToolFactory& RichFunctionalCKResVpForRecoTracksFactory = s_factory ;

// Standard constructor
RichFunctionalCKResVpForRecoTracks::
RichFunctionalCKResVpForRecoTracks ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase ( type, name, parent      ),
    m_ckAngle       ( 0                       ),
    m_refIndex      ( 0                       ),
    m_chromFact     ( Rich::NRadiatorTypes, 0 ),
    m_matThickness  ( Rich::NRadiatorTypes, 0 ),
    m_curvX         ( Rich::NRadiatorTypes, 0 ),
    m_curvY         ( Rich::NRadiatorTypes, 0 )
{

  // define interface
  declareInterface<IRichCherenkovResolution>(this);

  // job options

  m_asmpt[Rich::Aerogel] = std::vector<double>( Rich::Track::NTrTypes, 0.00249  );
  (m_asmpt[Rich::Aerogel])[Rich::Track::Forward] = 0.0026;
  (m_asmpt[Rich::Aerogel])[Rich::Track::Match]   = 0.00249;
  (m_asmpt[Rich::Aerogel])[Rich::Track::VeloTT]  = 0.00267;
  (m_asmpt[Rich::Aerogel])[Rich::Track::KsTrack] = 0.00264;
  declareProperty( "AerogelAsymptopicErr", m_asmpt[Rich::Aerogel] );

  m_asmpt[Rich::C4F10]   = std::vector<double>( Rich::Track::NTrTypes, 0.00150  );
  (m_asmpt[Rich::C4F10])[Rich::Track::Forward] = 0.00150;
  (m_asmpt[Rich::C4F10])[Rich::Track::Match]   = 0.00167;
  (m_asmpt[Rich::C4F10])[Rich::Track::VeloTT]  = 0.00220;
  (m_asmpt[Rich::C4F10])[Rich::Track::KsTrack] = 0.00177;
  declareProperty( "C4F10AsymptopicErr", m_asmpt[Rich::C4F10] );

  m_asmpt[Rich::CF4]     = std::vector<double>( Rich::Track::NTrTypes, 0.000329 );
  (m_asmpt[Rich::CF4])[Rich::Track::Forward] = 0.000300;
  (m_asmpt[Rich::CF4])[Rich::Track::Match]   = 0.000450;
  (m_asmpt[Rich::CF4])[Rich::Track::Seed]    = 0.000766;
  (m_asmpt[Rich::CF4])[Rich::Track::KsTrack] = 0.000400;
  declareProperty( "CF4AsymptopicErr", m_asmpt[Rich::CF4] );

}

StatusCode RichFunctionalCKResVpForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle      );
  acquireTool( "RichRefractiveIndex",     m_refIndex     );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // cache values

  // chromatic error factors
  //-----------------------------------------------------------------------------------------------
  const double aeroI = m_refIndex->refractiveIndex(Rich::Aerogel);
  m_chromFact[Rich::Aerogel] = ( aeroI>0  ? m_refIndex->refractiveIndexRMS(Rich::Aerogel)/aeroI : 0 );
  const double c4f10I = m_refIndex->refractiveIndex(Rich::C4F10);
  m_chromFact[Rich::C4F10]   = ( c4f10I>0 ? m_refIndex->refractiveIndexRMS(Rich::C4F10)/c4f10I : 0 );
  const double cf4I = m_refIndex->refractiveIndex(Rich::CF4);
  m_chromFact[Rich::CF4]     = ( cf4I>0   ? m_refIndex->refractiveIndexRMS(Rich::CF4)/cf4I : 0 );

  // scattering factors
  // ---------------------------------------------------------------------------------------------

  m_scatt = 13.6e-03; // should be used with p in GeV (CRJ : where does this come from ?)

  // aero thickness
  m_matThickness[Rich::Aerogel] = m_scatt * sqrt(1e-2); // ?

  // C4F10 thickness
  m_matThickness[Rich::C4F10]   = m_scatt * sqrt(2e-2); // ?

  // CF4 thickness
  const double thickWindowCF4 = 1.0e-2;   // rad length
  const double thickCF4       = 2.04e-02; // rad length
  const double thickT3        = 0; // rad length
  m_matThickness[Rich::CF4] = thickWindowCF4 + thickCF4 + thickT3;

  // Track curvature
  // ---------------------------------------------------------------------------------------------

  m_curvX[Rich::Aerogel] = 0;
  m_curvX[Rich::C4F10]   = 5.770e-3;
  m_curvX[Rich::CF4]     = 4.702e-3;

  m_curvY[Rich::Aerogel] = 0;
  m_curvY[Rich::C4F10]   = 0.154e-3;
  m_curvY[Rich::CF4]     = 0.272e-3;

  // Printouts
  //---------------------------------------------------------------------------------------------

  info() << "Aerogel Asymptopic Errors : " << m_asmpt[Rich::Aerogel] << endreq
         << "C4F10 Asymptopic Errors   : " << m_asmpt[Rich::C4F10]   << endreq
         << "CF4 Asymptopic Errors     : " << m_asmpt[Rich::CF4]     << endreq;

  return sc;
}

StatusCode RichFunctionalCKResVpForRecoTracks::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichFunctionalCKResVpForRecoTracks::
ckThetaResolution( RichRecSegment * segment,
                   const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {
    double res2 = 0;

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // Check track parent type is Track or TrStoredTrack
    if ( Rich::TrackParent::Track         != tkID.parentType() &&
         Rich::TrackParent::TrStoredTrack != tkID.parentType() )
    {
      Exception( "Track parent type is not Track or TrStoredTrack" );
    }

    // Expected Cherenkov theta angle
    const double ckExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( ckExp > 1e-6 )
    {

      // track type
      const Rich::Track::Type tkType = tkID.trackType();

      // track segment shortcut
      RichTrackSegment & tkSeg = segment->trackSegment();

      // radiator
      const Rich::RadiatorType rad = tkSeg.radiator();

      // momentum for this segment
      const double ptot = tkSeg.bestMomentum().mag() / GeV;

      // tan(cktheta)
      const double tanCkExp = tan(ckExp);

      if ( rad == Rich::CF4 )
        debug() << "Track " << segment->richRecTrack()->key() << " " << id
                << " : ptot " << ptot << " ckExp " << ckExp << endreq;

      // asymtopic error
      const double asymptotErr = gsl_pow_2( (m_asmpt[rad])[tkType] );
      res2 += asymptotErr;

      // chromatic error
      const double chromatErr = gsl_pow_2( m_chromFact[rad] / tanCkExp );
      res2 += chromatErr;

      // multiple scattering
      const double tx = tkSeg.entryMomentum().x() / tkSeg.entryMomentum().z();
      const double ty = tkSeg.entryMomentum().y() / tkSeg.entryMomentum().z();
      const double effectiveLength = sqrt( 1 + tx*tx + ty*ty ) * m_matThickness[rad];
      const double multScattCoeff  = m_scatt * sqrt(effectiveLength)*(1+0.038*log(effectiveLength));
      const double scattErr = 2 * gsl_pow_2(multScattCoeff/ptot);
      res2 += scattErr;
      if ( rad == Rich::CF4 )
        debug() << "  tx " << tx << " ty " << ty << " matT " << m_matThickness[rad]
                << " effL " << effectiveLength << " scatt " <<  m_scatt << endreq;

      // track curvature in the radiator volume
      const double curvErr = (gsl_pow_2(m_curvX[rad]) + gsl_pow_2(m_curvY[rad]))/(ptot*ptot);
      res2 += curvErr;

      // tracking direction errors
      const double dirErr = tkSeg.entryErrors().errTX2() + tkSeg.entryErrors().errTY2();
      res2 += dirErr;

      // momentum error
      const double mass2 = m_richPartProp->massSq(id)/(GeV*GeV);
      const double massFactor = mass2 / ( mass2 + ptot*ptot );
      const double momErr =
        tkSeg.entryErrors().errP2()/(GeV*GeV) * gsl_pow_2( massFactor / ptot / tanCkExp );
      res2 += momErr;

      if ( rad == Rich::CF4 )
        debug() << "  Asmy " << asymptotErr << " chro " << chromatErr << " scatt "
                << scattErr << " curv " << curvErr << " dir " << dirErr
                << " mom " << momErr << " : " << sqrt(res2) << endreq;

    }

    // Save final resolution value
    segment->setCKThetaResolution( id, sqrt(res2) );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Segment " << segment->key() << " : " << id << " ckRes " << sqrt(res2) << endreq;
    }

  }

  return segment->ckThetaResolution( id );
}
