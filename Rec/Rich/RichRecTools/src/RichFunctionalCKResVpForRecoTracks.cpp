
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResVpForRecoTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResVpForRecoTracks
 *
 *  $Id: RichFunctionalCKResVpForRecoTracks.cpp,v 1.7 2006-05-10 09:07:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichFunctionalCKResVpForRecoTracks.h"

// namespaces
using namespace LHCb;

//----------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalCKResVpForRecoTracks>          s_factory ;
const        IToolFactory& RichFunctionalCKResVpForRecoTracksFactory = s_factory ;

// Standard constructor
RichFunctionalCKResVpForRecoTracks::
RichFunctionalCKResVpForRecoTracks ( const std::string& type,
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

}

StatusCode RichFunctionalCKResVpForRecoTracks::initialize()
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
  m_chromFact[Rich::Aerogel] = ( aeroI>0  ? m_refIndex->refractiveIndexRMS(Rich::Aerogel)/aeroI : 0 );
  const double rich1GasI = m_refIndex->refractiveIndex(Rich::Rich1Gas);
  m_chromFact[Rich::Rich1Gas]   = ( rich1GasI>0 ? m_refIndex->refractiveIndexRMS(Rich::Rich1Gas)/rich1GasI : 0 );
  const double rich2GasI = m_refIndex->refractiveIndex(Rich::Rich2Gas);
  m_chromFact[Rich::Rich2Gas]     = ( rich2GasI>0   ? m_refIndex->refractiveIndexRMS(Rich::Rich2Gas)/rich2GasI : 0 );

  // scattering factors
  // ---------------------------------------------------------------------------------------------

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

  return sc;
}

