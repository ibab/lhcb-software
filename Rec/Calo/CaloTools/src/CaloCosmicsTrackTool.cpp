// Include files 

// From std
#include <math.h>

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h" 

// from LHCb
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
#include "Event/ODIN.h"
#include "Event/Track.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloInterfaces/ICaloCosmicsTool.h"

// local
#include "CaloCosmicsTrackTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloCosmicsTrackTool
//
// 2008-05-17 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloCosmicsTrackTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloCosmicsTrackTool::CaloCosmicsTrackTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
  , m_eCosmics(NULL)
  , m_hCosmics(NULL)
  , m_odin(NULL)
  , m_track()
{
  declareInterface<ICaloCosmicsTrackTool>(this);

  declareProperty("CosmicsTool"     , m_cosmics = "CaloCosmicsTool");
  declareProperty("MaxChi2"         , m_chi2max =15);
  declareProperty("MinChi2"         , m_chi2min =0);
  declareProperty("UseInternalPlanes", m_intern=false);
  declareProperty("UseExternalPlanes", m_extern=false);
  declareProperty("Factor"          , m_fac =1.5);
  declareProperty("Timer"           , m_timer = "EcalElseHcal");
  declareProperty("Ntupling"        , m_tuple=false);     // produce ntuple

}
//=============================================================================
// Destructor
//=============================================================================
CaloCosmicsTrackTool::~CaloCosmicsTrackTool() {} 

