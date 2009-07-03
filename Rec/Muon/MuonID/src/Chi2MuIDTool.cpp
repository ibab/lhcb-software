// $Id: Chi2MuIDTool.cpp,v 1.3 2009-07-03 19:54:55 polye Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "Chi2MuIDTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : muIDTool
//
// 2008-07-02 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Chi2MuIDTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Chi2MuIDTool::Chi2MuIDTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ImuIDTool>(this);
  
  // Number of sigmas for FOI
  declareProperty("NSigmas",m_nsigmas = 5.);
  
  // Discrimination value to select hits in FOI    
  // - 1 chi2
  // - 2 chi2x
  // - 3 chi2y
  // - 4 distance
  // - 5 distx
  // - 6 disty
  declareProperty("DiscrValue",m_discrval = 1);
  
  //Use background and return clratio?
  declareProperty("UseBkg",m_useBkg=false);
  
  //Min momentum to accept hits in stations 3 and 4
  declareProperty("MinMomSt3",m_2hits = 3500);

  //Min momentum to accept hits in station 4
  declareProperty("MinMomSt4",m_3hits = 4500);   

}


StatusCode Chi2MuIDTool::initialize() {
  m_fitter = tool<ITrackFitter>("TrackMasterFitter","fitter",this); 
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider",this);  
  m_mySeedState=0;

  m_mudet = getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  //Find z of mu stations
  for (int i=0;i<5;i++) 
  {
    m_zstations.push_back(m_mudet->getStationZ(i));
    //m_muonProviderInRange.push_back(MeasPairList());
  }
  
  // Build CLquality and GetArrival tools
  m_CLQuality  = tool<ICLTool>("CLTool","CLQuality",this);
  // m_CLArrival  = tool<ICLTool>("CLTool/CLArrival",this);
  m_GetArrival = tool<IGetArrival>("GetArrival","GetArrival",this);
  
  m_IsMuonTool = tool<IIsMuonCandidateC>("IsMuonCandidateC","IsMuonCandidateC",this);
  

  // Find min momentum to accept any track
  m_minmom=m_CLQuality->minRange();
  
  // Find inside which stations must be the track to be in acceptance
  m_stations_acceptance.push_back(0);
  m_stations_acceptance.push_back(4);

  return StatusCode::SUCCESS;
}



  
StatusCode Chi2MuIDTool::isGoodSeed(const LHCb::Track& seed)
{
  
  StatusCode sc;
  //track below min momentum
  if (seed.p()<m_minmom)
  {
    debug()<< "Track mom below momentum threshold of "<<m_minmom<<endreq;
    sc.setCode(101);
    return sc;
  }
  
  
  //track not in acceptance (standard definition)
  if (!(isTrackInAcceptance(seed)))
  {
    debug()<< "Track not in MuonChambers acceptance"<<endreq;
    sc.setCode(102);
    return sc;
  }
  return StatusCode::SUCCESS;
}


