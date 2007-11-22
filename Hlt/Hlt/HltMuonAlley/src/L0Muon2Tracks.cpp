// $Id: L0Muon2Tracks.cpp,v 1.4 2007-11-22 10:59:09 sandra Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "L0Muon2Tracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Muon2Tracks
//
// 2007-01-03 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Muon2Tracks );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Muon2Tracks::L0Muon2Tracks( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
 declareProperty( "L0MuonType",   m_L0MuonType      );
 declareProperty( "L0MuonThresold",   m_L0MuonThreshold      );
 declareProperty("OutputL0MuonTracksName"   ,
                  m_outputL0MuonTracksName);
 declareProperty("OutputL0MuonVerticesName"   ,
                  m_outputL0MuonVerticesName);
 declareProperty("RejectClone"   ,
                  m_rejClone=true);



}
//=============================================================================
// Destructor
//=============================================================================
L0Muon2Tracks::~L0Muon2Tracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Muon2Tracks::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg; 
  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );

  if( m_L0MuonType!="single"&& m_L0MuonType!="dimuon"){
    error()<<"unknown type of L0Muon trigger type "<<endreq;
    
    return StatusCode::FAILURE;
  }
  m_myState = new State();
  m_vertexState = new State();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Muon2Tracks::execute() {

  debug() << "==> Execute" << endmsg;
  //HltAlgorithm::beginExecute();  
  setFilterPassed(false);
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default);
  Tracks* muontracks = new Tracks();
  muontracks->reserve(50);

  Tracks* muonSelected = new Tracks();
   muonSelected->reserve(50);

  RecVertices* muonpairs = new RecVertices();
  muonpairs->reserve(50);
  //first copy all muons to a Track container
  //check if the container is empty otherwise another alleys have already fill it
  
//  if( m_outputL0MuonTracks->size()==0){
    int i = 0;
    for (std::vector<L0MuonCandidate*>::const_iterator itL0Mu=m_inputL0Muon->begin();
              itL0Mu!=m_inputL0Muon->end();itL0Mu++){
      i++;
      debug() << "L0 Nr. "<<  i << endmsg;      
      L0MuonCandidate* itMuon = *itL0Mu;
      Track* outputTrack=new Track();

      if(m_rejClone){
        bool isClone=checkClone(itMuon, muontracks);
        debug()<<itMuon->pt()<<" is clone "<<isClone<<endreq;
       
        if(!isClone) {
          fillMuon(itMuon, *outputTrack);
          debug()<<" a new L0 muon has been added "<<endreq;
          muontracks->insert(outputTrack);
        }
      }      
      else {
        fillMuon(itMuon, *outputTrack);
        debug()<<" a new L0 muon has been added "<<endreq;
        muontracks->insert(outputTrack);

      }
    }
//  }else debug()<<" the container is already filled"<<endreq;
  
  

  if(m_L0MuonType=="single"){
     fillSingle(muontracks, muonSelected);
     debug() << "muontracks->size()  "<<muontracks->size()<<endreq;
     put(muonSelected,m_outputL0MuonTracksName);
  }
  if(m_L0MuonType=="dimuon"){
     fillDiMuon(muontracks,muonpairs);
     put(muonpairs, m_outputL0MuonVerticesName);
     put(muontracks, m_outputL0MuonTracksName);
  }
  debug()<<" number of stored tracks "<<muontracks->size()<<endreq;




  if(m_L0MuonType=="single" && muonSelected->size()>0)
	 setFilterPassed(true);
  if(m_L0MuonType=="dimuon" && muonpairs->size()>0)
         setFilterPassed(true);
 // delete(muontracks);
  //HltAlgorithm::endExecute();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Muon2Tracks::finalize() {

  debug() << "==> Finalize" << endmsg;
  delete m_myState;
  delete m_vertexState; 
  return HltAlgorithm::finalize();


}

//=============================================================================
StatusCode L0Muon2Tracks::fillSingle(Tracks* muontracks, Tracks* muonSelected) {
  // Loop over L0 muons inserted in  tracks container
  for(std::vector<Track*>::const_iterator pItr=muontracks->begin();
       pItr<muontracks->end();pItr++){
    double pt=(*pItr)->firstState().pt();
    debug()<< " selecting the L0 single "<<pt<<endreq;
    
    if(fabs(pt)<0.1)continue;
    if(fabs(pt)> m_L0MuonThreshold){
      muonSelected->insert(*pItr);
      m_outputTracks->push_back(*pItr);
      debug() << " single " << *pItr << endmsg;
       debug()<< " selected the L0 single "<<pt<<endreq;
    }
  }  
  return StatusCode::SUCCESS;
}

StatusCode L0Muon2Tracks::fillDiMuon(Tracks* muontracks,RecVertices* muonpairs) {
  for(std::vector<Track*>::const_iterator pItrOne=
              muontracks->begin();
              pItrOne<muontracks->end();pItrOne++){
    double ptOne=(*pItrOne)->firstState().pt();
    debug()<<" new  L0 "<<ptOne<<endreq;
    
    if(fabs(ptOne)<0.1)continue;
    for(std::vector<Track*>::const_iterator pItrTwo=pItrOne+1;
                pItrTwo<muontracks->end();pItrTwo++){
      double ptTwo     =(*pItrTwo)->firstState().pt();
      debug()<<" second  L0 "<<ptTwo<<endreq;
      
      if(fabs(ptTwo)<0.1)continue;
      if(fabs(ptOne)+fabs(ptTwo)>m_L0MuonThreshold){
        fillVertex(*pItrOne,*pItrTwo,muonpairs);      
        debug() << "FillDimuon " << *pItrOne << " " << *pItrTwo << endmsg;
      }        
    }
  }

  
  return StatusCode::SUCCESS;
}



