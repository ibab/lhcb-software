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
DECLARE_TOOL_FACTORY(MakeMuonTool)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MakeMuonTool::MakeMuonTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : GaudiTool(type, name, parent),
      m_NStation(0){
  declareInterface<MakeMuonTool>(this);

  declareProperty("FindQuality", m_FindQuality = true);
}

StatusCode MakeMuonTool::initialize() {
  StatusCode sc = GaudiTool::initialize();  // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  muonTool_ = tool<ICommonMuonTool>("CommonMuonTool");

  m_fitter = tool<ITrackFitter>("TrackMasterFitter", "fitter", this);
  m_extrapolator =
    tool<ITrackExtrapolator>("TrackLinearExtrapolator", "extrapol", this);
    //      tool<ITrackExtrapolator>("TrackMasterExtrapolator", "extrapol", this);
    
  m_measProvider = tool<IMeasurementProvider>("MuonMeasurementProvider", this);
  m_mySeedState = 0;

  MuonBasicGeometry basegeometry(detSvc(), msgSvc());
  m_NStation = basegeometry.getStations();

  m_mudet = getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  // Find z of mu stations
  for (unsigned i = 0; i != m_NStation; ++i) {
    m_zstations.push_back(m_mudet->getStationZ(i));
  }

  return sc;
}

StatusCode MakeMuonTool::muonCandidate(
    const LHCb::Track& seed, LHCb::Track& muTrack,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation,
    const std::vector<LHCb::LHCbID>& ids_init) {
  StatusCode sc = StatusCode::SUCCESS;

  if (msgLevel(MSG::DEBUG))
    debug() << "ids_init.size()=" << ids_init.size() << endmsg;

  // get closest hit to the seed extrapolation
  StatusCode sc1 = StatusCode::SUCCESS; 
  StatusCode sc2 = makeStates(seed); 
  if (sc2.isFailure()) {
    sc1.setCode(202);
    return Error("search: make states", sc1);
  }

  // get states and hits
  if (msgLevel(MSG::DEBUG)) debug() << "get states and hits" << endmsg;
  LHCb::State lstate = seed.closestState(9450.);
  if (msgLevel(MSG::DEBUG)) debug() << "closest state found" << endmsg;
  muTrack.addToStates(lstate);
  if (msgLevel(MSG::DEBUG))
    debug() << "x=" << lstate.x() << ", y=" << lstate.y()
            << ", z=" << lstate.z() << endmsg;
  // TODO: the following does also the chi2 now.
  // it could be divided, but at present is faster like this.
  addLHCbIDsToMuTrack(muTrack, ids_init, extrapolation); 
  // apply a sort of isMuon
  if (muTrack.lhcbIDs().size() > 0)
    return sc1;
  else {
    sc1.setCode(204);
    return sc1;
  }
  // ============================================================
  // TODO: what is the point of what follows? 
  //       the code can never reach here. 

  // check to how many different stations the hits belong
  const std::vector<LHCb::LHCbID>& idsFromTrack=muTrack.lhcbIDs();
  std::vector<LHCb::LHCbID>::const_iterator id;
  std::vector<unsigned> stations;
  // TODO: all this is a paranoid check, there is no way it could have hits in only two stations here.
  bool inSt = false;
  for( const LHCb::LHCbID &id: idsFromTrack){
    if(!id.isMuon())  continue;
    for_each(std::begin(stations), std::end(stations), [&](unsigned station){
      unsigned myst = station;
      if (myst==id.muonID().station()) inSt = true;
    });
    inSt = false;
    if (!inSt) stations.push_back(id.muonID().station());
  }

  if(stations.size() < 2){ // at least two different stations
    sc1.setCode(203);
    return sc1;
  }
 
  // add seed track to muon track ancestors
  muTrack.addToAncestors(seed); 

  if (msgLevel(MSG::DEBUG)) debug() << "muTrack with seeds on it" << endmsg;
  if (sc.isFailure()) {
    if (msgLevel(MSG::DEBUG)) debug() << "No new track built" << endmsg;
    if (sc.getCode() != 203)
      Warning("No available info about IsMuon", StatusCode::SUCCESS).ignore();
    return sc;
  }

  return sc;
}

