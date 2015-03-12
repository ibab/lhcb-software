/** Implementation of MuonIDAlgLite.
 *
 * 2015-06-01: Kevin Dungs
 */
#include "MuonIDAlgLite.h"

#include <string>

#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "MuonID/ICommonMuonTool.h"
#include "DLLMuonTool.h"

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
  if (msgLevel(MSG::DEBUG)) debug() << "The tools have been loaded" << endmsg;
  return StatusCode::SUCCESS;
  StatusCode scNorm = DLLTool_->calcLandauNorm();
  if (scNorm.isFailure())
    return Error(" Normalizations of Landaus not properly set ", scNorm);
}

/** Iterates over all tracks in the current event and performs muon id on them.
 * Resulting PID objects as well as muon tracks are stored on the TES.
 */
StatusCode MuonIDAlgLite::execute() {
  if (msgLevel(MSG::DEBUG))
    debug() << "Start the algorithm execution " << endmsg;

  const LHCb::Tracks *tracks = getIfExists<LHCb::Tracks>(tesPathInputTracks_);
  /*
  if(!tracks->empty()){
    if(msgLevel(MSG::DEBUG)) debug() << "No tracks found in the event" <<
  endmsg;
    return StatusCode::FAILURE;
  }
  */
  // Acquire TES containers for storing muon PIDs and tracks
  LHCb::MuonPIDs *muPids =
      getOrCreate<LHCb::MuonPIDs, LHCb::MuonPIDs>(tesPathOutputMuonPid_);
  if (!muPids->empty()) {
    muPids->clear();
    verbose() << "MuonPIDs already existed at '" << tesPathOutputMuonPid_
              << "'. Container has been cleared and its contents will be "
                 "replaced.";
  }
  muPids->setVersion(1);  // TODO: still valid? do we need a new version?
  LHCb::Tracks *muTracks =
      getOrCreate<LHCb::Tracks, LHCb::Tracks>(tesPathOutputMuonTracks_);
  if (!muTracks->empty()) {
    muTracks->clear();
    verbose()
        << "Muon tracks already existed at '" << tesPathOutputMuonTracks_
        << "'. Container has been cleared and its contents will be replaced.";
  }

  counter("nPreSelTrack");
  counter("nExtrapolated");
  counter("nGoodOffline");
  counter("nMuonPIDs");
  counter("nIsAcceptance");
  counter("nIsMuon");
  counter("nIsMuonLoose");
  counter("nIsMuonTight");
  counter("nHitAndOcc");
  double ProbMu, ProbNonMu;
  // Iterate over all tracks and do the offline identification for each of them
  LHCb::Tracks::const_iterator iTrack;
  for (iTrack = tracks->begin(); iTrack != tracks->end(); iTrack++) {
    const auto &track = *iTrack;
    LHCb::MuonPID *muPid = new LHCb::MuonPID;
    // auto muPid = LHCb::MuonPID{};

    // set default values for the muonPID

    muPid->setMuonLLMu(-10000.);
    muPid->setMuonLLBg(-10000.);
    muPid->setIsMuon(0);
    muPid->setIsMuonLoose(0);
    muPid->setIsMuonTight(0);
    muPid->setNShared(0);

    if (msgLevel(MSG::DEBUG))
      debug() << "################# New track with p = " << track->p()
              << "##################" << endmsg;
    if (!isGoodOfflineTrack(*track)) {
      continue;
    }
    counter("nGoodOffline")++;
    const auto extrapolation = muonTool_->extrapolateTrack(*track);
    if (msgLevel(MSG::DEBUG))
      debug() << "The extrapolation in X went to " << extrapolation[1].first
              << ", " << extrapolation[2].first << ", "
              << extrapolation[3].first << ", " << extrapolation[4].first
              << endmsg;
    if (msgLevel(MSG::DEBUG))
      debug() << "The extrapolation in X went to " << extrapolation[1].second
              << ", " << extrapolation[2].second << ", "
              << extrapolation[3].second << ", " << extrapolation[4].second
              << endmsg;
    counter("nExtrapolated")++;
    if (!muonTool_->preSelection(*track, extrapolation)) {
      continue;
    }
    counter("nPreSelTrack")++;
    CommonConstMuonHits hits, hitsTight;
    std::array<unsigned, CommonMuonTool::nStations> occupancies,
        occupanciesTight;
    std::tie(hits, occupancies) =
        muonTool_->hitsAndOccupancies(*track, extrapolation);
    counter("nHitAndOcc")++;
    std::tie(hitsTight, occupanciesTight) = muonTool_->extractCrossed(hits);
    const auto isMuon = muonTool_->isMuon(occupancies, track->p());
    const auto isMuonTight = muonTool_->isMuon(occupanciesTight, track->p());
    const auto isMuonLoose = muonTool_->isMuonLoose(occupancies, track->p());
    muPid->setIsMuon(isMuon);
    muPid->setIsMuonLoose(isMuonLoose);
    muPid->setIsMuonTight(isMuonTight);

    if (msgLevel(MSG::DEBUG))
      debug() << " ---------  ISMUON ALREADY COMPUTED ------------" << endmsg;
    if (msgLevel(MSG::DEBUG))
      debug() << "For track with p = " << track->p() << ", isMuon = " << isMuon
              << ", and isMuonLoose = " << isMuonLoose
              << ". Occ M2 = " << occupancies[1]
              << ", occ M3= " << occupancies[2]
              << ", occ M4= " << occupancies[3]
              << ", occ M5= " << occupancies[4] << endmsg;
    if (isMuon == 1) counter("nIsMuon")++;
    if (isMuonTight == 1) counter("nIsMuonTight")++;
    if (isMuonLoose == 1) {
      counter("nIsMuonLoose")++;
      if (msgLevel(MSG::DEBUG))
        debug() << "Inserting the track with momentum = " << track->p()
                << " into muonPids" << endmsg;
      muPids->insert(muPid, (*track).key());  // insert only the tracks that
                                              // pass the isMuonLoose as
                                              // afterwards this requirement is
                                              // asked
      // auto muPid = LHCb::MuonPID{};
      // if(muPid.empty()){
      //  if(msgLevel(MSG::DEBUG)) debug() << "No valid muPid for the considered
      //  track" << endmsg;
      //  continue;
      //}
      counter("nMuonPIDs")++;
      muPid->setIDTrack(*iTrack);

      // fill the muonMap with the hits in FoI already computed
      m_muonMap[muPid] = hits;

      // auto muTrack = makeMuonTrack(muPid); // To be added in the future
      // muPid->setMuonTrack(&muTrack);  // TODO: is this safe?

      if (msgLevel(MSG::DEBUG))
        debug() << "########## CALCULATE THE DLL for track with p = << "
                << track->p() << " #############" << endmsg;
      std::tie(ProbMu, ProbNonMu) = DLLTool_->calcMuonLL_tanhdist_landau(
          *track, extrapolation, hits, occupancies);
      muPid->setMuonLLMu(log(ProbMu));
      muPid->setMuonLLBg(log(ProbNonMu));

      // compute the NShared
      DLLTool_->calcNShared(muPid, &(*muPids), hits, extrapolation, m_muonMap);

    }  // end calculations for isMuonLoose tracks
    if (msgLevel(MSG::DEBUG))
      debug() << " ProbMu = " << exp(muPid->MuonLLMu())
              << ", ProbNonMu = " << exp(muPid->MuonLLBg())
              << ". Momentum p = " << track->p() << endmsg;

    // the value of nShared is set inside the function

    // muPid.insert(&muPid, track->key());
    // muTracks->insert(&muTrack, track.key());

  }  // end loop over tracks
  // if(msgLevel(MSG::DEBUG)) debug() << "The size of pMuids is = " <<
  // muPids->size() << endmsg;

  // nShared = DLLTool_->calcNShared(&muPid,&(*muPids),hits,extrapolation);
  muPids->clear();
  muTracks->clear();
  // tracks->clear();
  //&tracks->clear();
  // delete &tracks;
  // m_coords->clear();
  m_muonMap.clear();
  return StatusCode::SUCCESS;
}

/** Tear down.
 */
StatusCode MuonIDAlgLite::finalize() {
  if (msgLevel(MSG::DEBUG)) debug() << "End of the execution" << endmsg;
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

/** Build a muon track based on PID information.
 * TODO: Implement properly.
 * This uses return value optimsation, so no pointers are necessary.
 */
LHCb::Track MuonIDAlgLite::makeMuonTrack(const LHCb::MuonPID &muPid) const {
  auto muTrack = LHCb::Track{muPid.key()};
  muTrack.addToAncestors(muPid.idTrack());

  // The old MuonIDAlg had a method makeMuonTrack that does some magic I don't
  // fully comprehend at this point.
  // TODO: Get explanations.
  // TODO: Implement!

  muTrack.setType(LHCb::Track::Muon);
  muTrack.setHistory(LHCb::Track::MuonID);
  muTrack.addInfo(LHCb::Track::IsMuon, muPid.IsMuon());
  return muTrack;
}
