// $Id: MuonIDDLLTool.cpp,v 1.5 2005-10-26 06:18:00 pkoppenb Exp $

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
//
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
{

  declareInterface<IMuonIDDLLTool>(this);
  declareProperty("distPion",m_distPion);
  declareProperty("distMuon",m_distMuon);

}
//==================================================================
// Initialize
//==================================================================
StatusCode MuonIDDLLTool::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  debug()  << "==> Initialise" << endreq;
  // get geometry tool
  m_iTileTool = tool<IMuonTileXYZTool>("MuonTileIDXYZ") ;
  m_iGeomTool = tool<IMuonGeometryTool>("MuonGeometryTool");

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
  if( m_distPion.size() == 0 || m_distMuon.size() == 0){
    err() << "OPTIONS are wrong:"
          << " size of m_distPion or m_distMuon vector is not correct"
          << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
  
}

//==================================================================
// Calculate the likelihood given a MuonID
//==================================================================
StatusCode MuonIDDLLTool::calcMuonDLL( const MuonID* muonid, double& dll ) {


  if ( !muonid->IsMuon() ) {
    return StatusCode::FAILURE;
  }
  const TrStoredTrack* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);
  if(!sc){
    err() << "stopped in trackExtrapolate"  << endreq;
    return StatusCode::FAILURE;
  }

  SmartRefVector<MuonCoord> mcoord = muonid->Coords();
  SmartRefVector<MuonCoord>::iterator iCoord;
  double dist = 0.;
  for( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){
    double x,dx,y,dy,z,dz;
    StatusCode sc =
      m_iTileTool->calcTilePos((*iCoord)->key(),x,dx,y,dy,z,dz);
    if(sc.isFailure()){
      err() << "Failed to get x,y,z of tile " << (*iCoord)->key() << endreq;
      return sc;
    }
    int station = (*iCoord)->key().station();
    dist = dist + sqrt( pow(x - m_trackX[station],2) +
                        pow(y - m_trackY[station],2) );
	
  }
  dist = dist / mcoord.size(); 
  double distmax = 600.;
  double nbins = 30.;
  double small = 1.*pow(10,-20);
  int ibin = (int)( dist * nbins / distmax );
  debug()  << "ibin =  " << ibin << endreq;
  debug()  << "mdistpion =  " << m_distPion[0] << endreq;
  debug()  << "m_distPion[ibin] =  " << m_distPion[ibin] << endreq;
  debug()  << "m_distMuon[ibin] =  " << m_distMuon[ibin] << endreq;
  if (dist < distmax && m_distPion[ibin] >0.) {
    dll = m_distMuon[ibin] / m_distPion[ibin];
    if (dll < small) {
	    dll = -20.;
    }
    else{
	    dll = log(dll);	 
    } 	     
  }
  else {
    return StatusCode::FAILURE;
  }
  debug()  << "dist =  " << dist << endreq;
  debug()  << "dll =  " << dll << endreq;

  return StatusCode::SUCCESS;

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
  debug()  << " mom =  " << m_Momentum << endreq;
  debug()  << " z =  " << m_stationZ[0] << endreq;
  debug()  << " m_trackX " << m_trackX[0] << endreq;
  debug()  << " m_trackY " << m_trackY[0] << endreq;

  return StatusCode::SUCCESS;
}
void MuonIDDLLTool::resetTrackLocals(){
  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
}

