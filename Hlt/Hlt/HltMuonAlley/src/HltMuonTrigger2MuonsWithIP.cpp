// $Id: HltMuonTrigger2MuonsWithIP.cpp,v 1.4 2007-11-22 10:59:09 sandra Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonTrigger2MuonsWithIP.h"

#include "HltBase/HltUtils.h"
#include "HltBase/EFunctions.h"
#include "HltBase/ESequences.h"
#include "Event/HltEnums.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMuonPreTriggerSingle
//
// 2006-08-28 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMuonTrigger2MuonsWithIP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonTrigger2MuonsWithIP::HltMuonTrigger2MuonsWithIP( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  
  declareProperty("MassWithIP", m_minMassWithIP = 500.);
  declareProperty("IP", m_minIP = 0.05);
  declareProperty("DOCA", m_maxDOCA = 0.5);
  declareProperty("OutputDimuonVerticesName"   ,
                  m_outputDimuonVerticesName);
}
//=============================================================================
// Destructor
//=============================================================================
HltMuonTrigger2MuonsWithIP::~HltMuonTrigger2MuonsWithIP() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonTrigger2MuonsWithIP::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  
  _ipFun = new Hlt::IP();
  
  m_massKey= HltNames::particleInfoID("Mass");
  m_IPKey = HltNames::particleInfoID("IP");
  m_DOCAKey=HltNames::particleInfoID("DOCA");
  m_MuonKey=HltNames::particleInfoID("MuonKey");
  
  Hlt::Info<RecVertex> massInfo(m_massKey);
  Hlt::AbsInfo<RecVertex> ipInfo(m_IPKey); 
  Hlt::Info<RecVertex> docaInfo(m_DOCAKey);
  Hlt::Info<RecVertex> L0ConfirmationInfo(m_MuonKey);
  verbose() << "mass and ip cut " << m_minMassWithIP << " " << m_minIP << endmsg;
  _massAndDOCAAndIPCut=((massInfo> m_minMassWithIP)&&
                        (docaInfo< m_maxDOCA)&& (ipInfo > m_minIP)).clone();
  
  
  _triggerMassAndIPCut=_massAndDOCAAndIPCut;
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonTrigger2MuonsWithIP::execute() {
  
  StatusCode sc = StatusCode::SUCCESS;
  debug()<<" Number of input tracks " << m_inputTracks->size() << endmsg;
  if (m_inputTracks->size() < 2) return stop(" Less than 2 muons ");
 
  RecVertices* muonpairs = new RecVertices();
  muonpairs->reserve(50);
   
  
  //first calculate IP for each muon
  
  ELoop::map_compare_value(*m_inputTracks,*m_primaryVertices,  
                           *_ipFun, Estd::abs_min(),
                           m_IPKey);
  m_overtices.clear();
  
  makeDiMuonPair( m_overtices);  
  if (m_overtices.size() <1) return stop(" No vertices");
  if(m_debug){
    debug()<<" vertices "<<m_overtices.size()<<endreq;
  }
  
  
  m_selevertices.clear();  
  m_testedvertices.clear();
  ELoop::select(m_overtices,*_triggerMassAndIPCut,m_selevertices);
  verbose() << "selevertices.size " << m_selevertices.size() << endmsg;
  if(m_selevertices.size()>0){
    int i = 0;
    for(  Hlt::VertexContainer::const_iterator idimuon=m_selevertices.begin();
          idimuon<m_selevertices.end();idimuon++){
      verbose() << " vertex number " << i << endmsg;
      RecVertex* toTest=*idimuon;
      
      //If both muons come from same muon origin (L0 or muon segment)
      //do not consider this vertex
      StatusCode sc1 = checkIfSameAncestor(toTest);
      if (sc1.isSuccess()) debug()  << "Not same ancestor "<< endmsg;
      if (!sc1.isSuccess()){
        debug()  << "Same ancestor "<< endmsg;
        continue;
      }
      StatusCode sc=checkIfL0Dimuon(toTest);
      if(sc.isSuccess()){
        debug() << "vertex accepted " << endmsg;
        muonpairs->insert(toTest);
        m_testedvertices.push_back(toTest);
        m_outputVertices->push_back(toTest);
      }
      
      
      i++;
    }//for(  Hlt::VertexContainer::const_iterator idimuon=m_selevertices
    
    
    
    if(m_testedvertices.size()>0){
      float m=(*(m_testedvertices.begin()))->info(m_massKey,0);
      float d=(*(m_testedvertices.begin()))->info(m_DOCAKey,0);
      float p=(*(m_testedvertices.begin()))->info(m_IPKey,0);
      
      fillHisto(h_mass,m,1.0);
      fillHisto(h_DOCA,d,1.0);
      fillHisto(h_IP,p,1.0); 
    }
    
  }
  
  if(m_debug){  
    debug()<<" mass and Ip cut "<<m_selevertices.size()<<" and L0 chek "
           <<m_testedvertices.size()<<endreq;
  }
  put(muonpairs, m_outputDimuonVerticesName);  
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonTrigger2MuonsWithIP::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



void HltMuonTrigger2MuonsWithIP::makeDiMuonPair(Hlt::VertexContainer& vcon)
{
  
  double massmu=105;
  
  for (Hlt::TrackContainer::const_iterator it = m_inputTracks->begin();
       it != m_inputTracks->end(); ++it) {
    for (Hlt::TrackContainer::const_iterator it2 = it+1;
         it2 != m_inputTracks->end(); ++it2) {
      int L0track=0;
      if((*it)->checkFlag(Track::L0Candidate))L0track++;
      if((*it2)->checkFlag(Track::L0Candidate))L0track++;
      if (L0track<1) continue;
      double mass= HltUtils::invariantMass( **it, **it2,massmu,massmu); 
      double doca=HltUtils::closestDistanceMod( **it, **it2);
      RecVertex* ver = new RecVertex();
      _vertexCreator(**it,**it2, *ver) ;
      ver->addInfo(m_massKey,mass); 
      ver->addInfo(m_DOCAKey,doca);    
      //add also IP of the two muons... 
      double ip1=(*it)->info(m_IPKey,0.);
      double ip2=(*it2)->info(m_IPKey,0.);
      
      if(fabs(ip1)<fabs(ip2)){
	ver->addInfo(m_IPKey,ip1);
      }else  ver->addInfo(m_IPKey,ip2);
      ver->addInfo(m_MuonKey,L0track);
      vcon.push_back( ver);
      verbose() << " make vertices with mass " << mass 
		<< " and IP " << ip1 << " " << ip2 << endreq;
      verbose()<<" P of the two tracks "<< (**it).p()
	       <<" "<<(**it2).p()<<endreq;       
      verbose()<<" Charge of the two tracks " << (**it).charge() 
	       <<" "<<(**it2).charge() << endmsg;
      verbose()<<"number of L0 cand in this vertex " << L0track << endmsg;
      verbose()<<"doca " << doca << endmsg;
      
    }      
  }
  
}
//============================
StatusCode HltMuonTrigger2MuonsWithIP::checkIfSameAncestor(RecVertex* vertexToTest)
{
  debug() << "Checking if Same Ancestor  " << endmsg;
  SmartRefVector< LHCb::Track > list_tracks=    (vertexToTest)->tracks();
  if(list_tracks.size()!=2)return StatusCode::FAILURE;
  Track* firstMuon=NULL;
  Track* secondMuon=NULL;
  
  firstMuon=list_tracks[0];
  secondMuon=list_tracks[1];
  if(firstMuon==NULL||secondMuon==NULL)return StatusCode::FAILURE;
  verbose()<<"muons in pre-trigger vertex "
	   <<firstMuon<<" charge "<< (firstMuon)->charge()
	   << " L0 " << (firstMuon)->checkFlag(Track::L0Candidate)<< " "
	   <<secondMuon<<" charge "<< (secondMuon)->charge()
	   << " L0 " << (secondMuon)->checkFlag(Track::L0Candidate)
	   <<endreq;
  for(SmartRefVector< LHCb::Track >::iterator
	ance=firstMuon->ancestors().begin();
      ance<firstMuon->ancestors().end();ance++){
    
    Track* ancestor=*ance;
    verbose()<<"ancestor of first muon "<<ancestor<<" "
             <<firstMuon<< " history " 
             << ancestor->checkHistory(Track::MuonID) << endreq;
    
    for(SmartRefVector< LHCb::Track >::iterator
	  anceSec=secondMuon->ancestors().begin();
	anceSec<secondMuon->ancestors().end();anceSec++){
      Track* ancestorSecond=*anceSec;
      verbose()<<"ancestor of second muon "<<ancestorSecond<<" "
	       <<secondMuon <<" history " 
	       << ancestorSecond->checkHistory(Track::MuonID) <<endreq;
      
      if(ancestor == ancestorSecond) return StatusCode::FAILURE;
    }//for(SmartRefVector< LHCb::Track >::iterator anceSec=secondMuon->ancestors().begin()
  }//for(SmartRefVector< LHCb::Track >::iterator ance=firstMuon->ancestors().begin()
  return StatusCode::SUCCESS;
}
//===============================
StatusCode HltMuonTrigger2MuonsWithIP::checkIfL0Dimuon(RecVertex* vertexToTest)
{
  debug() << "Checking if in L0 dimuon " << endmsg; 
  //colect both tracks that formed the vertex
  SmartRefVector< LHCb::Track > list_tracks= 	(vertexToTest)->tracks();
  if(list_tracks.size()!=2)return StatusCode::FAILURE;
  
  Track* firstMuon=NULL;  
  Track* secondMuon=NULL;  
  
  
  firstMuon=list_tracks[0];
  secondMuon=list_tracks[1];
  if(firstMuon==NULL||secondMuon==NULL)return StatusCode::FAILURE;
  verbose()<<"muons in pre-trigger vertex "
	   <<firstMuon<<" charge "<< (firstMuon)->charge()
	   << " L0 " << (firstMuon)->checkFlag(Track::L0Candidate)<< " "
	   <<secondMuon<<" charge "<< (secondMuon)->charge()
	   << " L0 " << (secondMuon)->checkFlag(Track::L0Candidate)
	   <<endreq;
  
  int i = 0;
  for(  Hlt::VertexContainer::const_iterator L0dimuon=
          m_inputVertices->begin();
        L0dimuon<m_inputVertices->end();L0dimuon++){ 
    Track* firstL0track=NULL;
    Track* secondL0track=NULL;
    
    SmartRefVector< LHCb::Track >  vertex_tracks=	(*L0dimuon)->tracks();
    verbose() << "index of vertex in input vertex " << i << endmsg;
    if(vertex_tracks.size()!=2)return StatusCode::FAILURE;
    Track* firstMuonInL0=vertex_tracks[0];
    Track* secondMuonInL0=vertex_tracks[1];
    if(m_debug){
      verbose()<<"muons in input vertex "<<firstMuonInL0<<" "<<secondMuonInL0<<endreq;
      verbose() << "first muon in PrepareDimuon charge, L0Flag " 
		<< (firstMuonInL0)->charge() << " " 
		<<(firstMuonInL0)-> checkFlag(Track::L0Candidate)
		<< endmsg;
      debug() << "second muon in PrepareDimuon charge, L0Flag " 
	      << (secondMuonInL0)->charge() << " " <<
	(secondMuonInL0)-> checkFlag(Track::L0Candidate)
	      << endmsg;
      
    }
    //now check if both tracks are ancestors of the tracks of input vertex
    for(SmartRefVector< LHCb::Track >::iterator 
          ance=firstMuon->ancestors().begin();
        ance<firstMuon->ancestors().end();ance++){
      
      Track* ancestor=*ance;
      
      verbose()<<"ancestor of first muon "<<ancestor
	       <<" "<<firstMuon<< " history " 
	       << ancestor->checkHistory(Track::MuonID) << endreq;
      
      if(ancestor==firstMuonInL0){
        firstL0track=firstMuonInL0;     
        verbose() << "ancestor first == first Muon in prepareDimuon " << endmsg;
	
        break;            
      }          
      if(ancestor==secondMuonInL0){
        firstL0track=secondMuonInL0;            
	verbose() << "ancestor first == second Muon in prepareDimuon " << endmsg;
        break;            
      }          
    }
    for(SmartRefVector< LHCb::Track >::iterator 
          ance=secondMuon->ancestors().begin();
        ance<secondMuon->ancestors().end();ance++){
      Track* ancestor=*ance;
      verbose()<<"ancestor of second muon "
	       <<ancestor<<" "<<secondMuon <<" history " 
	       << ancestor->checkHistory(Track::MuonID) <<endreq;
      
      if(ancestor==firstMuonInL0){
        secondL0track=firstMuonInL0;            
        verbose() << "ancestor second == first Muon in prepareDimuon " << endmsg;
        break;            
      }          
      if(ancestor==secondMuonInL0){
        secondL0track=secondMuonInL0;            
        verbose() << "ancestor second == second Muon in prepareDimuon " << endmsg;
        break;            
      }          
    }
    
    if(firstL0track!=NULL&&secondL0track!=NULL&&firstL0track!=secondL0track){      
      debug() << "in input vertex dimuon " << endmsg;
      return StatusCode::SUCCESS;
    }  
    i++;
  }
  return StatusCode::FAILURE; 
}






