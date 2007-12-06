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
#include "PrepareMuonTSeedTool.h"

using namespace LHCb;

// Declaration of the Tool Factory
// static const  ToolFactory<PrepareMuonSeed>          s_factory ;
// const        IToolFactory& PrepareMuonSeedFactory = s_factory ;


DECLARE_TOOL_FACTORY( PrepareMuonTSeedTool );


// Standard Constructor
PrepareMuonTSeedTool::PrepareMuonTSeedTool(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
    
{
  // Declare the algorithm's properties which can be set at run time
    declareInterface<IPrepareMuonTSeedTool>(this);

  
}

// Standard Destructor -------------------------------------
PrepareMuonTSeedTool::~PrepareMuonTSeedTool() {}

//-------------------------------------
StatusCode PrepareMuonTSeedTool::initialize() 
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
//Resolution for muon segments
//   *  4 regions:  sigmaX:  34 / 62 / 114 / 196 mm

//using 2* sigmaX from L0
  //sigmaX2[0]=256.; sigmaX2[1]=900.; sigmaX2[2]=3600.; sigmaX2[3]=11664.;
  sigmaY2[0]=36.; sigmaY2[1]=100.; sigmaY2[2]=400.; sigmaY2[3]=1600.;
  sigmaTx2[0]=4.e-6; sigmaTx2[1]=16.e-6; sigmaTx2[2]=49.e-6;   sigmaTx2[3]=144.e-6;
  sigmaTy2[0]=1.e-6; sigmaTy2[1]=6.5e-6; sigmaTy2[2]=12.25e-6; sigmaTy2[3]=36.e-6;
  return StatusCode::SUCCESS;
}

StatusCode PrepareMuonTSeedTool::prepareSeed( const LHCb::Track& muonTrack,
                                         LHCb::Track& seedTrack )
{
  LHCb::State seedState; 
  seedState.setState(0,0,0,0,0,0);

  std::vector<LHCbID> muonTiles=
    muonTrack.lhcbIDs();
  MuonTileID tileM2=muonTiles[0].muonID();
  MuonTileID tileM3=muonTiles[1].muonID();
  int regionL0Cand;
  if (tileM2){
    regionL0Cand = tileM2.region();
  }
  else{
    err()<<"No valid Muon Tile in M2, quit loop"<<endmsg;
    return StatusCode::SUCCESS;
  }
  seedTrack.addToLhcbIDs(muonTiles[0].muonID());
  seedTrack.addToLhcbIDs(muonTiles[1].muonID());
  seedTrack.addToLhcbIDs(muonTiles[2].muonID());
  seedTrack.addToLhcbIDs(muonTiles[3].muonID());

  StatusCode sc;
 
  double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;
  
  m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);   
  m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);
  
  
  double dxdz   = (xM2 - xM3)/ ( zM2 - zM3);
  double dydz   = (yM2 - yM3)/ ( zM2 - zM3);
  
  
    
  seedState.setState( xM2 , yM2 , zM2 , dxdz , dydz , 0 );
  m_fCalcPtKick->calculate(&seedState);
  
  
  double L0p = seedState.p(); //this p is more accurate
  //estimation ofthe charge
  float xmg=xM2-dxdz*(zM2-5300);
  float dxdzfirst=xmg/5300;
  float pestima=1280.0/(dxdz-dxdzfirst);
  
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
  seedTrack.addToStates(seedState);
  seedTrack.setType(LHCb::Track::Muon);

  return StatusCode::SUCCESS;
  
}
