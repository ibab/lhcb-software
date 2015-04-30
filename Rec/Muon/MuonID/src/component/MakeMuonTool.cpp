// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MakeMuonTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MakeMuonTool
//
// 2015-04-15 : Ricardo Vazquez Gomez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MakeMuonTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MakeMuonTool::MakeMuonTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_nsigmasUsed(0.)
  , m_NStation(0)
  , m_arrivalCuts(false)
  , m_applyIsMuon(false)
{
  declareInterface<MakeMuonTool>(this);

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
  
  //Min momentum to accept hits in stations 3 and 4
  declareProperty("MinMomSt3",m_2hits = 3500);

  //Min momentum to accept hits in station 4
  declareProperty("MinMomSt4",m_3hits = 4500);   

  //Use chi2cut when selecting candidates?
  //declareProperty("Chi2Cut",m_chi2cut=0.);

}

StatusCode MakeMuonTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_fitter = tool<ITrackFitter>("TrackMasterFitter","fitter",this); 
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","extrapol",this);
  m_measProvider = tool<IMeasurementProvider>("MuonMeasurementProvider",this);  
  m_mySeedState=0;

  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();

  m_isM1defined = basegeometry.retrieveM1status();

  m_mudet = getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  //Find z of mu stations
  for (int i=0;i<m_NStation;i++) 
  {
    m_zstations.push_back(m_mudet->getStationZ(i));
    //m_muonProviderInRange.push_back(MeasPairList());
  }
  
  // Build CLquality and GetArrival tools
  m_IsMuonTool = tool<IIsMuonCandidateC>("IsMuonCandidateC","IsMuonCandidateC",this);
  
  // Find inside which stations must be the track to be in acceptance
  m_stations_acceptance.push_back(0);
  m_stations_acceptance.push_back(4);

  if ( msgLevel(MSG::DEBUG)){
    debug()<<"INITIAL VALUES:"<<endmsg;
    debug()<<"NSigmas="<<m_nsigmas<<endmsg;
    debug()<<"DiscrValue="<<m_discrval<<endmsg;
    debug()<<"MinMomSt3="<<m_2hits<<endmsg;
    debug()<<"MinMomSt4="<<m_3hits<<endmsg;
    //debug()<<"Chi2Cut="<<m_chi2cut<<endmsg;
    debug()<<"M1 exists ? "<<m_isM1defined<<endmsg;
  }
  
  return sc;
}

