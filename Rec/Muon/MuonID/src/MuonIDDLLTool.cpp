// $Id: MuonIDDLLTool.cpp,v 1.6 2005-12-07 07:01:55 pkoppenb Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"

#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/IMuonGeometryTool.h"
// local
#include "MuonIDDLLTool.h"

//--------------------------------------------------------------------
//
//  ClassName  : MuonIDDLLTool
// 
//  Description: Calculates the likelihood variable for the MuonID
//
//  Author     : Joao R. Torres de Mello Neto, Miriam Gandelman 
//   updated   : Andre Massafferri 
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================

static ToolFactory<MuonIDDLLTool> s_factory;
const IToolFactory& MuonIDDLLToolFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================

MuonIDDLLTool::MuonIDDLLTool(const std::string& type, 
                             const std::string& name, const IInterface* parent) 
  : GaudiTool( type, name, parent)
  , m_trackSlopeX(0.)
  , m_Momentum(.0)
  , m_NStation()
{

  declareInterface<IMuonIDDLLTool>(this);
  declareProperty("distMuon",m_distMuon);
  declareProperty("distPion",m_distPion);

}
//==================================================================
// Initialize
//==================================================================
StatusCode MuonIDDLLTool::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc; // in principle useless

  debug() << "==> Initialise" << endreq;
  // get geometry tool
  m_iTileTool = tool<IMuonTileXYZTool>("MuonTileIDXYZ");
  if( !m_iTileTool ) {
    fatal() << "    Unable to create MuonTileIDToXYZ tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_iGeomTool = tool<IMuonGeometryTool>("MuonGeometryTool");
  if( !m_iGeomTool) {
    fatal() << "Unable to create MuonGeometry tool" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_iGeomTool->nStation(m_NStation);
  if(!sc){
    return sc;
  }
  m_stationZ.resize(m_NStation);
  int station;
  double dx,dy,dz;
  for(station = 0 ; station < m_NStation ; station++ ){
    sc = m_iGeomTool->getStationBox(station,dx,dy,
                                    m_stationZ[station],dz);
    if(!sc){
      return sc;
    }
  }
  if( m_distPion.size() != 9 || m_distMuon.size() != 9){
    err() << "OPTIONS are wrong:"
          <<" size of m_distPion or m_distMuon vector is not correct"
          << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  
}

//==================================================================
// Calculate the likelihood given a MuonID
//==================================================================

StatusCode MuonIDDLLTool::calcMuonDLL( const MuonID* muonid, double& dll){

  if ( !muonid->IsMuon() ) {
    return StatusCode::FAILURE;
  }

  const TrStoredTrack* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);

  if (!sc){
    err() << "stopped in trackExtrapolate"  << endreq;
    return StatusCode::FAILURE;
  }

  SmartRefVector<MuonCoord> mcoord = muonid->Coords();

  SmartRefVector<MuonCoord>::iterator iCoord;
  double dist = 0.;

  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){

    double x,dx,y,dy,z,dz;
        
    StatusCode sc =
      m_iTileTool->calcTilePos((*iCoord)->key(),x,dx,y,dy,z,dz);
         
    if (sc.isFailure()){
      err() << "Failed to get x,y,z of tile " << (*iCoord)->key() << endreq;
      return sc;
    }

    int station = (*iCoord)->key().station();
	
    dist = dist + pow(((x - m_trackX[station])/dx),2) +
      pow(((y - m_trackY[station])/dy),2) ;
  }

  dist = dist / mcoord.size(); 
     
  double LklhMu = m_distMuon[0] * (landau_root (dist,m_distMuon[1],m_distMuon[2])) + 
    m_distMuon[3] * (landau_root (dist,m_distMuon[4],m_distMuon[5]));
  double LklhPi = m_distPion[0] * (landau_root (dist,m_distPion[1],m_distPion[2])) + 
    m_distPion[3] * (landau_root (dist,m_distPion[4],m_distPion[5])) +
    m_distPion[6] * (landau_root (dist,m_distPion[7],m_distPion[8]));
  double small  = 1e-20;    
     
  dll = 5.0;  // initialization: is a muon
  
  if ((LklhPi >= small) && (dist < 200.0)){
    dll = LklhMu / LklhPi;   // the main case
    if (dll < small) {   // small LklhMu or/and high LklhPi
      dll = -11.0;      // thus is a pion
    } else {
      dll = log(dll);  // the main case
    }
  } else {
    dll = 5.0; // is a muon
    if (dist >= 200.0){  // change the initialization for very high dists
      dll = -11.0;  // is a pion
    }
  }

  return StatusCode::SUCCESS;

}

// defining the Landau function as placed in root


