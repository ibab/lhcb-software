
#include "TrackMon.h"

#include <GaudiKernel/AlgFactory.h>

#include <Event/ODIN.h>

#include <Event/Track.h>
#include <Event/Measurement.h>
#include <Event/OTMeasurement.h>
#include <Event/State.h>
#include <Event/Node.h>
#include <Event/FitNode.h>

using namespace LHCb;

#include <iostream>

#include <boost/preprocessor/seq.hpp>
#include <boost/foreach.hpp>

#include <boost/assign.hpp>
using namespace boost::assign;

DECLARE_ALGORITHM_FACTORY(TrackMon);

#define VARS (x)(y)(z)(errx2)(erry2)(errz2)(outlier)(timeUsed) \
             (station)(layer)(quarter)(module)(modulen)(straw)(strawt0)(strawlen) \
             (tdc)(tof)(reftof) \
             (tdrift)(dist)(timeResidual)(distResidual)(err2)(errmes2) \
             (mubtdrift)(mubdist)(mubtimeResidual)(mubdistResidual)(mubchi2)(muberr2)(muberrmes2)

TrackMon::TrackMon(const std::string& name, ISvcLocator* pSvcLocator): GaudiTupleAlg(name, pSvcLocator),
  decoder("OTRawBankDecoder"),
  projector("TrajOTProjector"),
  condReadoutGate(0),
  readoutGateT1(28),
  readoutGateT2(30),
  readoutGateT3(32)
{
  detector = 0;

  declareProperty("Fitter", fitter);
  declareProperty("Projector", projector);
  declareProperty("RawBankDecoder", decoder);

  declareProperty("TrackLocation", trackLocation = TrackLocation::Default);

  declareProperty("MaxTrackChi2", maxTrackChi2 = 16, "Max track Chi^2 (default 16)");

  declareProperty("MinNDF", minNDF = 16);
  declareProperty("MinP", minP = 3.2);
  declareProperty("TrackType", trackType = 3);
}

StatusCode TrackMon::initialize()
{
  if(GaudiTupleAlg::initialize() != StatusCode::SUCCESS) return StatusCode::FAILURE;

  // get DeOTDetector
  detector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if(detector == 0) return StatusCode::FAILURE;

  fitter.retrieve().ignore();
  projector.retrieve().ignore();

  decoder.retrieve().ignore();

/*  if(existDet<DataObject>(detSvc(), "Conditions/ParticleID/Muon/PreSelMomentum"))
  try {
    detDataSvc = svc<IDetDataSvc>("DetectorDataSvc", true);
    registerCondition("Conditions/Calibration/OT/ReadoutGates", condReadoutGate, &TrackMon::condReadoutGateUpdate);
    StatusCode statusUpdate = runUpdate();
    if(statusUpdate.isFailure())
      return Error("Unable to update conditions from database.", statusUpdate);
  } catch(GaudiException &ex) {
    fatal() << ex << endmsg;
    return StatusCode::FAILURE;
  }*/

  return StatusCode::SUCCESS;
}

StatusCode TrackMon::finalize()
{
  fitter.release().ignore();
  projector.release().ignore();
  decoder.release().ignore();

  return GaudiTupleAlg::finalize();
}