StatusCode Chi2MuIDTool::muonCandidate(const LHCb::Track& seed, 
                                       LHCb::Track& muTrack,
                                       bool isMuonCandidate,
                                       const std::vector<LHCb::LHCbID> ids_init)
{
  StatusCode sc;
  m_arrivalCuts=true;
  m_applyIsMuon=true;
  debug()<<"ids_init.size()="<<ids_init.size()<<endreq;
  bool del_muonprov=false;

  if (ids_init.size()==0)
  {
    // access muon hits info from TES
    m_muonProvider = get<SmartMuonMeasProvider>("Rec/Muon/SmartMuonMeasProvider");
    m_nsigmasUsed=m_nsigmas;
    if (m_muonProvider==NULL) 
    {
      error()<<"SmartMuonMeasProvider not in TES, please load"<<endreq;
      sc.setCode(201);
      return sc;
    }    
  }
  
  else
  {
    // build SmartMuonMeasProvider with LHCbIDs provided and 
    // fix big nsigmas to ensure all ids are taken into account
    // don't apply ismuon or parrival cuts
    del_muonprov=true;
    m_arrivalCuts=false;
    m_applyIsMuon=false;

    debug()<<"READY TO CHARGE IDS onto m_muonProvider"<<endreq;
    std::vector<LHCb::LHCbID>::const_iterator it;
    
    m_muonProvider = new SmartMuonMeasProvider();
    
    for(it = ids_init.begin(); it != ids_init.end(); ++it ) 
    {
      LHCb::LHCbID id=*it;
      //bug, some non muon chambers IDs?
      //if (!id.isMuon()) continue;
      
      int istation=id.muonID().station();

      //from LHCbID, get xmeas and ymeas
      debug()<<"istation="<<istation<<endreq;
      debug()<<"channelID="<<id.channelID()<<endreq;
      if (!id.isMuon()) {
        warning()<<"NON MUON ID!"<<endreq;
        continue;
      }
      
      LHCb::Measurement* xmeas = m_measProvider->measurement(id,0);
      LHCb::Measurement* ymeas = m_measProvider->measurement(id,1);
      //debug()<<"xmeas="<<(*xmeas)<<endreq;
      //debug()<<"ymeas="<<(*ymeas)<<endreq;
      debug()<<"channelID after measprovider="<<xmeas->lhcbID().channelID()<<endreq;
      debug()<<"xmeas before smmp="<<xmeas<<endreq;

      debug()<<"m_muonProvider->atStation("<<istation<<").size()="<<m_muonProvider->atStation(istation).size()<<endreq;
      //for each station, fill muprovider with corresponding MeasPair
      m_muonProvider->atStation(istation).push_back( MeasPair(xmeas,ymeas) );
      m_nsigmasUsed=50;
      debug()<<"ID correctly loaded into m_muonProvider"<<endreq;
    }
    debug()<<"ID correctly loaded into m_muonProvider"<<endreq;
  }
  
  // get hits in FOI (nsigmas). Run IsMuon
  sc = search(seed, muTrack);
  debug()<<"muTrack with seeds on it"<<endreq;
  if (sc.isFailure()) 
  {
    debug()<< "No new track built"<<endreq;
    if (sc.getCode()==203) isMuonCandidate=false;
    else if ((sc.getCode()!=204) && (m_applyIsMuon))  warning()<< "No available info about IsMuon"<<endreq;
    if (del_muonprov) delete m_muonProvider;
    return sc;
  }
  else isMuonCandidate=true;

  //add seed track to muon track ancestors
  muTrack.addToAncestors(seed) ;

  if (del_muonprov) delete m_muonProvider;
  return StatusCode::SUCCESS;
}



StatusCode Chi2MuIDTool::muonQuality(LHCb::Track& muTrack, double& Quality)
{

  StatusCode sc;
  
  if (muTrack.states().size()>1) warning()<<"muonQuality:: MUTRACK WITH MORE THAN ONE SEED STATE ON IT"<<endreq;
  else if (muTrack.states().size()<1) 
  {
    error()<<"muonQuality:: MUTRACK WITHOUT ANY SEED STATE ON IT"<<endreq;
    sc.setCode(301);
    Quality=0;
    return sc;
  }
  
  if (muTrack.lhcbIDs().size()<1) 
  {
    error()<<"muonQuality:: NO LHCbIDs ON TRACK. IMPOSSIBLE TO CALCULATE QUALITY"<<endreq;
    sc.setCode(302);
    Quality=0;
    return sc;
  }
  
  // load these measurements into a track and perform fit
  // m_measProvider->load(muTrack);  
  StatusCode sc2 = m_fitter->fit(muTrack);
  debug()<<"BACK in muonQuality"<<endreq;
  debug()<<"sc2"<<sc2.getCode()<<endreq;
  debug()<<"sc2.isFailure()"<<sc2.isFailure()<<endreq;
  
  if (sc2.isFailure())
  {
      debug()<< "WRONG FIT PERFORMED"<<endreq;
      sc.setCode(303);
      Quality=0;
      return sc;
  }
  
  
  debug()<<"muTrack="<<muTrack<<endreq;
  debug()<<"ids size="<<muTrack.lhcbIDs().size()<<endreq;
  debug()<<"chi2="<<muTrack.chi2()<<endreq;
  Quality=muTrack.chi2()/(2.*muTrack.lhcbIDs().size());
  return StatusCode::SUCCESS;
}



StatusCode Chi2MuIDTool::muonArrival(LHCb::Track& muTrack, double& Arrival)
{
  //compute arrival probability
  StatusCode sc=m_GetArrival->getArrivalFromTrack(muTrack,Arrival);
  debug()<<"Arrival="<<Arrival<<endreq;
  if (sc.isFailure()) Arrival=0;
  return sc;
}



StatusCode Chi2MuIDTool::muonDLL(LHCb::Track& muTrack, const double& Quality, double& CLQuality, 
                                 const double& Arrival, double& CLArrival, double& DLL)
  