StatusCode MakeMuonTool::muonTrackFit(LHCb::Track& muTrack) {
  StatusCode sc = StatusCode::SUCCESS;

  if (muTrack.states().size() > 1)
    Warning("muonTrackFit:: MUTRACK WITH MORE THAN ONE SEED STATE ON IT",
            StatusCode::SUCCESS).ignore();
  else if (muTrack.states().size() < 1) {
    sc.setCode(301);
    return Error("muonTrackFit:: MUTRACK WITHOUT ANY SEED STATE ON IT", sc);
  }

  if (muTrack.lhcbIDs().size() < 1) {
    sc.setCode(302);
    return Error("muonTrackFit:: NO LHCbIDs ON TRACK!", sc);
  }

  if (msgLevel(MSG::DEBUG))
    debug() << "This is the track before the fit =" << muTrack << endmsg;
  
  StatusCode sc2 = m_fitter->fit(muTrack);

  if (sc2.isFailure()) {
    sc.setCode(303);
    return Error("WRONG FIT PERFORMED", sc);
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "BACK in muonTrackFit" << endmsg;
    debug() << "sc2" << sc2.getCode() << endmsg;
    debug() << "sc2.isFailure()" << sc2.isFailure() << endmsg;
    debug() << "muTrack=" << muTrack << endmsg;
    debug() << "ids size=" << muTrack.lhcbIDs().size() << endmsg;
    debug() << "chi2=" << muTrack.chi2() << endmsg;
  }
  return sc;
}

// Function makeStates: build extrapolated states for each station from seed
StatusCode MakeMuonTool::makeStates(const LHCb::Track& seed) {
  StatusCode sc = StatusCode::SUCCESS;

  const std::vector<LHCb::State*>& muStates = seed.states();
  if (muStates.size() == 0) {
    sc.setCode(200);
    return Error("MAKE STATES ERROR: NO AVAILABLE STATES IN SEED");
  }

  LHCb::State* muState = (LHCb::State*)&(seed.closestState(9450.));

  if(msgLevel(MSG::DEBUG)) {
    debug() << "muState" << endmsg;
    debug() << muState << endmsg;
    debug() << "x=" << muState->x() << ",y=" << muState->y()
            << ",z=" << muState->z() << endmsg;
  }
  
  // to be refined: even if the pointer is the same, it may happen that is is
  // pointing really to different states (from the previous event),
  // so the extrapolation would not be the correct. To avoid that, extrapolate
  // to one station and check if the momentum coincides
  if(m_mySeedState == muState) {
    if(msgLevel(MSG::DEBUG))
      debug() << "m_mySeedState and muState same pointer. Checking first state"
              << endmsg;
    unsigned i = 0;
    bool cond = true;
    for( double it: m_zstations){
      double z = it;
      LHCb::State muStateC(*muState);
      StatusCode code = m_extrapolator->propagate(muStateC, z);
      if(code.isSuccess()) {
        if(muStateC.p() != m_states[i].p()) cond = false;
        if(msgLevel(MSG::DEBUG))
          debug() << "muStateC.p()=" << muStateC.p() << ",m_states[" << i
                  << "].p()=" << m_states[i].p() << ",cond=" << cond << endmsg;
        break;
      }
      i++;
    }
    if(cond) return sc;
  }
  else m_mySeedState = muState;
  m_states.clear();
  if (msgLevel(MSG::DEBUG)) debug() << " seed state = " << *muState << endmsg;

  // propagate state to each station
  // for control purpose, also check is extrapolation was succesful for every
  // station
  unsigned ist = 0;
  for(double it: m_zstations){
    double z = it;
    LHCb::State muStateC(*muState);  //TODO, why are we copying at each step?
    if (msgLevel(MSG::DEBUG)) debug() << " seed first state clone" << muStateC;
    StatusCode code = m_extrapolator->propagate(muStateC, z);
    if (msgLevel(MSG::DEBUG))
      debug() << " makeState: state at " << z << " = " << muStateC << endmsg;
    if (code.isFailure()) {
      if (msgLevel(MSG::DEBUG))
        debug() << "EXTRAPOLATION OF STATES FAILED AT STATION " << ist
                << endmsg;
      return code;
    }
    m_states.push_back(muStateC);
    ist++;
  }
  if (msgLevel(MSG::DEBUG))
    debug() << " makeStates: number of states " << m_states.size() << endmsg;
  return sc;
}