StatusCode RichFunctionalCKResVpForRecoTracks::finalize()
{
  // Execute base class method
  return RichRecHistoToolBase::finalize();
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
    //if ( Rich::TrackParent::Track != tkID.parentType() )
    //{
    //  Exception( "Track parent type is not Track or TrStoredTrack" );
    //}

    // Expected Cherenkov theta angle
    const double ckExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( ckExp > 1e-6 )
    {

      // track type
      const Rich::Track::Type tkType = tkID.trackType();

      // track segment shortcut
      const RichTrackSegment & tkSeg = segment->trackSegment();

      // radiator
      const Rich::RadiatorType rad = tkSeg.radiator();

      // momentum for this segment
      const double ptot = sqrt(tkSeg.bestMomentum().Mag2()) / GeV;

      // tan(cktheta)
      const double tanCkExp = tan(ckExp);

      // asymtopic error
      //const double asymptotErr = gsl_pow_2( (m_asmpt[rad])[tkType] );
      const double asymptotErr = 0;
      res2 += asymptotErr;

      // chromatic error
      const double chromatErr = gsl_pow_2( m_chromFact[rad] / tanCkExp );
      res2 += chromatErr;

      // multiple scattering
      const Gaudi::XYZVector & entV = tkSeg.entryMomentum();
      const double tx = ( fabs(entV.z())>0 ? entV.x() / entV.z() : 0 );
      const double ty = ( fabs(entV.z())>0 ? entV.y() / entV.z() : 0 );
      const double effectiveLength = sqrt( 1 + tx*tx + ty*ty ) * m_matThickness[rad];
      // should actually be material between last measured point and exit point
      // should also cache this information in the RichRecSegment class ?
      //HepPoint3D startPoint;
      //findLastMeasuredPoint( segment, startPoint );
      //const double effectiveLength = transSvc()->distanceInRadUnits( startPoint,
      //                                                              tkSeg.exitPoint() );
      //const double effectiveLength = transSvc()->distanceInRadUnits( tkSeg.entryPoint(),
      //                                                              tkSeg.exitPoint() );
      const double multScattCoeff  = m_scatt * sqrt(effectiveLength)*(1+0.038*log(effectiveLength));
      const double scattErr        = 2 * gsl_pow_2(multScattCoeff/ptot);
      res2 += scattErr;

      // CRJ : Should consider moving the pure geometry errors into the RichTrackSegment

      // track curvature in the radiator volume
      const double curvErr = 
        ( Rich::Aerogel == rad ? 0 :
          gsl_pow_2(Rich::Geom::AngleBetween(tkSeg.entryMomentum(),tkSeg.exitMomentum())/4) );
      res2 += curvErr;

      // tracking direction errors
      const double dirErr = tkSeg.entryErrors().errTX2() + tkSeg.entryErrors().errTY2();
      res2 += dirErr;

      // momentum error
      const double mass2 = m_richPartProp->massSq(id)/(GeV*GeV);
      const double massFactor = mass2 / ( mass2 + ptot*ptot );
      const double momErr = ( tkSeg.entryErrors().errP2()/(GeV*GeV) *
                              gsl_pow_2( massFactor / ptot / tanCkExp ) );
      res2 += momErr;

      // Histos
      if ( produceHistos() )
      {
        // Histo stuff
        const RichHistoID hid;
        MAX_CKTHETA_RAD;
        MIN_CKTHETA_RAD;
        // Versus CK theta
        profile1D( ckExp, sqrt(asymptotErr), Rich::text(tkType)+"/"+hid(rad,id,"asymErrVc"),
                   "Asymptotic CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ckExp, sqrt(chromatErr), Rich::text(tkType)+"/"+hid(rad,id,"chroErrVc"),
                   "Chromatic CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ckExp, sqrt(scattErr), Rich::text(tkType)+"/"+hid(rad,id,"scatErrVc"),
                   "Scattering CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ckExp, sqrt(curvErr), Rich::text(tkType)+"/"+hid(rad,id,"curvErrVc"),
                   "Curvature CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ckExp, sqrt(dirErr), Rich::text(tkType)+"/"+hid(rad,id,"dirErrVc"),
                   "Track direction CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ckExp, sqrt(momErr), Rich::text(tkType)+"/"+hid(rad,id,"momErrVc"),
                   "Track momentum CK theta error V CK theta",
                   minCkTheta[rad], maxCkTheta[rad] );
        // Versus momentum
        profile1D( ptot, sqrt(asymptotErr), Rich::text(tkType)+"/"+hid(rad,id,"asymErrVp"),
                   "Asymptotic CK theta error V momentum",
                   0, 100 );
        profile1D( ptot, sqrt(chromatErr), Rich::text(tkType)+"/"+hid(rad,id,"chroErrVp"),
                   "Chromatic CK theta error V momentum",
                   0, 100 );
        profile1D( ptot, sqrt(scattErr), Rich::text(tkType)+"/"+hid(rad,id,"scatErrVp"),
                   "Scattering CK theta error V momentum",
                   0, 100 );
        profile1D( ptot, sqrt(curvErr), Rich::text(tkType)+"/"+hid(rad,id,"curvErrVp"),
                   "Curvature CK theta error V momentum",
                   0, 100 );
        profile1D( ptot, sqrt(dirErr), Rich::text(tkType)+"/"+hid(rad,id,"dirErrVp"),
                   "Track direction CK theta error V momentum",
                   0, 100 );
        profile1D( ptot, sqrt(momErr), Rich::text(tkType)+"/"+hid(rad,id,"momErrVp"),
                   "Track momentum CK theta error V momentum",
                   0, 100 );
      }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Track " << segment->richRecTrack()->key() << " " << rad << " " << id
                << " : ptot " << ptot << " ckExp " << ckExp << endreq;
        debug() << "  Rad length " << effectiveLength << endreq;
        debug() << "  Asmy " << asymptotErr << " chro " << chromatErr << " scatt "
                << scattErr << " curv " << curvErr << " dir " << dirErr
                << " mom " << momErr << " : " << sqrt(res2) << endreq;
      }

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

/*
bool
RichFunctionalCKResVpForRecoTracks::findLastMeasuredPoint( RichRecSegment * segment,
                                                           HepPoint3D & point ) const
{
  // pointer to underlying track
  const Track * trTrack =
    dynamic_cast<const Track*>(segment->richRecTrack()->parentTrack());
  if ( !trTrack ) Exception( "Null Track pointer" );

  // track segment shortcut
  const RichTrackSegment & tkSeg = segment->trackSegment();

  // get z position of last measurement before start of track segment
  // a better search could perhaps be used here ?
  const Measurement * lastMeas = 0;
  const std::vector<Measurement *> & measurements = trTrack->measurements();
  for ( std::vector<Measurement *>::const_iterator iM = measurements.begin();
        iM != measurements.end(); ++iM )
  {
    if      ( (*iM)->z() < tkSeg.entryPoint().z() ) { lastMeas = *iM; }
    else if ( (*iM)->z() > tkSeg.entryPoint().z() ) { break;          }
  }
  if ( !lastMeas ) return false;

  // get the track position at this z
  trackExtrap()->position( *trTrack, lastMeas->z(), point );

  return true;
}
*/
