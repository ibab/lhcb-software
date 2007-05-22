// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/State.h"
//#include "Event/RecHeader.h"

#include "Event/Track.h"
#include "MuonDet/IMuonPosTool.h"
#include "TrackInterfaces/ITrackPtKick.h"

//local
#include "PrepareMuonTSeed.h"

using namespace LHCb;

// Declaration of the Tool Factory
// static const  ToolFactory<PrepareMuonSeed>          s_factory ;
// const        IToolFactory& PrepareMuonSeedFactory = s_factory ;


DECLARE_TOOL_FACTORY( PrepareMuonTSeed );


// Standard Constructor
PrepareMuonTSeed::PrepareMuonTSeed(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
    
{
  // Declare the algorithm's properties which can be set at run time
    declareInterface<IPrepareMuonTSeed>(this);

  
}

// Standard Destructor -------------------------------------
PrepareMuonTSeed::~PrepareMuonTSeed() {}

//-------------------------------------
StatusCode PrepareMuonTSeed::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  //  always() << " Initialize PrepareMuonSeed" << endmsg;

  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  m_fCalcPtKick = tool<ITrackPtKick>("TrackPtKick");
  
  m_debugInfo = false; //@ja put in job options
  
  /*
   *  Resolution of the L0Cand at T3, tune JA 2007-03-22
   *  4 regions:  sigmaX:  8 / 15 / 29 / 54 mm
   *              sigmaY:  6 / 10 / 20 / 40 mm
   *              sigmaTx: 2 /  4 /  7 / 12 mrad
   *              sigmaTy: 1 / 2.5/ 3.5/  6 mrad
   */
  sigmaX2[0]=64.; sigmaX2[1]=225.; sigmaX2[2]=841.; sigmaX2[3]=2916.;
  sigmaY2[0]=36.; sigmaY2[1]=100.; sigmaY2[2]=400.; sigmaY2[3]=1600.;
  sigmaTx2[0]=4.e-6; sigmaTx2[1]=16.e-6; sigmaTx2[2]=49.e-6;   sigmaTx2[3]=144.e-6;
  sigmaTy2[0]=1.e-6; sigmaTy2[1]=6.5e-6; sigmaTy2[2]=12.25e-6; sigmaTy2[3]=36.e-6;
  return StatusCode::SUCCESS;
}

StatusCode PrepareMuonTSeed::prepareSeed( const LHCb::Track& muonTrack,
                                         LHCb::State& seedState )
{
  //always()<<"PrepareMuonSeed::execute!"<<endmsg;

  seedState.setState(0,0,0,0,0,0);

  std::vector<LHCbID> muonTiles=
    muonTrack.lhcbIDs();
  MuonTileID tileM2=muonTiles[0].muonID();
  MuonTileID tileM3=muonTiles[1].muonID();
  int regionL0Cand;
  if (tileM2){
    regionL0Cand = tileM2.region();
  }
  // else if( mpad3 ){
  //       regionL0Cand = mpad3.region();
  //     }
  else{
    err()<<"No valid Muon Tile in M2, quit loop"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  //  double x , y, z;
  //double dxTileM1, dyTileM1, dzTileM1;
  //int numberOfTiles = 0;
  //double xTileM1 = 0.;
  //double yTileM1 = 0.;
  //double zTileM1 = 0.;
  StatusCode sc;
 
  double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;
  
  m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);   
  m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);
  
  
  double zT3 = 9315.;//middle T3

  double dxdz   = (xM2 - xM3)/ ( zM2 - zM3);
  double dydz   = (yM2 - yM3)/ ( zM2 - zM3);
  
  double xT3 = xM2 - dxdz * (zM2 - zT3);
  double yT3 = yM2 - dydz * (zM2 - zT3);
  
//   double sigmaX = 0.;
//   double sigmaY = 0.;
//   if (regionL0Cand == 0) sigmaX = 8.*2 ; sigmaY = 6.*2;
//   if (regionL0Cand == 1) sigmaX = 15.*2; sigmaY = 11.*2;
//   if (regionL0Cand == 2) sigmaX = 30.*2; sigmaY = 22.*2;
//   if (regionL0Cand == 3) sigmaX = 57.*2; sigmaY = 44.*2;
  
  //double L0p = muonL0Cand.pt();//first estimate
    
    //@ja do i need q/p info?
  seedState.setState( xT3 , yT3 , zT3 , dxdz , dydz , 0 );
  m_fCalcPtKick->calculate(&seedState);
  
  
  double L0p = seedState.p(); //this p is more accurate
  //estimation ofthe charge
  float xmg=xM2-dxdz*(zM2-5300);
  float dxdzfirst=xmg/5300;
  float pestima=1280.0/(dxdz-dxdzfirst);
  
  //double L0pt = muonL0Cand.pt();//get pt with sign!
  double L0q    = (pestima == 0) ? 0.0 : -1.*pestima/fabs(pestima);
  double qOverP = L0q / L0p;
  
  seedState.setQOverP( qOverP );
  
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  
  stateCov(0,0) = sigmaX2[regionL0Cand];
  stateCov(1,1) = sigmaY2[regionL0Cand];
  stateCov(2,2) = sigmaTx2[regionL0Cand];
  stateCov(3,3) = sigmaTy2[regionL0Cand];
  stateCov(4,4) = 8.41e-6;
  
  seedState.setCovariance(stateCov);
  
  return StatusCode::SUCCESS;
  
}
