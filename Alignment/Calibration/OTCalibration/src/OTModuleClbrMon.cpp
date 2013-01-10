   
#include "OTModuleClbrMon.h"

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
#include <fstream>

#include <boost/preprocessor/seq.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/assign.hpp>
using namespace boost::assign;

#define PHOENIX_LIMIT 6
#include <boost/spirit/home/classic.hpp>
#include <boost/spirit/home/phoenix.hpp>

DECLARE_ALGORITHM_FACTORY(OTModuleClbrMon);

OTModuleClbrMon::OTModuleClbrMon(const std::string& name, ISvcLocator* pSvcLocator): GaudiTupleAlg(name, pSvcLocator),
  projector("TrajOTProjector"),
  decoder("OTRawBankDecoder")
{
  stationNames = list_of("T1")("T2")("T3");
  layerNames = list_of("X1")("U")("V")("X2");
  quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");

  detector = 0;

  declareProperty("Fitter", fitter);
  declareProperty("Projector", projector);
  declareProperty("RawBankDecoder", decoder);

  declareProperty("TrackLocation", trackLocation = TrackLocation::Default);

  declareProperty("MaxTrackChi2", maxTrackChi2 = 16, "Max track Chi^2 (default 16)");
  declareProperty("MaxTrackUbChi2", maxTrackUbChi2 = 2, "Max track unbiased Chi^2 (default 2)");

  declareProperty("MinNDF", minNDF = 16);
  declareProperty("MinP", minP = 3.2);
  declareProperty("TrackType", trackType = 3);

  declareProperty("ReadXMLs", readXMLs = false, "Read condition XML files (default false)");

  declareProperty("HistDriftTime", histDriftTimeOpts, "Options for drift time histograms (default t = [-30, 70]/200)");
  if(histDriftTimeOpts.size() != 3) histDriftTimeOpts= list_of(-5.0)(45.0)(200);

  declareProperty("HistDriftTimeResidual", histDriftTimeResidualOpts, "Options for drift time residual histograms (default t = [-25, 25]/200)");
  if(histDriftTimeResidualOpts.size() != 3) histDriftTimeResidualOpts = list_of(-25.0)(25.0)(200);

  declareProperty("HistDriftTimeVsDist", histDriftTimeVsDistOpts, "Options for drift time VS distance histograms (default r = [-5, 5]/200 and t = [-30, 70]/200)");
  if(histDriftTimeVsDistOpts.size() != 6) histDriftTimeVsDistOpts = list_of(-3.0)(3.0)(120)(-5.0)(45.0)(200);

  declareProperty("HistResidual", histResidualOpts, "Options for residual histograms (default r = [-2.5, 2.5]/50)");
  if(histResidualOpts.size() != 3) histResidualOpts = list_of(-5.0)(5.0)(100);
}

