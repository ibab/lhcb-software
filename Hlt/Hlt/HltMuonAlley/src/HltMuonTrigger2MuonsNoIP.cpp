// $Id: HltMuonTrigger2MuonsNoIP.cpp,v 1.2 2007-07-26 07:19:12 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonTrigger2MuonsNoIP.h"

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
DECLARE_ALGORITHM_FACTORY( HltMuonTrigger2MuonsNoIP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonTrigger2MuonsNoIP::HltMuonTrigger2MuonsNoIP( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  //  declareProperty("PtMin", m_ptMin = 0.);
  // declareProperty("Selection2Name", m_selection2SummaryName="");
 declareProperty("MassWithIP", m_minMassWithIP = 500.);
 declareProperty("MassNoIP", m_minMassNoIP = 2500.);
 //declareProperty("IP", m_minIP = 0.075);
 declareProperty("L0Confirm", m_L0confirm=1);
 declareProperty("DOCA", m_maxDOCA = 0.5);
 //declareCondition("MassWithIP", m_minMassWithIP);
 //declareCondition("MassNoIP", m_minMassNoIP);
 //declareCondition("IP", m_minIP); 
 //declareCondition("DOCA", m_maxDOCA );
 //declareCondition("L0Confirm", m_L0confirm=1);
 declareProperty("OutputDimuonVerticesName"   ,
                  m_outputDimuonVerticesName);
 declareProperty("PreTrigger"   ,
                  m_pretrigger=false);

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonTrigger2MuonsNoIP::~HltMuonTrigger2MuonsNoIP() {
  delete _negMuonFil;
  delete _posMuonFil;

  delete _massCut;
  //  delete _massAndIPCut;
  delete _massAndL0Cut;
  // delete _massAndIPAndL0Cut;
  delete _massAndDOCACut;
  //delete _massAndDOCAAndIPCut;

  delete _triggerMassCut;
  //  delete _triggerMassAndIPCut;

  delete _chargeFun;
  delete _ipFun ;
  delete _docaFun;
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonTrigger2MuonsNoIP::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_patVertexBank =
    m_patDataStore->createVertexContainer(m_outputDimuonVerticesName,200);
  
  _chargeFun= new Hlt::Charge();
  _ipFun = new Hlt::IP();
  _docaFun=new Hlt::DOCA();
  
  _negMuonFil = ( *_chargeFun < 0.0).clone();
  _posMuonFil = ( *_chargeFun > 0.0).clone();
  m_massKey= HltNames::particleInfoID("Mass");
  m_IPKey = HltNames::particleInfoID("IP");
  m_DOCAKey=HltNames::particleInfoID("DOCA");
  m_MuonKey=HltNames::particleInfoID("MuonKey");
  
  Hlt::Info<RecVertex> massInfo(m_massKey);
  Hlt::Info<RecVertex> ipInfo(m_IPKey); 
  Hlt::Info<RecVertex> docaInfo(m_DOCAKey);
  Hlt::Info<RecVertex> L0ConfirmationInfo(m_MuonKey);
  //  _massAndIPCut=((massInfo> m_minMassWithIP)&& (ipInfo > m_minIP)).clone();
  _massCut=((massInfo> m_minMassNoIP)).clone();  
  //_massAndDOCAAndIPCut=((massInfo> m_minMassWithIP)&&
  //  (docaInfo< m_maxDOCA)&& (ipInfo > m_minIP)).clone();
  _massAndDOCACut=((massInfo> m_minMassNoIP)&&(docaInfo<m_maxDOCA)).clone();
  
  
  //  _massAndIPAndL0Cut=((massInfo> m_minMassWithIP)&& (ipInfo > m_minIP) && (L0ConfirmationInfo>0)).clone();
  _massAndL0Cut=((massInfo> m_minMassNoIP)&&(L0ConfirmationInfo>0)).clone();
  debug() << "m_pretrigger "<< m_pretrigger << endmsg;
  if(m_pretrigger){
    
    _triggerMassCut=_massCut;
    //_triggerMassAndIPCut=_massAndIPCut;
  }
  else{
    _triggerMassCut=_massAndDOCACut;
    //_triggerMassAndIPCut=_massAndDOCAAndIPCut;
  }
  
  //   if(m_L0confirm!=0){
  //     _triggerMassCut=_massAndL0Cut;
  //     _triggerMassAndIPCut=_massAndIPAndL0Cut;
  //   }else{
  //     _triggerMassCut=_massCut;
  //     _triggerMassAndIPCut=_massAndIPCut;
  //   }
  
  
  
  //   if(m_selection2SummaryName!=""){
  //         m_selection2SummaryID=
  //         HltNames::selectionSummaryID(m_selection2SummaryName);
  //   }
  debug() << "==> Initialize" << endmsg;
  
  
  
  initializeHisto(h_mass,"Mass",0.,10000.,100);
  initializeHisto(h_IP,"IP",0,1.0,100);
  initializeHisto(h_DOCA,"DOCA",0.,1.0,100);
  
  
  
//  checkInput(m_inputTracks," input tracks");
//  checkInput(m_outputTracks," output tracks");
//  checkInput(m_outputTracks2," output tracks 2 ");
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonTrigger2MuonsNoIP::execute() {
  //info()<<" sono qui "<<endreq;
  
  StatusCode sc = StatusCode::SUCCESS;
  //info()<<" sono qui "<<endreq;
  //bool ok  = HltAlgorithm::beginExecute();
  // if (!ok) return stop(" No input tracks");
  
  //  ELoop::map(*m_inputTracks,*_ptFun,m_ptKey);
  //  info()<<" size "<<*m_inputTracks->size()
  //double pt1 = (*m_inputTracks->begin())->pt();
  //fillHisto( m_histoPt1, pt1, 1.);
  //  setDecision(false);
  //  HltAlgorithm::monitor(*m_inputTracks,m_ptKey,m_histoPt);
  
  
  
  
  
  
  // m_outputTracks->clear();
  m_otrack.clear();
  m_otrack2.clear();
  ELoop::select(*m_inputTracks,*_negMuonFil,m_otrack);
  int n1 = m_otrack.size();
  if (n1 == 0) return stop(" No negative muon tracks ");
  //m_outputTracks2->clear();
  ELoop::select(*m_inputTracks,*_posMuonFil,m_otrack2);
  int n2 = m_otrack2.size();
  if (n2 == 0) return stop(" No positive muon tracks ");
  if(m_debug){
    debug()<<" positive and negative muosn size "<<n1<<" "<<n2<<endreq;
  }
  // now we have positive and negative muons
  //first calculate IP for each muon
  
//   ELoop::map_compare_value(m_otrack,*m_patInputVertices,  
//                            *_ipFun, Estd::abs_min(),
//                            m_IPKey);
//   ELoop::map_compare_value(m_otrack2,*m_patInputVertices,  
//                            *_ipFun, Estd::abs_min(),
//                            m_IPKey);
  m_overtices.clear();

  makeDiMuonPair(m_otrack,m_otrack2, m_overtices);  
  if (m_overtices.size() <1) return stop(" No vertices");
  if(m_debug){
    debug()<<" vertices "<<m_overtices.size()<<endreq;
  }
  // now check if some vertex is ok
  //bool store=false;
  //bool store1=false;
  
  
  m_selevertices.clear();  
  m_testedvertices.clear();
  //   ELoop::select(m_overtices,*_triggerMassAndIPCut,m_selevertices);
  //   if(m_selevertices.size()>0)store=true;
  //   if(store){
  //     //check if L0 dimuon is confirmed
  //     for(  Hlt::VertexContainer::const_iterator idimuon=m_selevertices.begin();
  //           idimuon<m_selevertices.end();idimuon++){
  //       RecVertex* toTest=*idimuon;
  //       StatusCode sc=checkIfL0Dimuon(toTest);
  //       if(sc.isSuccess()){
  //         //info()<<" qui"<<endreq;
  
  //         m_testedvertices.push_back(toTest);
  //         m_outputVertices->push_back(toTest);
  //       }
  
  //     }
  
  //   }
  //   store=false;
  //   if(m_testedvertices.size()>0)store=true;
  //   if(store){
  //     float m=(*(m_testedvertices.begin()))->info(m_massKey,0);
  //     float d=(*(m_testedvertices.begin()))->info(m_DOCAKey,0);
  //     float p=(*(m_testedvertices.begin()))->info(m_IPKey,0);
  
  //     fillHisto(h_mass,m,1.0);
  //     fillHisto(h_DOCA,d,1.0);
  //     fillHisto(h_IP,p,1.0); 
  //     setDecisionType(HltEnums::DiMuon,m_selection2SummaryID);
  //     saveInSummary(m_testedvertices,m_selection2SummaryID);
  //   }
  
  
  //   if(m_debug){  
  //     debug()<<" mass and Ip cut "<<m_selevertices.size()<<" and L0 chek "
  //            <<m_testedvertices.size()<<endreq;
  //   }
  //   m_selevertices.clear();  
  //   m_testedvertices.clear();
  
  ELoop::select(m_overtices,*_triggerMassCut,m_selevertices);
  if(m_selevertices.size()>0)
  {
    
    //  if(store1){
    
    for(  Hlt::VertexContainer::const_iterator idimuon=m_selevertices.begin();
          idimuon<m_selevertices.end();idimuon++){
      RecVertex* toTest=*idimuon;
      StatusCode sc=checkIfL0Dimuon(toTest);
      if(sc.isSuccess()){ 
        //info()<<" qui 1 "<<endreq;
        m_testedvertices.push_back(toTest);
        m_outputVertices->push_back(toTest);
      }
    
    }
  }
  //store1=false;
  if(m_testedvertices.size()>0){
    
    //store1=true;
  
    //if(store1){
    float m=(*(m_testedvertices.begin()))->info(m_massKey,0);
    float d=(*(m_testedvertices.begin()))->info(m_DOCAKey,0);
    //    float p=(*(m_selevertices.begin()))->info(m_IPKey,0);
    //info()<<" massa qui "<<(*(m_outputVertices->begin()))->info(m_massKey,0)<<endreq;    
    fillHisto(h_mass,m,1.0);
    fillHisto(h_DOCA,d,1.0); 
    //setDecisionType(HltEnums::JPsi);
    //saveInSummary(m_testedvertices);
  }
  
  if(m_debug){ 
    debug()<<" mass  cut "<<m_selevertices.size()<<
      " and L0 chek "<<m_testedvertices.size()<<endreq;
  }
  //if(!store&&!store1)return stop("no mu pair is ok");
  //setDecision(true);	
  //HltAlgorithm::endExecute();


  if (m_debug) {
    debug() << " number of muon positive tracks " << n1 << endreq;
  }
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonTrigger2MuonsNoIP::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



void HltMuonTrigger2MuonsNoIP::makeDiMuonPair(const Hlt::TrackContainer& tcon1,
                                              const Hlt::TrackContainer& tcon2,
                                              // Estd::bifunction<Track,Track>& fun,
  // Estd::filter<double>& cut,
                                              Hlt::VertexContainer& vcon)
{

  double massmu=105;
  
  for (Hlt::TrackContainer::const_iterator it = tcon1.begin();
       it != tcon1.end(); ++it) {
    for (Hlt::TrackContainer::const_iterator it2 = tcon2.begin();
         it2 != tcon2.end(); ++it2) {
        double mass= HltUtils::invariantMass( **it, **it2,massmu,massmu); 
        double doca=HltUtils::closestDistanceMod( **it, **it2);
        //        RecVertex* ver=new RecVertex;; 
        RecVertex* ver = m_patVertexBank->newEntry();
        _vertexCreator(**it,**it2, *ver) ;
        ver->addInfo(m_massKey,mass); 
        ver->addInfo(m_DOCAKey,doca);    
        //double IPmuon1=
        //add also IP of the two muons... 
        double ip1=(*it)->info(m_IPKey,0.);
        double ip2=(*it2)->info(m_IPKey,0.);
        int L0track=0;
        if((*it)->checkFlag(Track::L0Candidate))L0track++;
        if((*it2)->checkFlag(Track::L0Candidate))L0track++;
        
        if(fabs(ip1)<fabs(ip2)){
          ver->addInfo(m_IPKey,ip1);
        }else  ver->addInfo(m_IPKey,ip2);
        ver->addInfo(m_MuonKey,L0track);
        verbose()<<"vertex mass "<<ver->info(m_massKey,0)<<endreq;
        verbose()<<" vertex position "<< ver->position()<<endreq;   
        //m_patVertexBank->newEntry();        
        vcon.push_back( ver);
        verbose() << " make vertices with mass " << mass << endreq;
        verbose()<<" P of the two tracks "<< (**it).p()<<" "<<(**it2).p()<<endreq;       
      }      
    }
  
}


StatusCode HltMuonTrigger2MuonsNoIP::checkIfL0Dimuon(RecVertex* vertexToTest)
{
  
  //colect both tracks that formed the vertex
  SmartRefVector< LHCb::Track > list_tracks= 	(vertexToTest)->tracks();
  if(list_tracks.size()!=2)return StatusCode::FAILURE;

  Track* firstMuon=NULL;  
  Track* secondMuon=NULL;  
  Track* firstL0track=NULL;  
  Track* secondL0track=NULL;
  
  
  firstMuon=list_tracks[0];
  secondMuon=list_tracks[1];
  if(firstMuon==NULL||secondMuon==NULL)return StatusCode::FAILURE;
  if(m_debug){
    info()<<"both muons have been detected "
          <<firstMuon<<" "<<secondMuon<<endreq;
    
  }

  for(  Hlt::VertexContainer::const_iterator L0dimuon=
          m_inputVertices->begin();
        L0dimuon<m_inputVertices->end();L0dimuon++){ 
    SmartRefVector< LHCb::Track >  vertex_tracks=	(*L0dimuon)->tracks();
    //info()<<" vertex size "<<vertex_tracks.size()<<endreq;
    
    if(vertex_tracks.size()!=2)return StatusCode::FAILURE;
    if(m_debug){
      info()<<"both L0 muons exist in input vertex "<<endreq;
      
    }
    Track* firstMuonInL0=vertex_tracks[0];
    Track* secondMuonInL0=vertex_tracks[1];
    if(m_debug){
      info()<<"L0 tracks "<<firstMuonInL0<<" "<<secondMuonInL0<<endreq;
    }
    //now check if both tracks are ancestors of the tracks of input vertex
    for(SmartRefVector< LHCb::Track >::iterator 
          ance=firstMuon->ancestors().begin();
        ance<firstMuon->ancestors().end();ance++){

      Track* ancestor=*ance;
      if(m_debug){ 
        info()<<firstMuon->ancestors().size()<<endreq;
        
        info()<<"ancestor "<<ancestor<<" "<<firstMuon<<endreq;
      }
      
      if(ancestor==firstMuonInL0){
        firstL0track=firstMuonInL0;            
        break;            
      }          
      if(ancestor==secondMuonInL0){
        firstL0track=secondMuonInL0;            
        break;            
      }          
    }
    for(SmartRefVector< LHCb::Track >::iterator 
          ance=secondMuon->ancestors().begin();
        ance<secondMuon->ancestors().end();ance++){
      Track* ancestor=*ance;
      //info()<<"ancestor "<<ancestor<<endreq;
      if(ancestor==firstMuonInL0){
        secondL0track=firstMuonInL0;            
        break;            
      }          
      if(ancestor==secondMuonInL0){
        secondL0track=secondMuonInL0;            
        break;            
      }          
    }
  
    if(firstL0track!=NULL&&secondL0track!=NULL&&firstL0track!=secondL0track){      
      return StatusCode::SUCCESS;
    }  
  }
  return StatusCode::FAILURE; 
}