StatusCode TrackMon::execute()
{
  Tuple tuple = nTuple("tuple");
  Tuple tupleHits = nTuple("tupleHits");

  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  LHCb::Tracks* tracks = exist<LHCb::Tracks>(trackLocation) ? get<LHCb::Tracks>(trackLocation) : 0;
  if(tracks == 0) return StatusCode::SUCCESS;

  BOOST_FOREACH(LHCb::Track* track, *tracks)
  {
    if(track->fitStatus() != LHCb::Track::Fitted) fitter->fit(*track);
    if(!(track->fitStatus() == LHCb::Track::Fitted && track->nDoF() >= minNDF && track->p() / 1000.0 > minP)) continue;
    if(!(trackType == -1 || trackType == track->type())) continue;

    double trackChi2 = track->chi2() / track->nDoF();
    if(!(0 < trackChi2 && (maxTrackChi2 == -1 || trackChi2 < maxTrackChi2))) continue;

    #define DEFV(name) std::vector<double> name; name.clear(); name.reserve(64);
    #define BODY_(r, x, name) DEFV(name)
    BOOST_PP_SEQ_FOR_EACH(BODY_, BOOST_PP_EMPTY, VARS)
    #undef BODY_
    #undef DEFV

    int nhit = 0;

    BOOST_FOREACH(const LHCb::Node* node, track->nodes())
    {
      if(!(node->type() == LHCb::Node::HitOnTrack)) continue; // || node->type() == LHCb::Node::Outlier)) continue;
      if(!(node->measurement().type() == LHCb::Measurement::OT)) continue;

      const LHCb::FitNode* fitNode = dynamic_cast<const LHCb::FitNode*>(node);
      if(fitNode == 0) continue; // for safety

      const LHCb::OTMeasurement* measurement = dynamic_cast<const LHCb::OTMeasurement*>(&node->measurement());
      if(measurement == 0) continue; // for safety

      bool outlier_ = (node->type() == Node::Outlier);
      outlier += outlier_;

      const OTChannelID& channel = measurement->channel();

      tdc += channel.tdcTime();
      tof += measurement->timeOfFlight();
      reftof += measurement->module().strawReferenceTimeOfFlight(channel.straw());

      station += channel.station() - 1;
      layer += channel.layer();
      quarter += channel.quarter();
      module += channel.module() - 1;
      modulen += ( channel.module() - 1 + 9 * (channel.quarter() + 4 * (channel.layer() + 4 * (channel.station() - 1))) );
      straw += channel.straw() - 1;

      strawt0 += measurement->module().strawT0(channel.straw());
      strawlen += measurement->module().wireLength(channel);

      bool timeUsed_ = measurement->driftTimeStrategy();
      timeUsed += timeUsed_;

      {
        State state = fitNode->state();
        FitNode node(*fitNode);
        node.setState(state);
        node.setRefVector(state.stateVector());
        projector->projectReference(node);

        x += state.x();
        y += state.y();
        z += state.z();
        errx2 += state.errX2();
        erry2 += state.errY2();
        errz2 += 0;

        double tdrift_ = measurement->driftTimeFromY(state.y());
        tdrift += tdrift_;

        double dist_ = node.pocaVector().Dot(state.position() - measurement->trajectory().beginPoint());
        dist += dist_;

        double distResidual_ = fabs(dist_) - measurement->driftRadiusWithErrorFromY(state.y()).val;
        distResidual += distResidual_;
        double timeResidual_ = tdrift_ - measurement->module().driftTimeWithError(fabs(dist_)).val;
        timeResidual += timeResidual_;

        err2 += node.errResidual2();
        errmes2 += node.errMeasure2();
      }

      nhit++;
    }

    if(!(nhit >= 9)) continue;

    fillUnbiasedResiduals(track, 432, mubtdrift, mubdist, mubtimeResidual, mubdistResidual, mubchi2, muberr2, muberrmes2);

    tuple->column("ndof", track->nDoF());
    tuple->column("chi2", trackChi2);
    tuple->column("p", track->p() / 1000);
    tuple->column("trackType", track->type());

    tuple->column("slopex", track->slopes().X());
    tuple->column("slopey", track->slopes().Y());

    #define SAVEV(tuple, name) tuple->farray(#name, name, "nhit", 64);
    #define BODY_(r, x, name) SAVEV(tuple, name)
    BOOST_PP_SEQ_FOR_EACH(BODY_, BOOST_PP_EMPTY, VARS)
    #undef BODY_
    #undef SAVEV

    tuple->column("ntrack", tracks->size());
    tuple->column("runNumber", odin->runNumber());
    tuple->column("eventNumber", (unsigned int)odin->eventNumber());
    tuple->column("calibStep", odin->calibrationStep());

    tuple->column("timeYear",   odin->eventTime().year  (false));
    tuple->column("timeMonth",  odin->eventTime().month (false));
    tuple->column("timeDay",    odin->eventTime().day   (false));
    tuple->column("timeHour",   odin->eventTime().hour  (false));
    tuple->column("timeMinute", odin->eventTime().minute(false));
    tuple->column("timeSecond", odin->eventTime().second(false));

    tuple->write();
  }

  BOOST_FOREACH(const DeOTModule* module, detector->modules())
  {
    LHCb::OTChannelID modid = module->elementID();
    LHCb::OTLiteTimeRange ottimes = decoder->decodeModule(modid);
    BOOST_FOREACH(const LHCb::OTLiteTime& ottime, ottimes)
    {
      tupleHits->column("s", ottime.channel().station());
      tupleHits->column("l", ottime.channel().layer());
      tupleHits->column("q", ottime.channel().quarter());
      tupleHits->column("m", ottime.channel().module());
      double x = module->centerOfStraw(ottime.channel().straw()).x();
      tupleHits->column("x", x);
      tupleHits->write();
    }
  }

  return StatusCode::SUCCESS;
}

