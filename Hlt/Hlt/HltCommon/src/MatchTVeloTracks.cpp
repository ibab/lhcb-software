// $Id: MatchTVeloTracks.cpp,v 1.4 2007-12-12 13:15:22 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Track.h" 
#include "Event/State.h"
// local
#include "MatchTVeloTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchTVeloTracks
//
// 2007-03-07 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MatchTVeloTracks );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchTVeloTracks::MatchTVeloTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMatchTVeloTracks>(this);
  declareProperty( "x2dMatchCut",   m_2dx_cut =80      );
  declareProperty( "sectorToll",   m_sec_tol  =1.0    );
  
  declareProperty( "zMagCenter",   z_magnet_center =5300     );
  declareProperty( "xMatchCut",   m_x_cut  =30    );
  declareProperty( "yMatchCut",   m_y_cut  =40    ); 
  declareProperty( "spaceMatchCut",   m_space_cut =60     );  
  declareProperty( "ptkickConstant",  m_ptkickConstant  =  1263.0      );
info()<<" ecco"<<endreq; 
}
//=============================================================================
// Destructor
//=============================================================================
MatchTVeloTracks::~MatchTVeloTracks() {} 

//=============================================================================
StatusCode MatchTVeloTracks::initialize() 
{
 StatusCode sc = GaudiTool::initialize();


  m_fastPTool = tool <IFastMomentumEstimate>("FastMomentumEstimate");

 return StatusCode::SUCCESS;


}
StatusCode MatchTVeloTracks::finalize() 
{
 StatusCode sc = GaudiTool::finalize();
 return StatusCode::SUCCESS;
  
}

StatusCode MatchTVeloTracks::match2dVelo(LHCb::Track& veloTrack,LHCb::Track& Ttrack, float &x_distance )
{
  double trackDrDz = veloTrack.firstState().tx();
  int zone = veloTrack.specific();
  //phi at the center of sector (rad)
  double trackPhi  = (zone * M_PI/4.) - 3.*M_PI/8.;
  double trackZ = veloTrack.firstState().z();
  double trackR = veloTrack.firstState().x();
  // y edges of velo sector
  double y1AtVelo = trackR*sin(trackPhi-22.5*Gaudi::Units::degree);
  double y2AtVelo = trackR*sin(trackPhi+22.5*Gaudi::Units::degree);
  //debug()<<trackDrDz<<" "<<trackR<<endreq;
  
  if(y1AtVelo>y2AtVelo) {
    double yswap = y1AtVelo;
    y1AtVelo = y2AtVelo;
    y2AtVelo = yswap;   
  }
  
  
  //use zpv =z - r/slr
  double zpv = trackZ - (trackR/trackDrDz)  ;
  double x_velo_mag = 0;
  double yVelo= 0;
  double xVelo= 0;
  //  float x_cut;
  
  //double  xmin = 99999.;
  
      

  float xT=Ttrack.firstState().x();
  float yT=Ttrack.firstState().y();
  float zT=Ttrack.firstState().z();
  float slopexT=Ttrack.firstState().tx();
  float slopeyT=Ttrack.firstState().ty();
/*
  debug() << "T track first state z " << zT << endmsg;
  State & stateAtT1 = Ttrack.stateAt(State::beginT1);
  float zAtT1 = stateAtT1.z();
  debug() << "T track at T1 " << zAtT1 << endmsg;
*/


  slopeyT=yT/(zT-zpv);
//   debug()<<" parameter "<<yT<<" "<<slopeyT<<" "<<zT<<" 
//"<<trackZ<<endreq;    
  yVelo = yT -  (slopeyT * (zT - trackZ));
  //debug()<<"yvelo "<<yVelo<<endreq;
    
  if (yVelo < (y2AtVelo+ m_sec_tol) && yVelo > (y1AtVelo- m_sec_tol)) {
    //debug()<<" y test "<<endreq;
    
    //  debug() << " yvelo" << xM1<<" "<< " "<<yVelo<<endreq;
    // calculate x  
    double xVelo2 = trackR*trackR - yVelo*yVelo;
    if (xVelo2 < 0) xVelo2 = -xVelo2;
    xVelo= sqrt(xVelo2);
    if(!(trackPhi/Gaudi::Units::degree < 90 ||
         trackPhi/Gaudi::Units::degree > 270))xVelo = -xVelo;
    
    
    // get the slopes
      
    double tx = xVelo / (trackZ - zpv);
    
    //Extrapolate to the center of magnet at 5.25 m.
      // x(z') = x(z) + (dx/dz * (z' - z))
    x_velo_mag = xVelo + (tx *(z_magnet_center - trackZ));
    
    double x_muon_magnet=xT-slopexT*(zT-z_magnet_center) ;
    //debug()<<xM1 <<" "<<dxdz<<" "<<zM1<<" "<<endreq;
    
    //  double x_muon_magnet=xM2-(xM3-xM2)/(zM3-zM2)*(zM2-z_magnet_center$
    x_distance= x_velo_mag -x_muon_magnet;

    //debug()<<"distanza in X  "<<x_distance<<" "<<x_muon_magnet<<" "
    //       <<x_velo_mag<<" "<<endreq;
    
    //      if(fabs(x_distance)>m_x_cut)continue;        
    if(fabs(x_distance)<m_2dx_cut){
      return StatusCode::SUCCESS;
    }
      
    
    
  }
  
  //debug()<<" end of match"<<endreq;
  
  return StatusCode::FAILURE;

}


