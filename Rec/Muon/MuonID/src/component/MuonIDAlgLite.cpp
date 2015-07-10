/** Implementation of MuonIDAlgLite.
 *
 * 2015-06-01: Kevin Dungs, Ricardo Vazquez Gomez
 */
#include "MuonIDAlgLite.h"

DECLARE_ALGORITHM_FACTORY(MuonIDAlgLite)

/** C'tor that exposes properties via declareProperty.
 */
MuonIDAlgLite::MuonIDAlgLite(const std::string &name, ISvcLocator *pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator) {
  declareProperty("TracksLocation",
                  tesPathInputTracks_ = LHCb::TrackLocation::Default);
  declareProperty("MuonIDLocation",
                  tesPathOutputMuonPid_ = LHCb::MuonPIDLocation::Default);
  declareProperty("MuonTrackLocation",
                  tesPathOutputMuonTracks_ = LHCb::TrackLocation::Muon);
  declareProperty("useTTrack", useTTrack_ = false);

  // TODO: This has to go
  //--------------------
  // flag to use DLL:
  //--------------------
  // 4 -- binned tanh(distance) with closest hit(Muon) + integral of Landau
  // fit(NonMuon).
  // 5 -- binned tanh(distance) with closest hit(Muon) + integral. New tunning
  // with runI data
  //-----------------------------
  declareProperty("DLL_flag", m_dllFlag = 4);
}

/** Load the CommonMuonTool.
 */
StatusCode MuonIDAlgLite::initialize() {
  const auto sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  muonTool_ = tool<ICommonMuonTool>("CommonMuonTool");
  DLLTool_ = tool<DLLMuonTool>("DLLMuonTool");
  makeMuonTool_ = tool<MakeMuonTool>("MakeMuonTool");
  return StatusCode::SUCCESS;
}

/** Iterates over all tracks in the current event and performs muon id on them.
 * Resulting PID objects as well as muon tracks are stored on the TES.
 */