//=============================================================================

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloCosmicsTrackTool::initialize() {
  StatusCode sc = GaudiTupleTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // tools
  m_eCosmics = tool<ICaloCosmicsTool>(m_cosmics , "Ecal", this);
  m_hCosmics = tool<ICaloCosmicsTool>(m_cosmics , "Hcal", this);
  // odin
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloCosmicsTrackTool::processing() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  
  m_odin->getTime();
  m_run = -1;
  m_evt = -1;
  m_bx  = -1;
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN> (LHCb::ODINLocation::Default);
  if( NULL != odin ){
    m_run = odin->runNumber();
    m_evt = (long) odin->eventNumber()  ;
    m_bx  = odin->bunchId() ;
  }
  
  // init
  m_track.reset();
  m_tracked=false;
  m_timed = false;
  m_time  = -999.;
  m_stime = 999.;
  m_chi2  = 9999.;
  m_phi   = -999.;
  m_sPhi  = 999.;
  m_theta = -999.;
  m_sTheta  = 999.;
  m_dir = 0;
  m_ref = 0; // Ecal is reference by default
  m_refPoint[0] = Gaudi::XYZPoint();
  m_refPointCov[0]=Gaudi::SymMatrix3x3();
  m_refPt = Gaudi::XYZPoint();
  m_refPtCov=Gaudi::SymMatrix3x3();
  //
  StatusCode esc = ecal()->processing();
  StatusCode hsc = hcal()->processing();
  if(esc.isFailure() )
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "EcalCosmic processing failed"<<endmsg;
  if(hsc.isFailure() )
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "HcalCosmic processing failed"<<endmsg;
  

  if( !ecal()->tracked() || !hcal()->tracked() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Cannot reconstruct Cosmics (Ecal/Hcal):("
              <<ecal()->tracked()<<"/"<<hcal()->tracked()<<")"<<endmsg;
    return StatusCode::SUCCESS;
  }
  

  // 1) ecal-hcal matching
  if( matching().isFailure() )return StatusCode::SUCCESS;
  
  // 2) reconstruct 3D trajectory
  if( fit3D().isFailure() )return StatusCode::SUCCESS;

  // 3) Produce Track object
  if( buildTrack().isFailure() )return StatusCode::SUCCESS;
  m_tracked = true;


  // 4) Timing
  m_timed = true;
  if(m_timer == "Ecal"  ){
    if(ecal()->timed()){
      m_time = ecal()->time();
      m_stime = ecal()->timeVariance();
    }else{
      m_timed = false;
    }    
  }
  else if(m_timer == "Hcal" ){
    if(hcal()->timed()){
      m_time = hcal()->time();
      m_stime = hcal()->timeVariance();
    }else{
      m_timed = false;
    }    
  }
  else if(m_timer == "EcalElseHcal" ){
    if(ecal()->timed() ){
      m_time = ecal()->time();
      m_stime = ecal()->timeVariance();
    }else if(hcal()->timed() ){
      m_time=hcal()->time();
      m_stime = hcal()->timeVariance();
    } else m_timed = false;
  } else{
    warning() << "Unexpected timer : '" << m_timer << "' -> NO TIMING INFORMATION" << endmsg;
    m_timed = false;
  }
  // Update track info
  if( timed() ){
    m_track.setFlag( LHCb::Track::Selected , true );
    // not stored anymore in Track class SHM
    //m_track.addInfo( LHCb::Track::CaloTimeInfo, m_time );
  }  


  // 5) Ntupling
  if(m_tuple){
    if( tupling(500).isFailure())return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCosmicsTrackTool::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiTupleTool::finalize();  // must be called after all other actions
}

//=============================================================================
StatusCode CaloCosmicsTrackTool::matching(){

  // match Ecal and Hcal 
  // return phi and Chi2

  m_chi2 = 9999;
  

  Gaudi::XYZVector vec = ecal()->referencePoint()-hcal()->referencePoint();
  m_dir = (vec.Y() > 0) ? +1 : -1;
  // Phi = atan(DX/DY)
  double dX  = vec.X();
  double dY  = vec.Y();
  double s2X = ecal()->referencePointVariance().X()+ hcal()->referencePointVariance().X();
  double s2Y = ecal()->referencePointVariance().Y()+ hcal()->referencePointVariance().Y();
  m_phi  = (dX != 0) ?    atan( dY/dX ) :  acos( 0. );  
  double d2 = (dX*dX+dY*dY);
  m_sPhi = (d2 !=0) ? dY*dY/d2/d2*s2X + dX*dX/d2/d2*s2Y :  acos(-1.)*acos(-1.);

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Phi = " << m_phi << " +- " << m_sPhi << endmsg;
  // Chi2 = (phi-phi_eca<l)^2/sig^2 + (phi-phi_hcal)^2/sig^2 
  double ePhi = ecal()->phi();
  double hPhi = hcal()->phi();
  double s2ePhi = ecal()->phiVariance();
  double s2hPhi = hcal()->phiVariance();

  if(m_phi<0)m_phi += acos(-1.);
  if(ePhi<0)ePhi += acos(-1.);
  if(hPhi<0)hPhi += acos(-1.);

  m_chi2 =  (m_phi-ePhi)*(m_phi-ePhi)/(s2ePhi+m_sPhi) + (m_phi-hPhi)*(m_phi-hPhi)/(s2hPhi+m_sPhi);
  m_chi2 /= 2.;

  //
  if(m_dir == 1)m_phi -= acos(-1.);




  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Chi2 = " << m_chi2 << endmsg;

  if(m_chi2 > m_chi2max)return StatusCode::FAILURE;
  if(m_chi2 < m_chi2min)return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}





//=============================================================================
StatusCode CaloCosmicsTrackTool::fit3D(){

  Gaudi::Plane3D backEcal  = ecal()->det()->plane(CaloPlane::Back);
  Gaudi::Plane3D frontHcal = hcal()->det()->plane(CaloPlane::Front);  
  Gaudi::Plane3D frontEcal = ecal()->det()->plane(CaloPlane::Front);
  //  Gaudi::Plane3D middleEcal  = ecal()->det()->plane(CaloPlane::Middle);
  //  Gaudi::Plane3D middleHcal  = hcal()->det()->plane(CaloPlane::Middle);
  Gaudi::Plane3D backHcal  = hcal()->det()->plane(CaloPlane::Back);

  // Assign closest Ecal/Hcal cells to the back/front plane respectively
  typedef std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> pointPair;
  pointPair eBound =  ecal()->extrema();
  pointPair hBound =  hcal()->extrema();
  Gaudi::XYZPoint eP = eBound.first;
  Gaudi::XYZPoint hP = hBound.first;
  Gaudi::XYZPoint eM = eBound.second;
  Gaudi::XYZPoint hM = hBound.second;
  if( dist2D(eBound.first ,hBound.second) < dist2D(eP,hP)){eP=eBound.first;hP=hBound.second;eM=eBound.second;hM=hBound.first;}
  if( dist2D(eBound.second,hBound.second) < dist2D(eP,hP)){eP=eBound.second;hP=hBound.second;eM=eBound.first;hM=hBound.first;}
  if( dist2D(eBound.second,hBound.first)  < dist2D(eP,hP)){eP=eBound.second;hP=hBound.first;eM=eBound.first;hM=hBound.second;}
  //
  double ePZ=( -backEcal.Normal().X()*eP.X()-backEcal.Normal().Y()*eP.Y() - backEcal.HesseDistance() )/backEcal.Normal().Z();  
  double hPZ=(-frontHcal.Normal().X()*hP.X()-frontHcal.Normal().Y()*hP.Y()-frontHcal.HesseDistance() )/frontHcal.Normal().Z();  
  eP.SetZ(ePZ);
  hP.SetZ(hPZ);
  double eMZ=(-frontEcal.Normal().X()*eM.X()-frontEcal.Normal().Y()*eM.Y()-frontEcal.HesseDistance() )/frontEcal.Normal().Z();  
  double hMZ=(-backHcal.Normal().X()*hM.X()-backHcal.Normal().Y()*hM.Y()-backHcal.HesseDistance() )/backHcal.Normal().Z();  
  eM.SetZ(eMZ);
  hM.SetZ(hMZ);
  const LHCb::CaloCellID ePId =  ecal()->det()->Cell( eP );
  const LHCb::CaloCellID hPId =  hcal()->det()->Cell( hP );
  const LHCb::CaloCellID eMId =  ecal()->det()->Cell( eM );
  const LHCb::CaloCellID hMId =  hcal()->det()->Cell( hM );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "Closest Points " << eP << "/" << hP <<endmsg;
    debug() << "--> " << ePId << "/" << hPId <<endmsg;  
    debug() << "Farest Points " << eM << "/" << hM <<endmsg;
    debug() << "--> " << eMId << "/" << hMId <<endmsg;
  }
  //
  std::vector<pointPair> points; // position and variance
  double ePSig = ecal()->det()->cellSize(ePId)*m_fac;
  double hPSig = hcal()->det()->cellSize(hPId)*m_fac;
  double eMSig = ecal()->det()->cellSize(eMId)*m_fac;
  double hMSig = hcal()->det()->cellSize(hMId)*m_fac;
  ePSig *= ePSig/12.;  
  hPSig *= hPSig/12.;
  eMSig *= eMSig/12.;
  hMSig *= hMSig/12.;

  // closest point
  if(m_intern){
    points.push_back(std::make_pair(eP , Gaudi::XYZPoint(ePSig,ePSig,0.)));
    points.push_back(std::make_pair(hP , Gaudi::XYZPoint(hPSig,hPSig,0.)));
  }
  
  // middle plane (should depend on the trajectory.vs.acceptance)
  points.push_back( std::make_pair( ecal()->referencePoint(),ecal()->referencePointVariance()));
  points.push_back( std::make_pair( hcal()->referencePoint(),hcal()->referencePointVariance()));

  if(m_extern){
    // maxima (should depend on the trajectory.vs.acceptance)
    points.push_back( std::make_pair(eM, Gaudi::XYZPoint(eMSig,eMSig,0.)));
    points.push_back( std::make_pair(hM, Gaudi::XYZPoint(hMSig,hMSig,0.)));
  }

  /*
  // @TODO TAKE THE TRAJECTORY.vs.ACCEPTANCE INTO ACCOUNT 
  Gaudi::Math::XYZLine   line(eP, hP-eP); // 3D line
  // if the 3D line is in the whole Ecal acceptance add Ecal middle and front plane points
  Gaudi::XYZPoint  point; 
  double mu; 
  Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,frontEcal,point,mu); 
  if( ecal()()->det()->geometry()->isInside(point) ){ 
    debug() << " cosmics cross the whole Ecal" << endmsg; 
    const LHCb::CaloCellID id =  ecal()->det()->Cell( point ); 
    points.push_back( std::make_pair(eM, Gaudi::XYZPoint(eMsig,eMsig,0.)));
  } 
  Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,backHcal,point,mu); 
  if( hcal()()->det()->geometry()->isInside(point) ){ 
    debug() << " cosmics cross the whole Hcal" << endmsg; 
    const LHCb::CaloCellID id =  hcal()->det()->Cell( point ); 
    points.push_back( std::make_pair(hM, Gaudi::XYZPoint(hMsig,hMsig,0.)));
  } 
  */
  
  //  
  double Ax = 0; 
  double Bx = 0; 
  double Cx = 0; 
  double Dx = 0; 
  double Ex = 0; 
  double Ay = 0; 
  double By = 0; 
  double Cy = 0; 
  double Dy = 0; 
  double Ey = 0;

  Gaudi::XYZVector d = ecal()->referencePoint()-hcal()->referencePoint();

  for(std::vector<pointPair>::iterator ip = points.begin() ; ip != points.end() ; ip++){
    pointPair p = *ip;
    double sig2X= p.second.X()+ d.X()/d.Z()*p.second.Z();
    double sig2Y= p.second.Y()+ d.Y()/d.Z()*p.second.Z();
    Ax += p.first.Z()*p.first.Z()/sig2X;
    Bx += p.first.Z()/sig2X;
    Cx += -(p.first.X()*p.first.Z())/sig2X;
    Dx += 1./sig2X;
    Ex += -p.first.X()/sig2X;
    //
    Ay += p.first.Z()*p.first.Z()/sig2Y;
    By += p.first.Z()/sig2Y;
    Cy += -(p.first.Y()*p.first.Z())/sig2Y;
    Dy += 1./sig2Y;
    Ey += -p.first.Y()/sig2Y;
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "(A->E)x " << Ax << " " << Bx << " " << Cx << " " << Dx << " " << Ex << endmsg;
    debug() << "(A->E)y " << Ay << " " << By << " " << Cy << " " << Dy << " " << Ey << endmsg;
  }
  
  m_tx = -(Dx*Cx-Bx*Ex)/(Ax*Dx-Bx*Bx);
  m_ty = -(Dy*Cy-By*Ey)/(Ay*Dy-By*By);
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "tx = " << m_tx << "  ty = " << m_ty << endmsg;
  double x0   =  (Bx*Cx-Ax*Ex)/(Ax*Dx-Bx*Bx);
  double y0   =  (By*Cy-Ay*Ey)/(Ay*Dy-By*By);

  // Theta 
  double tth = sqrt( m_tx*m_tx + m_ty*m_ty);
  m_theta = atan( tth );
  // Approximate error
  Gaudi::XYZVector sD = hcal()->referencePointVariance() - (-1.)*ecal()->referencePointVariance();
  m_stx = m_tx*m_tx * (sD.X()/d.X()/d.X() + sD.Z()/d.Z()/d.Z() );
  m_sty = m_ty*m_ty * (sD.Y()/d.Y()/d.Y() + sD.Z()/d.Z()/d.Z() );
  double sTth  = (m_tx/tth)*(m_tx/tth) * m_stx + (m_ty/tth)*(m_ty/tth) * m_sty;
  m_sTheta     = sTth / (1+tth*tth)/(1+tth*tth);
  
  // Phi
  double tphi = m_ty/m_tx;
  m_phi = atan(tphi);
  if(m_phi<0.)m_phi += acos(-1.);
  if(m_dir == 1)m_phi -= acos(-1.);
  
  // Ecal reference
  double ex = x0 + m_tx *ecal()->referencePoint().Z();
  double ey = y0 + m_ty *ecal()->referencePoint().Z();
  m_refPoint[0].SetX(ex);
  m_refPoint[0].SetY(ey);
  m_refPoint[0].SetZ( ecal()->referencePoint().Z() );
  m_refPointCov[0](0,0) = ecal()->referencePointVariance().X();  // APPROXIMATIVE ERROR
  m_refPointCov[0](1,1) = ecal()->referencePointVariance().Y();  // APPROXIMATIVE ERROR
  // Hcal reference
  double hx = x0 + m_tx *hcal()->referencePoint().Z();
  double hy = y0 + m_ty *hcal()->referencePoint().Z();
  m_refPoint[1].SetX(hx);
  m_refPoint[1].SetY(hy);
  m_refPoint[1].SetZ( hcal()->referencePoint().Z() );
  m_refPointCov[1](0,0)= hcal()->referencePointVariance().X();  // APPROXIMATIVE ERROR
  m_refPointCov[1](1,1)= hcal()->referencePointVariance().Y();  // APPROXIMATIVE ERROR

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " Ecal Reference Point : (" 
            << ecal()->referencePoint().X() << ","
            << ecal()->referencePoint().Y() << ","
            << ecal()->referencePoint().Z() << ") "
            << "  -> Fit3D : (" 
            << ex << ","
            << ey  << ","
            << ecal()->referencePoint().Z() << ") " << endmsg;
    debug() << " Theta = " << m_theta << endmsg;
  }
  
  m_slopes = Gaudi::XYZVector( m_tx, m_ty , m_dir );
  m_slopesCov(0,0) = m_stx;
  m_slopesCov(1,1) = m_sty;  

  return StatusCode::SUCCESS;
}
//=============================================================================
double CaloCosmicsTrackTool::dist2D(Gaudi::XYZPoint p1 ,Gaudi::XYZPoint p2){
  Gaudi::XYZVector v = p1-p2;
  return v.rho();
}
//=============================================================================
StatusCode CaloCosmicsTrackTool::buildTrack(){


  //  Gaudi::XYZVector  direction = hcal()->referencePoint() - ecal()->referencePoint();

  Gaudi::TrackVector eTrV ;
  Gaudi::TrackSymMatrix eTrM;
  eTrV(0) = m_refPoint[0].X();
  eTrV(1) = m_refPoint[0].Y();
  eTrV(2) = m_tx;
  eTrV(3) = m_ty;
  eTrV(4) = 1./ ecal()->deposit() ;
  eTrM(0,0) = m_refPointCov[0](0,0);
  eTrM(1,1) = m_refPointCov[0](1,1);
  eTrM(2,2) = m_stx;
  eTrM(3,3) = m_sty;
  LHCb::State eState = LHCb::State( eTrV , eTrM , ecal()->referencePoint().Z() , LHCb::State::MidECal);


  Gaudi::TrackVector hTrV ;
  Gaudi::TrackSymMatrix hTrM;
  hTrV(0) = m_refPoint[1].X();
  hTrV(1) = m_refPoint[1].Y();
  hTrV(2) = m_tx;
  hTrV(3) = m_ty;
  hTrV(4) = 1./ hcal()->deposit() ;
  hTrM(0,0) = m_refPointCov[1](0,0);
  hTrM(1,1) = m_refPointCov[1](1,1);
  hTrM(2,2) = m_stx;
  hTrM(3,3) = m_sty;

  LHCb::State hState = LHCb::State( hTrV , hTrM , hcal()->referencePoint().Z() , LHCb::State::MidHCal);

  // Track
  m_track.setType(LHCb::Track::Calo);
  // Default reference position is Ecal
  m_ref = 0; // reference is Ecal by default
  // Set first state according to 'timer'
  if(m_timer == "Ecal"){
    m_track.addToStates( eState ); // 1st state is Ecal
    m_track.addToStates( hState );
  }
  else if(m_timer == "Hcal"){
    m_ref = 1; // reference is Hcal
    m_track.addToStates( hState ); // 1st state is Hcal
    m_track.addToStates( eState );
  }
  else if(m_timer == "EcalElseHcal"){
    if( !ecal()->timed() && hcal()->timed()){
      m_ref = 1; // reference is Hcal
      m_track.addToStates( hState ); // 1st state is Hcal
      m_track.addToStates( eState );
    }else{
      m_track.addToStates( eState ); // 1st state is Ecal
      m_track.addToStates( hState );
    }
  }
  else{
    warning() << "Unexpected timer : '" << m_timer << "' -> 1st state is Ecal by default" << endmsg;
    m_track.addToStates( eState ); // 1st state is Ecal
    m_track.addToStates( hState );
  }

  // not stored anymore in Track class
  //m_track.addInfo( LHCb::Track::CaloMatchChi2, m_chi2);
  if( !forward())m_track.setFlag( LHCb::Track::Backward, true );


  // Global reference
  m_refPt = m_refPoint[m_ref];
  m_refPtCov = m_refPointCov[m_ref];  

  return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode CaloCosmicsTrackTool::tupling(unsigned int unit){  


  if( ecal()->tupling(unit+1).isFailure())return StatusCode::FAILURE;
  if( hcal()->tupling(unit+2).isFailure())return StatusCode::FAILURE;


  Tuple ntp = nTuple(unit, "CosmicsTrack" ,CLID_ColumnWiseTuple);
  StatusCode sc;
  Gaudi::XYZVector vec = hcal()->referencePoint()-ecal()->referencePoint();
  sc = ntp->column("Run"    , m_run );
  sc = ntp->column("Event"  , m_evt );
  sc = ntp->column("BCID"   , m_bx  );
  sc = ntp->column("L"      , vec.R() );
  sc = ntp->column("eTime"  , ecal()->time());
  sc = ntp->column("hTime"  , hcal()->time());
  sc = ntp->column("eSlot"  , ecal()->slot() );
  sc = ntp->column("hSlot"  , hcal()->slot() );
  sc = ntp->column("eDep"  , ecal()->deposit() );
  sc = ntp->column("hDep"  , hcal()->deposit() );
  sc = ntp->column("direction"   , m_dir); 
  sc = ntp->column("L"      , vec.R() );
  sc = ntp->column("chi2"   , m_chi2 );
  sc = ntp->column("phi"    , m_phi );
  sc = ntp->column("phiVariance", sqrt( m_sPhi ) );
  sc = ntp->column("theta"  , m_theta );
  sc = ntp->column("thetaVariance", m_sTheta );
  sc = ntp->column("th"  , vec.theta( ));
  sc = ntp->column("ph"  , vec.phi( ));
  sc = ntp->column("ecalRefX"   , m_refPoint[0].X() );
  sc = ntp->column("ecalRefY"   , m_refPoint[0].Y() );
  sc = ntp->column("ecalRefZ"   , m_refPoint[0].Z() );
  sc = ntp->column("hcalRefX"   , m_refPoint[1].X() );
  sc = ntp->column("hcalRefY"   , m_refPoint[1].Y() );
  sc = ntp->column("hcalRefZ"   , m_refPoint[1].Z() );
  sc = ntp->column("eTimed"  ,  (int)ecal()->timed());
  sc = ntp->column("hTimed"  ,  (int)hcal()->timed());
  sc = ntp->column("eR"  , ecal()->asymmetry());
  sc = ntp->column("hR"  , hcal()->asymmetry());
  sc = ntp->column("dX"  , vec.X());
  sc = ntp->column("dY"  , vec.Y());
  sc = ntp->column("dZ"  , vec.Z());

  sc=ntp->write(); 


  if(sc.isFailure())Warning(name() + " Ntupling failed").ignore();
  return sc; 
} 
  


StatusCode CaloCosmicsTrackTool::propagate(double z){
  Gaudi::XYZPoint  ref = m_refPoint[m_ref];
  Gaudi::SymMatrix3x3 refCov = m_refPointCov[m_ref];
  m_refPt.SetX( ref.X() + slopes().X() * (z-ref.Z()) );
  m_refPt.SetY( ref.Y() + slopes().Y() * (z-ref.Z()) );
  m_refPt.SetZ( z );
  m_refPtCov(0,0) = (refCov(0,0) + slopesCovariance()(0,0) * (z-ref.Z()) * (z-ref.Z()));
  m_refPtCov(1,1) = (refCov(1,1) + slopesCovariance()(1,1) * (z-ref.Z()) * (z-ref.Z()));
  if( !timed() )return StatusCode::SUCCESS;
  double time = (m_ref == 0) ? ecal()->time() : hcal()->time();
  Gaudi::XYZVector vec = m_refPt - m_refPoint[m_ref];
  double sign = 0;
  if( vec.Z() != 0)sign = vec.Z()/fabs(vec.Z())*m_dir;
  m_time = time + (vec.R() / Gaudi::Units::c_light) *sign;
  return StatusCode::SUCCESS;
}


StatusCode CaloCosmicsTrackTool::propagate(Gaudi::Plane3D plane){
  Gaudi::XYZPoint  ref = m_refPoint[m_ref];
  Gaudi::XYZVector vec = slopes()/slopes().R();
  Gaudi::Math::XYZLine   line(ref, vec);
  Gaudi::XYZPoint  point;
  double mu;
  Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,point,mu);
  return propagate( point.Z() );
}
