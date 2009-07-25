// $Id: Chi2MuIDTool.cpp,v 1.7 2009-07-25 00:43:53 polye Exp $
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
  StatusCode sc = StatusCode::SUCCESS;

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

  return sc;
}



  
StatusCode Chi2MuIDTool::isGoodSeed(const LHCb::Track& seed)
{
  
  StatusCode sc = StatusCode::SUCCESS;
  //track below min momentum
  if (seed.p()<m_minmom)
  {
    if ( msgLevel(MSG::DEBUG)) debug() << "Track mom below momentum threshold of "<<m_minmom<<endmsg;
    sc.setCode(101); return sc;
  }
  
  //track not in acceptance (standard definition)
  if (!(isTrackInAcceptance(seed)))
  {
    if (msgLevel(MSG::DEBUG) ) debug()<< "Track not in MuonChambers acceptance"<<endmsg;
    sc.setCode(102); return sc;
  }
  return sc;
}


StatusCode Chi2MuIDTool::muonCandidate(const LHCb::Track& seed, 
                                       LHCb::Track& muTrack,
                                       bool isMuonCandidate,
                                       const std::vector<LHCb::LHCbID> ids_init)
{
  StatusCode sc = StatusCode::SUCCESS;
  m_arrivalCuts=true;
  m_applyIsMuon=true;
  if (msgLevel(MSG::DEBUG) ) debug()<<"ids_init.size()="<<ids_init.size()<<endmsg;
  bool del_muonprov=false;

  if (ids_init.size()==0)
  {
    // access muon hits info from TES
    m_muonProvider = get<SmartMuonMeasProvider>("Rec/Muon/SmartMuonMeasProvider");
    m_nsigmasUsed=m_nsigmas;
    if (m_muonProvider==NULL) 
    {
      sc.setCode(201);
      return Error("SmartMuonMeasProvider not in TES, please load",sc);
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

    if (msgLevel(MSG::DEBUG) ) debug()<<"READY TO CHARGE IDS onto m_muonProvider"<<endmsg;
    std::vector<LHCb::LHCbID>::const_iterator it;
    
    m_muonProvider = new SmartMuonMeasProvider();
    
    for(it = ids_init.begin(); it != ids_init.end(); ++it ) 
    {
      LHCb::LHCbID id=*it;
      //bug, some non muon chambers IDs?
      //if (!id.isMuon()) continue;
      
      int istation=id.muonID().station();

      //from LHCbID, get xmeas and ymeas
      if (msgLevel(MSG::DEBUG) ) {
	debug()<<"istation="<<istation<<endmsg;
	debug()<<"channelID="<<id.channelID()<<endmsg;
      }
      if (!id.isMuon()) {
        Warning("NON MUON ID!").ignore();
        continue;
      }
      
      LHCb::Measurement* xmeas = m_measProvider->measurement(id,0);
      LHCb::Measurement* ymeas = m_measProvider->measurement(id,1);
      //debug()<<"xmeas="<<(*xmeas)<<endmsg;
      //debug()<<"ymeas="<<(*ymeas)<<endmsg;
      if (msgLevel(MSG::DEBUG) ) {
	debug()<<"channelID after measprovider="<<xmeas->lhcbID().channelID()<<endmsg;
	debug()<<"xmeas before smmp="<<xmeas<<endmsg;
      }

      if (msgLevel(MSG::DEBUG) ) debug()<<"m_muonProvider->atStation("<<istation
                                        <<").size()="<<m_muonProvider->atStation(istation).size()<<endmsg;
      //for each station, fill muprovider with corresponding MeasPair
      m_muonProvider->atStation(istation).push_back( MeasPair(xmeas,ymeas) );
      m_nsigmasUsed=50;
      if (msgLevel(MSG::DEBUG) ) debug()<<"ID correctly loaded into m_muonProvider"<<endmsg;
    }
    if (msgLevel(MSG::DEBUG) ) debug()<<"ID correctly loaded into m_muonProvider"<<endmsg;
  }
  
  // get hits in FOI (nsigmas). Run IsMuon
  sc = search(seed, muTrack);
  if (msgLevel(MSG::DEBUG) ) debug()<<"muTrack with seeds on it"<<endmsg;
  if (sc.isFailure()) 
  {
    if (msgLevel(MSG::DEBUG) ) debug()<< "No new track built"<<endmsg;
    if (sc.getCode()==203) isMuonCandidate=false;
    else if ((sc.getCode()!=204) && (m_applyIsMuon))  warning()<< "No available info about IsMuon"<<endmsg;
    if (del_muonprov) delete m_muonProvider;
    return sc;
  }
  else isMuonCandidate=true;

  //add seed track to muon track ancestors
  muTrack.addToAncestors(seed) ;

  if (del_muonprov) delete m_muonProvider;
  return sc;
}



StatusCode Chi2MuIDTool::muonQuality(LHCb::Track& muTrack, double& Quality)
{

  Quality=0;
  StatusCode sc = StatusCode::SUCCESS;
  
  if (muTrack.states().size()>1) 
    warning()<<"muonQuality:: MUTRACK WITH MORE THAN ONE SEED STATE ON IT"<<endmsg;
  else if (muTrack.states().size()<1) 
  {
    sc.setCode(301);
    return Error("muonQuality:: MUTRACK WITHOUT ANY SEED STATE ON IT",sc);
  }
  
  if (muTrack.lhcbIDs().size()<1) 
  {
    sc.setCode(302);
    return Error("muonQuality:: NO LHCbIDs ON TRACK!",sc);

  }
  
  // load these measurements into a track and perform fit
  // m_measProvider->load(muTrack);  
  StatusCode sc2 = m_fitter->fit(muTrack);

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"BACK in muonQuality"<<endmsg;
    debug()<<"sc2"<<sc2.getCode()<<endmsg;
  }
  debug()<<"sc2.isFailure()"<<sc2.isFailure()<<endmsg;
  
  if (sc2.isFailure())
  {
    sc.setCode(303);
    return Error("WRONG FIT PERFORMED",sc);
  }
  
  
  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"muTrack="<<muTrack<<endmsg;
    debug()<<"ids size="<<muTrack.lhcbIDs().size()<<endmsg;
    debug()<<"chi2="<<muTrack.chi2()<<endmsg;
  }
  Quality=muTrack.chi2()/(2.*muTrack.lhcbIDs().size());

  return sc;
}



