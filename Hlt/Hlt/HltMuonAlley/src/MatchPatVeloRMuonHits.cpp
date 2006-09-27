
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "MatchPatVeloRMuonHits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchPatVeloRMuonHits
//
// 2005-09-21 : Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MatchPatVeloRMuonHits );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchPatVeloRMuonHits::MatchPatVeloRMuonHits( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty( "YTol", m_yTol = 1.0);
}
//=============================================================================
// Destructor
//=============================================================================
MatchPatVeloRMuonHits::~MatchPatVeloRMuonHits() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MatchPatVeloRMuonHits::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
 
  debug() << "==> Initialize" << endmsg;
 m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" ); 
   
  m_x_cut_track.push_back(100.);
  m_x_cut_track.push_back(200.);
  m_x_cut_track.push_back(300.);
  m_x_cut_track.push_back(800.);
  z_magnet_center=5200.;
  initializeHisto(m_histoXdist,"X dist",0.,1000.,100);

  
  return StatusCode::SUCCESS;
};
//=============================================================================
bool MatchPatVeloRMuonHits::acceptTrack(const Track& track) 
{
  bool ok = !(track.checkFlag( Track::Invalid ));
  //  debug() << " accepted track ? " << ok << endreq;
  return ok;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MatchPatVeloRMuonHits::execute() {
  
  debug() << "==> Execute" << endreq;
  setFilterPassed(true);
  bool ok  = HltAlgorithm::beginExecute();  
  int matchedhits = 0;
  // loop over 2 d tracks
  for ( std::vector<Track*>::const_iterator itT =m_patInputTracks->begin();
        m_patInputTracks->end() != itT; itT++ ) {
    Track* pTr2d = (*itT);    
    if(pTr2d->checkFlag( Track::Backward )) continue; // skip backward tracks
    if(pTr2d->checkFlag(Track::PIDSelected)) continue; 
    //skip the ones already matched
    StatusCode sc = match2dMuon(pTr2d);  
    
    if(StatusCode::SUCCESS == sc) {
      m_outputTracks->push_back(pTr2d);
//      pTr2d->setFlag(Track::PIDSelected,true);
      matchedhits++;
      //      debug() << " Selected track" << endreq;
    }
  }
  //info() << "Matched with hits " << matchedhits << endreq;

  HltAlgorithm::endExecute();

  return StatusCode::SUCCESS;
};


//=============================================================================
//  match2dMuon method
//=============================================================================
StatusCode MatchPatVeloRMuonHits::match2dMuon(Track* pTr2d)  {
  
  
  //loop over Muon Segments. Extrapolate it's M2 y to the z of 2d velotrack.
  // if inside the sector, 
  //calculate x as the intersection of r and y (x**2 + y**2 = r)
  // with this x  calculate x slope in 3d using the coords of PV as
  //(0,0,zpv) where zpv = r/slr  - z
  
  double trackDrDz = pTr2d->firstState().tx();
  int zone = pTr2d->specific();
  //phi at the center of sector (rad)
  double trackPhi  = (zone * M_PI/4.) - 3.*M_PI/8.;
  double trackZ = pTr2d->firstState().z(); 
  double trackR = pTr2d->firstState().x();
  // y edges of velo sector
  double y1AtVelo = trackR*sin(trackPhi-22.5*Gaudi::Units::degree); 
  double y2AtVelo = trackR*sin(trackPhi+22.5*Gaudi::Units::degree); 
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
  float x_cut;
  
  double  xmin = 99999.;
  unsigned int region = 0;
  std::vector<Track* >::const_iterator pMuon;
  for(pMuon=m_patInputTracks2->begin();
      pMuon<m_patInputTracks2->end();pMuon++){
    
    Track* muon=(*pMuon);   	
        
    std::vector<LHCbID> muonTiles=
      muon->lhcbIDs();
    MuonTileID tileM2=muonTiles[0].muonID();
    MuonTileID tileM3=muonTiles[1].muonID();
    double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;
    m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz); 
    m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);     
    //extrapolate this y hit to velo using slope from zpv
    double dydz = yM2 / (zM2 - zpv);
    yVelo = yM2 -  (dydz * (zM2 - trackZ));
    //check if this y is inside the sector
    if (yVelo < (y2AtVelo+ m_yTol) && yVelo > (y1AtVelo- m_yTol)) {
      // calculate x
      double xVelo2 = trackR*trackR - yVelo*yVelo;
      if (xVelo2 < 0) xVelo2 = -xVelo2;
      xVelo= sqrt(xVelo2);      
      //decide the sign
      //if phi is in the first or fourth quadrant, x is positive
      if(!(trackPhi/Gaudi::Units::degree < 90 || 
           trackPhi/Gaudi::Units::degree > 270))xVelo = -xVelo;
      
      // get the slopes
      
      double tx = xVelo / (trackZ - zpv);
      
      //Extrapolate to the center of magnet at 5.25 m.
      // x(z') = x(z) + (dx/dz * (z' - z))
      x_velo_mag = xVelo + (tx *(z_magnet_center - trackZ));
      
      unsigned int muon_region;
      muon_region=tileM2.region();
      x_cut=m_x_cut_track[muon_region];      
      double x_muon_magnet=xM2-(xM3-xM2)/(zM3-zM2)*(zM2-z_magnet_center) ;      
      float x_distance= x_velo_mag -x_muon_magnet;
      if(fabs(x_distance)>x_cut)continue;
      double aux;
      if (fabs(x_distance) < fabs(xmin)){
        aux = xmin;
        xmin = x_distance;
        region = muon_region;
      }
    }//if (yVelo < (y2AtVelo+2.0) && yVelo > (y1AtVelo-2.0))      
    
    //    }//if(!(itMuonTrack->clone()))
    
  }//for(itMuonTrack=m_muonTracks.begin()
  
  if (fabs(xmin) < 99999.){ 
    fillHisto(m_histoXdist, xmin,1.);
    if(fabs(xmin) <  m_x_cut_track[region]  )return StatusCode::SUCCESS; 
  }
  return StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MatchPatVeloRMuonHits::finalize() {  
  return HltAlgorithm::finalize();
}

//=============================================================================


