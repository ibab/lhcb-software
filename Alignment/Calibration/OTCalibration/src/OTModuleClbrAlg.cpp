
#include "OTModuleClbrAlg.h"

#include "GaudiUtils/Aida2ROOT.h"
#include "OTDet/DeOTModule.h"

#include "Event/Track.h"

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TMinuit.h"

#include <vector>
#include <iostream>
#include <fstream>

#include <boost/assign.hpp>
using namespace boost::assign;

#include <boost/foreach.hpp>

#define PHOENIX_LIMIT 6
#include <boost/spirit/home/classic.hpp>
#include <boost/spirit/home/phoenix.hpp>

DECLARE_ALGORITHM_FACTORY(OTModuleClbrAlg)

OTModuleClbrAlg::OTModuleClbrAlg(const std::string& name, ISvcLocator* pSvcLocator): GaudiHistoAlg(name, pSvcLocator)
{

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<std::string> sNames= list_of("T1")("T2")("T3");
  std::vector<std::string> lNames= list_of("X1")("U")("V")("X2");
  std::vector<std::string> qNames= list_of("Q0")("Q1")("Q2")("Q3");
  std::vector<std::string> mNames= list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
  stationNames = sNames;
  layerNames = lNames;
  quarterNames = qNames;
  moduleNames = mNames;
#else
  stationNames = list_of("T1")("T2")("T3");
  layerNames = list_of("X1")("U")("V")("X2");
  quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
#endif

  detector = 0;

  // --- load options ---

  declareProperty("TrackLocation", trackLocation = LHCb::TrackLocation::Default);

  declareProperty("Projector", projector);

  declareProperty("Granularity", granularity = 432, "T0 calibration granularity (default 432 - T0's per module)");

  declareProperty("MaxTrackChi2", maxTrackChi2 = 16, "Max track Chi^2 (default 16)");
  declareProperty("MaxTrackUbChi2", maxTrackUbChi2 = 4, "Max track unbiased Chi^2 (default 4)");

  declareProperty("UseOutliers", useOutliers = false, "Use outliers (default false)");

  declareProperty("HistDriftTime", histDriftTimeOpts, "Options for drift time histograms (default t = [-30, 70]/200)");

  declareProperty("HistDriftTimeResidual", histDriftTimeResidualOpts, "Options for drift time residual histograms (default t = [-50, 50]/200)");

  declareProperty("HistDriftTimeVsDist", histDriftTimeVsDistOpts, "Options for drift time VS distance histograms (default r = [-5, 5]/200 and t = [-30, 70]/200)");

  declareProperty("HistModuleDT0", histModuleDT0Opts, "Options for module dT0's histogram (default t = [-10, 20]/120)");

  declareProperty("HistModuleT0", histModuleT0Opts, "Options for module T0's histogram (default t = [-10, 20]/120)");

  declareProperty("PlotModuleDriftTime", plotModuleDriftTime = false, "Plot drift time histograms per module (default false)");
  declareProperty("PlotModuleDriftTimeVsDist", plotModuleDriftTimeVsDist = false, "Plot drift time VS distance histograms per module (default false)");

  declareProperty("WriteXMLs", writeXMLs = true, "Write condition XML files (default true)");
  declareProperty("ReadXMLs", readXMLs = true, "Read condition XML files (default true)");

  declareProperty("UseMinuit", useMinuit = true, "Use Minuit to fit TR-relation (default true)");
  declareProperty("FitTR", fitTR = false, "(default false)");

  // Check options - reset to default if wrong or missing
  #if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<double> dtOpts= list_of(-30.0)(70.0)(200);
  std::vector<double> dtrOpts= list_of(-50.0)(50.0)(200);
  std::vector<double> dtvdOpts= list_of(-5.0)(5.0)(200)(-30.0)(70.0)(200);
  std::vector<double> dt0Opts= list_of(-10.0)(20.0)(120);
  std::vector<double> t0Opts= list_of(-10.0)(20.0)(120);
  if(histDriftTimeOpts.size() != 3) histDriftTimeOpts= dtOpts;
  if(histDriftTimeResidualOpts.size() != 3) histDriftTimeResidualOpts = dtrOpts;
  if(histDriftTimeVsDistOpts.size() != 6) histDriftTimeVsDistOpts = dtvdOpts;
  if(histModuleDT0Opts.size() != 3) histModuleDT0Opts = dt0Opts;
  if(histModuleT0Opts.size() != 3) histModuleT0Opts = t0Opts;
#else
  if(histDriftTimeOpts.size() != 3) histDriftTimeOpts= list_of(-30.0)(70.0)(200);
  if(histDriftTimeResidualOpts.size() != 3) histDriftTimeResidualOpts = list_of(-50.0)(50.0)(200);
  if(histDriftTimeVsDistOpts.size() != 6) histDriftTimeVsDistOpts = list_of(-5.0)(5.0)(200)(-30.0)(70.0)(200);
  if(histModuleDT0Opts.size() != 3) histModuleDT0Opts = list_of(-10.0)(20.0)(120);
  if(histModuleT0Opts.size() != 3) histModuleT0Opts = list_of(-10.0)(20.0)(120);
#endif

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    histModuleDriftTime[s][l][q][m] = 0;
    histModuleDriftTimeResidual[s][l][q][m] = 0;
    histModuleDriftTimeVsDist[s][l][q][m] = 0;
  }

  trA = trB = trC = trSigmaA = trSigmaB = trSigmaC = 0;
}