StatusCode Chi2MuIDTool::muonArrival(LHCb::Track& muTrack, double& Arrival)
{
  //compute arrival probability
  Arrival = 0;
  StatusCode sc=m_GetArrival->getArrivalFromTrack(muTrack,Arrival);
  if (msgLevel(MSG::DEBUG) ) debug()<<"Arrival="<<Arrival<<endmsg;
  if (sc.isFailure()) Arrival=0;
  return sc;
}



StatusCode Chi2MuIDTool::muonDLL(LHCb::Track& muTrack, const double& Quality, double& CLQuality, 
                                 const double& Arrival, double& CLArrival, double& DLL)
  
{
  StatusCode sc = StatusCode::SUCCESS;
  //get clquality from cltool
  
  double qualityCLs=0;
  double qualityCLb=0;
  double qualityCLR=0;

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"Arrival="<<Arrival<<endmsg;
    debug()<<"Qualty ="<<Quality<<endmsg;
  }

  
  sc=m_CLQuality->cl(Quality,qualityCLs,qualityCLb,qualityCLR,muTrack.p());
  if (sc.isFailure())
  {    
    if (msgLevel(MSG::DEBUG) ) debug()<<"WRONG CLQUALITY VALUE"<<endmsg;
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
    CLArrival=0.0;
    Warning("WRONG CLARRIVAL VALUE");
  }
  if (CLArrival==0.0) CLArrival=1e-6;
  
  DLL = log(CLQuality*CLArrival);
  
  return sc;
}

