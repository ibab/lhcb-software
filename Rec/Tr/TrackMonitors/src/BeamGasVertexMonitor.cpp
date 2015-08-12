#include <Event/ODIN.h>
#include <Event/Track.h>
#include <Event/HltDecReports.h>
#include <Event/HltSelReports.h>
#include <Event/MCHeader.h>
#include <Event/GenHeader.h>
#include "BeamGasVertexMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasVertexMonitor
//
// 2015-05-08 Rosen Matev
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasVertexMonitor )

BeamGasVertexMonitor::BeamGasVertexMonitor(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("ReportLocation", m_reportLocation = "Hlt1/SelReports");
  declareProperty("Selections", m_selNames);
  declareProperty("SplitLocations", m_svLocations);
  declareProperty("SplitPrefixes", m_svPrefixes);
  declareProperty("MaxMatchDeltaZ", m_maxMatchDeltaZ = 10.);
}

BeamGasVertexMonitor::~BeamGasVertexMonitor()
{

}

StatusCode BeamGasVertexMonitor::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  if (m_svLocations.size() != m_svPrefixes.size()) {
    error() << "Number of prefixes does not match number of split vertex locations!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode BeamGasVertexMonitor::execute() {
  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  BeamGasCandidatesSet set;
  bool withHlt = false;
  bool withMC = false;

  for (unsigned int i = 0; i < m_svLocations.size(); ++i) {
    const auto* splitVertices = get<LHCb::RecVertices>(m_svLocations[i]);
    set.emplace_back("rec" + m_svPrefixes[i], getRecoCandidates(*splitVertices));
  }

  if (!m_selNames.empty()) {
    const auto* selReports = getIfExists<LHCb::HltSelReports>(m_reportLocation);
    if (selReports) {
      set.emplace_back("hlt", getHltCandidates(*selReports));
      withHlt = true;
    }
  }

  const auto* mcHeader = getIfExists<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  const LHCb::GenHeader* genHeader = nullptr;
  if (mcHeader) {
    genHeader = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
    set.emplace_back("mc", getMCCandidates(mcHeader->primaryVertices()));
    withMC = true;
  }

  if (set.empty()) return StatusCode::SUCCESS;

  Tuple tuple = nTuple("ntuple");

  const BeamGasCandidate defcand = BeamGasCandidate();

  unsigned int totCandidates = set[0].second.size();
  unsigned int nCandidate = 0;
  const auto& name = set[0].first;
  for (auto& cand : set[0].second) {
    bool test = true;
    test &= fillOdin(tuple, odin);
    test &= tuple->column("totCandidates", totCandidates);
    test &= tuple->column("nCandidate", nCandidate);

    // The main primary/split vertices
    if (name != "mc") {
      test &= fillVertex(tuple, name + "_", cand.pv, true);
      test &= fillVertex(tuple, name + "_1_", cand.sv1, false);
      test &= fillVertex(tuple, name + "_2_", cand.sv2, false);
    } else {
      test &= fillVertex(tuple, name + "_", cand.pv, false);
    }

    // Matched primary/split vertices
    for (auto it = ++set.begin(); it != set.end(); ++it) {
      const auto& mname = (*it).first;
      const auto& mcandidates = (*it).second;

      const BeamGasCandidate* matched = &defcand;
      auto mpair = matchCandidate(cand.pv, mcandidates);
      if (mpair.second != mcandidates.end())
        matched = &(*mpair.second);

      // For **rec** PVs (other than the main), don't write the PV and
      // the matching distance (which is always zero).
      if (mname.compare(0, 3, "rec") != 0) {
        test &= fillVertex(tuple, mname + "_", matched->pv, mname != "mc");
        test &= tuple->column(mname + "_match_dist", mpair.first);
      }

      if (mname != "mc") {
        test &= fillVertex(tuple, mname + "_1_", matched->sv1, false);
        test &= fillVertex(tuple, mname + "_2_", matched->sv2, false);
      }

      if (withHlt && !matched->selections.empty())
        cand.selections = matched->selections;
    }

    // HLT selections
    if (withHlt) {
      for (const auto& selname : m_selNames)
        test &= tuple->column(selname, cand.selections.count(selname) > 0);
    }

    if (withMC) {
      tuple->column("mc_evtype", genHeader->evType());
      //tuple->column("mc_ncollisions", genHeader->numOfCollisions());

      std::vector<double> processTypes;
      for (const auto& collision : genHeader->collisions()) {
        processTypes.push_back(collision->processType());
      }
      tuple->farray("mc_processtypes", processTypes, "mc_ncollisions", 10);
    }

    test &= tuple->write();

    if (!test) {
      error() << "Failed filling/writing tuple" << endmsg;
      return StatusCode::FAILURE;
    }

    nCandidate++;
  }

  return StatusCode::SUCCESS;
}