{
  StatusCode sc;
  //get clquality from cltool
  
  double qualityCLs=0;
  double qualityCLb=0;
  double qualityCLR=0;

  debug()<<"Arrival="<<Arrival<<endreq;
  debug()<<"Qualty ="<<Quality<<endreq;
  

  
  sc=m_CLQuality->cl(Quality,qualityCLs,qualityCLb,qualityCLR,muTrack.p());
  if (sc.isFailure())
  {
    
    debug()<< "WRONG CLQUALITY VALUE"<<endreq;
    return sc;
  }
  
  
  // use cl ratio or simply clsignal
  if (m_useBkg) CLQuality=qualityCLR;
  else 
  {
    if (qualityCLs==0.0) qualityCLs=1e-6;
    CLQuality=qualityCLs;
  }

  sc=m_GetArrival->clArrival(muTrack,CLArrival);
  if (sc.isFailure())
  {
    debug()<< "WRONG CLARRIVAL VALUE"<<endreq;
    CLArrival=0.0;
    return sc;
  }
  if (CLArrival==0.0) CLArrival=1e-6;
  
  DLL = log(CLQuality*CLArrival);
  
  return StatusCode::SUCCESS;
  
}

StatusCode Chi2MuIDTool::muonID(const LHCb::Track& seed,LHCb::Track& muTrack, 
                                bool isMuonCandidate,
                                double& Quality, double& CLQuality, 
                                double& Arrival, double& CLArrival, 
                                double& DLL,
                                const std::vector<LHCb::LHCbID> ids_init)
{
  StatusCode sc;
  
  sc=isGoodSeed(seed);
  if (sc.isFailure()) return sc;

  sc=muonCandidate(seed,muTrack,isMuonCandidate,ids_init);
  if (sc.isFailure()) return sc;
  
  sc=muonQuality(muTrack,Quality);
  if (sc.isFailure()) return sc;
  
  sc=muonArrival(muTrack, Arrival);
  if (sc.isFailure()) return sc;

  sc=muonDLL(muTrack, Quality, CLQuality, Arrival, CLArrival, DLL);
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}



 
//for acceptance: is track extrapolation inside istation?
bool Chi2MuIDTool::isTrackInsideStation(const int& istation)
{ 
  
  const LHCb::State& muStateC = m_states[istation];
  
  //get conditions: is track inside station?
  const Gaudi::XYZPoint& pos = muStateC.position();
  const bool xcond=((fabs(pos.x()) > m_mudet->getInnerX(istation)) && (fabs(pos.x())<m_mudet->getOuterX(istation)));
  const bool ycond=((fabs(pos.y()) > m_mudet->getInnerY(istation)) && (fabs(pos.y())<m_mudet->getOuterY(istation)));
  
  debug()<<"st="<<istation<<",xcond="<<xcond<<",ycond="<<ycond<<endreq;
  
  bool outcond=false;
  
  if (xcond && ycond) outcond=true;
  else
  {
    if (xcond && (fabs(pos.y())<m_mudet->getOuterY(istation))) outcond=true;
    if (ycond && (fabs(pos.x())<m_mudet->getOuterX(istation))) outcond=true;
  }

  return outcond;

}


// Check if track is in acceptance for given stations
bool Chi2MuIDTool::isTrackInAcceptance(const LHCb::Track& seed)
{
    
  StatusCode sc=makeStates(seed);
  if (sc.isFailure()) return false;
    
  
  for (std::vector<int>::const_iterator it = m_stations_acceptance.begin();
       it != m_stations_acceptance.end(); ++it) 
  {
    int istation = *it;
    if (!(isTrackInsideStation(istation))) return false;
  }
  
  return true;
}