// find closest hit to the seed and add them to track
void MakeMuonTool::addLHCbIDsToMuTrack(LHCb::Track& muTrack, const std::vector<LHCb::LHCbID>& ids_init,
                                       const ICommonMuonTool::MuonTrackExtrapolation& extrapolation) {
  
  double minDist[5] = {1e10, 1e10, 1e10, 1e10, 1e10};
  double distSeedHit[5] = {1e6, 1e6, 1e6, 1e6, 1e6};
  double chi2_contrib[5] = {0};
  double chi2 = 0;
  LHCb::LHCbID idToAdd[5];
  unsigned s;
  for_each(std::begin(ids_init), std::end(ids_init), [&](const LHCb::LHCbID &id){
    s = id.muonID().station();
    LHCb::Measurement* xmeas = m_measProvider->measurement(id,0);
    LHCb::Measurement* ymeas = m_measProvider->measurement(id,1);
    // compute the distance but the chi2 was used in the original code
    // need to think which is the metric that we want
    // Removed sqrt, not needed as all the meas are squared.
    distSeedHit[s] = pow((xmeas->measure() - extrapolation[s].first), 2) +
      pow((ymeas->measure() - extrapolation[s].second), 2);
    if(distSeedHit[s] < minDist[s]) {
      minDist[s] = distSeedHit[s]; 
      idToAdd[s] = id;
      chi2_contrib[s] = pow( (xmeas->measure() - extrapolation[s].first) / xmeas->errMeasure() , 2) +
        pow( (ymeas->measure() - extrapolation[s].second ) / ymeas->errMeasure() , 2); 
    }
  }); 

  unsigned idCounter = 0;
  for(unsigned i = 0; i != 5; ++i){
    if(idToAdd[i].isMuon()!=0){
      muTrack.addToLhcbIDs(idToAdd[i]);
      // Calculate chi2 assuming the error on the extrapolation is negligible 
      chi2 += chi2_contrib[i];
      idCounter+=1;
    }
  } 
  // TODO: this is a barbatrick to pass the chi2 value to the other track, it should be moved 
  muTrack.setChi2AndDoF(chi2,1);

  if (msgLevel(MSG::DEBUG))
    debug() << "Number of LHCbIDs added =" << idCounter << endmsg;
}

/**
 * Function to make the muon track. Modified from the original MuonIDAlg.cpp
*/
LHCb::Track* MakeMuonTool::makeMuonTrack(
    LHCb::MuonPID* mupid,
    CommonConstMuonHits &hits,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation){
  const LHCb::Track* mother = mupid->idTrack();
  LHCb::Track* mtrack;

  mtrack = new LHCb::Track(mupid->key());
  // add mother track to ancestors
  mtrack->addToAncestors(*mother);
  mtrack->addToStates(mother->closestState(9450.));

  std::vector<LHCb::LHCbID> ids_init;
  for_each(std::begin(hits), std::end(hits),
                [&](const CommonMuonHit* hit) {
                  LHCb::LHCbID id(hit->tile());
                  mtrack->addToLhcbIDs(id);
                  ids_init.push_back(id);
                });
  if (msgLevel(MSG::DEBUG)) debug() << "ids ready to get chi2" << endmsg;

  if (m_FindQuality) {
    //TODO: why we do not calculate the chi2 of the whole muon track 
    // instead of only of the closest hits? 

    // get chi2 value
    LHCb::Track mtrack_partial;
    if (!ids_init.empty()) {
      // add only the closest hit from ids_init 
      // to mtrack_partial
      StatusCode sc =
        muonCandidate(*mother, mtrack_partial, extrapolation, ids_init);
      if (!sc.isFailure()) {
        if (msgLevel(MSG::DEBUG)) {
          for_each(std::begin(mtrack_partial.lhcbIDs()), 
                std::end(mtrack_partial.lhcbIDs()), [&](const LHCb::LHCbID id){
            debug() << "id is muon? " << id.isMuon() << endmsg;
            if (id.isMuon()) {
              debug() << "id station  " << id.muonID().station() << endmsg;
              debug() << "id channelID=" << id.channelID() << endmsg;
            }
          });
        }
        double Quality = -1;
        Quality = mtrack_partial.chi2() / (2. * mtrack_partial.lhcbIDs().size());
        if (msgLevel(MSG::DEBUG))
          debug() << "\t Quality=" << Quality << endmsg;
        mtrack->setChi2PerDoF(Quality);

      }
    }
  }

  return mtrack;
} 