StatusCode MatchTVeloTracks::match3dVelo(LHCb::Track& veloTrack,
                                         LHCb::Track& Ttrack,
                                         LHCb::Track& matchedTrack,
                                         float &x_dist,
                                         float &y_dist ){
  
  double trackDxDz = veloTrack.firstState().tx();
  double trackDyDz = veloTrack.firstState().ty();
  double trackX = veloTrack.firstState().x();
  double trackY = veloTrack.firstState().y();
  double trackZ = veloTrack.firstState().z();
  
  
 // double x_velo_mag = 0;
  double yVelo_mag= 0;
  double xVelo_mag= 0;
  //  float x_cut;
  
  //double  xmin = 99999.;
  xVelo_mag=trackDxDz*(z_magnet_center-trackZ)+trackX;
  
  

  float xT=Ttrack.firstState().x();
  float yT=Ttrack.firstState().y();
  float zT=Ttrack.firstState().z();
  float slopexT=Ttrack.firstState().tx();
  // float slopeyT=muon->firstState().ty();
  
  
    
  yVelo_mag = trackY +  (trackDyDz) * (zT - trackZ);
  float xMuon_magnet=slopexT*(z_magnet_center-zT)+xT;
  x_dist=xVelo_mag - xMuon_magnet;
  y_dist=yVelo_mag-yT;
  if(fabs(x_dist)<m_x_cut&&fabs(y_dist)<m_y_cut){
    //fill output track parameter
    matchedTrack.copy(veloTrack);
    m_myState=&(Ttrack.firstState());
    matchedTrack.addToStates( *m_myState); 

    
    double zMidT =  StateParameters::ZMidT;

    LHCb::State* veloState= &(veloTrack.firstState());
    LHCb::State* TStateAtTheMiddle = &(Ttrack.closestState(zMidT));

    double qOverP = 0;
    double sigmaQOverP = 0; 
    StatusCode sc = m_fastPTool->calculate(veloState, TStateAtTheMiddle 
                                            ,qOverP, sigmaQOverP , 
                                                        false );

    debug() << "new p " << 1.0/qOverP << endmsg;





    matchedTrack.firstState().setQOverP(qOverP);   
    matchedTrack.setHistory( Track::TrackMatching );

    matchedTrack.addToAncestors(veloTrack);
    debug() << "velo track " << &veloTrack << endmsg;
    const SmartRefVector<Track> ancestors = veloTrack.ancestors();
    for (SmartRefVector<Track>::const_iterator it4 = ancestors.begin();
        it4 != ancestors.end();  ++it4){
          matchedTrack.addToAncestors(*(*it4));
          debug() << "velo track ancestors "  << (*it4) << endmsg;
    }
    const SmartRefVector<Track> Tancestors = Ttrack.ancestors();
    for (SmartRefVector<Track>::const_iterator it4 = Tancestors.begin();
        it4 != Tancestors.end();  ++it4) {
        matchedTrack.addToAncestors(*(*it4));
        debug() << "T ancestors " << (*it4) << endmsg;

    }
    //matchedTrack.addToAncestors( veloTrack.ancestors() );
    //matchedTrack.addToAncestors( Ttrack.ancestors() );
    return StatusCode::SUCCESS;    
  }  
  return StatusCode::FAILURE;
}
float MatchTVeloTracks::calcP(Track& velotrack,Track& Ttrack)
{
  double trackDxDz = velotrack.firstState().tx();
  //  double trackDyDz = track->firstState().ty();
  //double trackX = track->firstState().x();
  //double trackY = track->firstState().y();
  //double trackZ = track->firstState().z();
  double slopeX=Ttrack.firstState().tx();
  return m_ptkickConstant/(slopeX-trackDxDz);
  
    
}


