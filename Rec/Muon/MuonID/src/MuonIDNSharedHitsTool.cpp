// $Id: MuonIDNSharedHitsTool.cpp,v 1.1 2005-09-30 08:53:52 pkoppenb Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// from Event 
//#include "Event/EventHeader.h"
#include "Event/TrStoredTrack.h"
#include "Event/MuonID.h"

#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/IMuonGeometryTool.h"
// local
#include "MuonIDNSharedHitsTool.h"

//--------------------------------------------------------------------
//
//  ClassName  : MuonIDNSharedHitsTool
// 
//  Description: Calculates the number of muonID's that share a hit
//               with a given MuonID and are closest to the hit
//               than this one.
//
//  Authors    : Andre Massafferri, Erica Polycarpo, Miriam Gandelman 
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<MuonIDNSharedHitsTool> s_factory;
const IToolFactory& MuonIDNSharedHitsToolFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
MuonIDNSharedHitsTool::MuonIDNSharedHitsTool(const std::string& type, 
                          const std::string& name,
                          const IInterface* parent )
                               
  : GaudiTool ( type, name, parent )
{

  declareInterface<IMuonIDNSharedHitsTool>(this);

}
//==================================================================
// Initialize
//==================================================================
StatusCode MuonIDNSharedHitsTool::initialize() {
  
  info() << "==> Initialise" << endreq;
   // get geometry tool
     StatusCode sc;
     sc = toolSvc()->retrieveTool("MuonTileIDXYZ", m_iTileTool);
           if( sc.isFailure() ) {
            fatal() << "    Unable to create MuonTileIDToXYZ tool" << endreq;
            return sc;
           }
     sc = toolSvc()->retrieveTool("MuonGeometryTool", m_iGeomTool);
	   if( sc.isFailure() ) {
	    fatal() << "    Unable to create MuonGeometry tool" << endreq;
	   return sc;
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

  return StatusCode::SUCCESS;
  
}

//==================================================================
// Calculate the number of shared hits given a MuonID
//==================================================================
StatusCode MuonIDNSharedHitsTool::calcSharedHits( const MuonID* muonid, int& nSharedHits ) {


  // calculate the distance from the hit to the extrapolated position
  StatusCode sc = calcDist(muonid);
  if( sc.isFailure() ) {
    fatal() << " failed in calcDist 1" << endreq;
    return sc;
  }

  double dist1 = m_dist;
  debug()  << " mdist 1=  " << m_dist << endreq;
  nSharedHits = 0;

  // load all muonPid results
  SmartDataPtr<MuonIDs> muonpids ( evtSvc(), MuonIDLocation::Default );
  if( !muonpids || 0 == muonpids->size() ) {
    info() << "Failed to retrieve any MuonIDs " << endreq;
  }
  // loop over the muonIDs
  MuonIDs::const_iterator iMuon;
  for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
  
   // skip if this muonID is the same as the input or if IsMuon is false 
   if ( *iMuon == muonid || !(*iMuon)->IsMuon() ) continue;

   // see if there are shared hits between the muonIDs
   if ( compareHits( muonid, *iMuon ) ){

      // get dist for this muonID
      StatusCode sc = calcDist(*iMuon);
      if( sc.isFailure() ) {
        fatal() << " failed in calcDist 2 " << endreq;
        return sc;
      }

      double dist2 = m_dist;
      debug()  << " mdist 2=  " << m_dist << endreq;
      // the muonID which gets the number of shared hits is the one
      // which has the biggest dist
      if( dist2 < dist1 ) nSharedHits++;
   } 
  }
  debug()  << "nSharedHits =  " << nSharedHits << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Method to compare the Coords of the muonIDs
//============================================================================
bool MuonIDNSharedHitsTool::compareHits( const MuonID* muonid1, const MuonID* muonid2 ){

  bool theSame = false;
  
  SmartRefVector<MuonCoord> mcoord1 = muonid1->Coords();
  SmartRefVector<MuonCoord> mcoord2 = muonid2->Coords();
  SmartRefVector<MuonCoord>::iterator iCoord1;
  SmartRefVector<MuonCoord>::iterator iCoord2;
  for( iCoord1 = mcoord1.begin() ; iCoord1 != mcoord1.end() ; iCoord1++ ){
     for( iCoord2 = mcoord2.begin() ; iCoord2 != mcoord2.end() ; iCoord2++ ){
       if( (*iCoord1)->key() == (*iCoord2)->key() ) {
          if ( ((*iCoord1)->key().station()) > 0 )  theSame = true;
       }
     }
  }

return theSame;

}

//=============================================================================
//  Method to calculate the distance from the hit to the extrapolated position
//============================================================================
StatusCode MuonIDNSharedHitsTool::calcDist( const MuonID* muonid ){

  m_dist = 0.;

  if ( !muonid->IsMuon() ) {
      err() << "stopped: not a muon "  << endreq;
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
     double mCoordX[5] = {0.,0.,0.,0.,0.};
     double mCoordY[5] = {0.,0.,0.,0.,0.};
     double mCoordDX[5] = {0.,0.,0.,0.,0.};
     double mCoordDY[5] = {0.,0.,0.,0.,0.};
     for( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){
        double x,dx,y,dy,z,dz;
        StatusCode sc =
         m_iTileTool->calcTilePos((*iCoord)->key(),x,dx,y,dy,z,dz);
         if(sc.isFailure()){
            err() << "Failed to get x,y,z of tile " << (*iCoord)->key() << endreq;
            return sc;
	 }
        int station = (*iCoord)->key().station();
            if(mCoordX[station] == 0) {
              // this is the first coord found
              mCoordX[station] = x;
              mCoordY[station] = y;
              // difference between x,y and the extrapolated x,y
              mCoordDX[station] = ( x - m_trackX[station] ) / dx;
              mCoordDY[station] = ( y - m_trackY[station] ) / dy;
              
            }else{
              // get best match in X and Y
              if( (x - m_trackX[station])*(x - m_trackX[station]) +
                  (y - m_trackY[station])*(y - m_trackY[station])  <
                  pow((mCoordX[station]*dx - m_trackX[station]),2) +
                  pow((mCoordY[station]*dy - m_trackY[station]),2) ){
                // this Coord is a better match
                mCoordX[station] = x;
                mCoordY[station] = y;
                mCoordDX[station] = ( x - m_trackX[station] ) / dx;
                mCoordDY[station] = ( y - m_trackY[station] ) / dy;
              }
            }
     } // end of Coords loop
 
     // calculate the square of the distances
     int nstn = 0;
     for( int stn = 0 ; stn < 5 ; stn++ ){
     debug()  << " mCoordDX =  " << stn << mCoordDX[stn] << endreq;
     debug()  << " mCoordDY =  " << stn << mCoordDY[stn] << endreq;
      if ( mCoordDX[stn] != 0. ) {
       nstn++;   
       m_dist += mCoordDX[stn]*mCoordDX[stn] + mCoordDY[stn]*mCoordDY[stn]; 
      }
     }
     m_dist = m_dist / nstn; //divide by the number of station 

 return StatusCode::SUCCESS;
}
//=============================================================================
//  Method to extrapolate the tracks
//=============================================================================
StatusCode MuonIDNSharedHitsTool::trackExtrapolate(const TrStoredTrack *pTrack){

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
void MuonIDNSharedHitsTool::resetTrackLocals(){
  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
}