StatusCode L0Muon2Tracks::fillMuon( LHCb::L0MuonCandidate* muon,
                                    LHCb::Track& outTr )
{
  double et     = muon->pt();
  std::vector<MuonTileID> list_of_tileM1= muon->muonTileIDs(0);
  MuonTileID tileM1=*(list_of_tileM1.begin());

  std::vector<MuonTileID> list_of_tileM2= muon->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tileM2.begin());

  std::vector<MuonTileID> list_of_tileM3= muon->muonTileIDs(2);
  MuonTileID tileM3=*(list_of_tileM3.begin());

  double theta  = muon->theta();  
  double phi  = muon->phi();
  double xM1,yM1,zM1,xM2,yM2,zM2;
  double dx,dy,dz;
  
  m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);
  if(tileM1.isValid()){
    m_iPosTool->calcTilePos(tileM1,xM1,dx,yM1,dy,zM1,dz);
  }else{
    zM1=12100;
    yM1=yM2*zM1/zM2;
    //slope after
    
    double xM3,yM3,zM3;
    m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);
        xM1=xM2+(xM2-xM3)/(zM2-zM3)*(zM1-zM2);
        
  }

  debug()<<" tileM1 "<<tileM1<<" tileM2 "<<tileM2<<" tileM3 "<<tileM3<<endreq;
  
  double dxdz=(xM2-xM1)/(zM2-zM1);
  
  double dydz = yM1 / (zM1);
  double qp=1/et*sin(theta);
  //      if(fabs(theta)>0.000001)qp=qp*theta;
  m_vertexState->setState( 0,
                           0,
                           0,
                           tan(theta)*cos(phi),
                           tan(theta)*sin(phi),
                           qp );
  if(tileM1.isValid()) {
    m_myState->setState( xM1,
                         yM1,
                         zM1,
                         dxdz,
                         dydz,
                         qp );
    
  }else{      
    m_myState->setState( xM2,
                         yM2,
                         zM2,
                         dxdz,
                         dydz,
                         qp );
  }
  
  
  
  
  debug()<<" adding a track "<<1/qp<<endreq;
  
  
  outTr.addToStates( *m_vertexState);
  outTr.addToStates( *m_myState); 
  outTr.firstState().setQOverP(m_myState->qOverP());
  outTr.setHistory( Track::MuonID );
  if(tileM1.isValid()) outTr.addToLhcbIDs(tileM1);
  outTr.addToLhcbIDs(tileM2);
  outTr.addToLhcbIDs(tileM3);
  debug()<<" the Pt of the muon is  "<<m_vertexState->pt()<<endreq;
  
  return StatusCode::SUCCESS;
  
};


StatusCode L0Muon2Tracks::fillVertex( LHCb::Track* firstmuon, 
                                      LHCb::Track* secondmuon,
                                      RecVertices* muonpairs){

 RecVertex* ver = new RecVertex();
 ver->addToTracks (firstmuon);
 ver->addToTracks (secondmuon);
    int n=firstmuon->nLHCbIDs();
    verbose() << "n  hits first " << n << endmsg;
    std::vector< LHCb::LHCbID >    lh=firstmuon->lhcbIDs();
    for(std::vector<LHCb::LHCbID>::iterator ii=lh.begin();ii<lh.end();ii++){
        if(ii->isMuon()){
          MuonTileID tileM = ii->muonID();
           verbose()<<  tileM.station()<< " " <<tileM <<endmsg;
        }//if(ii->isMuon())
    }//for(ii=lh.begin();ii<lh.end();ii++)
    n=secondmuon->nLHCbIDs();
    verbose() << "n  hits second " << n << endmsg;
    lh=secondmuon->lhcbIDs();
    for(std::vector<LHCb::LHCbID>::iterator ii=lh.begin();ii<lh.end();ii++){
        if(ii->isMuon()){
          MuonTileID tileM = ii->muonID();
           verbose()<<  tileM.station()<< " " <<tileM <<endmsg;
        }//if(ii->isMuon())
    }//for(ii=lh.begin();ii<lh.end();ii++)

 muonpairs->insert(ver); 
 m_outputVertices->push_back(ver); 
 return StatusCode::SUCCESS;
 
};



bool L0Muon2Tracks::checkClone(L0MuonCandidate* muon, Tracks* muontracks)
{
  std::vector<MuonTileID> list_of_tileM1= muon->muonTileIDs(0);
  MuonTileID tileM1=*(list_of_tileM1.begin());

  std::vector<MuonTileID> list_of_tileM2= muon->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tileM2.begin());
  for(std::vector<Track*>::const_iterator pItr=muontracks->begin();
                                     pItr<muontracks->end();pItr++){
    std::vector< LHCb::LHCbID > list_lhcb= 	(*pItr)->lhcbIDs();
    MuonTileID oldTileM1;
    MuonTileID oldTileM2;
    
    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==0){
          oldTileM1=iM1->muonID();
          break;          
        }        
      }      
    }  
    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==1){
          oldTileM2=iM1->muonID();
          break;          
        }        
      }      
    }
    if(oldTileM1.isValid()&&oldTileM1==tileM1&&oldTileM2.isValid()&&oldTileM2==tileM2)return true;
    

    
  }
  return false;
  
}

bool L0Muon2Tracks::isIn(Track* track)
{
  Hlt::TrackContainer::const_iterator it;
  for(it=m_outputTracks->begin();it<m_outputTracks->end();it++){
    if(track==*it)return true;
    
  }
  return false;
  
}
