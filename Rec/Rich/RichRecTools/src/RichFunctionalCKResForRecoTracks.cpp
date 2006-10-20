
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResForRecoTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResForRecoTracks
 *
 *  $Id: RichFunctionalCKResForRecoTracks.cpp,v 1.3 2006-10-20 13:17:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "RichFunctionalCKResForRecoTracks.h"

// namespaces
using namespace LHCb;

//----------------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichFunctionalCKResForRecoTracks );

// Standard constructor
RichFunctionalCKResForRecoTracks::
RichFunctionalCKResForRecoTracks ( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
  : RichRecHistoToolBase ( type, name, parent  ),
    m_ckAngle       ( 0                        ),
    m_refIndex      ( 0                        ),
    m_trExt         ( 0                        ),
    m_Ext           ( "TrackHerabExtrapolator" ),
    m_transSvc      ( 0                        ),
    m_chromFact     ( Rich::NRadiatorTypes, 0  ),
    m_matThickness  ( Rich::NRadiatorTypes, 0  ),
    m_scatt         ( 13.6e-03                 ) // should be used with p in GeV
{

  // define interface
  declareInterface<IRichCherenkovResolution>(this);

  // job options

  declareProperty( "TrackExtrapolator", m_Ext );

  m_asmpt[Rich::Aerogel] = std::vector<double>( Rich::Track::NTrTypes, 0.00249 );
  (m_asmpt[Rich::Aerogel])[Rich::Track::Forward] = 0.00177;
  (m_asmpt[Rich::Aerogel])[Rich::Track::Match]   = 0.00174;
  (m_asmpt[Rich::Aerogel])[Rich::Track::VeloTT]  = 0.00218;
  (m_asmpt[Rich::Aerogel])[Rich::Track::KsTrack] = 0.00189;
  declareProperty( "AerogelAsymptopicErr", m_asmpt[Rich::Aerogel] );

  m_asmpt[Rich::Rich1Gas]   = std::vector<double>( Rich::Track::NTrTypes, 0.00150 );
  (m_asmpt[Rich::Rich1Gas])[Rich::Track::Forward] = 0.00118;
  (m_asmpt[Rich::Rich1Gas])[Rich::Track::Match]   = 0.00130;
  (m_asmpt[Rich::Rich1Gas])[Rich::Track::VeloTT]  = 0.00178;
  (m_asmpt[Rich::Rich1Gas])[Rich::Track::KsTrack] = 0.00132;
  declareProperty( "Rich1GasAsymptopicErr", m_asmpt[Rich::Rich1Gas] );

  m_asmpt[Rich::Rich2Gas]     = std::vector<double>( Rich::Track::NTrTypes, 0.000329 );
  (m_asmpt[Rich::Rich2Gas])[Rich::Track::Forward] = 0.000319;
  (m_asmpt[Rich::Rich2Gas])[Rich::Track::Match]   = 0.000457;
  (m_asmpt[Rich::Rich2Gas])[Rich::Track::Seed]    = 0.000636;
  (m_asmpt[Rich::Rich2Gas])[Rich::Track::KsTrack] = 0.000400;
  declareProperty( "Rich2GasAsymptopicErr", m_asmpt[Rich::Rich2Gas] );

  declareProperty( "UseTransportService", m_useTS = false );

}

StatusCode RichFunctionalCKResForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle      );
  acquireTool( "RichRefractiveIndex",     m_refIndex     );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // cache values

  // chromatic error factors
  //-----------------------------------------------------------------------------------------------
  const double aeroI = m_refIndex->refractiveIndex(Rich::Aerogel);
  m_chromFact[Rich::Aerogel]  = ( aeroI>0     ? m_refIndex->refractiveIndexRMS(Rich::Aerogel)/aeroI : 0 );
  const double rich1GasI = m_refIndex->refractiveIndex(Rich::Rich1Gas);
  m_chromFact[Rich::Rich1Gas] = ( rich1GasI>0 ? m_refIndex->refractiveIndexRMS(Rich::Rich1Gas)/rich1GasI : 0 );
  const double rich2GasI = m_refIndex->refractiveIndex(Rich::Rich2Gas);
  m_chromFact[Rich::Rich2Gas] = ( rich2GasI>0 ? m_refIndex->refractiveIndexRMS(Rich::Rich2Gas)/rich2GasI : 0 );

  // scattering factors
  // ---------------------------------------------------------------------------------------------
  // To Be Remove once TranportService is used by default

  // aero thickness
  m_matThickness[Rich::Aerogel] = m_scatt * sqrt(1e-2); // ?

  // Rich1Gas thickness
  m_matThickness[Rich::Rich1Gas] = m_scatt * sqrt(2e-2); // ?

  // Rich2Gas thickness
  const double thickWindowRich2Gas = 1.0e-2;   // rad length
  const double thickRich2Gas       = 2.04e-02; // rad length
  const double thickT3             = 0; // rad length
  m_matThickness[Rich::Rich2Gas] = thickWindowRich2Gas + thickRich2Gas + thickT3;

  // Printouts
  //---------------------------------------------------------------------------------------------

  info() << "Aerogel  Asymptopic Errors : " << m_asmpt[Rich::Aerogel]  << endreq
         << "Rich1Gas Asymptopic Errors : " << m_asmpt[Rich::Rich1Gas] << endreq
         << "Rich2Gas Asymptopic Errors : " << m_asmpt[Rich::Rich2Gas] << endreq;

  if ( m_useTS ) info() << "Will use TransportService to calculate material traversed" << endreq;

  return sc;
}

