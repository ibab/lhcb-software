// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/State.h"
//#include "Event/RecHeader.h"

#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"
//#include "TrackInterfaces/ITrackPtKick.h"

//local
#include "PrepareMuonSeed.h"

using namespace LHCb;

// Declaration of the Tool Factory
// static const  ToolFactory<PrepareMuonSeed>          s_factory ;
// const        IToolFactory& PrepareMuonSeedFactory = s_factory ;


DECLARE_TOOL_FACTORY( PrepareMuonSeed );


// Standard Constructor
PrepareMuonSeed::PrepareMuonSeed(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
    
{
  // Declare the algorithm's properties which can be set at run time
    declareInterface<IPrepareMuonSeed>(this);

  
}

// Standard Destructor -------------------------------------
PrepareMuonSeed::~PrepareMuonSeed() {}

//-------------------------------------
StatusCode PrepareMuonSeed::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  
  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  //  m_fCalcPtKick = tool<ITrackPtKick>("TrackPtKick");
  
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

StatusCode PrepareMuonSeed::prepareSeed( const LHCb::L0MuonCandidate& muonL0Cand,
                                         LHCb::State& seedState )
{
  seedState.setState(0,0,0,0,0,0);

  std::vector<MuonTileID> mpads1 = muonL0Cand.muonTileIDs(0); 
  std::vector<MuonTileID> mpads2 = muonL0Cand.muonTileIDs(1); 
  
  MuonTileID mpad2 = *(mpads2.begin());
  
  int regionL0Cand;
  if (mpad2){
    regionL0Cand = mpad2.region();
  }
  else{
    err()<<"No valid Muon Tile in M2, quit loop"<<endmsg;
    return StatusCode::SUCCESS;
  }

    double x , y, z;
    double dxTileM1, dyTileM1, dzTileM1;
    int numberOfTiles = 0;
    double xTileM1 = 0.;
    double yTileM1 = 0.;
    double zTileM1 = 0.;
    StatusCode sc;
    
    for( std::vector<MuonTileID>::iterator it = (mpads1.begin());
         it != (mpads1.end()) ; 
         it++ ) {
      
      numberOfTiles++;
      MuonTileID mpad1 = *it;
      
      //positions in M1
      sc = m_iPosTool->calcTilePos( mpad1,
                                    x, dxTileM1,
                                    y, dyTileM1,
                                    z, dzTileM1);
      
      if (!sc) {
        err()<<"Unable to get Position for M1"<<endmsg;
        return StatusCode::SUCCESS;
      }
      
      xTileM1 += x;
      yTileM1 += y;
      zTileM1 += z;
    }
    
    xTileM1 = xTileM1 / double(numberOfTiles);
    yTileM1 = yTileM1 / double(numberOfTiles);
    zTileM1 = zTileM1 / double(numberOfTiles);
    

    double xTileM2 , yTileM2, zTileM2;
    double dxTileM2, dyTileM2, dzTileM2;
    
    sc = m_iPosTool->calcTilePos(mpad2,
                                 xTileM2, dxTileM2,
                                 yTileM2, dyTileM2,
                                 zTileM2, dzTileM2);
    if (!sc) {
      err()<<"Unable to get Position for M2"<<endmsg;
      return StatusCode::SUCCESS;
    }

    double zT3 = 9315.;//middle T3
    double dxdz   = (xTileM2 - xTileM1)/ ( zTileM2 - zTileM1);
    double dydz   = (yTileM2 - yTileM1)/ ( zTileM2 - zTileM1);
    
    double xT3 = xTileM2 - dxdz * (zTileM2 - zT3);
    double yT3 = yTileM2 - dydz * (zTileM2 - zT3);

    seedState.setState( xT3 , yT3 , zT3 , dxdz , dydz , 0 );
    //m_fCalcPtKick->calculate(&seedState);
    
    double L0p = fabs( muonL0Cand.pt()/sin(muonL0Cand.theta()) ) ;
    double L0pt = muonL0Cand.pt();//get pt with sign!
    double L0q    = (L0pt == 0) ? 0.0 : -1.*L0pt/fabs(L0pt);
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