StatusCode OTModuleClbrAlg::initialize()
{
  StatusCode statusCode = GaudiHistoAlg::initialize();

  // --- print options ---

  info() << "TrackLocation: " << trackLocation << endmsg;
  info() << "Granularity: " << granularity << endmsg;
  info() << "MaxTrackChi2: " << maxTrackChi2 << endmsg;
  info() << "MaxTrackUbChi2: " << maxTrackUbChi2 << endmsg;
  info() << "HistDriftTime: " <<
    "t = [" << histDriftTimeOpts[0] << ", " << histDriftTimeOpts[1] << "]/" << (int)histDriftTimeOpts[2] << endmsg;
  info() << "HistDriftTimeResidual: " <<
    "t = [" << histDriftTimeResidualOpts[0] << ", " << histDriftTimeResidualOpts[1] << "]/" << (int)histDriftTimeResidualOpts[2] << endmsg;
  info() << "HistDriftTimeVsDist: " <<
    "r = [" << histDriftTimeVsDistOpts[0] << ", " << histDriftTimeVsDistOpts[1] << "]/" << (int)histDriftTimeVsDistOpts[2] <<
    ", t = [" << histDriftTimeVsDistOpts[3] << ", " << histDriftTimeVsDistOpts[4] << "]/" << (int)histDriftTimeVsDistOpts[5] << endmsg;
  info() << "HistModuleDT0: " <<
    "t = [" << histModuleDT0Opts[0] << ", " << histModuleDT0Opts[1] << "]/" << (int)histModuleDT0Opts[2] << endmsg;
  info() << "HistModuleT0: " <<
    "t = [" << histModuleT0Opts[0] << ", " << histModuleT0Opts[1] << "]/" << (int)histModuleT0Opts[2] << endmsg;
  info() << "WriteXMLs: " << (writeXMLs ? "true" : "false") << endmsg;
  info() << "ReadXMLs: " << (readXMLs ? "true" : "false") << endmsg;
  info() << "UseMinuit: " << (useMinuit ? "true" : "false") << endmsg;
  info() << "FitTR: " << (fitTR ? "true" : "false") << endmsg;

  detector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if(detector == 0) return StatusCode::FAILURE;

  projector.retrieve().ignore();

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m];

    if(plotModuleDriftTime)
    {
      histModuleDriftTime[s][l][q][m] = book(histPath + "/" + "driftTime", "drift time",
        histDriftTimeOpts[0], histDriftTimeOpts[1], (int)histDriftTimeOpts[2]);
    }

    histModuleDriftTimeResidual[s][l][q][m] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

    if(plotModuleDriftTimeVsDist)
    {
      histModuleDriftTimeVsDist[s][l][q][m] = book2D(histPath + "/" + "driftTimeVsDist", "drift time vs distance",
        histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
        histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);
    }
  }

  // book drift time residual histograms for quarters
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q];
    histQuarterDriftTimeResidual[s][l][q] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
  }

  // book drift time residual histograms for layers
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l];
    histLayerDriftTimeResidual[s][l] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
  }

  // book drift time residual histograms for stations
  for(int s = 0; s < 3; s++)
  {
    std::string histPath = stationNames[s];
    histStationDriftTimeResidual[s] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
  }

  histDriftTimeVsDist = book2D("driftTimeVsDist", "drift time vs distance",
    histDriftTimeVsDistOpts[0], histDriftTimeVsDistOpts[1], (int)histDriftTimeVsDistOpts[2],
    histDriftTimeVsDistOpts[3], histDriftTimeVsDistOpts[4], (int)histDriftTimeVsDistOpts[5]);

  histModuleDT0VsModuleN = book("moduleDT0VsModuleN", "delta module T0 vs module N",
    -0.5, 432 - 0.5, 432);
  histModuleT0VsModuleN = book("moduleT0VsModuleN", "module T0 vs module N",
    -0.5, 432 - 0.5, 432);

  histModuleDT0 = book("moduleDT0", "delta module T0",
    histModuleDT0Opts[0], histModuleDT0Opts[1], (int)histModuleDT0Opts[2]);
  histModuleT0 = book("moduleT0", "module T0",
    histModuleT0Opts[0], histModuleT0Opts[1], (int)histModuleT0Opts[2]);

  if(readXMLs) statusCode = readCondXMLs();

  return statusCode;
}

