// $Id: HltMuonPreTriggerDiMuons.cpp,v 1.5 2007-06-20 16:12:57 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonPreTriggerDiMuons.h"

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
DECLARE_ALGORITHM_FACTORY( HltMuonPreTriggerDiMuons );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonPreTriggerDiMuons::HltMuonPreTriggerDiMuons( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  //  declareProperty("PtMin", m_ptMin = 0.);
 declareProperty("Selection2Name", m_selection2SummaryName="");
 declareProperty("MassWithIP", m_minMassWithIP = 500.);
 declareProperty("MassNoIP", m_minMassNoIP = 2500.);
 declareProperty("IP", m_minIP = 0.075);
 declareCondition("MassWithIP", m_minMassWithIP);
 declareCondition("MassNoIP", m_minMassNoIP);
 declareCondition("IP", m_minIP);

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonPreTriggerDiMuons::~HltMuonPreTriggerDiMuons() {
  delete _negMuonFil;
  delete _posMuonFil;

  delete _massCut;
  delete _massAndIPCut;
  delete _chargeFun;
  delete _ipFun ;
  
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonPreTriggerDiMuons::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_patVertexBank =
    m_patDataStore->createVertexContainer("Hlt/Vertex/PreDiMuonVer",200);

  //  m_ptKey = HltNames::particleInfoID("PT");
  //double cutr=0.0;
  //_ptFun = new  Hlt::PT();
  _chargeFun= new Hlt::Charge();
  _ipFun = new Hlt::IP();
  

  _negMuonFil = ( *_chargeFun < 0.0).clone();
  _posMuonFil = ( *_chargeFun > 0.0).clone();
  m_massKey= HltNames::particleInfoID("Mass");
  m_IPKey = HltNames::particleInfoID("IP");
  m_DOCAKey=HltNames::particleInfoID("DOCA");
  Hlt::Info<RecVertex> massInfo(m_massKey);
  Hlt::Info<RecVertex> ipInfo(m_IPKey);

  _massAndIPCut=((massInfo> m_minMassWithIP)&& (ipInfo > m_minIP)).clone();
  _massCut=((massInfo> m_minMassNoIP)).clone();
  if(m_selection2SummaryName!=""){
        m_selection2SummaryID=
        HltNames::selectionSummaryID(m_selection2SummaryName);
  }
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
StatusCode HltMuonPreTriggerDiMuons::execute() {
//info()<<" sono qui "<<endreq;

  StatusCode sc = StatusCode::SUCCESS;
  //info()<<" sono qui "<<endreq;

  //  ELoop::map(*m_inputTracks,*_ptFun,m_ptKey);
  //  info()<<" size "<<*m_inputTracks->size()
  //double pt1 = (*m_inputTracks->begin())->pt();
  //fillHisto( m_histoPt1, pt1, 1.);
  setDecision(false);
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

  ELoop::map_compare_value(m_otrack,*m_patInputVertices,  
                           *_ipFun, Estd::abs_min(),
                           m_IPKey);
  ELoop::map_compare_value(m_otrack2,*m_patInputVertices,  
                           *_ipFun, Estd::abs_min(),
                           m_IPKey);
  m_overtices.clear();

  makeDiMuonPair(m_otrack,m_otrack2, m_overtices);  
  if (m_overtices.size() <1) return stop(" No vertices");
  if(m_debug){
    debug()<<" vertices "<<m_overtices.size()<<endreq;
  }
  // now check if some vertex is ok
  bool store=false;
  bool store1=false;
  
  
  m_selevertices.clear();
  ELoop::select(m_overtices,*_massAndIPCut,m_selevertices);
  if(m_selevertices.size()>0)store=true;
  if(store){
    float m=(*(m_selevertices.begin()))->info(m_massKey,0);
    float d=(*(m_selevertices.begin()))->info(m_DOCAKey,0);
    float p=(*(m_selevertices.begin()))->info(m_IPKey,0);
    
    fillHisto(h_mass,m,1.0);
    fillHisto(h_DOCA,d,1.0);
    fillHisto(h_IP,p,1.0); 
    setDecisionType(HltEnums::DiMuon,m_selection2SummaryID);
    saveInSummary(m_selevertices,m_selection2SummaryID);
  }
  

  if(m_debug){  
    debug()<<" mass and Ip cut "<<m_selevertices.size()<<endreq;
  }
  m_selevertices.clear();
  ELoop::select(m_overtices,*_massCut,m_selevertices);
  if(m_selevertices.size()>0)store1=true;
  if(store1){
    float m=(*(m_selevertices.begin()))->info(m_massKey,0);
    float d=(*(m_selevertices.begin()))->info(m_DOCAKey,0);
    //    float p=(*(m_selevertices.begin()))->info(m_IPKey,0);
    
    fillHisto(h_mass,m,1.0);
    fillHisto(h_DOCA,d,1.0); 
    setDecisionType(HltEnums::JPsi);
    saveInSummary(m_selevertices);
  }
  
  if(m_debug){ 
    debug()<<" mass  cut "<<m_selevertices.size()<<endreq;
  }
  if(!store&&!store1)return stop("no mu pair is ok");
  setDecision(true);

  if (m_debug) {
    debug() << " number of muon positive tracks " << n1 << endreq;
  }
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonPreTriggerDiMuons::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



void HltMuonPreTriggerDiMuons::makeDiMuonPair(const Hlt::TrackContainer& tcon1,
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
        if(fabs(ip1)<fabs(ip2)){
          ver->addInfo(m_IPKey,ip1);
        }else  ver->addInfo(m_IPKey,ip2);
        
        vcon.push_back( ver);
        verbose() << " make vertices with mass " << mass << endreq;
        verbose()<<" P of the two tracks "<< (**it).p()<<" "<<(**it2).p()<<endreq;       
      }      
    }
  
}


