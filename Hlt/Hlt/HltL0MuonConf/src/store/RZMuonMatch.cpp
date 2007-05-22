// $Id: RZMuonMatch.cpp,v 1.1.1.1 2007-05-22 10:20:32 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "RZMuonMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RZMuonMatch
//
// 2007-02-21 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RZMuonMatch );
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RZMuonMatch::RZMuonMatch( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
 declareProperty( "xMatchCut",   m_x_cut      ); 
 declareProperty( "sectorToll",   m_sec_tol      ); 

 declareProperty( "zMagCenter",   z_magnet_center      );
}
//=============================================================================
// Destructor
//=============================================================================
RZMuonMatch::~RZMuonMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RZMuonMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_x_cut=80;
  m_sec_tol=1.0;
  z_magnet_center=5300;
  


 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RZMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  HltAlgorithm::beginExecute();
  debug()<<" pat "<<m_patInputTracks->size()<<" hlt "<<m_inputTracks->size()
        <<endreq;
  int tt=0;
  
  for ( std::vector<Track*>::const_iterator itT = m_patInputTracks->begin();
        m_patInputTracks->end() != itT; itT++ ) {
    //info()<<" new pat track "<<endreq;
    
    Track* pTr2d = (*itT);
    pTr2d->setFlag(Track::PIDSelected,false);
    pTr2d->setFlag(Track::L0Candidate,false);
    if( pTr2d->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new pat track "<<tt<<endreq;
    tt++;
    
    StatusCode sc=tryMatch(pTr2d);
    
    if(StatusCode::SUCCESS == sc) {
      debug()<<" success "<<endreq;
      
      m_outputTracks->push_back(pTr2d);

      pTr2d->setFlag(Track::PIDSelected,true);
      pTr2d->setFlag(Track::L0Candidate,true);
      debug()<<" selected track "<<endreq;
      
      if(m_debug){
        debug() << " Selected track" << endreq;
      }
      
      setFilterPassed(true);
    }
  }
  



  
  
  HltAlgorithm::endExecute();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RZMuonMatch::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



StatusCode RZMuonMatch::tryMatch(Track* track)
{

  debug()<<" enter "<<track<<endreq;
  
  double trackDrDz = track->firstState().tx();
  int zone = track->specific();
  //phi at the center of sector (rad)
  double trackPhi  = (zone * M_PI/4.) - 3.*M_PI/8.;
  double trackZ = track->firstState().z();
  double trackR = track->firstState().x();
  // y edges of velo sector
  double y1AtVelo = trackR*sin(trackPhi-22.5*Gaudi::Units::degree);
  double y2AtVelo = trackR*sin(trackPhi+22.5*Gaudi::Units::degree);
  debug()<<trackDrDz<<" "<<trackR<<endreq;
  
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
  
  
  for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks->begin();
        m_inputTracks->end() != itMuon; itMuon++ ) {  
    
    debug()<<" enter loop new muon "<<m_inputTracks->size()<<endreq;
    
    //  std::vector<Track* >::const_iterator pMuon;
    
      
    Track* muon=(*itMuon);
    float xT=muon->firstState().x();
    float yT=muon->firstState().y();
    float zT=muon->firstState().z();
    float slopexT=muon->firstState().tx();
    float slopeyT=muon->firstState().ty();
    slopeyT=yT/(zT-zpv);
debug()<<" parameter "<<yT<<" "<<slopeyT<<" "<<zT<<" "<<trackZ<<endreq;    
    yVelo = yT -  (slopeyT * (zT - trackZ));
    debug()<<"yvelo "<<yVelo<<endreq;
    
    if (yVelo < (y2AtVelo+ m_sec_tol) && yVelo > (y1AtVelo- m_sec_tol)) {
      debug()<<" y test "<<endreq;
      
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
      
      float x_distance= x_velo_mag -x_muon_magnet;
      debug()<<"distanza in X  "<<x_distance<<" "<<x_muon_magnet<<" "
             <<x_velo_mag<<" "<<endreq;
      
//      if(fabs(x_distance)>m_x_cut)continue;        
      if(fabs(x_distance)<m_x_cut){
        return StatusCode::SUCCESS;
      }
      
            
    }  
  }
  debug()<<" end of match"<<endreq;
  
  return StatusCode::FAILURE;
}