void TrackMon::fillUnbiasedResiduals(const Track* track, int mode, std::vector<double>& tdrift, std::vector<double>& dist, std::vector<double>& timeResidual, std::vector<double>& distResidual, std::vector<double>& chi2, std::vector<double>& err2, std::vector<double>& errmes2)
{
  Track::ConstNodeRange nodes = track->nodes();
  for(unsigned int inode = 0; inode < nodes.size(); inode++)
  {
    const Node* node = nodes[inode];
    if(!(node->type() == LHCb::Node::HitOnTrack)) continue; // || node->type() == LHCb::Node::Outlier)) continue;
    if(!(node->measurement().type() == LHCb::Measurement::OT)) continue;

    const OTMeasurement* measurement = dynamic_cast<const OTMeasurement*>(&node->measurement());
    if(measurement == 0) continue;
    const OTChannelID& channel = measurement->channel();

    int s = channel.station() - 1;
    int l = channel.layer();
    int q = channel.quarter();
    int m = channel.module() - 1;

    Track* clone = track->clone();

    int nhit = 0;

    BOOST_FOREACH(const Node* node_, clone->nodes())
    {
      if(!(node_->type() == LHCb::Node::HitOnTrack)) continue; // || node_->type() == LHCb::Node::Outlier)) continue;
      if(!(node_->measurement().type() == LHCb::Measurement::OT)) continue;

      const OTMeasurement* measurement_ = dynamic_cast<const OTMeasurement*>(&node_->measurement());
      if(measurement_ == 0) continue;
      const OTChannelID& channel_ = measurement_->channel();

      int s_ = channel_.station() - 1;
      int l_ = channel_.layer();
      int q_ = channel_.quarter();
      int m_ = channel_.module() - 1;

      bool unbias = false;

      switch(mode)
      {
      default:
      case 432:
        // same station, layer, quarter and module
        unbias = (s == s_) && (l == l_) && (q == q_) && (m == m_);
        break;
      case 12:
        // same layer
        unbias = (s == s_) && (l == l_);
        break;
      case 6:
        // same station
        unbias = s == s_;
        // AND same (X1U layer OR VX2 layer)
        unbias = unbias &&
          (((l == 0 || l == 1) && (l_ == 0 || l_ == 1)) ||
          ((l == 2 || l == 3) && (l_ == 2 || l_ == 3)));
        // AND same (Left side OR Right side)
        unbias = unbias &&
          (((q == 0 || q == 2) && (q_ == 0 || q_ == 2)) ||
          ((q == 1 || q == 3) && (q_ == 1 || q_ == 3)));
        break;
      case 3:
        // same station
        unbias = (s == s_);
        break;
      }

      if(unbias) const_cast<Node*>(node_)->setType(Node::Outlier);
      else nhit++;
    }

    fitter->fit(*clone);

    {
      const FitNode* fitNode = dynamic_cast<const FitNode*>(clone->nodes()[inode]);
      State state = fitNode->state();
      FitNode node(*fitNode);
      node.setState(state);
      node.setRefVector(state.stateVector());
      projector->projectReference(node);

      const OTMeasurement* measurement = dynamic_cast<const OTMeasurement*>(&fitNode->measurement());

      double tdrift_ = measurement->driftTimeFromY(state.y());
      tdrift += tdrift_;

      double dist_ = node.pocaVector().Dot(state.position() - measurement->trajectory().beginPoint());
      dist += dist_;

      double distResidual_ = fabs(dist_) - measurement->driftRadiusWithErrorFromY(state.y()).val;
      distResidual += distResidual_;
      double timeResidual_ = tdrift_ - measurement->module().driftTimeWithError(fabs(dist_)).val;
      timeResidual += timeResidual_;

      err2 += node.errResidual2();
      errmes2 += node.errMeasure2();

      chi2 += clone->chi2() / clone->nDoF();
    }

    delete clone;
  }
}

StatusCode TrackMon::condReadoutGateUpdate()
{
  std::vector<double> readoutGates = condReadoutGate->paramVect<double>("ReadoutGates");
  info() << " :: readoutGateT1 = " << (readoutGateT1 = readoutGates[0]) << endmsg;
  info() << " :: readoutGateT2 = " << (readoutGateT2 = readoutGates[1]) << endmsg;
  info() << " :: readoutGateT3 = " << (readoutGateT3 = readoutGates[2]) << endmsg;
  return StatusCode::SUCCESS;
}