StatusCode OTModuleClbrMon::initialize()
{
  StatusCode statusCode = GaudiTupleAlg::initialize();

  // --- print options ---

  info() << "TrackLocation: " << trackLocation << endmsg;
  info() << "MaxTrackChi2: " << maxTrackChi2 << endmsg;
  info() << "MaxTrackUbChi2: " << maxTrackUbChi2 << endmsg;
  info() << "MinNDF: " << minNDF << endmsg;
  info() << "MinP: " << minP << " GeV" << endmsg;
  info() << "TrackType: " << trackType << endmsg;
  info() << "ReadXMLs: " << (readXMLs ? "True" : "False") << endmsg;
  info() << "HistDriftTime: " <<
    "t = [" << histDriftTimeOpts[0] << ", " << histDriftTimeOpts[1] << "]/" << (int)histDriftTimeOpts[2] << endmsg;
  info() << "HistDriftTimeResidual: " <<
    "t = [" << histDriftTimeResidualOpts[0] << ", " << histDriftTimeResidualOpts[1] << "]/" << (int)histDriftTimeResidualOpts[2] << endmsg;
  info() << "HistDriftTimeVsDist: " <<
    "r = [" << histDriftTimeVsDistOpts[0] << ", " << histDriftTimeVsDistOpts[1] << "]/" << (int)histDriftTimeVsDistOpts[2] <<
    ", t = [" << histDriftTimeVsDistOpts[3] << ", " << histDriftTimeVsDistOpts[4] << "]/" << (int)histDriftTimeVsDistOpts[5] << endmsg;
  info() << "HistResidual: " <<
    "r = [" << histResidualOpts[0] << ", " << histResidualOpts[1] << "]/" << (int)histResidualOpts[2] << endmsg;

  detector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if(detector == 0) return StatusCode::FAILURE;

  fitter.retrieve().ignore();
  projector.retrieve().ignore();

  std::cout << "book module histograms" << std::endl;
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m];

    histModuleDriftTime[s][l][q][m] = book(histPath + "/" + "driftTime", "drift time",
      histDriftTimeOpts[0], histDriftTimeOpts[1], (int)histDriftTimeOpts[2]);

    histModuleDriftTimeResidual01L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual01L", "drift time residual (otis01 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual01R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual01R", "drift time residual (otis01 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual23L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual23L", "drift time residual (otis23 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual23R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual23R", "drift time residual (otis23 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

    histModuleDriftTimeResidual[s][l][q][m] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

    histOTISResidual[s][l][q][m][0] = book(histPath + "/" + "residualOTIS0", "residual",
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidual[s][l][q][m][1] = book(histPath + "/" + "residualOTIS1", "residual",
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidual[s][l][q][m][2] = book(histPath + "/" + "residualOTIS2", "residual",
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidual[s][l][q][m][3] = book(histPath + "/" + "residualOTIS3", "residual",
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);

    histOTISResidualVsHitY[s][l][q][m][0] = book2D(histPath + "/" + "residualVsHitYOTIS0", "residual",
      0, 2400, 48,
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidualVsHitY[s][l][q][m][1] = book2D(histPath + "/" + "residualVsHitYOTIS1", "residual",
      0, 2400, 48,
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidualVsHitY[s][l][q][m][2] = book2D(histPath + "/" + "residualVsHitYOTIS2", "residual",
      0, 2400, 48,
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
    histOTISResidualVsHitY[s][l][q][m][3] = book2D(histPath + "/" + "residualVsHitYOTIS3", "residual",
      0, 2400, 48,
      histResidualOpts[0], histResidualOpts[1], (int)histResidualOpts[2]);
  }

  std::cout << "book quarter histograms" << std::endl;
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q];
    histQuarterDriftTime[s][l][q] = book(histPath + "/" + "driftTime", "drift time",
      histDriftTimeOpts[0], histDriftTimeOpts[1], (int)histDriftTimeOpts[2]);
    histQuarterDriftTimeResidual[s][l][q] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
  }

  std::cout << "book layer histograms" << std::endl;
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l];
    histLayerDriftTime[s][l] = book(histPath + "/" + "driftTime", "drift time",
      histDriftTimeOpts[0], histDriftTimeOpts[1], (int)histDriftTimeOpts[2]);
    histLayerDriftTimeResidual[s][l] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

    for(int clbrStep = 0; clbrStep < 10; clbrStep++)
    {
      histLayerDriftTimeResidualClbr[s][l][clbrStep] = book(histPath + "/" + "driftTimeResidualClbr" + boost::lexical_cast<std::string>(clbrStep), "drift time residual",
        histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    }
  }

  std::cout << "book station histograms" << std::endl;
  for(int s = 0; s < 3; s++)
  {
    std::string histPath = stationNames[s];
    histStationDriftTime[s] = book(histPath + "/" + "driftTime", "drift time",
      histDriftTimeOpts[0], histDriftTimeOpts[1], (int)histDriftTimeOpts[2]);
    histStationDriftTimeResidual[s] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
  }

  histDriftTimeVsDist = book2D("driftTimeVsDist", "drift time vs distance",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);

  histDriftTimeVsDistGood = book2D("driftTimeVsDistGood", "drift time vs distance (p > 10 GeV)",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);
  histDriftTimeVsDist17Good = book2D("driftTimeVsDist17Good", "drift time vs distance (p > 10 GeV && M == 1-7)",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);
  histDriftTimeVsDist89Good = book2D("driftTimeVsDist89Good", "drift time vs distance (p > 10 GeV && M == 8-9)",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);
/*  histDriftTimeVsDistGood50 = book2D("driftTimeVsDistGood50", "drift time vs distance (ntrack < 50 && p > 10 GeV)",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);
  histDriftTimeVsDistGood25 = book2D("driftTimeVsDistGood25", "drift time vs distance (ntrack < 25 && p > 10 GeV)",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);*/

  histTrackErrorGood = book("histTrackErrorGood", "track error (p > 10 GeV)", 0.0, 0.5, 100);
  histTrackError17Good = book("histTrackError17Good", "track error (p > 10 GeV && M == 1-7)", 0.0, 0.5, 100);
  histTrackError89Good = book("histTrackError89Good", "track error (p > 10 GeV && M == 8-9)", 0.0, 0.5, 100);

  histDriftTimeResidualVsHitY = book2D("driftTimeResidualVsHitY", "drift time residual vs hit y",
    0, 2400, 96, histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

  histWalkVsHitStrawY = book2D("walkVsHitStrawY", "walk vs hit straw y",
    0, 2400, 96, histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

  histDriftTimeResidualVsHitYGood = book2D("driftTimeResidualVsHitYGood", "drift time residual vs hit y (p > 10 GeV)",
    0, 2400, 96, histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

  histWalkVsHitStrawYGood = book2D("walkVsHitStrawYGood", "walk vs hit straw y (p > 10 GeV)",
    0, 2400, 96, histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

  histResidualPullVsDist17Good = book2D("histResidualPullVsDist17Good", "distance residual pull (p> 10 GeV && M == 1-7)",
    0, 2.5, 100, -5, 5, 100);

  histResidualPullVsDist89Good = book2D("histResidualPullVsDist89Good", "distance residual pull (p> 10 GeV && M == 8-9)",
    0, 2.5, 100, -5, 5, 100);

  if(readXMLs) statusCode = readCondXMLs();

  return statusCode;
}

StatusCode OTModuleClbrMon::execute()
{
  if(readXMLs)
  {
    readCondXMLs();
    readXMLs = false;
  }

  Tuple tuple = nTuple("tuple");

  LHCb::Tracks* tracks = exist<LHCb::Tracks>(trackLocation) ? get<LHCb::Tracks>(trackLocation) : 0;
  if(tracks == 0) return StatusCode::SUCCESS;

  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  Gaudi::Time eventTime = odin->eventTime();
  unsigned int gpsTime = odin->gpsTime() / 1000000;
  int clbrStep = odin->calibrationStep();

  BOOST_FOREACH(LHCb::Track* track, *tracks)
  {
    if(track->fitStatus() != LHCb::Track::Fitted) fitter->fit(*track);
    if(!(track->fitStatus() == LHCb::Track::Fitted && track->nDoF() >= minNDF && track->p() / 1000.0 > minP)) continue;
    if(!(trackType == -1 || trackType == track->type())) continue;

    double trackChi2 = track->chi2() / track->nDoF();
    if(!(0 < trackChi2 && (maxTrackChi2 == -1 || trackChi2 < maxTrackChi2))) continue;

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

        if((s == s_) && (l == l_) && (q == q_) && (m == m_)) const_cast<Node*>(node_)->setType(Node::Outlier);
      }

      fitter->fit(*clone);

      double unbiasedChi2 = clone->chi2() / clone->nDoF();
      if(unbiasedChi2 == -1 || unbiasedChi2 < maxTrackUbChi2)
      {
        const FitNode* fitNode = dynamic_cast<const FitNode*>(clone->nodes()[inode]);
        State state = fitNode->state();
        FitNode node(*fitNode);
        node.setState(state);
        node.setRefVector(state.stateVector());
        projector->projectReference(node);

        const OTMeasurement* measurement = dynamic_cast<const OTMeasurement*>(&fitNode->measurement());

        double tdrift = measurement->driftTimeFromY(state.y());
        double dist = node.pocaVector().Dot(state.position() - measurement->trajectory().beginPoint());

        double timeResidual = tdrift - measurement->module().driftTimeWithError(fabs(dist)).val;
        double distResidual = fabs(dist) - measurement->driftRadiusWithErrorFromY(state.y()).val;

        double err2 = node.errResidual2();
        double errmes2 = node.errMeasure2();

        double dist2readout = (measurement->module().trajectory(channel)->endPoint().y() - state.y()) * measurement->module().propagationVelocity() / measurement->module().propagationVelocityY();
        double dist2strawend = measurement->module().wireLength(channel) - dist2readout;

        double ttdc = 1.0 * channel.tdcTime() * 75.0 / 192.0;
        double t0 = measurement->module().strawT0(channel.straw());
        double ttof = measurement->timeOfFlight();
        double ttofref = measurement->module().strawReferenceTimeOfFlight(channel.straw());
        double tprop = dist2readout / measurement->module().propagationVelocity();
        double dtprop = ttdc - t0 - measurement->module().walkRelation().getOff() - ttof + ttofref - tprop - measurement->module().driftTimeWithError(fabs(dist)).val;

        if(clbrStep > 0 && 4 * s + l == (clbrStep - 1) / 10)
        {
          fill(histLayerDriftTimeResidualClbr[s][l][(clbrStep - 1) % 10], timeResidual, 1.0);
          if((clbrStep - 1) % 10 != 0) continue;
        }

        fill(histModuleDriftTime[s][l][q][m], tdrift, 1.0);
        fill(histQuarterDriftTime[s][l][q], tdrift, 1.0);
        fill(histLayerDriftTime[s][l], tdrift, 1.0);
        fill(histStationDriftTime[s], tdrift, 1.0);

        int otis = (channel.straw() - 1) / 32;
        if(otis <= 1 && dist < 0) fill(histModuleDriftTimeResidual01L[s][l][q][m], timeResidual, 1.0);
        if(otis <= 1 && dist > 0) fill(histModuleDriftTimeResidual01R[s][l][q][m], timeResidual, 1.0);
        if(otis >= 2 && dist < 0) fill(histModuleDriftTimeResidual23L[s][l][q][m], timeResidual, 1.0);
        if(otis >= 2 && dist > 0) fill(histModuleDriftTimeResidual23R[s][l][q][m], timeResidual, 1.0);

        fill(histModuleDriftTimeResidual[s][l][q][m], timeResidual, 1.0);
        fill(histQuarterDriftTimeResidual[s][l][q], timeResidual, 1.0);
        fill(histLayerDriftTimeResidual[s][l], timeResidual, 1.0);
        fill(histStationDriftTimeResidual[s], timeResidual, 1.0);

        fill(histOTISResidual[s][l][q][m][otis], distResidual * (dist < 0 ? -1.0 : +1.0), 1.0);
        fill(histOTISResidualVsHitY[s][l][q][m][otis], fabs(state.y()), distResidual * (dist < 0 ? -1.0 : +1.0), 1.0);

        fill(histDriftTimeVsDist, dist, tdrift, 1.0);
        fill(histDriftTimeResidualVsHitY, fabs(state.y()), timeResidual, 1.0);
        fill(histWalkVsHitStrawY, dist2strawend, dtprop, 1.0);
        if(clone->p() / 1000.0 > 10.0)
        {
          fill(histDriftTimeVsDistGood, dist, tdrift, 1.0);
          if(m < 7) fill(histDriftTimeVsDist17Good, dist, tdrift, 1.0);
          if(m >= 7) fill(histDriftTimeVsDist89Good, dist, tdrift, 1.0);
          fill(histDriftTimeResidualVsHitYGood, fabs(state.y()), timeResidual, 1.0);
          fill(histWalkVsHitStrawYGood, dist2strawend, dtprop, 1.0);

          fill(histTrackErrorGood, sqrt(err2 - errmes2), 1.0);
          if(m < 7) fill(histTrackError17Good, sqrt(err2 - errmes2), 1.0);
          if(m >= 7) fill(histTrackError89Good, sqrt(err2 - errmes2), 1.0);

//          if(tracks->size() <= 50) fill(histDriftTimeVsDistGood50, dist, tdrift, 1.0);
//          if(tracks->size() <= 25) fill(histDriftTimeVsDistGood25, dist, tdrift, 1.0);

          if(m < 7) fill(histResidualPullVsDist17Good, fabs(dist), distResidual / sqrt(err2), 1.0);
          if(m >= 7) fill(histResidualPullVsDist89Good, fabs(dist), distResidual / sqrt(err2), 1.0);
        }

        if(clone->p() / 1000.0 > 10.0)
        {
          tuple->column("runNumber", odin->runNumber());
          tuple->column("gpsTime", gpsTime);
          tuple->column("eventTime",
            (eventTime.second(false) << 0) +
            (eventTime.minute(false) << 6) +
            (eventTime.hour(false) << 12) +
            (eventTime.day(false) << 17) +
            (eventTime.month(false) << 22)
          );
          tuple->column("timeResidual", timeResidual);
          tuple->write();
        }
      }

      delete clone;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::finalize()
{
  fitter.release().ignore();
  projector.release().ignore();
  decoder.release().ignore();

  return GaudiTupleAlg::initialize();
}

StatusCode OTModuleClbrMon::readCondXMLs()
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = quarterId + "@" + prefix + ".xml";

    // load file with condition for specific station-layer-quarter
    std::ifstream file(fileName.c_str());
    if(file.fail())
    {
      warning() << "Can't open file: '" << fileName << "'" << endmsg;
      continue;
    }

    std::string xml;

    // read file to string
    file.seekg(0, std::ios_base::end);
    xml.resize(file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&xml.at(0), xml.size());
    file.close();

    std::vector<double> cRt[9], cRtErr[9], cT0[9], cWalk[9];

    {
      using namespace boost::spirit::classic;
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      int m; std::string param;

      rule<phrase_scanner_t> xmlRule =
          !("<?xml" >> *(anychar_p - '>') >> '>') // header: <?xml ... >
          >> !("<!DOCTYPE" >> *(anychar_p - '>') >> '>') // header <!DOCTYPE ... >
          >> "<DDDB>"
          >> "<catalog" >> *(anychar_p - '>') >> '>' // <catalog ... >
          >> +(
              lexeme_d[
                  "<condition"
                  >> *(
                      (
                          " name" >> *space_p >> '=' >> *space_p >> '\"'
                          >> *(anychar_p - 'M') >> 'M' >> int_p[ref(m) = arg1 - 1]
                          >> '\"'
                      )
                      | (anychar_p - '>')
                  )
                  >> '>' // <condition ... name="...M{m + 1}" ... >
              ]
              >> +(
                  lexeme_d[
                      "<paramVector"
                      >> *(
                          (
                              " name" >> *space_p >> '=' >> *space_p >> '\"'
                              >> (+(anychar_p - '\"'))[ref(param) = construct<std::string>(arg1, arg2)]
                              >> '\"'
                          )
                          | (anychar_p - '>')
                      ) >> '>' // <paramVector ... name="{param}" ... >
                  ]
                  >> +real_p // vector of values
                      [
                          if_(ref(param) == "TRParameters")
                              [ boost::phoenix::push_back(ref(cRt)   [ref(m)], arg1) ],
                          if_(ref(param) == "STParameters")
                              [ boost::phoenix::push_back(ref(cRtErr)[ref(m)], arg1) ],
                          if_(ref(param) == "TZero")
                              [ boost::phoenix::push_back(ref(cT0)   [ref(m)], arg1) ],
                          if_(ref(param) == "WalkParameters")
                              [ boost::phoenix::push_back(ref(cWalk) [ref(m)], arg1) ]
                      ]
                  >> "</paramVector>"
              )[ref(param) = ""]
              >> "</condition>"
          )[ref(m) = -1]
          >> "</catalog>"
          >> "</DDDB>"
          >> end_p;

      if(parse(xml.c_str(), xmlRule, space_p).full != true) return StatusCode::FAILURE;
    }

    for(int m = 0; m < 9; m++)
    {
      LHCb::OTChannelID id(s + 1, l, q, m + 1, 1);
      DeOTModule* module = detector->findModule(id);
      module->setStrawT0s(cT0[m]);
      OTDet::RtRelation rt(2.45, cRt[m], cRtErr[m], 50);
      module->setRtRelation(rt);
      if(cWalk[m].size() == 4)
      {
        OTDet::WalkRelation walk(cWalk[m][0], cWalk[m][1], cWalk[m][2], cWalk[m][3]);
        module->setWalkRelation(walk);
      }
      else std::cout << "skip walk initialization: npars != 4" << std::endl;
    }
  }

  return StatusCode::SUCCESS;
}
