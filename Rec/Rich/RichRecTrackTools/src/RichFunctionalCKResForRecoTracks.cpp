
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResForRecoTracks.cpp
 *
 *  Implementation file for tool : Rich::Rec::FunctionalCKResForRecoTracks
 *
 *  $Id: RichFunctionalCKResForRecoTracks.cpp,v 1.9 2010-02-13 18:29:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/Node.h"

// local
#include "RichFunctionalCKResForRecoTracks.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//----------------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( FunctionalCKResForRecoTracks );

// Standard constructor
FunctionalCKResForRecoTracks::
FunctionalCKResForRecoTracks ( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : HistoToolBase   ( type, name, parent  ),
    m_ckAngle       ( NULL                     ),
    m_refIndex      ( NULL                     ),
    m_trExt         ( NULL                     ),
    m_Ext           ( "TrackHerabExtrapolator" ),
    m_transSvc      ( NULL                     ),
    m_matThickness  ( Rich::NRadiatorTypes, 0  ),
    m_scatt         ( 13.6e-03                 ) // should be used with p in GeV
{

  // define interface
  declareInterface<ICherenkovResolution>(this);

  // job options

  declareProperty( "TrackExtrapolator", m_Ext );

  m_asmpt[Rich::Aerogel] = std::vector<double>( Rich::Rec::Track::NTrTypes, 0.00249 );
  (m_asmpt[Rich::Aerogel])[Rich::Rec::Track::Forward] = 0.00177;
  (m_asmpt[Rich::Aerogel])[Rich::Rec::Track::Match]   = 0.00174;
  (m_asmpt[Rich::Aerogel])[Rich::Rec::Track::VeloTT]  = 0.00218;
  (m_asmpt[Rich::Aerogel])[Rich::Rec::Track::KsTrack] = 0.00189;
  declareProperty( "AerogelAsymptopicErr", m_asmpt[Rich::Aerogel] );

  m_asmpt[Rich::Rich1Gas]   = std::vector<double>( Rich::Rec::Track::NTrTypes, 0.00150 );
  (m_asmpt[Rich::Rich1Gas])[Rich::Rec::Track::Forward] = 0.00118;
  (m_asmpt[Rich::Rich1Gas])[Rich::Rec::Track::Match]   = 0.00130;
  (m_asmpt[Rich::Rich1Gas])[Rich::Rec::Track::VeloTT]  = 0.00178;
  (m_asmpt[Rich::Rich1Gas])[Rich::Rec::Track::KsTrack] = 0.00132;
  declareProperty( "Rich1GasAsymptopicErr", m_asmpt[Rich::Rich1Gas] );

  m_asmpt[Rich::Rich2Gas]     = std::vector<double>( Rich::Rec::Track::NTrTypes, 0.000329 );
  (m_asmpt[Rich::Rich2Gas])[Rich::Rec::Track::Forward] = 0.000319;
  (m_asmpt[Rich::Rich2Gas])[Rich::Rec::Track::Match]   = 0.000457;
  (m_asmpt[Rich::Rich2Gas])[Rich::Rec::Track::Seed]    = 0.000636;
  (m_asmpt[Rich::Rich2Gas])[Rich::Rec::Track::KsTrack] = 0.000400;
  declareProperty( "Rich2GasAsymptopicErr", m_asmpt[Rich::Rich2Gas] );

  declareProperty( "UseTransportService", m_useTS = false );

  // default to having histograms disabled
  setProduceHistos ( false );
  setHistoDir( "CKRes" );

}

StatusCode FunctionalCKResForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle      );
  acquireTool( "RichTrackEffectiveRefIndex", m_refIndex  );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endmsg;

  // cache values

  // scattering factors
  // ---------------------------------------------------------------------------------------------
  // To Be Remove once TranportService is used by default

  // aero thickness
  m_matThickness[Rich::Aerogel] = m_scatt * std::sqrt(1e-2); // ?

  // Rich1Gas thickness
  m_matThickness[Rich::Rich1Gas] = m_scatt * std::sqrt(2e-2); // ?

  // Rich2Gas thickness
  const double thickWindowRich2Gas = 1.0e-2;   // rad length
  const double thickRich2Gas       = 2.04e-02; // rad length
  const double thickT3             = 0; // rad length
  m_matThickness[Rich::Rich2Gas] = thickWindowRich2Gas + thickRich2Gas + thickT3;

  // Printouts
  //---------------------------------------------------------------------------------------------

  info() << "Aerogel  Asymptopic Errors : " << m_asmpt[Rich::Aerogel]  << endmsg
         << "Rich1Gas Asymptopic Errors : " << m_asmpt[Rich::Rich1Gas] << endmsg
         << "Rich2Gas Asymptopic Errors : " << m_asmpt[Rich::Rich2Gas] << endmsg;

  if ( m_useTS ) info() << "Will use TransportService to calculate material traversed" << endmsg;

  return sc;
}