StatusCode OTModuleClbrAlg::execute()
{
  LHCb::Tracks* tracks = exist<LHCb::Tracks>(trackLocation) ? get<LHCb::Tracks>(trackLocation) : 0;
  if(tracks == 0) return StatusCode::SUCCESS;

  BOOST_FOREACH(const LHCb::Track* track, *tracks)
  {
    if(track->fitStatus() != LHCb::Track::Fitted || track->nDoF() < 4) continue;

    double trackChi2 = track->chi2() / track->nDoF();

    BOOST_FOREACH(const LHCb::Node* node, track->nodes())
    {
      if(node->type() != LHCb::Node::HitOnTrack && node->type() != LHCb::Node::Outlier) continue;
      if(node->measurement().type() != LHCb::Measurement::OT) continue;

      const LHCb::FitNode* fitNode = dynamic_cast<const LHCb::FitNode*>(node);
      if(fitNode == 0) continue; // for safety

      const LHCb::OTMeasurement* measurement = dynamic_cast<const LHCb::OTMeasurement*>(&node->measurement());
      if(measurement == 0) continue; // for safety

      bool isOutlier = (node->type() == LHCb::Node::Outlier);
      LHCb::State unbiasedState = isOutlier ? fitNode->state() : fitNode->unbiasedState();

      // unbias node
      LHCb::FitNode unbiasedNode(*fitNode);
      unbiasedNode.setState(unbiasedState);
      unbiasedNode.setRefVector(unbiasedState.stateVector());
      projector->projectReference(unbiasedNode);

      // calc unbiased Chi^2
      double residualPull = fitNode->residual() / fitNode->errResidual();
      double unbiasedChi2 = (track->chi2() - residualPull * residualPull) / (track->nDoF() - 1);

      if(trackChi2 < maxTrackChi2 && unbiasedChi2 < maxTrackUbChi2 && (useOutliers || !isOutlier))
      {
        const LHCb::OTChannelID& channel = measurement->channel();

        int s = channel.station();
        int l = channel.layer();
        int q = channel.quarter();
        int m = channel.module();

        double driftTime = measurement->driftTimeFromY(unbiasedState.y());
        if(histModuleDriftTime[s-1][l][q][m-1] != 0) fill(histModuleDriftTime[s-1][l][q][m-1], driftTime, 1.0);

        LHCb::FitNode node(*fitNode);
        LHCb::State state = fitNode->state();
        node.setState(state);
        node.setRefVector(state.stateVector());
        projector->projectReference(node);

        double dist = node.pocaVector().Dot(state.position() - measurement->trajectory().beginPoint());
        double time = measurement->module().driftTimeWithError(std::abs(dist)).val;
        double driftTimeResidual = driftTime - time;

        fill(histModuleDriftTimeResidual[s-1][l][q][m-1], driftTimeResidual, 1.0);
        fill(histQuarterDriftTimeResidual[s-1][l][q], driftTimeResidual, 1.0);
        fill(histLayerDriftTimeResidual[s-1][l], driftTimeResidual, 1.0);
        fill(histStationDriftTimeResidual[s-1], driftTimeResidual, 1.0);

        double unbiasedDist = unbiasedNode.pocaVector().Dot(unbiasedState.position() - measurement->trajectory().beginPoint());

        if(histModuleDriftTimeVsDist[s-1][l][q][m-1] != 0) fill(histModuleDriftTimeVsDist[s-1][l][q][m-1], unbiasedDist, driftTime, 1.0);

        fill(histDriftTimeVsDist, unbiasedDist, driftTime, 1.0);
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::finalize()
{
  StatusCode statusCode = GaudiHistoAlg::initialize();

  statusCode = fitT0s();
  if(statusCode != StatusCode::SUCCESS) return statusCode;

  if(fitTR)
  {
    if(useMinuit) statusCode = fitTRMinuit();
    else statusCode = fitTRSimple();
    if(statusCode != StatusCode::SUCCESS) return statusCode;
  }

  if(writeXMLs) statusCode = writeCondXMLs();
  if(statusCode != StatusCode::SUCCESS) return statusCode;

  return GaudiHistoAlg::finalize();
}

StatusCode OTModuleClbrAlg::fitT0s()
{

  // read current module T0's
  BOOST_FOREACH(const DeOTModule* module, detector->modules())
  {
    if(module != 0)
    {
      LHCb::OTChannelID id = module->elementID();
      t0s[id.station() - 1][id.layer()][id.quarter()][id.module() - 1] = module->strawT0(1);

      tcoeff = module->rtRelation().tcoeff();
      terrcoeff = module->rtRelation().terrcoeff();
    }
  }

  double dt0s[3][4][4][9]; memset(dt0s, 0, 432 * sizeof(double));
  double dt0errs[3][4][4][9]; memset(dt0errs, 0, 432 * sizeof(double));

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    TH1* hist = 0;

    switch(granularity)
    {
    case 3:
      hist = Gaudi::Utils::Aida2ROOT::aida2root(histStationDriftTimeResidual[s]);
      break;
    case 12:
      hist = Gaudi::Utils::Aida2ROOT::aida2root(histLayerDriftTimeResidual[s][l]);
      break;
    case 48:
      hist = Gaudi::Utils::Aida2ROOT::aida2root(histQuarterDriftTimeResidual[s][l][q]);
      break;
    case 432:
    default:
      hist = Gaudi::Utils::Aida2ROOT::aida2root(histModuleDriftTimeResidual[s][l][q][m]);
    }

    if(hist == 0 || hist->GetEntries() < 16) continue;

    hist->Fit("gaus", "Q0+");

    TF1* func = hist->GetFunction("gaus");
    if(func == 0) continue;

    dt0s[s][l][q][m] = func->GetParameter(1);
    dt0errs[s][l][q][m] = func->GetParError(1);
  }

  double t0mean = 0;
  double dt0mean = 0;
/*
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    t0mean += t0s[s][l][q][m];
    dt0mean += dt0s[s][l][q][m];
  }
*/
  t0mean /= 432.0;
  dt0mean /= 432.0;

  TH1* histModuleDT0VsModuleN = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histModuleDT0VsModuleN);
  if(histModuleDT0VsModuleN == 0) return StatusCode::FAILURE;
  histModuleDT0VsModuleN->GetYaxis()->SetRangeUser(-10.0, 20.0);
  TH1* histModuleDT0 = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histModuleDT0);
  if(histModuleDT0 == 0) return StatusCode::FAILURE;

  TH1* histModuleT0VsModuleN = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histModuleT0VsModuleN);
  if(histModuleT0VsModuleN == 0) return StatusCode::FAILURE;
  histModuleDT0VsModuleN->GetYaxis()->SetRangeUser(-10.0, 20.0);
  TH1* histModuleT0 = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histModuleT0);
  if(histModuleT0 == 0) return StatusCode::FAILURE;

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    double t0 = t0s[s][l][q][m] - t0mean;
    double dt0 = dt0s[s][l][q][m] - dt0mean;
    double dt0err = dt0errs[s][l][q][m];

    int moduleN = m + 9 * (q + 4 * (l + 4 * s));

    histModuleDT0VsModuleN->SetBinContent(moduleN, dt0);
    histModuleDT0VsModuleN->SetBinError(moduleN, dt0err);
    histModuleDT0->Fill(dt0);

    t0 += dt0;

    histModuleT0VsModuleN->SetBinContent(moduleN, t0);
    histModuleT0VsModuleN->SetBinError(moduleN, dt0err);
    histModuleT0->Fill(t0);

    t0s[s][l][q][m] = t0;
  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::fitTRSimple()
{
  TH2* histDriftTimeVsDist = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histDriftTimeVsDist);
  if(histDriftTimeVsDist == 0) return StatusCode::FAILURE;

  TGraphErrors graphDriftTimeVsDist;

  for(int ny = 0, nymax = histDriftTimeVsDist->GetYaxis()->GetNbins(); ny < nymax; ny++)
  for(int nx = 0, nxmax = histDriftTimeVsDist->GetXaxis()->GetNbins(); nx < nxmax; nx++)
  {
    double x = histDriftTimeVsDist->GetXaxis()->GetBinCenter(nx);
    double y = histDriftTimeVsDist->GetYaxis()->GetBinCenter(ny);

    double entries = histDriftTimeVsDist->GetBinContent(nx, ny);
    if(entries == 0) continue;

    double xerr = 0.2 / std::sqrt(entries);
    double yerr = 2.7 / std::sqrt(entries);

    graphDriftTimeVsDist.SetPoint(graphDriftTimeVsDist.GetN(), x, y);
    graphDriftTimeVsDist.SetPointError(graphDriftTimeVsDist.GetN() - 1, xerr, yerr);
  }

  TF1 funcDriftTimeVsDist(
    "funcDriftTimeVsDist",
    "[0] + [1] * abs(x) / 2.45 - 4.0 * [2] * abs(x) / 2.45 * (1.0 - abs(x) / 2.45)",
    -3.0, 3.0
  );
  funcDriftTimeVsDist.SetParameters(0.0, 36.0, 5.0);
  graphDriftTimeVsDist.Fit("funcDriftTimeVsDist", "QNR");