// Function makeStates: build extrapolated states for each station from seed
StatusCode Chi2MuIDTool::makeStates(const LHCb::Track& seed){

  const std::vector<LHCb::State*> & muStates = seed.states();
  if (muStates.size()==0) {
    debug()<<"MAKE STATES ERROR: NO AVAILABLE STATES IN SEED"<<endreq;
    return StatusCode::FAILURE;
  }
  
  LHCb::State* muState = (LHCb::State*)&(seed.closestState(9450.));
  
  if (m_mySeedState!=0) {  
    debug()<<"m_mySeedState"<<endreq;
    debug()<<m_mySeedState<<endreq;
    debug()<<"x="<<m_mySeedState->x()<<",y="<<m_mySeedState->y()<<",z="<<m_mySeedState->z()<<endreq;
  }
  debug()<<"muState"<<endreq;
  debug()<<muState<<endreq;
  debug()<<"x="<<muState->x()<<",y="<<muState->y()<<",z="<<muState->z()<<endreq;

  if (m_mySeedState==muState) return StatusCode::SUCCESS;
  else m_mySeedState=muState;
  
  
  m_states.clear();
  debug()<< " seed state"<< *muState;
  
  //propagate state to each station
  //for control purpose, also check is extrapolation was succesful for every station
  int ist=0;
  for (std::vector<double>::const_iterator it = m_zstations.begin();
       it != m_zstations.end(); ++it) {
    double z = *it;
    //const LHCb::State& muState= seed.firstState();
    // LHCb::State* muStateC = muState.clone();

    LHCb::State muStateC(*muState);
    debug()<< " seed first state clone"<< muStateC;
    StatusCode code= m_extrapolator->propagate(muStateC,z);
    debug() << " makeState: state at " << z << " = " << muStateC << endreq;
    if (code.isFailure()) 
      {
        debug()<<"EXTRAPOLATION OF STATES FAILED AT STATION"<<ist<<endreq;
        return StatusCode::FAILURE;
      }
    m_states.push_back(muStateC);
    ist++;
  }
  debug() << " makeStates: number of states " << m_states.size() << endreq;
  return StatusCode::SUCCESS;
  
}


//find hits in n sigmas and add them to track
void Chi2MuIDTool::addLHCbIDsToMuTrack(LHCb::Track& muTrack,double mom) 
{

  int j=0;
  //from muonProvider, get idsInRange if any
  for (int i = 1; i < 5; ++i) {
    //if (m_muonProviderInRange[i].size() == 0 ) continue;
    debug()<<"m_nsigmasUsed="<<m_nsigmasUsed<<endreq;
    debug()<<"Z of states="<<m_states[i].position().z()<<endreq;
    std::vector<LHCb::LHCbID> ids = 
      m_muonProvider->idsInRange(m_states[i],m_nsigmasUsed,m_discrval);
    
    debug()<<"ids size="<<ids.size()<<endreq;

    if (ids.size()>0) 
    { 
      int ist=ids[0].muonID().station();
      debug()<<"ids0 station="<<ist<<endreq;
      debug()<<"ids0 channelID="<<ids[0].channelID()<<endreq;
      // if mom below m_2hits, check if hit is in stations 4 or 5
      if ((mom<m_2hits) && (ist==3 || ist==4) && m_arrivalCuts) 
      {
        continue;
        debug()<<"skiped spurius hit in station"<<ist<<endreq;
      }
      
      // if mom below m_2hits, check if hit is in station 5
      if ((mom<m_3hits) && (ist==4) && m_arrivalCuts)
      {
        continue;
        debug()<<"skiped spurius hit in station"<<ist<<endreq;
      }
      muTrack.addToLhcbIDs(ids[0]);
      j += 1;
    }
    
  }
  debug() << "Number of LHCbIDs added =" << j
         << endreq;
}


//build states and search for hits. With this, calculate isMuon
StatusCode Chi2MuIDTool::search(const LHCb::Track& seed, LHCb::Track& muTrack) {
  
  StatusCode sc;
  StatusCode sc1=makeStates(seed);
  if (sc1.isFailure())
  {
    sc.setCode(202);
    return sc;
  }
  

  //get states and hits
  debug()<<"get states and hits"<<endreq;
  LHCb::State lstate = seed.closestState(9450.);
  debug()<<"closest state found"<<endreq;
  muTrack.addToStates(lstate);
  debug()<<"x="<<lstate.x()<<", y="<<lstate.y()<<", z="<<lstate.z()<<endreq;

  addLHCbIDsToMuTrack(muTrack,seed.p());
  // apply isMuon
  if (!m_applyIsMuon){
    if (muTrack.lhcbIDs().size()>0) return StatusCode::SUCCESS;
    else 
    {
      sc.setCode(204);
      return sc;
    }
  }
  
  
  bool ism = m_IsMuonTool->IsMuonCandidate(muTrack);
  if (!ism)
  {
    sc.setCode(203);
    return sc;
  }
  
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
Chi2MuIDTool::~Chi2MuIDTool() {} 

//=============================================================================