StatusCode MuonIDAlgLite::execute() {
  const auto tracksPtr = get<LHCb::Tracks>(tesPathInputTracks_);
  if (tracksPtr == nullptr) {
    // TODO: What is the right thing to do here? Error or Warning and return
    // "Success"?
    return Error("Got nullptr from TES.");
  }
  const auto& tracks = *tracksPtr;

  // Acquire TES containers for storing muon PIDs and tracks
  //const auto muPidsPtr = 
  //    getOrCreate<LHCb::MuonPIDs, LHCb::MuonPIDs>(tesPathOutputMuonPid_);
  LHCb::MuonPIDs *muPids =
      getOrCreate<LHCb::MuonPIDs, LHCb::MuonPIDs>(tesPathOutputMuonPid_);
  if (!muPids->empty()) {
    muPids->clear();
    verbose() << "MuonPIDs already existed at '" << tesPathOutputMuonPid_
              << "'. Container has been cleared and its contents will be "
                 "replaced.";
  }
  //const auto &muPids = *muPidsPtr;
  
  LHCb::Tracks *muTracks =
      getOrCreate<LHCb::Tracks, LHCb::Tracks>(tesPathOutputMuonTracks_);
  if (!muTracks->empty()) {
    muTracks->clear();
    verbose()
        << "Muon tracks already existed at '" << tesPathOutputMuonTracks_
        << "'. Container has been cleared and its contents will be replaced.";
  }


  muPids->setVersion(1);  // TODO: still valid? do we need a new version?

  counter("nMomentumCut");
  counter("nPreSelTrack");
  counter("nGoodOffline");
  counter("nMuonPIDs");
  counter("nInAcceptance");
  counter("nIsMuon");
  counter("nIsMuonLoose");
  counter("nIsMuonTight");
  // Iterate over all tracks and do the offline identification for each of them
  for (const auto trackPtr : tracks) {
    const auto &track = *trackPtr;
    if (!isGoodOfflineTrack(track)) {
      continue;
    }
    counter("nGoodOffline")++;
    auto muPid =
        new LHCb::MuonPID;  // TODO: Put to where the values are set after the
                            // muonMap thing has been figured out...
    muPid->setIDTrack(trackPtr);
    muPid->setIsMuon(0);
    muPid->setIsMuonLoose(0);
    muPid->setIsMuonTight(0);
    muPid->setNShared(0);
    muPid->setPreSelMomentum(0);
    muPid->setInAcceptance(0);
    muPid->setMuonLLMu(-10000.);
    muPid->setMuonLLBg(-10000.);

    double probMu = 0.00000000001, probNonMu = 0.00000000001;
    double Dsquare = -1;

    bool preSel = muonTool_->preSelection(track);
    const auto extrapolation = muonTool_->extrapolateTrack(track);
    bool inAcc = muonTool_->inAcceptance(extrapolation); 
    if (preSel) {
      muPid->setPreSelMomentum(1);
      counter("nMomentumCut")++;
    }
    if (inAcc) {
      muPid->setInAcceptance(1);
      counter("nInAcceptance")++;
    }
    
    // Store the muonPIDs for tracks passing InAcceptance
    if (!inAcc) {
      delete muPid;
      continue;
    }
    CommonConstMuonHits hits, hitsTight;
    if (preSel) {
      counter("nPreSelTrack")++;

      ICommonMuonTool::MuonTrackOccupancies occupancies, occupanciesTight;
      std::tie(hits, occupancies) =
         muonTool_->hitsAndOccupancies(track, extrapolation);
      std::tie(hitsTight, occupanciesTight) = muonTool_->extractCrossed(hits);
      const auto isMuon = muonTool_->isMuon(occupancies, track.p());
      const auto isMuonTight = muonTool_->isMuon(occupanciesTight, track.p());
      const auto isMuonLoose = muonTool_->isMuonLoose(occupancies, track.p());
      muPid->setIsMuon(isMuon);
      muPid->setIsMuonLoose(isMuonLoose);
      muPid->setIsMuonTight(isMuonTight);
      if (isMuon) {
        counter("nIsMuon")++;
      }
      if (isMuonTight) {
        counter("nIsMuonTight")++;
      }
      if (isMuonLoose) {
        counter("nIsMuonLoose")++;

        // TODO: This has to go
        if (m_dllFlag == 4) {
          std::tie(probMu, probNonMu, Dsquare) = DLLTool_->calcMuonLL_tanhdist_landau(
              track, extrapolation, hits, occupancies);
        } else if (m_dllFlag == 5) {
          std::tie(probMu, probNonMu, Dsquare) = DLLTool_->calcMuonLL_tanhdist(
              track, extrapolation, hits, occupancies);
        } else {
          Error("Invalid DLL flag");  // TODO: seriously this is just a quick fix,
          // this whole flag thing has to go
        }
        
        muPid->setMuonLLMu(log(probMu));
        muPid->setMuonLLBg(log(probNonMu));

        DLLTool_->calcNShared(muPid, muPids, hits, extrapolation);
      }
    }
    
    muPids->insert(muPid, track.key());  // Necessary, as the insertion sets
                                         // the key which is then used in
                                         // making the track...
    counter("nMuonPIDs")++; 
    
    // make the muon track only for tracks passing IsMuonLoose
    if(muPid->IsMuonLoose()){
      auto muTrack = makeMuonTool_->makeMuonTrack(muPid, hits, extrapolation);
      
      // Insert in muonTrack
      muTrack->setType(LHCb::Track::Muon);
      muTrack->setHistory(LHCb::Track::MuonID);
      muTrack->addInfo(LHCb::Track::MuonMomentumPreSel, muPid->PreSelMomentum());
      muTrack->addInfo(LHCb::Track::MuonInAcceptance, muPid->InAcceptance());
      muTrack->addInfo(LHCb::Track::IsMuonLoose, muPid->IsMuonLoose());
      muTrack->addInfo(LHCb::Track::IsMuon, muPid->IsMuon());
      muTrack->addInfo(LHCb::Track::IsMuonTight, muPid->IsMuonTight()); 
      muTrack->addInfo(LHCb::Track::MuonDist2, Dsquare);
      muTrack->addInfo(LHCb::Track::MuonDLL, muPid->MuonLLMu() - muPid->MuonLLBg());
      muTrack->addInfo(LHCb::Track::MuonNShared, muPid->nShared());
      muTrack->addInfo(LHCb::Track::MuonChi2perDoF, muTrack->chi2PerDoF());
      muPid->setMuonTrack(muTrack);
      muTracks->insert(muTrack, track.key());
    }
  }
  
  // Clear the tool maps
  DLLTool_->clearMap();

  return StatusCode::SUCCESS;
}

/** Tear down.
 */
StatusCode MuonIDAlgLite::finalize() { 
  return GaudiAlgorithm::finalize(); 
}

/** Offline requirement for a good track. The track mustn't be a clone. Also it
 * should either be a long track or a downstream track or in case TT tracks are
 * supposed to be used a TT track.
 */
bool MuonIDAlgLite::isGoodOfflineTrack(const LHCb::Track &track) const {
  return !track.checkFlag(LHCb::Track::Clone) &&
         (track.checkType(LHCb::Track::Long) ||
          track.checkType(LHCb::Track::Downstream) ||
          (useTTrack_ && track.checkType(LHCb::Track::Ttrack)));
}
