
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingRaytracing.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoUsingRaytracing
 *
 * CVS Log :-
 *
 * @author Claus P Buszello
 * @date 2008-01-11
 */
//-----------------------------------------------------------------------------

// local
#include <iostream>
#include "RichPhotonRecoUsingRaytracing.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY ( PhotonRecoUsingRaytracing );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingRaytracing::
PhotonRecoUsingRaytracing( const std::string& type,
			   const std::string& name,
			   const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent ),

    m_idTool        ( NULL ),
    m_ckAngle       ( NULL ),
    m_raytrace      ( NULL ),
    m_ERLSet (3),
    m_maxdiff (3),
    m_maxiter (3),
    m_fudge (3)

{

  // declare interface
  declareInterface<IPhotonReconstruction>(this);


  declareProperty( "DampingFactor", m_damping = 1. );
  declareProperty( "MaxIterations", m_maxiter );
  declareProperty( "MaxDifference", m_maxdiff);
  declareProperty( "ERL", m_ERL = 1. );
  declareProperty( "IterFail", failiter=true );
  declareProperty( "IterDiscard", discard=false );
  
  m_ERLSet[Rich::Aerogel]  = 1440;
  m_ERLSet[Rich::Rich1Gas] = 1440;
  m_ERLSet[Rich::Rich2Gas] = 4400;
  
  m_maxdiff[Rich::Aerogel]  = 1.;
  m_maxdiff[Rich::Rich1Gas] = 1.;
  m_maxdiff[Rich::Rich2Gas] = 1.;
  
  m_fudge[Rich::Aerogel]  = -7e-5;
  m_fudge[Rich::Rich1Gas] = 1.66e-4;
  m_fudge[Rich::Rich2Gas] = -1.0524e-5;
  
  declareProperty( "LightPathLengths", m_ERLSet );

  m_maxiter[Rich::Aerogel]  = 10;
  m_maxiter[Rich::Rich1Gas] = 10;
  m_maxiter[Rich::Rich2Gas] = 12;


  for  (int i=0;i<20;++i) m_itersA[i] = m_iters1[i] = m_iters2[i] =0;
  declareProperty( "LightPathLengths", m_ERLSet );

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
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  acquireTool( "RichSmartIDTool",     m_idTool, 0, true  );
  acquireTool( "RichCherenkovAngle",  m_ckAngle          );
  acquireTool( "RichRayTracing",  m_raytrace  );

  return sc;
}
StatusCode PhotonRecoUsingRaytracing::finalize()
{
//   // initialise base class
   const StatusCode sc = StatusCode::SUCCESS;
//   for  (int i=0;i<50;++i) 
//     info() << "Iterations " << i <<": "<<m_itersA[i]<<"    "<<m_iters1[i]<<"   "<<m_iters2[i]<<endreq;

  return sc;
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

  // Detector information (RICH, radiator and HPD panel)
  //const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator   = trSeg.radiator();
  //  const Rich::Side side           = pixel->panel().panel();

  float maxdiff = m_maxdiff[radiator];
  int maxiter = m_maxiter[radiator];
  // Emission point to use for photon reconstruction
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissionPoint = trSeg.bestPoint();


  const Gaudi::XYZPoint & HP = segment->pdPanelHitPointLocal((pixel->panel()).panel());
  float mx =  HP.x();
  float my =  HP.y();

  const Gaudi::XYZPoint & HPp = pixel->localPosition();
  float x = HPp.x();
  float y = HPp.y();

  float dx = x - mx;
  float dy = y - my;


  //  Gaudi::XYZPoint emissionPoint = emissionPoint;
  
  LHCb::RichTraceMode mode;
  mode.setAeroRefraction(true);
  mode.setForcedSide (true);

  Gaudi::XYZPoint hitpos;
  Gaudi::XYZPoint locpos;
  Gaudi::XYZVector sv;
  //naive phi
  //USE Rich::Maths::atan2_f LATER IF POSSIBLE

    double tphi = Rich::Maths::atan2_f(dy,dx);
    double phi0 = tphi;
  	
    //estimate the length of the photon path ERL

    float ERL;
    if (m_ERL<0){
      // is this necessary or can I just set it to some value
      //Rich::ParticleIDType hypo2 = static_cast<Rich::ParticleIDType>(2);
      //float predpi = m_ckAngle->avgCherenkovTheta(segment,hypo2);
      float predpi = 0.03; // a typical angle;
      if (radiator == Rich::Aerogel)  predpi = .15;
     


      sv = trSeg.vectorAtThetaPhi(predpi,tphi);
      
      const LHCb::RichTraceMode::RayTraceResult result = m_raytrace->traceToDetector(trSeg.rich(),emissionPoint,sv,hitpos,mode,(pixel->panel()).panel(),trSeg.avPhotonEnergy());
      if ( result < LHCb::RichTraceMode::InHPDPanel ) {
	info()<<"Error in raytracing "<<endreq;
	return StatusCode::FAILURE;;
      }
      locpos =  m_idTool->globalToPDPanel(hitpos);
      
      float R2 = (locpos.y()-my )*(locpos.y()-my ) +  ( locpos.x()-mx )*( locpos.x()-mx );
      
      ERL = sqrt(R2)/tan(predpi);
  
    }
    else ERL = m_ERLSet[trSeg.radiator()];
    
    //naive theta
    float R= dx*dx+dy*dy;
    //    float ttheta =  atan(sqrt(R)/ERL);
    double ttheta =  (sqrt(R)/ERL);
    double theta0 =  ttheta;

  
    double theta1 = ttheta;		
    double phi1 = tphi;
    

    // the actual iteration

    int ii;
    float damp = m_damping;


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

    for (ii  =maxiter;ii>0;--ii){
 
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
	//info() << "its a trip hop" <<endreq;
	ttheta = (ttheta + tthetal) / 2.;
	ii = 1;
	//break;
      }
      if (((maxiter-ii)>5) && fabs(phil3-phi1)<.00001 && fabs(thetal3-theta1)<.00001){
	//info() << "its a quad hop" <<endreq;
	ttheta = (ttheta + tthetal + tthetal2) / 3.;
	ii = 1;	
	//break;
      }
      if (((maxiter-ii)> 5) && fabs(phil4-phi1)<.00001&& fabs(thetal4-theta1)<.00001 ){
	//info() << "its a quint hop" <<endreq;
	ii = 1;	ttheta = (ttheta + tthetal + tthetal2 + thetal3) / 4.;	
	//break;
      }
      if (((maxiter-ii)>5) && fabs(phil5-phi1)<.00001&& fabs(thetal5-theta1)<.00001 ){
	//info() << "its a sext hop" <<endreq;
	ii = 1;	ttheta = (ttheta + tthetal + tthetal2+ thetal3+ thetal4) / 5.;	
	//break;
      }
      
      lastdphi = dphi;
      lastdtheta = dtheta;

      sv = trSeg.vectorAtThetaPhi(ttheta,tphi);
      const LHCb::RichTraceMode::RayTraceResult result
	= m_raytrace->traceToDetector(trSeg.rich(),emissionPoint,sv,hitpos,mode,(pixel->panel()).panel(),trSeg.avPhotonEnergy());
      
      if ( result < LHCb::RichTraceMode::InHPDPanel ) {
	tphi = 0.95 *tphi;
	ttheta = 0.9 * ttheta;
	info()<<"raytracing failed in "<<radiator<< " : "<<result<<endreq;
	continue;
      }
      

      locpos =  m_idTool->globalToPDPanel(hitpos);
      
      // the hit pixel corresponds to what naive theta and phi
      float yi = locpos.y();
      float xi = locpos.x();

//       if (isnan(xi)){
// 	info() <<ii<<" nan "<<locpos.x()<<" "<<hitpos.x()<<" "<<ttheta<<" "<<tphi<<endreq;
// 	info() <<theta0<<" "<<theta1<<" "<<ERL<<endreq;
//       }
      float R2 = (yi-my )*(yi-my ) +  ( xi-mx )*( xi-mx ); 	      
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
      theta1 = (sqrt(R2)/ERL);  // assuming atan is identity for tiny angles. 
      
      tphil = tphi;

      phil5 = phil4;
      phil4 = phil3;
      phil3 = phil2;
      phil2 = phil;
      phil = phi1;


      
      phi1 = /*Gaudi::Units::pi + */Rich::Maths::atan2_f( yi-my, xi-mx);
      
      dphi = phi1-phil;
      dtheta = theta1-thetal;

      if ((fabs(yi - y) <maxdiff) && (fabs(xi - x)<maxdiff)) {
	break;
      }
      
      if ((fabs(phi0 - phi1) <.000001) && (fabs(theta0 - theta1)<.000001)) {
	break;
      }


      //   if (ii<3) debug()<<"I used all but "<< ii << " of my iterations. Distance: "<<fabs(yi - y)<< " , "<<fabs(xi - x)<<endreq;
      
//       if (ii<10) info()<<ii<<" "<<radiator<<endreq;
//       if (ii<10) info()<<  phi0<<" "<<phi1<<" "<<tphi<<" "<<(phil+phi1)/2.<<endreq;
//       if (ii<10) info()<<theta0<<" "<<theta1<<" "<<ttheta<<" "<<(thetal+theta1)/2.<<endreq;
//       if (ii<10) info()<<yi<<" "<<y<<" "<<xi<<" "<<x<<" "<<my<<" "<<mx<<endreq;
      


    }
   
    if (ii<1){
      //info()<<"===================================="<<endreq; 
      ttheta = (ttheta+tthetal+tthetal2+tthetal3+tthetal4+tthetal5)/6.;
    }
    float besttheta = ttheta-m_fudge[radiator];
    float bestphi = tphi;
    
    
    // Cannot set these yet - Could be done if needed, but at what CPU cost ?
    //Gaudi::XYZPoint & sphReflPoint = gPhoton.sphMirReflectionPoint();
    //Gaudi::XYZPoint & secReflPoint = gPhoton.flatMirReflectionPoint();
    
    // fraction of segment path length accessible to the photon
    // cannot determine this here so set to 1
    const float fraction(1);

    // flag to say if this photon candidate is un-ambiguous
    // cannot determine this here so set to false
    const bool unambigPhoton(false);

    // Positions on the the local det plane
    // const Gaudi::XYZPoint & segPSide = segment->pdPanelHitPointLocal(side);
    //const Gaudi::XYZPoint & pixPRad  = pixel->radCorrLocalPositions().position(radiator);
    
    
    // Check if the point is OK for uses as a reference
    // if (sameSide(radiator,HP,HPp))
    
    //if (i>0) 
    
    // --------------------------------------------------------------------------------------
    // Set (remaining) photon parameters
    // --------------------------------------------------------------------------------------
    gPhoton.setCherenkovTheta         ( besttheta  );
    gPhoton.setCherenkovPhi           ( bestphi    );
    gPhoton.setActiveSegmentFraction  ( fraction       );
    gPhoton.setDetectionPoint         ( pixel->globalPosition() );
    gPhoton.setSmartID                ( pixel->hpdPixelCluster().primaryID() );
    gPhoton.setMirrorNumValid         ( unambigPhoton  );
    //gPhoton.setSphMirrorNum           ( 0 );
    //gPhoton.setFlatMirrorNum          ( 0 );
    // --------------------------------------------------------------------------------------

    // if (!sameSide(radiator,HP,HPp)) return StatusCode::FAILURE;

    // photon reco worked !
    // return StatusCode::SUCCESS;
   //  if (ii < 50) {
//       if (radiator == Rich::Aerogel)	
// 	(m_itersA[ii])+=1;
//       if (radiator == Rich::Rich1Gas)	
// 	(m_iters1[ii])+=1;
//       if (radiator == Rich::Rich2Gas)	
// 	(m_iters2[ii])+=1;
//     }

    if ((ii==0) && failiter)	// if we get here photon reco failed
      return StatusCode::FAILURE;
    else 
      return StatusCode::SUCCESS;
    
}