StatusCode Chi2MuIDTool::muonID(const LHCb::Track& seed,LHCb::Track& muTrack, 
                                bool isMuonCandidate,
                                double& Quality, double& CLQuality, 
                                double& Arrival, double& CLArrival, 
                                double& DLL,
                                const std::vector<LHCb::LHCbID> ids_init)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  sc=isGoodSeed(seed);
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG) ) debug()<<"Chi2MuIDTool: Fail good seed, sc="<<sc.getCode()<<endmsg;
    return sc;
  }
  
  sc=muonCandidate(seed,muTrack,isMuonCandidate,ids_init);
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG) ) debug()<<"Chi2MuIDTool: Fail muon candidate, sc="<<sc.getCode()<<endmsg;
    return sc;
  }
  
  sc=muonQuality(muTrack,Quality);
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG) ) debug()<<"Chi2MuIDTool: Fail muon quality, sc="<<sc.getCode()<<endmsg;
    return sc;
  }
  
  sc=muonArrival(muTrack, Arrival);
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG) ) debug()<<"Chi2MuIDTool: Fail muon arrival, sc="<<sc.getCode()<<endmsg;
    return sc;
  }
  
  sc=muonDLL(muTrack, Quality, CLQuality, Arrival, CLArrival, DLL);
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG) ) debug()<<"Chi2MuIDTool: Fail muon dll, sc="<<sc.getCode()<<endmsg;
    return sc;
  } 
  return sc;
}


 
//for acceptance: is track extrapolation inside istation?
bool Chi2MuIDTool::isTrackInsideStation(const int& istation)
{ 
  
  const LHCb::State& muStateC = m_states[istation];
  
  //get conditions: is track inside station?
  const Gaudi::XYZPoint& pos = muStateC.position();
  const bool xcond=((fabs(pos.x()) > m_mudet->getInnerX(istation)) && (fabs(pos.x())<m_mudet->getOuterX(istation)));
  const bool ycond=((fabs(pos.y()) > m_mudet->getInnerY(istation)) && (fabs(pos.y())<m_mudet->getOuterY(istation)));
  
  if (msgLevel(MSG::DEBUG) ) debug()<<"st="<<istation<<",xcond="<<xcond<<",ycond="<<ycond<<endmsg;
  
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

  StatusCode sc = StatusCode::SUCCESS;

  const std::vector<LHCb::State*> & muStates = seed.states();
  if (muStates.size()==0) {
    sc.setCode(200);
    return Error("MAKE STATES ERROR: NO AVAILABLE STATES IN SEED");
  }
  
  LHCb::State* muState = (LHCb::State*)&(seed.closestState(9450.));
  
  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"muState"<<endmsg;
    debug()<<muState<<endmsg;
    debug()<<"x="<<muState->x()<<",y="<<muState->y()<<",z="<<muState->z()<<endmsg;
  }

  if (m_mySeedState==muState) return sc;
  else m_mySeedState=muState;
  
  
  m_states.clear();
  if (msgLevel(MSG::DEBUG) ) debug()<< " seed state"<< *muState;
  
  //propagate state to each station
  //for control purpose, also check is extrapolation was succesful for every station
  int ist=0;
  for (std::vector<double>::const_iterator it = m_zstations.begin();
       it != m_zstations.end(); ++it) {
    double z = *it;
    //const LHCb::State& muState= seed.firstState();
    // LHCb::State* muStateC = muState.clone();

    LHCb::State muStateC(*muState);
    if (msgLevel(MSG::DEBUG) ) debug()<< " seed first state clone"<< muStateC;
    StatusCode code= m_extrapolator->propagate(muStateC,z);
    if (msgLevel(MSG::DEBUG) ) debug() << " makeState: state at " << z << " = " << muStateC << endmsg;
    if (code.isFailure()) 
      {
        return Error("EXTRAPOLATION OF STATES FAILED AT STATION",sc);
      }
    m_states.push_back(muStateC);
    ist++;
  }
  if (msgLevel(MSG::DEBUG) ) debug() << " makeStates: number of states " << m_states.size() << endmsg;
  return sc;
  
}


