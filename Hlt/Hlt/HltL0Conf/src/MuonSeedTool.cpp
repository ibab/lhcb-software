// $Id: MuonSeedTool.cpp,v 1.19 2010-04-30 12:00:30 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// from Event
#include "Event/State.h"
#include "Event/Track.h"

#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonFastPosTool.h"

// local
#include "MuonSeedTool.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonSeedTool
//
// 2007-07-03 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonSeedTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonSeedTool::MuonSeedTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_fieldOff{false}
  , m_iPosTool{nullptr}
  , m_magFieldSvc{nullptr}
  , m_momentumTool{nullptr}
  , m_DataStore{nullptr}
  , m_recalculateP{false}
{
  declareInterface<IMuonSeedTool>(this);

  declareProperty("debugMode",m_debugMode = false );
  declareProperty("useM1",m_useM1 = true );
  declareProperty("momentumToolName",m_momentumToolName="FastMomentumEstimate");

  //resolution for track hypothesis taken from muon stations M1 and M2 
  declareProperty("sigmaX2",  m_sigmaX2  = {64.,225.,841.,2916.} );
  declareProperty("sigmaY2",  m_sigmaY2  = {36.,100.,400.,1600.} );
  declareProperty("sigmaTx2", m_sigmaTx2 = {9.e-6,16.e-6,49.e-6,169.e-6} );
  declareProperty("sigmaTy2", m_sigmaTy2 = {16.e-6,36.e-6,64e-6,100.e-6} );

  //resolution for track hypothesis taken from muon stations M2 and M3 
  declareProperty("sigmaX2NoM1",  m_sigmaX2NoM1  =  {484.,1849.,7396.,25600.} );
  declareProperty("sigmaY2NoM1",  m_sigmaY2NoM1  =  {81.,529.,1936.,2209.} );
  declareProperty("sigmaTx2NoM1", m_sigmaTx2NoM1 =  {16.e-6,64.e-6,225e-6,529.e-6} );
  declareProperty("sigmaTy2NoM1", m_sigmaTy2NoM1 =  {16.e-6,49.e-6,64e-6,100.e-6} );

  declareProperty("recalculateP", m_recalculateP = true );
}
//=============================================================================
// Destructor
//=============================================================================
MuonSeedTool::~MuonSeedTool() {} 

//=============================================================================

//-------------------------------------
StatusCode MuonSeedTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  
  m_iPosTool=tool<IMuonFastPosTool>( "MuonFastPosTool" );
  m_momentumTool = tool<ITrackMomentumEstimate>(m_momentumToolName);

  //tool for debug information
  m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");

  //check vectors set by job options
  if( 4!=m_sigmaX2.size() || 4!=m_sigmaY2.size() 
      || 4!=m_sigmaTx2.size() || 4!=m_sigmaTy2.size() 
      || 4!=m_sigmaX2NoM1.size() || 4!=m_sigmaY2NoM1.size() 
      || 4!=m_sigmaTx2NoM1.size() || 4!=m_sigmaTy2NoM1.size() ){
    error()<<"Size of search winow vectors is not valid, check you options!"<<endmsg;
    return StatusCode::FAILURE;
  }

  // subscribe to the updatemanagersvc with a dependency on the magnetic field svc
  IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  m_magFieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_updMgrSvc->registerCondition( this,m_magFieldSvc,&MuonSeedTool::updateField) ;
  
  // initialize with the current conditions
  return m_updMgrSvc->update(this) ;
}

StatusCode MuonSeedTool::updateField()
{
  if(msgLevel(MSG::DEBUG)) 
    debug()<<"magnetic field is: "<<m_magFieldSvc->signedRelativeCurrent()<<endmsg;
  
  m_fieldOff=false;
  if( fabs(m_magFieldSvc->signedRelativeCurrent()) < 0.1 ) {
    m_fieldOff=true;
    if(msgLevel(MSG::INFO)) {
      info()<<" magnetic field is below 10% of nominal field! \n Use options for no field!"<<endmsg;
      info()<<"Tool configured for no B field!"<<endmsg;
      info()<<"Position and slope is set correctly, covariance and momemtum _not_!"<<endmsg;
    }
  }
  return StatusCode::SUCCESS ;
}