//  double t0 = funcDriftTimeVsDist.GetParameter(0);
  double tm = funcDriftTimeVsDist.GetParameter(1);
  double tc = funcDriftTimeVsDist.GetParameter(2);

  trA = 0.0;
  trB = tm - 4.0 * tc;
  trC = 4.0 * tc;

  trSigmaA = 2.7;
  trSigmaB = 0.0;
  trSigmaC = 0.0;

  return StatusCode::SUCCESS;
}

TH2D* histTVsR = 0;

void fMinuit(int&, double*, double& result, double* par, int)
{
  static const double sqrt2pi = std::sqrt(2.0 * 3.1415927);

  static const double rmax = 2.45;
  static const double rmax2 = rmax * rmax;

  // "Gaussian Quadrature" coefficiens used for integration
  static const int M = 10;
  static const double weights[M] = {  0.29552422, 0.29552422,  0.26926672, 0.26926672,  0.21908636, 0.21908636,  0.14945135, 0.14945135,  0.06667134, 0.06667134};
  static const double offsets[M] = { -0.14887434, 0.14887434, -0.43339539, 0.43339539, -0.67940957, 0.67940957, -0.86506337, 0.86506337, -0.97390653, 0.97390653};

  double t0 = par[0];
  double tm = par[1];
  double tc = par[2];
  double sigmaRA = par[3];
  double sigmaRB = par[4];
  double sigmaT = par[5];
  double rho = par[6];
  double sigmaRBG = par[7];
  double bg = par[8];

  result = 0;

  double ipr_ = 0;
  {
    const int N = 10;
    const double dr_ = rmax / N;
    for(int i = 0; i < N; i++) for(int m = 0; m < M; m++)
    {
      double r_ = dr_ * (2 * i + 1) - rmax;
      r_ += offsets[m] * dr_;

      ipr_ += weights[m] * ( 1.0 - exp( - 2.0 * rho * std::sqrt(rmax2 - r_ * r_) ) );
    }
    ipr_ *= dr_;
  }

  for(int nr = 0, nrmax = histTVsR->GetXaxis()->GetNbins(); nr < nrmax; nr++) for(int nt = 0, ntmax = histTVsR->GetYaxis()->GetNbins(); nt < ntmax; nt++)
  {
    double scale = histTVsR->GetBinContent(nr, nt);
    if(scale == 0) continue;

    double r = histTVsR->GetXaxis()->GetBinCenter(nr);
    double t = histTVsR->GetYaxis()->GetBinCenter(nt);

    double ptr = 0;
    {
      const int N = 6;
      const double dr_ = rmax / N;
      for(int i = 0; i < N; i++) for(int m = 0; m < M; m++)
      {
        double r_ = dr_ * (2 * i + 1) - rmax;
        r_ += offsets[m] * dr_;

        double r0_ = fabs(r_) / rmax;

        double pr0 = 1.0 - exp( - 2.0 * rho * std::sqrt(rmax2 - r_ * r_) );

        double sigmaR = sigmaRA * (1.0 - r0_) + sigmaRB * r0_;
        double pr = exp( -0.5 * std::pow((r - r_) / sigmaR, 2) ) / sigmaR;
        double prbg = exp( -0.5 * std::pow((r - r_) / sigmaRBG, 2) ) / sigmaRBG;

        double t_ = t0 + tm * r0_ - 4.0 * tc * r0_ * (1 - r0_);
        sigmaT = ( (tm - 4.0 * tc) + 8.0 * tc * r0_ ) * sigmaR / rmax;
        double pt = exp( -0.5 * std::pow((t - t_) / sigmaT, 2) ) / sigmaT;

        ptr += weights[m] * pr0 * ((1.0 - bg) * pr + bg * prbg) * pt;
      }
      ptr /= sqrt2pi * sqrt2pi;
      ptr *= dr_;
    }

    result -= scale * log(ptr / ipr_);
  }
}