bool BeamGasVertexMonitor::fillOdin(const Tuple& tuple, const LHCb::ODIN* odin) {
  bool test = true;
  test &= tuple->column("run", odin->runNumber());
  test &= tuple->column("gps", odin->gpsTime());
  test &= tuple->column("evt", odin->eventNumber());
  test &= tuple->column("orbit", odin->orbitNumber());
  test &= tuple->column("step", odin->calibrationStep());
  test &= tuple->column("bcid", odin->bunchId());
  test &= tuple->column("bx", odin->bunchCrossingType());
  return test;
}

bool BeamGasVertexMonitor::fillVertex(const Tuple& tuple, const std::string& prefix, const BeamGasVertex& vertex, bool all) {
  bool test = true;
  test &= tuple->column(prefix + "nTr", vertex.ntr);
  if (all) {
    test &= tuple->column(prefix + "nTrBw", vertex.ntrbw);
    test &= tuple->column(prefix + "chi2", vertex.chi2);
    test &= tuple->column(prefix + "ndof", vertex.ndof);
  }
  test &= tuple->column(prefix + "x", vertex.x);
  test &= tuple->column(prefix + "y", vertex.y);
  test &= tuple->column(prefix + "z", vertex.z);
  return test;
}

std::pair<double, BeamGasCandidates::const_iterator> BeamGasVertexMonitor::matchCandidate(
  const BeamGasVertex& vertex, const BeamGasCandidates& candidates) {

  auto it = std::min_element(
    candidates.begin(), candidates.end(),
    [&vertex](const BeamGasCandidate& a, const BeamGasCandidate& b) {
      return std::abs(a.pv.z - vertex.z) < std::abs(b.pv.z - vertex.z);
    }
  );

  double dist = 1e10; //std::numeric_limits<double>::infinity();
  if (it != candidates.end()) {
    if (std::abs((*it).pv.z - vertex.z) < m_maxMatchDeltaZ)
      dist = std::abs((*it).pv.z - vertex.z);
    else
      it = candidates.end();
  }
  return std::pair<double, BeamGasCandidates::const_iterator>(dist, it);
}


BeamGasCandidates BeamGasVertexMonitor::getHltCandidates(const LHCb::HltSelReports& selReports) {
  BeamGasCandidates candidates;
  for (const auto& name : m_selNames) {
    const auto& sr = selReports.selReport(name);
    if (sr) {
      const SmartRefVector<LHCb::HltObjectSummary>& sub = sr->substructure();
      Assert(sub.size() % 3 == 0, "HLT vertex selection length must be a multiple of 3.");
      for (unsigned int i = 0; i < sub.size(); i += 3) {
        BeamGasVertex pv(*(sub[i]), m_velo);

        BeamGasCandidates::iterator it;
        it = std::find_if(candidates.begin(), candidates.end(),
                          [&pv](const BeamGasCandidate& cand) {
                            return cand.pv.z == pv.z;
                          });
        if (it == candidates.end()) {
          candidates.emplace_back(pv,
            BeamGasVertex(*(sub[i+1]), m_velo),
            BeamGasVertex(*(sub[i+2]), m_velo));
          it = candidates.end() - 1;
        }
        (*it).selections.insert(name);
      }
    }
  }
  return candidates;
}

