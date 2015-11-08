// LHCb
#include "Event/Track.h"
#include "Event/Node.h"

// Local
#include "VPTrackMonitor.h"

DECLARE_ALGORITHM_FACTORY(VPTrackMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPTrackMonitor::VPTrackMonitor(const std::string& name,
                               ISvcLocator* pSvcLocator)
    : GaudiTupleAlg(name, pSvcLocator) {
  declareProperty("TrackContainer",
                  m_trackLocation = LHCb::TrackLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
VPTrackMonitor::~VPTrackMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPTrackMonitor::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDetIfExists<DeVP>(DeVPLocation::Default);
  if (!m_det) {
    return Error("No detector element at " + DeVPLocation::Default);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPTrackMonitor::execute() {

  // Get tracks
  const LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>(m_trackLocation);
  if (!tracks) {
    return Error("No tracks at " + m_trackLocation);
  } 
  const Gaudi::XYZPoint origin(0., 0., 0.);
  for (const LHCb::Track* track : *tracks) {
    // Skip tracks which are not VELO tracks and not long tracks.
    const bool bwd = track->checkFlag(LHCb::Track::Backward);
    const auto type = track->type();
    if (type != LHCb::Track::Types::Velo && type != LHCb::Track::Types::Long &&
        !bwd) {
      continue;
    }
    const bool fitted = track->checkFitStatus(LHCb::Track::FitStatus::Fitted);
    if (!fitted) continue;
    const double chi2NDOF = track->chi2PerDoF();
    const double probChi2 = track->probChi2();
    const double phi = track->phi();
    const double eta = track->pseudoRapidity();

    unsigned int nClusters = track->nodes().size();
    for (const LHCb::Node* node : track->nodes()) {
      const LHCb::FitNode& fitnode = dynamic_cast<const LHCb::FitNode&>(*node);
      if (!fitnode.hasMeasurement()) continue;

      if (node->type() != LHCb::Node::HitOnTrack) continue;
      // Skip non-VP measurements.
      if ((node->measurement()).type() != LHCb::Measurement::VP) continue;
      // Get the sensor.
      LHCb::LHCbID id = (node->measurement()).lhcbID();
      const DeVPSensor* sens = m_det->sensorOfChannel(id.vpID());
     
      const auto corner = sens->localToGlobal(origin);
      const auto cluPos = node->state().position();
      const auto residual = getResidual(cluPos, sens, fitnode);
      Tuple theTuple = nTuple("VPTrackMonitor", "");
      theTuple->column("resX", residual.x());
      theTuple->column("resY", residual.y());
      theTuple->column("nodeResidual", node->residual());
      theTuple->column("clusX", cluPos.x());
      theTuple->column("clusY", cluPos.y());
      theTuple->column("clusZ", cluPos.z());
      theTuple->column("sensEdgeX", corner.x());
      theTuple->column("sensEdgeY", corner.y());
      theTuple->column("Error", node->errMeasure());
      theTuple->column("module", sens->module());
      theTuple->column("station", sens->station());
      theTuple->column("isRight", sens->isRight());
      theTuple->column("isLeft", sens->isLeft());

      theTuple->column("phi", phi);
      theTuple->column("eta", eta);

      theTuple->column("chi2PerDoF", chi2NDOF);
      theTuple->column("probChi2", probChi2);
      theTuple->column("nClusters", nClusters);
      theTuple->column("TrackType", type);

      theTuple->write();
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Calculat teh residuals.
//=============================================================================
Gaudi::XYZVector VPTrackMonitor::getResidual(const Gaudi::XYZPoint& clusterGlobal, 
                                             const DeVPSensor* sens,
                                             const LHCb::FitNode& fitnode) const {
  if (msgLevel(MSG::DEBUG)) debug() << "Calculate unbiased residuals" << endmsg;
  const LHCb::State state = fitnode.unbiasedState();
  const Gaudi::XYZPoint trackInterceptGlobal(state.x(), state.y(), state.z());
  const auto trackInterceptLocal = sens->globalToLocal(trackInterceptGlobal);
  const auto clusterLocal = sens->globalToLocal(clusterGlobal);
  const double resx = trackInterceptLocal.x() - clusterLocal.x();
  const double resy = trackInterceptLocal.y() - clusterLocal.y();
  const double resz = trackInterceptLocal.z() - clusterLocal.z();
  return Gaudi::XYZVector(resx, resy, resz);
}