StatusCode RichFunctionalCKResForRecoTracks::finalize()
{
  // Execute base class method
  return RichRecHistoToolBase::finalize();
}

double
RichFunctionalCKResForRecoTracks::
ckThetaResolution( RichRecSegment * segment,
                   const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {
    double res2 = 0;

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // track type
    const Rich::Track::Type tkType = tkID.trackType();

    // track segment shortcut
    const RichTrackSegment & tkSeg = segment->trackSegment();

    // radiator
    const Rich::RadiatorType rad = tkSeg.radiator();

    // momentum for this segment
    const double ptot = sqrt(tkSeg.bestMomentum().Mag2()) / Gaudi::Units::GeV;

    // asymtopic error
    //-------------------------------------------------------------------------------
    const double asymptotErr = gsl_pow_2( (m_asmpt[rad])[tkType] );
    res2 += asymptotErr;
    //-------------------------------------------------------------------------------

    // multiple scattering
    //-------------------------------------------------------------------------------
    double effectiveLength(0);
    if ( m_useTS )
    {
      Gaudi::XYZPoint startPoint;
      const bool ok = findLastMeasuredPoint( segment, startPoint );
      effectiveLength = transSvc()->distanceInRadUnits( (ok ? startPoint : tkSeg.entryPoint()),
                                                        tkSeg.exitPoint() );
    }
    else
    {
      const Gaudi::XYZVector & entV = tkSeg.entryMomentum();
      const double tx = ( fabs(entV.z())>0 ? entV.x() / entV.z() : 0 );
      const double ty = ( fabs(entV.z())>0 ? entV.y() / entV.z() : 0 );
      effectiveLength = sqrt( 1 + tx*tx + ty*ty ) * m_matThickness[rad];
    }
    const double multScattCoeff  = m_scatt * sqrt(effectiveLength)*(1+0.038*log(effectiveLength));
    const double scattErr        = 2 * gsl_pow_2(multScattCoeff/ptot);
    res2 += scattErr;
    //-------------------------------------------------------------------------------

    // track curvature in the radiator volume
    //-------------------------------------------------------------------------------
    const double curvErr =
      ( Rich::Aerogel == rad ? 0 :
        gsl_pow_2(Rich::Geom::AngleBetween(tkSeg.entryMomentum(),tkSeg.exitMomentum())/4) );
    res2 += curvErr;
    //-------------------------------------------------------------------------------

    // tracking direction errors
    //-------------------------------------------------------------------------------
    const double dirErr = tkSeg.entryErrors().errTX2() + tkSeg.entryErrors().errTY2();
    res2 += dirErr;
    //-------------------------------------------------------------------------------

    // Fill all mass hypos in one go, to save cpu (transport service slow)
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      double hypo_res2 = 0;

      // Expected Cherenkov theta angle
      const double ckExp = m_ckAngle->avgCherenkovTheta( segment, hypo );
      if ( ckExp > 1e-6 )
      {
        hypo_res2 = res2;

        // tan(cktheta)
        const double tanCkExp = tan(ckExp);

        // chromatic error
        //-------------------------------------------------------------------------------
        const double chromatErr = gsl_pow_2( m_chromFact[rad] / tanCkExp );
        hypo_res2 += chromatErr;
        //-------------------------------------------------------------------------------

        // momentum error
        //-------------------------------------------------------------------------------
        const double mass2      = m_richPartProp->massSq(hypo)/(Gaudi::Units::GeV*Gaudi::Units::GeV);
        const double massFactor = mass2 / ( mass2 + ptot*ptot );
        const double momErr     = ( tkSeg.entryErrors().errP2()/(Gaudi::Units::GeV*Gaudi::Units::GeV) *
                                    gsl_pow_2( massFactor / ptot / tanCkExp ) );
        hypo_res2 += momErr;
        //-------------------------------------------------------------------------------

        // Histos
        if ( produceHistos() )
        {
          // Histo stuff
          const RichHistoID hid;
          MAX_CKTHETA_RAD;
          MIN_CKTHETA_RAD;
          // Versus CK theta
          profile1D( ckExp, sqrt(asymptotErr), Rich::text(tkType)+"/"+hid(rad,hypo,"asymErrVc"),
                     "Asymptotic CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(chromatErr), Rich::text(tkType)+"/"+hid(rad,hypo,"chroErrVc"),
                     "Chromatic CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(scattErr), Rich::text(tkType)+"/"+hid(rad,hypo,"scatErrVc"),
                     "Scattering CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(curvErr), Rich::text(tkType)+"/"+hid(rad,hypo,"curvErrVc"),
                     "Curvature CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(dirErr), Rich::text(tkType)+"/"+hid(rad,hypo,"dirErrVc"),
                     "Track direction CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(momErr), Rich::text(tkType)+"/"+hid(rad,hypo,"momErrVc"),
                     "Track momentum CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, sqrt(hypo_res2), Rich::text(tkType)+"/"+hid(rad,hypo,"overallErrVc"),
                     "Overall CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          // Versus momentum
          profile1D( ptot, sqrt(asymptotErr), Rich::text(tkType)+"/"+hid(rad,hypo,"asymErrVp"),
                     "Asymptotic CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(chromatErr), Rich::text(tkType)+"/"+hid(rad,hypo,"chroErrVp"),
                     "Chromatic CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(scattErr), Rich::text(tkType)+"/"+hid(rad,hypo,"scatErrVp"),
                     "Scattering CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(curvErr), Rich::text(tkType)+"/"+hid(rad,hypo,"curvErrVp"),
                     "Curvature CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(dirErr), Rich::text(tkType)+"/"+hid(rad,hypo,"dirErrVp"),
                     "Track direction CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(momErr), Rich::text(tkType)+"/"+hid(rad,hypo,"momErrVp"),
                     "Track momentum CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, sqrt(hypo_res2), Rich::text(tkType)+"/"+hid(rad,hypo,"overallErrVp"),
                     "Overall CK theta error V momentum",
                     0, 100 );
        } // do histos

        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "Track " << segment->richRecTrack()->key() << " " << rad << " " << hypo
                  << " : ptot " << ptot << " ckExp " << ckExp << endreq;
          debug() << "  Rad length " << effectiveLength << endreq;
          debug() << "  Asmy " << asymptotErr << " chro " << chromatErr << " scatt "
                  << scattErr << " curv " << curvErr << " dir " << dirErr
                  << " mom " << momErr << " : Overall " << sqrt(hypo_res2) << endreq;
        }

      } // ckexp > 0

      // Save final resolution value
      segment->setCKThetaResolution( hypo, sqrt(hypo_res2) );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Segment " << segment->key() << " : " << hypo << " ckRes " << sqrt(hypo_res2) << endreq;
      }

    } // loop over mass hypos

  }

  return segment->ckThetaResolution( id );
}

bool
RichFunctionalCKResForRecoTracks::findLastMeasuredPoint( RichRecSegment * segment,
                                                         Gaudi::XYZPoint & point ) const
{
  // pointer to underlying track
  const Track * tr =
    dynamic_cast<const Track*>(segment->richRecTrack()->parentTrack());
  if ( !tr ) Exception( "Null Track pointer" );

  // track segment shortcut
  const RichTrackSegment & tkSeg = segment->trackSegment();

  // get z position of last measurement before start of track segment
  // a better search could perhaps be used here ?
  const Measurement * lastMeas = NULL;
  for ( std::vector<Measurement *>::const_iterator iM = tr->measurements().begin();
        iM != tr->measurements().end(); ++iM )
  {
    if      ( (*iM)->z() < tkSeg.entryPoint().z() ) { lastMeas = *iM; }
    else if ( (*iM)->z() > tkSeg.entryPoint().z() ) { break;          }
  }
  if ( !lastMeas ) return false;

  // get the track position at this z
  trackExtrap()->position( *tr, lastMeas->z(), point );

  return true;
}
