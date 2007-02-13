// $id:  $

// // from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/State.h"
//#include "Event/RecHeader.h"

#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"
#include "TrackInterfaces/ITrackPtKick.h"

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
  //  always() << " Initialize PrepareMuonSeed" << endmsg;

  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  m_fCalcPtKick = tool<ITrackPtKick>("TrackPtKick");
  
  m_debugInfo = false; //@ja put in job options
  
  return StatusCode::SUCCESS;
}

StatusCode PrepareMuonSeed::prepareSeed( const LHCb::L0MuonCandidate& muonL0Cand,
                                         LHCb::State& seedState )
{
  //always()<<"PrepareMuonSeed::execute!"<<endmsg;

  seedState.setState(0,0,0,0,0,0);

  // JAH 12--2-07 temporally fix
  std::vector<MuonTileID> mpads1 = muonL0Cand.muonTileIDs(0); 
  std::vector<MuonTileID> mpads2 = muonL0Cand.muonTileIDs(1); 
  
  //MuonTileID mpad1 = *(mpads1.begin());
  MuonTileID mpad2 = *(mpads2.begin());
  //MuonTileID mpad3 = muonL0Cand.pad(2);

    int regionL0Cand;
    if (mpad2){
      regionL0Cand = mpad2.region();
    }
    // else if( mpad3 ){
    //       regionL0Cand = mpad3.region();
    //     }
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
      
      if (!sc)  return StatusCode::SUCCESS;
      
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
    
    if (!sc)  return StatusCode::SUCCESS;

    double zT3 = 9315.;//middle T3
    double dxdz   = (xTileM2 - xTileM1)/ ( zTileM2 - zTileM1);
    double dydz   = (yTileM2 - yTileM1)/ ( zTileM2 - zTileM1);
    
    double xT3 = xTileM2 - dxdz * (zTileM2 - zT3);
    double yT3 = yTileM2 - dydz * (zTileM2 - zT3);

    double sigmaX = 0.;
    double sigmaY = 0.;
    if (regionL0Cand == 0) sigmaX = 8. ; sigmaY = 6.;
    if (regionL0Cand == 1) sigmaX = 15.; sigmaY = 11.;
    if (regionL0Cand == 2) sigmaX = 30.; sigmaY = 22.;
    if (regionL0Cand == 3) sigmaX = 57.; sigmaY = 44.;

    //double L0p = muonL0Cand.pt();//first estimate
    
    //@ja do i need q/p info?
    seedState.setState( xT3 , yT3 , zT3 , dxdz , dydz , 0 );
    m_fCalcPtKick->calculate(&seedState);
    
    
    double L0p = seedState.p(); //this p is more accurate
    double L0pt = muonL0Cand.pt();//get pt with sign!
    double L0q    = (L0pt == 0) ? 0.0 : -1.*L0pt/fabs(L0pt);
    double qOverP = L0q / L0p;
    
    seedState.setQOverP( qOverP );
        
    Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
    
    stateCov(0,0) = sigmaX*sigmaX;
    stateCov(1,1) = sigmaY*sigmaY; 
    stateCov(2,2) = 2.8e-3; //not used
    stateCov(3,3) = 1.5e-3;
    stateCov(4,4) = 8.41e-6;

    seedState.setCovariance(stateCov);

    return StatusCode::SUCCESS;
    
}
