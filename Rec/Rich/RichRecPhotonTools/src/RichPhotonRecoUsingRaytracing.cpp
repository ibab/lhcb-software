
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingRaytracing.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoUsingRaytracing
 *
 * @author Claus P Buszello
 * @date 2008-01-11
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoUsingRaytracing.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY ( PhotonRecoUsingRaytracing )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingRaytracing::
PhotonRecoUsingRaytracing( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : PhotonRecoBase  ( type, name, parent ),
    m_idTool        ( NULL ),
    m_ckAngle       ( NULL ),
    m_raytrace      ( NULL ),
    m_ERLSet        (Rich::NRadiatorTypes),
    m_maxdiff       (Rich::NRadiatorTypes),
    m_maxiter       (Rich::NRadiatorTypes)
{

  // Update default CK theta correction values
  m_ckFudge[Rich::Aerogel]  = -7e-5;
  m_ckFudge[Rich::Rich1Gas] = 1.66e-4;
  m_ckFudge[Rich::Rich2Gas] = -1.0524e-5;
  //for  (int i=0;i<20;++i) m_itersA[i] = m_iters1[i] = m_iters2[i] =0;

  declareProperty( "DampingFactor", m_damping = 1. );
  declareProperty( "ERL",           m_ERL = 1. );
  declareProperty( "IterFail",      m_failiter = true );

  m_ERLSet[Rich::Aerogel]  = 1440;
  m_ERLSet[Rich::Rich1Gas] = 1440;
  m_ERLSet[Rich::Rich2Gas] = 4400;
  declareProperty( "LightPathLengths", m_ERLSet );

  m_maxdiff[Rich::Aerogel]  = 1.;
  m_maxdiff[Rich::Rich1Gas] = 1.;
  m_maxdiff[Rich::Rich2Gas] = 1.;
  declareProperty( "MaxDifference", m_maxdiff);

  m_maxiter[Rich::Aerogel]  = 10;
  m_maxiter[Rich::Rich1Gas] = 10;
  m_maxiter[Rich::Rich2Gas] = 12;
  declareProperty( "MaxIterations", m_maxiter );

}

//=============================================================================
// Destructor
//=============================================================================
PhotonRecoUsingRaytracing::~PhotonRecoUsingRaytracing() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode PhotonRecoUsingRaytracing::initialize()
{
  // initialise base class
  const StatusCode sc = PhotonRecoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichSmartIDTool",     m_idTool, 0, true  );
  acquireTool( "RichCherenkovAngle",  m_ckAngle          );
  acquireTool( "RichRayTracing",      m_raytrace         );

  // ray tracing mode
  m_mode.setAeroRefraction(true);
  m_mode.setForcedSide (true);
  info() << "Ray tracing mode " << m_mode << endmsg;

  return sc;
}

StatusCode PhotonRecoUsingRaytracing::finalize()
{
  // for  (int i=0;i<50;++i)
  //  debug() << "Iterations " << i <<": "<<m_itersA[i]<<"    "<<m_iters1[i]<<"   "<<m_iters2[i]<<endmsg;
  return PhotonRecoBase::finalize();
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and pixel
//-------------------------------------------------------------------------

StatusCode
PhotonRecoUsingRaytracing::
reconstructPhoton ( const LHCb::RichRecSegment * segment,
                    const LHCb::RichRecPixel * pixel,
                    LHCb::RichGeomPhoton& gPhoton ) const
{

  // track segment
  const LHCb::RichTrackSegment& trSeg = segment->trackSegment();
  const Rich::RadiatorType radiator   = trSeg.radiator();

  double maxdiff = m_maxdiff[radiator];
  int maxiter = m_maxiter[radiator];
  // Emission point to use for photon reconstruction
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissionPoint = trSeg.bestPoint();

  const Gaudi::XYZPoint & HP = segment->pdPanelHitPointLocal((pixel->panel()).panel());
  const double mx =  HP.x();
  const double my =  HP.y();
  const Gaudi::XYZPoint & HPp = pixel->localPosition();
  const double x = HPp.x();
  const double y = HPp.y();
  const double dx = x - mx;
  const double dy = y - my;

  Gaudi::XYZPoint locpos;
  Gaudi::XYZVector sv;

  double tphi = vdt::fast_atan2f(dy,dx);
  double phi0 = tphi;

  //estimate the length of the photon path ERL

  double ERL(0);
  if (m_ERL<0)
  {
    const double predpi = m_ckAngle->saturatedCherenkovTheta(segment);

    sv = trSeg.vectorAtThetaPhi( predpi, tphi );

    const LHCb::RichTraceMode::RayTraceResult result
      = m_raytrace->traceToDetector(trSeg.rich(),emissionPoint,sv,m_photon,trSeg,
                                    m_mode,(pixel->panel()).panel());
    if ( result < LHCb::RichTraceMode::InHPDPanel )
    {
      debug() << "raytracing failed in " << radiator << " : " << result << endmsg;
      return StatusCode::FAILURE;
    }
    locpos = m_idTool->globalToPDPanel(m_photon.detectionPoint());

    const double R2 = (locpos.y()-my)*(locpos.y()-my) + (locpos.x()-mx)*(locpos.x()-mx);

    ERL = std::sqrt(R2) / std::tan(predpi);

  }
  else { ERL = m_ERLSet[radiator]; }

  //naive theta
  double R= dx*dx+dy*dy;
  //    double ttheta =  atan(sqrt(R)/ERL);
  double ttheta =  (std::sqrt(R)/ERL);
  double theta0 =  ttheta;


  double theta1 = ttheta;
  double phi1 = tphi;


  // the actual iteration

  int ii;
  double damp = m_damping;


  double dphi,lastdphi,dtheta,lastdtheta;
  lastdtheta=lastdphi=100;
  dtheta=dphi=100;

  double phil2=100;
  double phil3=100;
  double phil4=100;
  double phil5=100;

  double thetal2=100;
  double thetal3=100;
  double thetal4=100;
  double thetal5=100;

  double tthetal2=0;
  double tthetal3=0;
  double tthetal4=0;
  double tthetal5=0;

  double phil=100;//phi0;
  double thetal=100;

  double tphil=phi0;

  double tthetal=theta0;

  for ( ii = maxiter; ii>0; --ii )
  {

    //start at estimated ttheta
    ttheta -= damp * (theta1-theta0);
    tphi -= damp * (phi1-phi0);

    if ((dphi*lastdphi < 0 ) &&fabs(lastdphi+dphi)<.00001 ){
      ttheta = (tthetal+ttheta)/2;
      tphi = (tphil+tphi)/2;
      ii=1;
      //break;
    }
    if ((dtheta*lastdtheta < 0 ) &&fabs(lastdtheta+dtheta)<.00001 ){
      ttheta = (tthetal+ttheta)/2.;
      tphi = (tphil+tphi)/2.;
      ii=1;
      //break;
    }
    if (((maxiter-ii)> 5) && fabs(phil2-phi1)<.00001&& fabs(thetal2-theta1)<.00001 ){
      //debug() << "its a trip hop" <<endmsg;
      ttheta = (ttheta + tthetal) / 2.;
      ii = 1;
      //break;
    }
    if (((maxiter-ii)>5) && fabs(phil3-phi1)<.00001 && fabs(thetal3-theta1)<.00001){
      //debug() << "its a quad hop" <<endmsg;
      ttheta = (ttheta + tthetal + tthetal2) / 3.;
      ii = 1;
      //break;
    }
    if (((maxiter-ii)> 5) && fabs(phil4-phi1)<.00001&& fabs(thetal4-theta1)<.00001 ){
      //debug() << "its a quint hop" <<endmsg;
      ii = 1; ttheta = (ttheta + tthetal + tthetal2 + thetal3) / 4.;
      //break;
    }
    if (((maxiter-ii)>5) && fabs(phil5-phi1)<.00001&& fabs(thetal5-theta1)<.00001 ){
      //debug() << "its a sext hop" <<endmsg;
      ii = 1; ttheta = (ttheta + tthetal + tthetal2+ thetal3+ thetal4) / 5.;
      //break;
    }

    lastdphi   = dphi;
    lastdtheta = dtheta;

    sv = trSeg.vectorAtThetaPhi(ttheta,tphi);

    const LHCb::RichTraceMode::RayTraceResult result
      = m_raytrace->traceToDetector(trSeg.rich(),emissionPoint,sv,m_photon,trSeg,
                                    m_mode,(pixel->panel()).panel());

    if ( result < LHCb::RichTraceMode::InHPDPanel ) {
      tphi   *= 0.95;
      ttheta *= 0.90;
      debug() << "raytracing failed in " << radiator << " : " << result << endmsg;
      continue;
    }

    locpos = m_idTool->globalToPDPanel(m_photon.detectionPoint());

    // the hit pixel corresponds to what naive theta and phi
    const double yi = locpos.y();
    const double xi = locpos.x();

    //       if (isnan(xi)){
    //  debug() <<ii<<" nan "<<locpos.x()<<" "<<m_photon.detectionPoint().x()<<" "<<ttheta<<" "<<tphi<<endmsg;
    //  debug() <<theta0<<" "<<theta1<<" "<<ERL<<endmsg;
    //       }
    const double R2 = (yi-my )*(yi-my ) +  ( xi-mx )*( xi-mx );
    //theta1 = atan(sqrt(R2)/ERL);

    tthetal5 = tthetal4;
    tthetal4 = tthetal3;
    tthetal3 = tthetal2;
    tthetal2 = tthetal;
    tthetal = ttheta;

    thetal5 = thetal4;
    thetal4 = thetal3;
    thetal3 = thetal2;
    thetal2 = thetal;
    thetal = theta1;
    theta1 = (std::sqrt(R2)/ERL);  // assuming atan is identity for tiny angles.

    tphil = tphi;

    phil5 = phil4;
    phil4 = phil3;
    phil3 = phil2;
    phil2 = phil;
    phil = phi1;



    phi1 = /* Gaudi::Units::pi + */ vdt::fast_atan2f( yi-my, xi-mx);

    dphi = phi1-phil;
    dtheta = theta1-thetal;

    if ((fabs(yi - y) <maxdiff) && (fabs(xi - x)<maxdiff)) {
      break;
    }

    if ((fabs(phi0 - phi1) <.000001) && (fabs(theta0 - theta1)<.000001)) {
      break;
    }


    //   if (ii<3) debug()<<"I used all but "<< ii << " of my iterations. Distance: "<<fabs(yi - y)<< " , "<<fabs(xi - x)<<endmsg;

    //       if (ii<10) debug()<<ii<<" "<<radiator<<endmsg;
    //       if (ii<10) debug()<<  phi0<<" "<<phi1<<" "<<tphi<<" "<<(phil+phi1)/2.<<endmsg;
    //       if (ii<10) debug()<<theta0<<" "<<theta1<<" "<<ttheta<<" "<<(thetal+theta1)/2.<<endmsg;
    //       if (ii<10) debug()<<yi<<" "<<y<<" "<<xi<<" "<<x<<" "<<my<<" "<<mx<<endmsg;



  }

  if (ii<1){
    //debug()<<"===================================="<<endmsg;
    ttheta = (ttheta+tthetal+tthetal2+tthetal3+tthetal4+tthetal5)/6.;
  }
  const double besttheta = ttheta-m_ckFudge[radiator];
  double bestphi = tphi;

  // is phi out of 0 -> 2PI range ?
  while ( bestphi <  0        ) { bestphi += 2.0*M_PI; }
  while ( bestphi >= 2.0*M_PI ) { bestphi -= 2.0*M_PI; }

  // Cannot set these yet - Could be done if needed, but at what CPU cost ?
  //Gaudi::XYZPoint & sphReflPoint = gPhoton.sphMirReflectionPoint();
  //Gaudi::XYZPoint & secReflPoint = gPhoton.flatMirReflectionPoint();

  // fraction of segment path length accessible to the photon
  // cannot determine this here so set to 1
  const double fraction(1);

  // flag to say if this photon candidate is un-ambiguous
  // cannot determine this here so set to false
  const bool unambigPhoton(false);

  // Check if the point is OK for uses as a reference
  // if (sameSide(radiator,HP,HPp))

  //if (i>0)

  // photon reco worked !
  // return StatusCode::SUCCESS;
  //  if (ii < 50) {
  //       if (radiator == Rich::Aerogel)
  //  (m_itersA[ii])+=1;
  //       if (radiator == Rich::Rich1Gas)
  //  (m_iters1[ii])+=1;
  //       if (radiator == Rich::Rich2Gas)
  //  (m_iters2[ii])+=1;
  //     }

  if ( (ii==0) && m_failiter )
  { // if we get here photon reco failed
    return StatusCode::FAILURE;
  }

  // --------------------------------------------------------------------------------------
  // CRJ : check that spherical mirror reflection point is on the same side as detection point
  // --------------------------------------------------------------------------------------
  const Gaudi::XYZPoint & sphReflPoint = m_photon.sphMirReflectionPoint();
  if ( !sameSide(radiator,sphReflPoint,pixel->globalPosition()) )
  {
    //return Warning( Rich::text(radiator)+" : Reflection point on wrong side" );
    return StatusCode::FAILURE;
  }
  if ( m_checkPhotCrossSides[radiator] && !sameSide(radiator,sphReflPoint,emissionPoint) )
  {
    //return Warning( Rich::text(radiator)+" : Photon cross between detector sides" );
    return StatusCode::FAILURE;
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Set (remaining) photon parameters
  // --------------------------------------------------------------------------------------
  gPhoton.setCherenkovTheta         ( (float)(besttheta) );
  gPhoton.setCherenkovPhi           ( (float)(bestphi)   );
  gPhoton.setActiveSegmentFraction  ( (float)(fraction)  );
  gPhoton.setDetectionPoint         ( pixel->globalPosition()    );
  gPhoton.setSmartID                ( pixel->hpdPixelCluster().primaryID() );
  gPhoton.setUnambiguousPhoton      ( unambigPhoton              );
  gPhoton.setPrimaryMirror          ( m_photon.primaryMirror()   );
  gPhoton.setSecondaryMirror        ( m_photon.secondaryMirror() );
  // --------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}