double MuonIDDLLTool::landau_root(double x, double mpv, double sigma){

  double p1[5] = {0.4259894875,-0.1249762550, 0.03984243700, -0.006298287635,   0.001511162253};
  double q1[5] = {1.0         ,-0.3388260629, 0.09594393323, -0.01608042283,    0.003778942063};
  double p2[5] = {0.1788541609, 0.1173957403, 0.01488850518, -0.001394989411,   0.0001283617211};
  double q2[5] = {1.0         , 0.7428795082, 0.3153932961,   0.06694219548,    0.008790609714};
  double p3[5] = {0.1788544503, 0.09359161662,0.006325387654, 0.00006611667319,-0.000002031049101};
  double q3[5] = {1.0         , 0.6097809921, 0.2560616665,   0.04746722384,    0.006957301675};
  double p4[5] = {0.9874054407, 118.6723273,  849.2794360,   -743.7792444,      427.0262186};
  double q4[5] = {1.0         , 106.8615961,  337.6496214,    2016.712389,      1597.063511};
  double p5[5] = {1.003675074,  167.5702434,  4789.711289,    21217.86767,     -22324.94910};
  double q5[5] = {1.0         , 156.9424537,  3745.310488,    9834.698876,      66924.28357};
  double p6[5] = {1.000827619,  664.9143136,  62972.92665,    475554.6998,     -5743609.109};
  double q6[5] = {1.0         , 651.4101098,  56974.73333,    165917.4725,     -2815759.939};
  double a1[3] = {0.04166666667,-0.01996527778, 0.02709538966};
  double a2[2] = {-1.845568670,-4.284640743};
					          
  if (sigma <= 0) return 0;
  double v = (x-mpv)/sigma;
  double u, ue, us, den;
  if (v < -5.5) {
    u   = exp(v+1.0);
    ue  = exp(-1/u);
    us  = sqrt(u);
    den = 0.3989422803*(ue/us)*(1+(a1[0]+(a1[1]+a1[2]*u)*u)*u);
  } else if(v < -1) {
    u   = exp(-v-1);
    den = exp(-u)*sqrt(u)*
      (p1[0]+(p1[1]+(p1[2]+(p1[3]+p1[4]*v)*v)*v)*v)/
      (q1[0]+(q1[1]+(q1[2]+(q1[3]+q1[4]*v)*v)*v)*v);
  } else if(v < 1) {
    den = (p2[0]+(p2[1]+(p2[2]+(p2[3]+p2[4]*v)*v)*v)*v)/
      (q2[0]+(q2[1]+(q2[2]+(q2[3]+q2[4]*v)*v)*v)*v);
  } else if(v < 5) {
    den = (p3[0]+(p3[1]+(p3[2]+(p3[3]+p3[4]*v)*v)*v)*v)/
      (q3[0]+(q3[1]+(q3[2]+(q3[3]+q3[4]*v)*v)*v)*v);
  } else if(v < 12) {
    u   = 1/v;
    den = u*u*(p4[0]+(p4[1]+(p4[2]+(p4[3]+p4[4]*u)*u)*u)*u)/
      (q4[0]+(q4[1]+(q4[2]+(q4[3]+q4[4]*u)*u)*u)*u);
  } else if(v < 50) {
    u   = 1/v;
    den = u*u*(p5[0]+(p5[1]+(p5[2]+(p5[3]+p5[4]*u)*u)*u)*u)/
      (q5[0]+(q5[1]+(q5[2]+(q5[3]+q5[4]*u)*u)*u)*u);
  } else if(v < 300) {
    u   = 1/v;
    den = u*u*(p6[0]+(p6[1]+(p6[2]+(p6[3]+p6[4]*u)*u)*u)*u)/
      (q6[0]+(q6[1]+(q6[2]+(q6[3]+q6[4]*u)*u)*u)*u);
  } else {
    u   = 1/(v-v*log(v)/(v+1));
    den = u*u*(1+(a2[0]+a2[1]*u)*u);
  }

  return den;

}

//=============================================================================
//  Method to extrapolate the tracks
//=============================================================================
StatusCode MuonIDDLLTool::trackExtrapolate(const TrStoredTrack *pTrack){

  resetTrackLocals();

  // get state closest to M1
  const TrStateP *stateP =
    dynamic_cast<const TrStateP*>(pTrack->closestState(m_stationZ[0]));
  if(!stateP){
    err() << " Failed to get stateP from track " << endreq;
    return StatusCode::FAILURE;
  }

  // get the momentum (MeV/c)
  m_Momentum = stateP->p();
 
  // get the track slope
  m_trackSlopeX = stateP->tx();
  
  // Project the state into the muon stations
  int station;
  for(station = 0; station < 5 ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    m_trackX.push_back(stateP->x() + ( stateP->tx() * 
                                       (m_stationZ[station] - stateP->z()) ));
    m_trackY.push_back(stateP->y() + ( stateP->ty() * 
                                       (m_stationZ[station] - stateP->z()) ));
  }
  debug() << " mom =  " << m_Momentum << endreq;
  debug() << " z =  " << m_stationZ[0] << endreq;
  debug() << " m_trackX " << m_trackX[0] << endreq;
  debug() << " m_trackY " << m_trackY[0] << endreq;

  return StatusCode::SUCCESS;
}
void MuonIDDLLTool::resetTrackLocals(){
  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
}