double
FunctionalCKResForRecoTracks::ckThetaResolution( LHCb::RichRecSegment * segment,
                                                 const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {
    double res2 = 0;

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // track type
    const Rich::Rec::Track::Type tkType = tkID.trackType();

    // track segment shortcut
    const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();

    // radiator
    const Rich::RadiatorType rad = tkSeg.radiator();

    // momentum for this segment
    const double ptot = std::sqrt(tkSeg.bestMomentum().Mag2()) / Gaudi::Units::GeV;

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
      effectiveLength = std::sqrt( 1 + tx*tx + ty*ty ) * m_matThickness[rad];
    }
    const double multScattCoeff  = m_scatt * std::sqrt(effectiveLength)*(1+0.038*log(effectiveLength));
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
    for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
          hypo != m_pidTypes.end(); ++hypo )
    {

      double hypo_res2 = 0;

      // Expected Cherenkov theta angle
      const double ckExp = m_ckAngle->avgCherenkovTheta( segment, *hypo );
      if ( ckExp > 1e-6 )
      {
        hypo_res2 = res2;

        // tan(cktheta)
        const double tanCkExp = tan(ckExp);

        // chromatic error
        //-------------------------------------------------------------------------------
        const double index      = m_refIndex->refractiveIndex(segment);
        const double chromFact  = ( index>0 ? m_refIndex->refractiveIndexRMS(segment)/index : 0 );
        const double chromatErr = gsl_pow_2( chromFact / tanCkExp );
        hypo_res2 += chromatErr;
        //-------------------------------------------------------------------------------

        // momentum error
        //-------------------------------------------------------------------------------
        const double mass2      = m_richPartProp->massSq(*hypo)/(Gaudi::Units::GeV*Gaudi::Units::GeV);
        const double massFactor = mass2 / ( mass2 + ptot*ptot );
        const double momErr     = ( tkSeg.entryErrors().errP2()/(Gaudi::Units::GeV*Gaudi::Units::GeV) *
                                    gsl_pow_2( massFactor / ptot / tanCkExp ) );
        hypo_res2 += momErr;
        //-------------------------------------------------------------------------------

        // Histos
        if ( produceHistos() )
        {
          // Histo stuff
          const Rich::HistoID hid;
          MAX_CKTHETA_RAD;
          MIN_CKTHETA_RAD;
          const std::string tkT = Rich::text(tkType);
          // Versus CK theta
          profile1D( ckExp, std::sqrt(asymptotErr), hid(rad,*hypo,tkT+"/asymErrVc"),
                     "Asymptotic CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(chromatErr), hid(rad,*hypo,tkT+"/chroErrVc"),
                     "Chromatic CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(scattErr), hid(rad,*hypo,tkT+"/scatErrVc"),
                     "Scattering CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(curvErr), hid(rad,*hypo,tkT+"/curvErrVc"),
                     "Curvature CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(dirErr), hid(rad,*hypo,tkT+"/dirErrVc"),
                     "Track direction CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(momErr), hid(rad,*hypo,tkT+"/momErrVc"),
                     "Track momentum CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          profile1D( ckExp, std::sqrt(hypo_res2), hid(rad,*hypo,tkT+"/overallErrVc"),
                     "Overall CK theta error V CK theta",
                     minCkTheta[rad], maxCkTheta[rad] );
          // Versus momentum
          profile1D( ptot, std::sqrt(asymptotErr), hid(rad,*hypo,tkT+"/asymErrVp"),
                     "Asymptotic CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(chromatErr), hid(rad,*hypo,tkT+"/chroErrVp"),
                     "Chromatic CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(scattErr), hid(rad,*hypo,tkT+"/scatErrVp"),
                     "Scattering CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(curvErr), hid(rad,*hypo,tkT+"/curvErrVp"),
                     "Curvature CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(dirErr), hid(rad,*hypo,tkT+"/dirErrVp"),
                     "Track direction CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(momErr), hid(rad,*hypo,tkT+"/momErrVp"),
                     "Track momentum CK theta error V momentum",
                     0, 100 );
          profile1D( ptot, std::sqrt(hypo_res2), hid(rad,*hypo,tkT+"/overallErrVp"),
                     "Overall CK theta error V momentum",
                     0, 100 );
        } // do histos

        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "Track " << segment->richRecTrack()->key() << " " << rad << " " << *hypo
                  << " : ptot " << ptot << " ckExp " << ckExp << endmsg;
          debug() << "  Rad length " << effectiveLength << endmsg;
          debug() << "  Asmy " << asymptotErr << " chro " << chromatErr << " scatt "
                  << scattErr << " curv " << curvErr << " dir " << dirErr
                  << " mom " << momErr << " : Overall " << std::sqrt(hypo_res2) << endmsg;
        }

      } // ckexp > 0

      // Save final resolution value
      segment->setCKThetaResolution( *hypo, 
                                     static_cast<LHCb::RichRecSegment::FloatType>(std::sqrt(hypo_res2)) );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Segment " << segment->key() << " : " 
                  << *hypo << " ckRes " << std::sqrt(hypo_res2) << endmsg;
      }

    } // loop over mass hypos

  }

  return segment->ckThetaResolution( id );
}

bool
FunctionalCKResForRecoTracks::findLastMeasuredPoint( LHCb::RichRecSegment * segment,
                                                     Gaudi::XYZPoint & point ) const
{
  // pointer to underlying track
  const LHCb::Track * tr =
    dynamic_cast<const LHCb::Track*>(segment->richRecTrack()->parentTrack());
  if ( !tr ) Exception( "Null Track pointer" );

  // track segment shortcut
  const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();

  // get z position of last measurement before start of track segment
  // a better search could perhaps be used here ?
  const LHCb::Node * lastMeas = NULL;
  const LHCb::Track::ConstNodeRange nodes = tr->nodes() ;
  for ( LHCb::Track::ConstNodeRange::const_iterator iM = nodes.begin();
        iM != nodes.end(); ++iM )
    if( (*iM)->type() == LHCb::Node::HitOnTrack ) {
      if      ( (*iM)->z() < tkSeg.entryPoint().z() ) { lastMeas = *iM; }
      else if ( (*iM)->z() > tkSeg.entryPoint().z() ) { break;          }
    }
  if ( !lastMeas ) return false;

  // get the track position at this z
  trackExtrap()->position( *tr, lastMeas->z(), point );

  return true;
}