//find hits in n sigmas and add them to track
void Chi2MuIDTool::addLHCbIDsToMuTrack(LHCb::Track& muTrack,double mom) 
{

  int j=0;
  //from muonProvider, get idsInRange if any
  for (int i = 1; i < 5; ++i) {
    //if (m_muonProviderInRange[i].size() == 0 ) continue;
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"m_nsigmasUsed="<<m_nsigmasUsed<<endmsg;
      debug()<<"Z of states="<<m_states[i].position().z()<<endmsg;
    }
    std::vector<LHCb::LHCbID> ids = 
      m_muonProvider->idsInRange(m_states[i],m_nsigmasUsed,m_discrval);
    
    if (msgLevel(MSG::DEBUG) ) debug()<<"ids size="<<ids.size()<<endmsg;

    if (ids.size()>0) 
    { 
      int ist=ids[0].muonID().station();
      if (msgLevel(MSG::DEBUG) ) {
	debug()<<"ids0 station="<<ist<<endmsg;
        debug()<<"ids0 channelID="<<ids[0].channelID()<<endmsg;
      }
      // if mom below m_2hits, check if hit is in stations 4 or 5
      if ((mom<m_2hits) && (ist==3 || ist==4) && m_arrivalCuts) 
      {
        continue;
        if (msgLevel(MSG::DEBUG) ) debug()<<"skiped spurius hit in station"<<ist<<endmsg;
      }
      
      // if mom below m_2hits, check if hit is in station 5
      if ((mom<m_3hits) && (ist==4) && m_arrivalCuts)
      {
        continue;
        if (msgLevel(MSG::DEBUG) ) debug()<<"skiped spurius hit in station"<<ist<<endmsg;
      }
      muTrack.addToLhcbIDs(ids[0]);
      j += 1;
    }
    
  }
  if (msgLevel(MSG::DEBUG) ) debug() << "Number of LHCbIDs added =" << j
         << endmsg;
}


//build states and search for hits. With this, calculate isMuon
StatusCode Chi2MuIDTool::search(const LHCb::Track& seed, LHCb::Track& muTrack) {
  
  StatusCode sc = StatusCode::SUCCESS;
  StatusCode sc1=makeStates(seed);
  if (sc1.isFailure())
  {
    sc.setCode(202);
    return Error("search: make states",sc);
  }
  

  //get states and hits
  if (msgLevel(MSG::DEBUG) ) debug()<<"get states and hits"<<endmsg;
  LHCb::State lstate = seed.closestState(9450.);
  if (msgLevel(MSG::DEBUG) ) debug()<<"closest state found"<<endmsg;
  muTrack.addToStates(lstate);
  if (msgLevel(MSG::DEBUG) ) debug()<<"x="<<lstate.x()<<", y="<<lstate.y()<<", z="<<lstate.z()<<endmsg;

  addLHCbIDsToMuTrack(muTrack,seed.p());
  // apply isMuon
  if (!m_applyIsMuon){
    if (muTrack.lhcbIDs().size()>0) return sc;
    else 
    {
      sc.setCode(204); return sc;
    }
  }
  
  bool ism = m_IsMuonTool->IsMuonCandidate(muTrack);
  if (!ism)
  {
    sc.setCode(203); return sc;
  }
  
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
Chi2MuIDTool::~Chi2MuIDTool() {} 

//============================================================================

StatusCode Chi2MuIDTool::finalize() {

  StatusCode sc = m_CLQuality->finalize();
  if (sc.isFailure()) return sc;

  sc = m_GetArrival->finalize();
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
  
}

