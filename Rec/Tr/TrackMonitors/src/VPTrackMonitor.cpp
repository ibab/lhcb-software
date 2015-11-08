#include <boost/foreach.hpp>

// LHCb
#include "Event/Node.h"
// Local
#include "VPTrackMonitor.h"

DECLARE_ALGORITHM_FACTORY(VPTrackMonitor)

using namespace Gaudi;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPTrackMonitor::VPTrackMonitor(const std::string& name,
                               ISvcLocator* pSvcLocator)
    : GaudiTupleAlg(name, pSvcLocator) {
  declareProperty("TrackContainer",
                  m_tracklocation = LHCb::TrackLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
VPTrackMonitor::~VPTrackMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPTrackMonitor::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize();  // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  m_veloDet_VP = getDet<DeVP>(DeVPLocation::Default);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPTrackMonitor::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  // Get tracks
  const LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_tracklocation);
  BOOST_FOREACH(const LHCb::Track * track, tracks) {
    double chi2NDOF = track->chi2PerDoF();
    double probChi2 = track->probChi2();
    double phi = track->phi();
    double eta = track->pseudoRapidity();

    enum {
      Backward = 12,
      MaxType = 32
    };

    //    LHCb::Track::Types type = track->type();
    const int type =
        (track->checkFlag(LHCb::Track::Backward) ? int(Backward)
                                                 : int(track->type()));

    if (type != int(LHCb::Track::Types::Velo) &&
        type != int(LHCb::Track::Types::Long) &&
        type != int(LHCb::Track::Backward))
      continue;
    bool fitStatus = track->checkFitStatus(LHCb::Track::FitStatus::Fitted);

    if (!fitStatus) continue;
    LHCb::Track::ConstNodeRange nodes = track->nodes();
    unsigned int nClusters = nodes.size();
    for (LHCb::Track::ConstNodeRange::iterator inode = nodes.begin();
         inode != nodes.end(); ++inode) {
      const LHCb::FitNode& fitnode =
          dynamic_cast<const LHCb::FitNode&>(**inode);
      if (!fitnode.hasMeasurement()) continue;

      if ((*inode)->type() == LHCb::Node::HitOnTrack) {
        if (((*inode)->measurement()).type() == LHCb::Measurement::VP) {

          LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();

          LHCb::VPChannelID chID = nodeID.vpID();
          const DeVPSensor* sens =
              (DeVPSensor*)m_veloDet_VP->sensorOfChannel(chID);

          unsigned int module = sens->module();
          unsigned int station = sens->station();
          bool isRight = sens->isRight();
          bool isLeft = sens->isLeft();
          double meas_error = (*inode)->errMeasure();

          XYZPoint SensEdge_loc(0., 0., 0.);
          XYZPoint SensEdge = sens->localToGlobal(SensEdge_loc);

          XYZPoint cluPos = (*inode)->state().position();

          XYZPoint residual = getResidual(cluPos, sens, fitnode);
          double nodeResidual = (*inode)->residual();
          Tuple theTuple = nTuple("VPTrackMonitor", "");
          theTuple->column("resX", residual.x());
          theTuple->column("resY", residual.y());
          theTuple->column("nodeResidual", nodeResidual),
              theTuple->column("clusX", cluPos.x());
          theTuple->column("clusY", cluPos.y());
          theTuple->column("clusZ", cluPos.z());
          theTuple->column("sensEdgeX", SensEdge.x());
          theTuple->column("sensEdgeY", SensEdge.y());
          theTuple->column("Error", meas_error);
          theTuple->column("module", module);
          theTuple->column("station", station);
          theTuple->column("isRight", isRight);
          theTuple->column("isLeft", isLeft);

          theTuple->column("phi", phi);
          theTuple->column("eta", eta);

          theTuple->column("chi2PerDoF", chi2NDOF);
          theTuple->column("probChi2", probChi2);
          theTuple->column("nClusters", nClusters);
          theTuple->column("TrackType", type);

          theTuple->write();
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================

XYZPoint VPTrackMonitor::getResidual(XYZPoint cluPos, const DeVPSensor* sens,
                                     const LHCb::FitNode& fitnode) {
  // if(track->checkFitHistory(LHCb::Track::BiKalman) == true)
  //{
  debug() << "Start Unbiased Residual method" << endmsg;
  const LHCb::State state = fitnode.unbiasedState();
  Gaudi::XYZPoint trackInterceptGlobal(state.x(), state.y(), state.z());
  Gaudi::XYZPoint trackInterceptLocal(0, 0, 0);
  trackInterceptLocal = sens->globalToLocal(trackInterceptGlobal);
  Gaudi::XYZPoint clusterLocal(0, 0, 0);
  clusterLocal = sens->globalToLocal(cluPos);

  double resx = trackInterceptLocal.X() - clusterLocal.X();
  double resy = trackInterceptLocal.Y() - clusterLocal.Y();
  double resz = trackInterceptLocal.Z() - clusterLocal.Z();
  Gaudi::XYZPoint residual(resx, resy, resz);

  return residual;
}