StatusCode MuonSeedTool::makeTrack( const LHCb::Track& inputTrack,
                                    LHCb::Track& outputTrack )
{
  outputTrack.setType( LHCb::Track::Muon );
  outputTrack.addToAncestors( inputTrack );
  outputTrack.setFlag( Track::L0Candidate,false ); 

  //prepare state here, with defaults to be updated
  //attach to track at the end
  LHCb::State seedState;
  const auto& firstState = inputTrack.firstState();
  seedState.setState( firstState.x() , 
		              firstState.y() ,
		              firstState.z() ,
		              firstState.tx() , 
		              firstState.ty() ,
		              firstState.qOverP() );
  
  //copy lhcbids from inputTrack to outputTrack
  MuonTileID tileM2 ;
  MuonTileID tileM3 ;
  for(auto& id : inputTrack.lhcbIDs() ) {
    if(id.isMuon()){
      outputTrack.addToLhcbIDs( LHCbID{ id.muonID() } );
      if( id.muonID().station()==1) tileM2 = id.muonID();
      if( id.muonID().station()==2) tileM3 = id.muonID();
    }
  }
  if(!(tileM2.isValid()&&tileM3.isValid())){   
    info()<<"No hit in M2 and/or M3 station return values of input track"<<endmsg;
    outputTrack.addToStates( seedState );
    return StatusCode::SUCCESS;
  }
  
  int muonRegion;
  if (tileM2){
    muonRegion = tileM2.region();
    if(msgLevel(MSG::DEBUG)) debug()<<"muonRegion "<< muonRegion<<endmsg;
  }
  else{
    if(msgLevel(MSG::DEBUG)) debug()<<"No valid Muon Tile in M2, return input state"<<endmsg;
    outputTrack.addToStates( seedState );
    return StatusCode::SUCCESS;
  }
  
  double xM2 ,yM2 ,zM2;
  double xM3 ,yM3 ,zM3;
  double dx,dy,dz;
  
  StatusCode sc = m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);   
  if(!sc){
    if(msgLevel(MSG::DEBUG)) debug()<<"Muon tile position at M2 not available, return input state"<<endmsg;
    outputTrack.addToStates( seedState );
    return StatusCode::SUCCESS; 
  }
  
  sc = m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);
  if(!sc){
    if(msgLevel(MSG::DEBUG)) debug()<<"Muon tile position at M3 not available, return input state"<<endmsg;
    outputTrack.addToStates( seedState );
    return StatusCode::SUCCESS; 
  }

  double dxdz   = !m_fieldOff ? ( (xM2 - xM3)/ ( zM2 - zM3) ) : (xM2 / zM2) ;
  double dydz   = yM2 / zM2;
 

  seedState.setState( xM2 , yM2 , zM2 , dxdz , dydz , 0 );
  double qOverP = inputTrack.firstState().qOverP();
  double sigmaQOverP = 0;

  if( m_recalculateP) {
    //NOTE: B=0 --> p will be 1 GeV with huge errors
    sc = m_momentumTool->calculate(&seedState ,qOverP, sigmaQOverP , false );
    if( sc.isFailure() ) {
    Warning( "MomentumEstimate tool  failed, but still adding State" );
    }
  }
  
  seedState.setQOverP( qOverP );
  seedState.setErrQOverP2( sigmaQOverP*sigmaQOverP );

  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_sigmaX2NoM1[muonRegion];
  stateCov(1,1) = m_sigmaY2NoM1[muonRegion];
  stateCov(2,2) = m_sigmaTx2NoM1[muonRegion];
  stateCov(3,3) = m_sigmaTy2NoM1[muonRegion];
  stateCov(4,4) = 8.41e-6;
  
  seedState.setCovariance(stateCov);
  outputTrack.addInfo(777,muonRegion);
  

  //add to states
  outputTrack.addToStates( seedState );
  return StatusCode::SUCCESS;
}


