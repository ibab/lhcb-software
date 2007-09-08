// $Id: L0MuonConfWithT.cpp,v 1.5 2007-09-08 18:34:11 sandra Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "L0MuonConfWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonConfWithT
//
// 2007-02-14 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonConfWithT );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonConfWithT::L0MuonConfWithT( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
,m_prepareMuonSeed(0)
,m_TsaConfirmTool(0)
{
    declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName = "Hlt/Tracks/ConfirmedL0Muon");

  declareProperty("debugInfo", m_debugInfo = false);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonConfWithT::~L0MuonConfWithT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonConfWithT::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_prepareMuonSeed = tool<IPrepareMuonSeed>("PrepareMuonSeed");

  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool", this );
  m_outputMuonTracks =
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
 
  
 

  m_myState = new State();


  sigmaX2[0]=64.; sigmaX2[1]=225.; sigmaX2[2]=841.; sigmaX2[3]=2916.;
  sigmaY2[0]=36.; sigmaY2[1]=100.; sigmaY2[2]=400.; sigmaY2[3]=1600.;
  sigmaTx2[0]=4.e-6; sigmaTx2[1]=16.e-6; sigmaTx2[2]=49.e-6;   sigmaTx2[3]=144.e-6;
  sigmaTy2[0]=1.e-6; sigmaTy2[1]=6.5e-6; sigmaTy2[2]=12.25e-6; sigmaTy2[3]=36.e-6;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonConfWithT::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(true);
//  HltAlgorithm::beginExecute();
  m_inputL0Muons = get<L0MuonCandidates>(L0MuonCandidateLocation::Default);  
  

  if( m_debugInfo ) info() << " Nr of L0 Muon candidates: " 
                           << m_inputTracks->size() << endmsg; 
  if( 0 == m_inputL0Muons->size() ){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

   std::vector<LHCb::Track*>* foundTracksTmp = new std::vector<LHCb::Track*>;
  foundTracksTmp->reserve(50);

  

  int muonCounter=0;  
//   L0MuonCandidates::const_iterator itL0Mu;
//  std::vector<LHCb::Track*>::iterator iterTrack;
//   for (itL0Mu = m_inputL0Muons->begin();
//        itL0Mu!=m_inputL0Muons->end();
//        itL0Mu++){
//     std::vector<MuonTileID> list_of_tile= (*itL0Mu)->muonTileIDs(1);
//     MuonTileID tileM2=*(list_of_tile.begin());
//     std::vector<MuonTileID> list_of_tileM3= (*itL0Mu)->muonTileIDs(2);
//     MuonTileID tileM3=*(list_of_tileM3.begin());


//     muonCounter++;

//     if(m_debugInfo) info()<<"Processing muon number "<<muonCounter<<endmsg;
    
//     LHCb::State myState;
//     StatusCode sctmp = m_prepareMuonSeed->prepareSeed( *(*itL0Mu) , myState );
//     if( sctmp.isFailure() ){
//       err()<<"Failed to prepare the seed"<<endmsg;
//     }
 std::vector<LHCb::Track*>::iterator iterTrack;
  Hlt::TrackContainer::const_iterator it;
  for(it=m_inputTracks->begin();it<m_inputTracks->end();it++){
    LHCb::State L0State =(*it)->closestState (12500);
    muonCounter++;
    
    if(m_debugInfo) info()<<"Processing muon number "<<muonCounter<<endmsg;
    double xTileM1 , yTileM1, zTileM1;
    xTileM1= L0State.x();
    yTileM1= L0State.y();
    zTileM1= L0State.z();
    debug()<<"M1 ile "<<xTileM1<<" "<<yTileM1<<" "<<zTileM1<<endreq;
    
    double zT3 = 9315.;//middle T3
    double dxdz   = L0State.tx();
    double dydz   = L0State.ty();
      
    double xT3 = xTileM1 - dxdz * (zTileM1 - zT3);
    double yT3 = yTileM1 - dydz * (zTileM1 - zT3);
    LHCb::State seedState;
    
    seedState.setState( xT3 , yT3 , zT3 , dxdz , dydz , 0 );


    seedState.setQOverP( L0State.qOverP() );
 debug()<<" before seed definition "<<seedState.qOverP()<<endreq;
    Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
    int regionL0Cand=-1;
    std::vector< LHCb::LHCbID > list_lhcb=(*it)->lhcbIDs();
      
    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==1){
          regionL0Cand=iM1->muonID().region();
          
          
        }
        
      }
    }
    stateCov(0,0) = sigmaX2[regionL0Cand];
    stateCov(1,1) = sigmaY2[regionL0Cand];
    stateCov(2,2) = sigmaTx2[regionL0Cand];
    stateCov(3,3) = sigmaTy2[regionL0Cand];
    stateCov(4,4) = 8.41e-6;
    
    seedState.setCovariance(stateCov);
    debug()<<" seed state "<<seedState.x()<<" "<<seedState.y()<<" "<<
      seedState.z()<<" "<<seedState.tx()<<" "<<seedState.ty()<<" "<<endreq;
    
    debug()<<" after seed definition "<<seedState.qOverP()<<" "<<regionL0Cand<<endreq;
    

    //play with seeding tool
    m_TsaConfirmTool->tracks( seedState , *foundTracksTmp );
    for( iterTrack = foundTracksTmp->begin();
       iterTrack != foundTracksTmp->end() ;
       iterTrack++ ) {
      LHCb::Track* fitTrack =  (*iterTrack)->clone();
      fitTrack->addToAncestors (*it);
      //copy the muon tiles from L0muon track
      std::vector< LHCb::LHCbID > list_lhcb=(*it)->lhcbIDs();
      
      for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
        if(iM1->isMuon()){
          fitTrack->addToLhcbIDs(iM1->muonID());
          debug()<<" adding tile "<<iM1->muonID()<<iM1->muonID().station()<<endreq;
          
        }
      }
 
      Track* outTr = m_outputMuonTracks->newEntry();
      outTr->copy(*fitTrack);
      outTr->setFlag(Track::L0Candidate,true);
//      outTr->addToAncestors(*it);
//info()<<" size "<<outTr->ancestors().size()<<" "
//	<<(outTr->ancestors())[0]<<" "<<*it<<" "<<outTr<<endreq;
      m_outputTracks->push_back(outTr);

     // info()<<" hlt container"<<endreq;
      setFilterPassed(true);
      delete  (*iterTrack);
      delete(fitTrack);
   
    }

    if(m_debugInfo) info()<<"Tool found : "<<foundTracksTmp->size()
                           <<" tracks for the seed number "<<muonCounter<<" !"<<endmsg;
     foundTracksTmp->clear();

    
  }//end loop muons


  delete foundTracksTmp;

// HltAlgorithm::endExecute();	
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonConfWithT::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