StatusCode OTModuleClbrAlg::fitTRMinuit()
{
  TMinuit minuit(9);

  minuit.SetPrintLevel(-1);

  histTVsR = Gaudi::Utils::Aida2ROOT::aida2root(histDriftTimeVsDist);
  minuit.SetFCN(fMinuit);

  minuit.DefineParameter(0, "t_0",        0.00,   0.1,  -10.0, 10.0);
  minuit.DefineParameter(1, "t_m",       35.00,   0.1,   25.0, 50.0);
  minuit.DefineParameter(2, "t_c",        3.00,   0.1,    0.0, 10.0);

  minuit.DefineParameter(3, "sigmaRA",    0.30,   0.01,   0.1,  1.0);
  minuit.DefineParameter(4, "sigmaRB",    0.20,   0.01,   0.1,  1.0);
  minuit.DefineParameter(5, "sigmaT",     2.50,   0.1,    0.1,  5.0); minuit.FixParameter(5);

  minuit.DefineParameter(6, "rho",        0.70,   0.1,    0.1,  5.0);

  minuit.DefineParameter(7, "sigmaRBG",   1.00,   0.01,   0.5,  5.0);
  minuit.DefineParameter(8, "bg",         0.001,  0.001,  0.0,  1.0);

  minuit.SetErrorDef(0.5);

  minuit.mnmigr();

  double t0, t0err;
  minuit.GetParameter(0, t0, t0err);
  double tm, tmerr;
  minuit.GetParameter(1, tm, tmerr);
  double tc, tcerr;
  minuit.GetParameter(2, tc, tcerr);
  double sigmaRA, sigmaRAerr;
  minuit.GetParameter(3, sigmaRA, sigmaRAerr);
  double sigmaRB, sigmaRBerr;
  minuit.GetParameter(4, sigmaRB, sigmaRBerr);
  double rho, rhoErr;
  minuit.GetParameter(6, rho, rhoErr);
  double sigmaRBG, sigmaRBGErr;
  minuit.GetParameter(7, sigmaRBG, sigmaRBGErr);
  double bg, bgErr;
  minuit.GetParameter(8, bg, bgErr);

  static const double rmax = 2.45;

  info() << "t0 = " << t0 << endmsg;
  info() << "tm = " << tm << endmsg;
  info() << "tc = " << tc << endmsg;
  info() << "sigmaRA = " << sigmaRA << endmsg;
  info() << "sigmaRB = " << sigmaRB << endmsg;
  info() << "rho = " << rho << endmsg;
  info() << "sigmaRBG = " << sigmaRBG << endmsg;
  info() << "bg = " << bg << endmsg;

  trA = t0;
  trB = tm - 4.0 * tc;
  trC = 4.0 * tc;

  trSigmaA = trB * sigmaRA / rmax;
  trSigmaB = trB * (sigmaRB - sigmaRA) / rmax + 2.0 * trC * sigmaRA / rmax;
  trSigmaC = 2.0 * trC * (sigmaRB - sigmaRA) / rmax;

  info() << "trA = " << trA << endmsg;
  info() << "trB = " << trB / 2.45 << " * 2.45" << endmsg;
  info() << "trC = " << trC / (2.45 * 2.45) << " * 2.45 * 2.45" << endmsg;
  info() << "trSigmaA = " << trSigmaA << endmsg;
  info() << "trSigmaB = " << trSigmaB / 2.45 << " * 2.45" << endmsg;
  info() << "trSigmaC = " << trSigmaC / (2.45 * 2.45) << " * 2.45 * 2.45" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::writeCondXMLs()
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = quarterId + "@" + prefix + ".xml";

    std::ofstream file(fileName.c_str());
    if(file.fail())
    {
      warning() << "Can't open file: '" << fileName << "'" << endmsg;
      continue;
    }

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
    file << "\n";
    file << "<DDDB>\n";
    file << "<catalog name=\"" << prefix << quarterId << "\">\n";

    for(int m = 0; m < 9; m++)
    {
      std::string moduleId = quarterId + moduleNames[m];

      file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

      file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
      file << "      ";
      if(fitTR) file << trSigmaA << " " << trSigmaB << " " << trSigmaC << "\n";
      else for(size_t i = 0; i < terrcoeff.size(); i++) file << terrcoeff[i] << " ";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
      file << "      ";
      if(fitTR) file << 0.0 /*trA*/ << " " << trB << " " << trC << "\n";
      else for(size_t i = 0; i < tcoeff.size(); i++) file << tcoeff[i] << " ";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
      file << "      " << t0s[s][l][q][m] << "\n";
      file << "    </paramVector>\n";

      file << "  </condition>\n";
    }

    file << "</catalog>\n";
    file << "</DDDB>\n";

    file.flush();
    file.close();
  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::readCondXMLs()
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

    std::vector<double> cRt[9], cRtErr[9], cT0[9];

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
                              >> (+(anychar_p - '\"'))[boost::phoenix::ref(param) = construct<std::string>(arg1, arg2)]
                              >> '\"'
                          )
                          | (anychar_p - '>')
                      ) >> '>' // <paramVector ... name="{param}" ... >
                  ]
                  >> +real_p // vector of values
                      [
                          if_(boost::phoenix::ref(param) == "TRParameters")
                              [ boost::phoenix::push_back(boost::phoenix::ref(cRt)   [boost::phoenix::ref(m)], arg1) ],
                          if_(boost::phoenix::ref(param) == "STParameters")
                              [ boost::phoenix::push_back(boost::phoenix::ref(cRtErr)[boost::phoenix::ref(m)], arg1) ],
                          if_(boost::phoenix::ref(param) == "TZero")
                              [ boost::phoenix::push_back(boost::phoenix::ref(cT0)   [boost::phoenix::ref(m)], arg1) ]
                      ]
                  >> "</paramVector>"
              )[boost::phoenix::ref(param) = ""]
              >> "</condition>"
          )[boost::phoenix::ref(m) = -1]
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
      OTDet::RtRelation rt(2.45, cRt[m], cRtErr[m]);
      module->setRtRelation(rt);
    }
  }

  return StatusCode::SUCCESS;
}