StatusCode MuonSeedTool::makeTrack( const LHCb::L0MuonCandidate& muonL0Cand,
                                    LHCb::Track& outputTrack )
{
  //prepare state here, with defaults to be updated
  //attach to track at the end
  LHCb::State seedState;
  seedState.setState( 0,0,0,0,0,0);


  std::vector<MuonTileID> mpads1 = muonL0Cand.muonTileIDs(0); 
  outputTrack.addToLhcbIDs( LHCbID{ mpads1.front() } ); 

  std::vector<MuonTileID> mpads2 = muonL0Cand.muonTileIDs(1); 
  outputTrack.addToLhcbIDs( LHCbID{ mpads2.front() } ); 
  MuonTileID mpad2 = mpads2.front();

  if (!mpad2) {
    if(msgLevel(MSG::DEBUG)) debug ()<<"No valid Muon Tile in M2, quit loop"<<endmsg;
    outputTrack.addToStates(seedState);
    return StatusCode::SUCCESS;
  }

  std::vector<MuonTileID> mpads3 = muonL0Cand.muonTileIDs(2); 
  outputTrack.addToLhcbIDs( LHCbID{ mpads3.front() } ); 

  outputTrack.setType(LHCb::Track::Muon);
  outputTrack.setFlag(Track::L0Candidate,true);

 
  int regionL0Cand = mpad2.region();

  double x , y, z;
  double dx, dy, dz;
  int numberOfTiles = 0;
  double xM1 = 0.;
  double yM1 = 0.;
  double zM1 = 0.;
  bool hasM1 = false;
  
  StatusCode sc;
  for( MuonTileID mpad1 : mpads1 ) { 
    ++numberOfTiles;
    if( mpad1.isValid() 
        && 0 == mpad1.station() 
        && m_useM1                 ){
      
      //positions in M1
      sc = m_iPosTool->calcTilePos( mpad1,x, dx,y, dy,z, dz );
      if (!sc) {
        if(msgLevel(MSG::DEBUG)) debug ()<<"Unable to get Position for M1"<<endmsg;
	    outputTrack.addToStates(seedState);
        continue;
      }
      
      hasM1 = true;
      xM1 += x;
      yM1 += y;
      zM1 += z;
    }//is valid tile
  }//loop M1 tiles
  
  if( hasM1 ) {
    double N = 1.0/double(numberOfTiles);
    xM1 *= N;
    yM1 *= N;
    zM1 *= N;
  }
  
  double xM2 , yM2, zM2;
  sc = m_iPosTool->calcTilePos(mpad2, xM2, dx, yM2, dy, zM2, dz);
  if (!sc) {
    if(msgLevel(MSG::DEBUG)) debug ()<<"Unable to get Position for M2"<<endmsg;
    outputTrack.addToStates(seedState);
    return StatusCode::SUCCESS;
  }
  
  double xM3 , yM3, zM3;
  sc = m_iPosTool->calcTilePos(mpads3.front(), xM3, dx, yM3, dy, zM3, dz );
  if (!sc) {
    if(msgLevel(MSG::DEBUG)) debug ()<<"Unable to get Position for M3"<<endmsg;
    outputTrack.addToStates(seedState);
    return StatusCode::SUCCESS;
  }

  /*
   *  set the state 
   */
  double dxdz, dydz;
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  if( hasM1 ){
    //valid M1 info available -> more accurate seed state
    dxdz   = (xM2 - xM1)/ ( zM2 - zM1);
    //dydz   = (yM2 - yM1)/ ( zM2 - zM1);
    dydz = yM2/  zM2;

    stateCov(0,0) = m_sigmaX2[regionL0Cand];
    stateCov(1,1) = m_sigmaY2[regionL0Cand];
    stateCov(2,2) = m_sigmaTx2[regionL0Cand];
    stateCov(3,3) = m_sigmaTy2[regionL0Cand];
    stateCov(4,4) = 8.41e-6;
  } else{
    //no valid M1 info available -> bigger uncertainties
    dxdz   = (xM3 - xM2)/ ( zM3 - zM2);
    dydz = yM2/  zM2;

    stateCov(0,0) = m_sigmaX2NoM1[regionL0Cand];
    stateCov(1,1) = m_sigmaY2NoM1[regionL0Cand];
    stateCov(2,2) = m_sigmaTx2NoM1[regionL0Cand];
    stateCov(3,3) = m_sigmaTy2NoM1[regionL0Cand];
    stateCov(4,4) = 8.41e-6;
  }

  if (m_fieldOff) dxdz = xM2 / zM2;

  seedState.setState( xM2 , yM2 , zM2 , dxdz , dydz , 0 );

  double L0p = fabs( muonL0Cand.pt()/sin(muonL0Cand.theta()) ) ;
  double L0pt =      muonL0Cand.pt();//get pt with sign!
  double L0q    = (L0pt == 0) ? 0.0 : -1.*L0pt/fabs(L0pt);
  double qOverP = L0q / L0p;
    
  seedState.setQOverP( qOverP );
  seedState.setCovariance(stateCov);

  if( m_debugMode ){
    m_DataStore->region.push_back( regionL0Cand );
  }
  
  outputTrack.addInfo(777,regionL0Cand);
  
     
  // add to states
  outputTrack.addToStates(seedState);
  return StatusCode::SUCCESS;
}