BeamGasCandidates BeamGasVertexMonitor::getRecoCandidates(const LHCb::RecVertices& splitVertices) {
  BeamGasCandidates candidates;
  Assert(splitVertices.size() % 3 == 0, "Split vertices container length must be a multiple of 3.");
  for (auto it = splitVertices.begin(); it != splitVertices.end(); ) {
    LHCb::RecVertex* pv = *(it++);
    LHCb::RecVertex* sv1 = *(it++);
    LHCb::RecVertex* sv2 = *(it++);
    Assert(sv1->info(1000003, -1) == pv->position().z(), "Split vertex 1 does not match PV");
    Assert(sv2->info(1000003, -1) == pv->position().z(), "Split vertex 2 does not match PV");
    candidates.emplace_back(*pv, *sv1, *sv2);
  }
  return candidates;
}

BeamGasCandidates BeamGasVertexMonitor::getMCCandidates(const SmartRefVector<LHCb::MCVertex>& mcVertices) {
  BeamGasCandidates candidates;
  for (const auto& mcv : mcVertices) {
    candidates.emplace_back(*mcv);
  }
  return candidates;
}

BeamGasVertex::BeamGasVertex()
  : x(0), y(0), z(0), chi2(-1), ndof(-1), ntr(-1), ntrbw(-1)
{
}

BeamGasVertex::BeamGasVertex(const LHCb::RecVertex& recvtx)
  : x(recvtx.position().x()),
    y(recvtx.position().y()),
    z(recvtx.position().z()),
    chi2(recvtx.chi2()),
    ndof(recvtx.nDoF()),
    ntr(recvtx.tracks().size())
{
  int n = 0;
  for (const auto& track : recvtx.tracks())
    n += track->checkFlag(LHCb::Track::Backward);
  this->ntrbw = n;
}

BeamGasVertex::BeamGasVertex(const LHCb::MCVertex& mcvtx)
  : x(mcvtx.position().x()),
    y(mcvtx.position().y()),
    z(mcvtx.position().z()),
    chi2(-1), ndof(-1), ntr(mcvtx.products().size()), ntrbw(-1)
{
}

BeamGasVertex::BeamGasVertex(const LHCb::HltObjectSummary& recvtx_hos, const DeVelo* velo) {
  if (recvtx_hos.summarizedObjectCLID() != 10030)
    throw std::runtime_error("Unexpected CLID of summarized object (expect 10030).");

  const auto& substr = recvtx_hos.substructureFlattened();
  int nTr = substr.size();
  int nTrBw = 0;
  for (const auto& track_hos : substr) {
    bool bwd = false;
    if (track_hos->summarizedObjectCLID() != 10010)
      throw std::runtime_error("Unexpected CLID of summarized object (expect 10010).");
    auto ni = track_hos->numericalInfoFlattened();
    auto flags = ni.find("10#Track.flags");
    if (flags != ni.end()) {
      bwd = (int)((*flags).second) & (LHCb::Track::Backward << 18);  // LHCb::Track::flagBits (= 18) is protected
    } else {
      bwd = isBackwardTrack(*track_hos, velo);
    }
    // if flags: assert bwd == is_backward(hos_tr)
    nTrBw += bwd;
  }
  const auto& ni = recvtx_hos.numericalInfoFlattened();

  this->x = ni.at("0#RecVertex.position.x");
  this->y = ni.at("1#RecVertex.position.y");
  this->z = ni.at("2#RecVertex.position.z");
  this->chi2 = ni.at("3#RecVertex.chi2");
  this->ndof = 2*nTr - 3;
  this->ntr = nTr;
  this->ntrbw = nTrBw;
}

bool BeamGasVertex::isBackwardTrack(const LHCb::HltObjectSummary& track_hos, const DeVelo* velo) {
  double z0 = track_hos.numericalInfoFlattened().at("0#Track.firstState.z");
  double zmax = -1e9;
  for (const auto& lhcbID : track_hos.lhcbIDsFlattened()) {
    if (lhcbID.isVelo()) zmax = std::max(zmax, velo->sensor(lhcbID.veloID())->z());
  }
  if (zmax == -1e9)
    throw std::runtime_error("No VELO LHCbIDs (hits) for given track!");
  return z0 > zmax;
}