/*
StatusCode OTModuleClbrAlg::readCondXMLs()
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = quarterId + "@" + prefix + ".xml";

    // load file with condition for specific station-layer-quarter
    std::ifstream file(fileName.c_str());
    if(file.fail()) continue;

    std::string xml;

    // read file to string
    file.seekg(0, std::ios_base::end);
    xml.resize(file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&xml.at(0), xml.size());
    file.close();

    unsigned int pos = 0;
    while(true)
    {
      // ok ... here were start to parse xml with condition:
      //   - first we try to find begin point - "<condition ... >"
      //   - then we extract name of this condition to get module number
      unsigned int conditionPos = xml.find("<condition", pos);
      if(conditionPos == (unsigned int)-1) break;
      unsigned int conditionPosEnd = xml.find("</condition>", conditionPos);
      if(conditionPosEnd == (unsigned int)-1) return Warning("Malformed xml file.");
      unsigned int conditionNamePos = xml.find("name=\"", conditionPos) + 6;
      if(conditionNamePos == (unsigned int)-1) return Warning("Malformed xml file.");
      unsigned int conditionNamePosEnd = xml.find("\"", conditionNamePos);
      if(conditionNamePosEnd == (unsigned int)-1) return Warning("Malformed xml file.");
      std::string conditionName = xml.substr(conditionNamePos, conditionNamePosEnd - conditionNamePos);
      if(conditionName.size() == 0) return Warning("Malformed xml file.");
      int m = (int)(conditionName.at(conditionName.size() - 1) - '0');

      // TR-realtion parameters, error coef's and T0's
      std::vector<double> cRt, cRtErr, t0s;

      pos = conditionPos;
      while(true)
      {
        // here we parse paramVecotor's:
        //   - also one have to extract name of the param
        //   - and also one have to parse content (which is actualy vector
        //     of doubles)
        unsigned int paramVectorPos = xml.find("<paramVector", pos);
        if(paramVectorPos == (unsigned int)-1 || paramVectorPos > conditionPosEnd) break;
        unsigned int paramVectorPosEnd = xml.find("</paramVector>", paramVectorPos);
        if(paramVectorPosEnd == (unsigned int)-1) return Warning("Malformed xml file.");
        unsigned int paramVectorNamePos = xml.find("name=\"", paramVectorPos) + 6;
        if(paramVectorNamePos == (unsigned int)-1) return Warning("Malformed xml file.");
        unsigned int paramVectorNamePosEnd = xml.find("\"", paramVectorNamePos);
        if(paramVectorNamePosEnd == (unsigned int)-1) return Warning("Malformed xml file.");
        std::string paramVectorName = xml.substr(paramVectorNamePos, paramVectorNamePosEnd - paramVectorNamePos);
        unsigned int paramVectorContentPos = xml.find(">", paramVectorPos) + 1;
        if(paramVectorContentPos == (unsigned int)-1) return Warning("Malformed xml file.");
        unsigned int paramVectorContentPosEnd = xml.find(">", paramVectorContentPos);
        if(paramVectorContentPosEnd == (unsigned int)-1) return Warning("Malformed xml file.");
        std::string paramVectorContent = xml.substr(paramVectorContentPos, paramVectorContentPosEnd - paramVectorContentPos);

        // this is actualy a bit strange way to use spirit (only to parse
        // sequence of doubles), may be we should use it to parse the
        // whole xml
        std::vector<double> values;

        {
          using namespace boost::spirit::classic;
          rule<> paramVectorRule = *space_p >> *(real_p[push_back_a(values)] >> +space_p);
          parse(paramVectorContent.c_str(), paramVectorRule);
        }

        // now depending on the name of the param we save values in
        // different variables
        if(paramVectorName == "TRParameters") cRt = values;
        if(paramVectorName == "STParameters") cRtErr = values;
        if(paramVectorName == "TZero") t0s = values;

        pos = paramVectorPosEnd;
      }

      // create OTChannelID and use it find module in detector, and then
      // set T0's and TR-relation
      // right now T0's are the same for all straws (need to add some sort
      // of configuration option to be able to use different values of T0's
      // for straws or may be OTIS's)
      LHCb::OTChannelID id(s + 1, l, q, m, 1);
      DeOTModule* module = detector->findModule(id);
      module->setStrawT0s(t0s);
      OTDet::RtRelation rt(2.45, cRt, cRtErr, 50);
      module->setRtRelation(rt);

      pos = conditionPosEnd;
    }
  }

  return StatusCode::SUCCESS;
}
*/