StatusCode MakeMuonTool::muonCandidate(const LHCb::Track& seed, 
                                       LHCb::Track& muTrack,
                                       const std::vector<LHCb::LHCbID> ids_init)
{
  StatusCode sc = StatusCode::SUCCESS;
  m_arrivalCuts=true;
  m_applyIsMuon=true;
  if (msgLevel(MSG::DEBUG) ) debug()<<"ids_init.size()="<<ids_init.size()<<endmsg;
  bool del_muonprov=false;

  if (ids_init.size()==0)
  {
    if ( !exist<SmartMuonMeasProvider>("Rec/Muon/SmartMuonMeasProvider") )
    {
      sc.setCode(201);
      return Error("SmartMuonMeasProvider not in TES, please load",sc);
    }
    // access muon hits info from TES
    m_muonProvider = get<SmartMuonMeasProvider>("Rec/Muon/SmartMuonMeasProvider");
    m_nsigmasUsed=m_nsigmas;
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
  
  //add seed track to muon track ancestors
  muTrack.addToAncestors(seed);
  
  if (msgLevel(MSG::DEBUG) ) debug()<<"muTrack with seeds on it"<<endmsg;
  if (sc.isFailure())
  {
    if (msgLevel(MSG::DEBUG) ) debug()<< "No new track built"<<endmsg;
    if (sc.getCode()!=203 && m_applyIsMuon)  
      Warning( "No available info about IsMuon",StatusCode::SUCCESS).ignore();
    if (del_muonprov) delete m_muonProvider;
    return sc;
  }

/*
  // if chi2cut required, perform fit with muonquality
  if (m_chi2cut>0.){
    if (msgLevel(MSG::DEBUG) ) debug()<< "Applying chi2cut"<<endmsg;
    //create copy of the track
    LHCb::Track muTrack_par;
    muTrack_par.copy(muTrack);
    double myquality;

    sc = muonQuality(muTrack_par, myquality);
    if (sc.isFailure())
    {
      if (msgLevel(MSG::DEBUG) ) debug()<< "muonCandidate: fit failed"<<endmsg;
      if (del_muonprov) delete m_muonProvider;
      sc.setCode(203);
      return sc;
    }
    else{
      if (msgLevel(MSG::DEBUG) ) debug()<< "muonCandidate: myquality="<<myquality<<endmsg;
      if (myquality>=m_chi2cut) {
        sc.setCode(203);
        return sc;
      }
      
    }
  }
*/

  if (del_muonprov) delete m_muonProvider;
  return sc;
}


StatusCode MakeMuonTool::muonQuality(LHCb::Track& muTrack, double& Quality)
{

  Quality=0;
  StatusCode sc = StatusCode::SUCCESS;
  
  if (muTrack.states().size()>1) 
    Warning("muonQuality:: MUTRACK WITH MORE THAN ONE SEED STATE ON IT",StatusCode::SUCCESS).ignore();
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
  if(msgLevel(MSG::DEBUG)) debug() << "This is the track before the fit =" << muTrack << endmsg;
  StatusCode sc2 = m_fitter->fit(muTrack);

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"BACK in muonQuality"<<endmsg;
    debug()<<"sc2"<<sc2.getCode()<<endmsg;
    debug()<<"sc2.isFailure()"<<sc2.isFailure()<<endmsg;
  }
  
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

// Function makeStates: build extrapolated states for each station from seed
StatusCode MakeMuonTool::makeStates(const LHCb::Track& seed){

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

  //to be refined: even if the pointer is the same, it may happen that is is 
  //pointing really to different states (from the previous event), 
  //so the extrapolation would not be the correct. To avoid that, extrapolate 
  //to one station and check if the momentum coincides  
  if (m_mySeedState==muState) {
    if (msgLevel(MSG::DEBUG)) debug()<<"m_mySeedState and muState same pointer. Checking first state"<<endmsg;
    int i=0;
    bool cond=true;
    for (std::vector<double>::const_iterator it = m_zstations.begin();
         it != m_zstations.end(); ++it) {
      double z = *it;
      LHCb::State muStateC(*muState);
      StatusCode code = m_extrapolator->propagate(muStateC,z);
      if (code.isSuccess())
      {
        if (muStateC.p() != m_states[i].p()) cond=false;
        if (msgLevel(MSG::DEBUG) ) debug()<<"muStateC.p()="<<muStateC.p()
                                          << ",m_states["<<i<<"].p()="
                                          <<m_states[i].p()<<",cond="<<cond<<endmsg;
        
        break;
      }
      i++;
    }
    if (cond) return sc;    
  }

  else m_mySeedState=muState;
  m_states.clear();
  if (msgLevel(MSG::DEBUG) ) debug()<< " seed state = "<< *muState << endmsg;
  
  //propagate state to each station
  //for control purpose, also check is extrapolation was succesful for every station
  int ist=0;
  for (std::vector<double>::const_iterator it = m_zstations.begin();
       it != m_zstations.end(); ++it) {
    double z = *it;
    LHCb::State muStateC(*muState);
    if (msgLevel(MSG::DEBUG) ) debug()<< " seed first state clone"<< muStateC;
    StatusCode code= m_extrapolator->propagate(muStateC,z);
    if (msgLevel(MSG::DEBUG) ) debug() << " makeState: state at " << z << " = " << muStateC << endmsg;
    if (code.isFailure()){
      if (msgLevel(MSG::DEBUG) ) debug() << "EXTRAPOLATION OF STATES FAILED AT STATION "<< ist<< endmsg;
      return code;
    }
    m_states.push_back(muStateC);
    ist++;
  }
  if (msgLevel(MSG::DEBUG) ) debug() << " makeStates: number of states " << m_states.size() << endmsg;
  return sc;
  
}

//find hits in n sigmas and add them to track
void MakeMuonTool::addLHCbIDsToMuTrack(LHCb::Track& muTrack,double mom) 
{

  int j=0;

  int firstStation = (m_isM1defined) ? 1 : 0;

  for (int i = firstStation; i < m_NStation; ++i) {
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"m_nsigmasUsed="<<m_nsigmasUsed<<endmsg;
      debug()<<"Z of states="<<m_states[i].position().z()<<endmsg;
    }
    if (m_states[i].momentum().z()==0 ){
      Warning("idsInRange-> pz=0: Local slope won't be calculated and hit won't be used").ignore();
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
        if (msgLevel(MSG::DEBUG) ) debug()<<"skiped spurius hit in station"<<ist<<endmsg;
        continue;
      }
      
      // if mom below m_2hits, check if hit is in station 5
      if ((mom<m_3hits) && (ist==4) && m_arrivalCuts)
      {
        if (msgLevel(MSG::DEBUG) ) debug()<<"skiped spurius hit in station"<<ist<<endmsg;
        continue;
      }
      muTrack.addToLhcbIDs(ids[0]);
      j += 1;
    }
    
  }
  if (msgLevel(MSG::DEBUG) ) debug() << "Number of LHCbIDs added =" << j
         << endmsg;
}


//build states and search for hits. With this, calculate isMuon
StatusCode MakeMuonTool::search(const LHCb::Track& seed, LHCb::Track& muTrack) {
  
  StatusCode sc  = StatusCode::SUCCESS;
  StatusCode sc1 = makeStates(seed);
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
MakeMuonTool::~MakeMuonTool() {} 

//============================================================================

