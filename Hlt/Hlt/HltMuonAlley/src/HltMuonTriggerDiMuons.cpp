// $Id: HltMuonTriggerDiMuons.cpp,v 1.8 2007-10-16 08:31:53 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonTriggerDiMuons.h"

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
DECLARE_ALGORITHM_FACTORY( HltMuonTriggerDiMuons );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonTriggerDiMuons::HltMuonTriggerDiMuons( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  //  declareProperty("PtMin", m_ptMin = 0.);
 declareProperty("Selection2Name", m_selection2SummaryName="");
 declareProperty("MassWithIP", m_minMassWithIP = 500.);
 declareProperty("MassNoIP", m_minMassNoIP = 2500.);
 declareProperty("IP", m_minIP = 0.075);
 declareProperty("DOCA", m_maxDOCA = 0.5);
 declareCondition("MassWithIP", m_minMassWithIP);
 declareCondition("MassNoIP", m_minMassNoIP );
 declareCondition("IP", m_minIP );
 declareCondition("DOCA", m_maxDOCA );

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonTriggerDiMuons::~HltMuonTriggerDiMuons() {
 //  delete _negMuonFil;
//   delete _posMuonFil;

//   delete _massAndDOCACut;
//   delete _massAndDOCAAndIPCut;
//   delete _chargeFun;
//   delete _ipFun ;
//   delete _docaFun;


  
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonTriggerDiMuons::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_patVertexBank =
    m_patDataStore->createVertexContainer("Hlt/Vertex/DiMuonVer",200);

  //  m_ptKey = HltNames::particleInfoID("PT");
  //double cutr=0.0;
  //_ptFun = new  Hlt::PT();
  _chargeFun= new Hlt::Charge();
  _ipFun = new Hlt::IP();
  _docaFun=new Hlt::DOCA();
  

  _negMuonFil = ( *_chargeFun < 0.0).clone();
  _posMuonFil = ( *_chargeFun > 0.0).clone();
  m_massKey= HltNames::particleInfoID("Mass");
  m_IPKey = HltNames::particleInfoID("IP");
  m_DOCAKey=HltNames::particleInfoID("DOCA");
  
  Hlt::Info<RecVertex> massInfo(m_massKey);
  Hlt::Info<RecVertex> ipInfo(m_IPKey);
  Hlt::Info<RecVertex> docaInfo(m_DOCAKey);
  
  _massAndDOCAAndIPCut=((massInfo> m_minMassWithIP)&&
                        (docaInfo< m_maxDOCA)&& (ipInfo > m_minIP)).clone();
  _massAndDOCACut=((massInfo> m_minMassNoIP)&&(docaInfo<m_maxDOCA)).clone();
  

 if(m_selection2SummaryName!=""){
    //        m_selection2SummaryID=
   // HltNames::selectionSummaryID(m_selection2SummaryName);
   //JAH 
   m_selection2SummaryID = 
     HltConfigurationHelper::getID(*m_conf,"SelectionID",m_selection2SummaryName);
   HltSelectionSummary& sel2 = 
     m_datasummary->selectionSummary(m_selection2SummaryID);
   Hlt::DataSizeHolder<Hlt::VertexContainer>* holder = 
     new Hlt::DataSizeHolder< Hlt::VertexContainer > (m_selevertices2);
   sel2.addData( *holder );
 }

//  checkInput(m_inputTracks," input tracks");
//  checkInput(m_outputTracks," output tracks");
//  checkInput(m_outputTracks2," output tracks 2 ");

  initializeHisto(h_mass,"Mass",0.,10000.,100);
  initializeHisto(h_IP,"IP",0,1.0,100);
  initializeHisto(h_DOCA,"DOCA",0.,1.0,100);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonTriggerDiMuons::execute() {
//info()<<" sono qui "<<endreq;

  StatusCode sc = StatusCode::SUCCESS;

  setDecision(false);
  //  ELoop::map(*m_inputTracks,*_ptFun,m_ptKey);
//  info()<<" size "<<*m_inputTracks->size()
  //double pt1 = (*m_inputTracks->begin())->pt();
  //fillHisto( m_histoPt1, pt1, 1.);

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
	
  m_selevertices2.clear();
  ELoop::select(m_overtices,*_massAndDOCAAndIPCut,m_selevertices2);
  if(m_selevertices2.size()>0)store=true;
  if(store){
    float m=(*(m_selevertices2.begin()))->info(m_massKey,0);
    float d=(*(m_selevertices2.begin()))->info(m_DOCAKey,0);
    float p=(*(m_selevertices2.begin()))->info(m_IPKey,0);
    
    fillHisto(h_mass,m,1.0);
    fillHisto(h_DOCA,d,1.0);
    fillHisto(h_IP,p,1.0);
    HltSelectionSummary& sel2 = 
      m_datasummary->selectionSummary(m_selection2SummaryID);
    sel2.setDecisionType(HltEnums::DiMuon,true);
    debug()<<" uu "<<m_selevertices2.size()<<endreq;
  }
  
  if(m_debug){  
    debug()<<" mass and Ip cut "<<m_selevertices2.size()<<endreq;
    debug()<<store<<endreq;
  }
  m_outputVertices->clear();
  ELoop::select(m_overtices,*_massAndDOCACut,*m_outputVertices);
  if(m_outputVertices->size()>0)store1=true;
  if(store1){
    float m=(*(m_outputVertices->begin()))->info(m_massKey,0.0);
    float d=(*(m_outputVertices->begin()))->info(m_DOCAKey,0.0);
    //    float p=(m_outputVertices->begin())->info(m_IPKey);
    
    fillHisto(h_mass,m,1.0);
    fillHisto(h_DOCA,d,1.0);
    //fillHisto(h_IP,p,1.0);
    setDecisionType(HltEnums::JPsi);
    debug()<<" dd "<<m_outputVertices->size()<<endreq;
    
  }
  
  if(!store1&&!store)return stop("no mu pair vertex is ok");
  if(m_debug){ 
    debug()<<store<<endreq;
    debug()<<" mass  cut "<<m_outputVertices->size()<<endreq;
  }
  if(store||store1){
    setFilterPassed(true);  
   
  }

  //setDecisionType(HltEnums::DiMuon);
  //saveInSummary(*m_outputTracks);
  //saveInSummary(*m_outputTracks2);


  if (m_debug) {
    debug() << " number of muon positive tracks " << n1 << endreq;
  }
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonTriggerDiMuons::finalize() {

  debug() << "==> Finalize" << endmsg;

  StatusCode sc =  HltAlgorithm::finalize();  // must be called after all other actions

  delete _negMuonFil;
  delete _posMuonFil;

  delete _massAndDOCACut;
  delete _massAndDOCAAndIPCut;
  delete _chargeFun;
  delete _ipFun ;
  delete _docaFun;

  return sc;
}

//=============================================================================



void HltMuonTriggerDiMuons::makeDiMuonPair(const Hlt::TrackContainer& tcon1,
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
        RecVertex* ver = m_patVertexBank->newEntry();
        //RecVertex* ver=new RecVertex;;
        _vertexCreator(**it,**it2, *ver) ;
        ver->addInfo(m_massKey,mass);
        ver->addInfo(m_DOCAKey,doca);        
        //double IPmuon1=
        //add also IP of the two muos... 
        double ip1=(*it)->info(m_IPKey,0.);
        double ip2=(*it2)->info(m_IPKey,0.);

        if(fabs(ip1)<fabs(ip2)){
          ver->addInfo(m_IPKey,ip1);
        }else  ver->addInfo(m_IPKey,ip2);
        
        vcon.push_back( ver);
        debug() << " make vertices with mass " << mass << " "<<doca<<" "<<ip1<<" "<<ip2<<endreq;
        verbose()<<" P of the two tracks "<< (**it).p()<<" "<<(**it2).p()<<endreq;       
      }      
    }
  
}


