
#include "OTModuleClbrAlg.h"

#include "GaudiUtils/Aida2ROOT.h"
#include "OTDet/DeOTModule.h"

#include "Event/Track.h"

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"

#include <vector>
#include <iostream>
#include <fstream>

#include <boost/assign.hpp>
using namespace boost::assign;

#include <boost/foreach.hpp>

OTModuleClbrAlg::OTModuleClbrAlg(const std::string& name, ISvcLocator* pSvcLocator): GaudiHistoAlg(name, pSvcLocator)
{
  detector = 0;

  declareProperty("TrackLocation", trackLocation = LHCb::TrackLocation::Default);

  declareProperty("Projector", projector);

  declareProperty("MaxTrackChi2", maxTrackChi2 = 16);
  declareProperty("MaxUnbiasedChi2", maxUnbiasedChi2 = 4);

  declareProperty("UseOutliers", useOutliers = false);

  declareProperty("HistDriftTime", confHistDriftTime);
  if(confHistDriftTime.size() != 3) confHistDriftTime = list_of(-30.0)(70.0)(200);
  declareProperty("HistDriftTimeResidual", confHistDriftTime);
  if(confHistDriftTimeResidual.size() != 3) confHistDriftTimeResidual = list_of(-50.0)(50.0)(200);
  declareProperty("HistDriftTimeVsDist", confHistDriftTime);
  if(confHistDriftTimeVsDist.size() != 6) confHistDriftTimeVsDist = list_of(-5.0)(5.0)(200)(-30.0)(70.0)(200);
  declareProperty("HistModuleDT0", confHistModuleDT0);
  if(confHistModuleDT0.size() != 3) confHistModuleDT0 = list_of(-10.0)(20.0)(120);
  declareProperty("HistModuleT0", confHistModuleT0);
  if(confHistModuleT0.size() != 3) confHistModuleT0 = list_of(-10.0)(20.0)(120);

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    histModuleDriftTime[s][l][q][m] = 0;
    histModuleDriftTimeResidual[s][l][q][m] = 0;
    histModuleDriftTimeVsDist[s][l][q][m] = 0;
  }

  declareProperty("WriteXMLs", writeXMLs = true);
  declareProperty("ReadXMLs", readXMLs = true);
}

StatusCode OTModuleClbrAlg::initialize()
{
  StatusCode statusCode = GaudiHistoAlg::initialize();

  detector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if(detector == 0) return StatusCode::FAILURE;

  projector.retrieve().ignore();

  // prepare OT elements names (stations, layers, quarters and modules)
  stationNames = list_of("T1")("T2")("T3");
  layerNames = list_of("X1")("U")("V")("X2");
  quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    std::string histPath = stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m];

/*
    histModuleDriftTime[s][l][q][m] = book(histPath + "/" + "driftTime", "drift time",
      confHistDriftTime[0], confHistDriftTime[1], (int)confHistDriftTime[2]);
*/
    histModuleDriftTimeResidual[s][l][q][m] = book(histPath + "/" + "driftTimeResidual", "drift time residual",
      confHistDriftTimeResidual[0], confHistDriftTimeResidual[1], (int)confHistDriftTimeResidual[2]);
/*
    histModuleDriftTimeVsDist[s][l][q][m] = book2D(histPath + "/" + "driftTimeVsDist", "drift time vs distance",
      confHistDriftTimeVsDist[0], confHistDriftTimeVsDist[1], (int)confHistDriftTimeVsDist[2],
      confHistDriftTimeVsDist[3], confHistDriftTimeVsDist[4], (int)confHistDriftTimeVsDist[5]);
*/
  }

  histDriftTimeVsDist = book2D("driftTimeVsDist", "drift time vs distance",
    confHistDriftTimeVsDist[0], confHistDriftTimeVsDist[1], (int)confHistDriftTimeVsDist[2],
    confHistDriftTimeVsDist[3], confHistDriftTimeVsDist[4], (int)confHistDriftTimeVsDist[5]);

  histModuleDT0VsModuleN = book("moduleDT0VsModuleN", "delta module T0 vs module N",
    -0.5, 432 - 0.5, 432);
  histModuleT0VsModuleN = book("moduleT0VsModuleN", "module T0 vs module N",
    -0.5, 432 - 0.5, 432);

  histModuleDT0 = book("moduleDT0", "delta module T0",
    confHistModuleDT0[0], confHistModuleDT0[1], (int)confHistModuleDT0[2]);
  histModuleT0 = book("moduleT0", "module T0",
    confHistModuleT0[0], confHistModuleT0[1], (int)confHistModuleT0[2]);

  statusCode = readCondXMLs();

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

      // this is realy ugly (may be use dynamic_cast, at least it will be
      // more safe)
      const LHCb::OTMeasurement* measurement = static_cast<const LHCb::OTMeasurement*>(&node->measurement());

      bool isOutlier = (node->type() == LHCb::Node::Outlier);
      LHCb::State unbiasedState = isOutlier ? fitNode->state() : fitNode->unbiasedState();

      // unbias node
      LHCb::FitNode unbiasedNode(*fitNode);
      unbiasedNode.setState(unbiasedState);
      unbiasedNode.setRefVector(unbiasedState.stateVector());
      projector->projectReference(unbiasedNode);

      // calc unbiased Chi^2 - i'm not sure that it is fully correct way
      // may be we can refit track without this node, but it will be time
      // consuming
      double residualPull = fitNode->residual() / fitNode->errResidual();
      double unbiasedChi2 = (track->chi2() - residualPull * residualPull) / (track->nDoF() - 1);

      if(trackChi2 < maxTrackChi2 && unbiasedChi2 < maxUnbiasedChi2 && (useOutliers || !isOutlier))
      {
        fillHists(*measurement, unbiasedNode, unbiasedState);
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
  statusCode = fitTR();
  if(statusCode != StatusCode::SUCCESS) return statusCode;
  statusCode = writeCondXMLs();
  if(statusCode != StatusCode::SUCCESS) return statusCode;

  return GaudiHistoAlg::finalize();
}

void OTModuleClbrAlg::fillHists(const LHCb::OTMeasurement& measurement, const LHCb::FitNode& node, const LHCb::State& state)
{
  const LHCb::OTChannelID& channel = measurement.channel();

  double driftTime = measurement.driftTimeFromY(state.y());
//  double driftDist = measurement.driftRadiusWithErrorFromY(state.y()).val;

  double dist = node.pocaVector().Dot(state.position() - measurement.trajectory().beginPoint());
  double time = measurement.module().driftTimeWithError(std::abs(dist)).val;

  double timeResidual = driftTime - time;

  int s = channel.station();
  int l = channel.layer();
  int q = channel.quarter();
  int m = channel.module();

  if(histModuleDriftTime[s-1][l][q][m-1] != 0) fill(histModuleDriftTime[s-1][l][q][m-1], driftTime, 1.0);
  if(histModuleDriftTimeResidual[s-1][l][q][m-1] != 0) fill(histModuleDriftTimeResidual[s-1][l][q][m-1], timeResidual, 1.0);
  if(histModuleDriftTimeVsDist[s-1][l][q][m-1] != 0) fill(histModuleDriftTimeVsDist[s-1][l][q][m-1], dist, driftTime, 1.0);

  fill(histDriftTimeVsDist, dist, driftTime, 1.0);
}

StatusCode OTModuleClbrAlg::fitT0s()
{
  // read current module T0's
  BOOST_FOREACH(const DeOTModule* module, detector->modules())
  {
    LHCb::OTChannelID id = module->elementID();
    t0s[id.station() - 1][id.layer()][id.quarter()][id.module() - 1] = module->strawT0(1);
  }

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

  // loop over all modules
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++) for(int m = 0; m < 9; m++)
  {
    // get histogram of drift time residuals for module
    TH1* hist = Gaudi::Utils::Aida2ROOT::aida2root(histModuleDriftTimeResidual[s][l][q][m]);
    if(hist == 0 || hist->GetEntries() < 16) continue;

    // fit it - may one should use LL fit?
    hist->Fit("gaus", "Q0+");

    TF1* func = hist->GetFunction("gaus");
    if(func == 0) continue;

    // we need only one parameter of the fit - namely position of the gaus
    double dt0 = func->GetParameter(1);
    double t0err = func->GetParError(1);

    int moduleN = m + 9 * (q + 4 * (l + 4 * s));

    histModuleDT0VsModuleN->SetBinContent(moduleN, dt0);
    histModuleDT0VsModuleN->SetBinError(moduleN, t0err);
    histModuleDT0->Fill(dt0);

    double t0 = t0s[s][l][q][m] + dt0;

    histModuleT0VsModuleN->SetBinContent(moduleN, t0);
    histModuleT0VsModuleN->SetBinError(moduleN, t0err);
    histModuleT0->Fill(t0);

    t0s[s][l][q][m] = t0;
  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::fitTR()
{
  TH2* histDriftTimeVsDist = Gaudi::Utils::Aida2ROOT::aida2root(OTModuleClbrAlg::histDriftTimeVsDist);
  if(histDriftTimeVsDist == 0) return StatusCode::FAILURE;

  TGraphErrors graphDriftTimeVsDist;

  // here we fill our graph: the error values modified a bit, and every bin
  // in the histogram is converted in one point in graph, this helps to
  // improve the speed of fit (with more entries in the hist this speed up
  // is quite sufficient)
  for(int ny = 0, nymax = histDriftTimeVsDist->GetYaxis()->GetNbins(); ny < nymax; ny++)
  for(int nx = 0, nxmax = histDriftTimeVsDist->GetXaxis()->GetNbins(); nx < nxmax; nx++)
  {
    double x = histDriftTimeVsDist->GetXaxis()->GetBinCenter(nx);
    double y = histDriftTimeVsDist->GetYaxis()->GetBinCenter(ny);

    double entries = histDriftTimeVsDist->GetBinContent(nx, ny);
    if(entries == 0) continue;

    // here base errors (0.2 mm and 2.7 ns) are fixed - it must be changed
    // to reflect TR-relation parameters
    double xerr = 0.2 / sqrt(entries);
    double yerr = 2.7 / sqrt(entries);

    graphDriftTimeVsDist.SetPoint(graphDriftTimeVsDist.GetN(), x, y);
    graphDriftTimeVsDist.SetPointError(graphDriftTimeVsDist.GetN() - 1, xerr, yerr);
  }

  // this is a fit function, it is a bit modified (a - offset, b - maximum
  // drift time, c - curvature of the TR-relation)
  TF1 funcDriftTimeVsDist(
    "funcDriftTimeVsDist",
    "[0] + [1] * abs(x) / 2.45 - 4.0 * [2] * abs(x) / 2.45 * (1.0 - abs(x) / 2.45)",
    -3.0, 3.0
  );
  funcDriftTimeVsDist.SetParameters(0.0, 36.0, 5.0);
  graphDriftTimeVsDist.Fit("funcDriftTimeVsDist", "QNR+");

  double a = funcDriftTimeVsDist.GetParameter(0);
  double b = funcDriftTimeVsDist.GetParameter(1);
  double c = funcDriftTimeVsDist.GetParameter(2);

  // convert parameters of the fit function to standard TR-relation
  // parameters
  trA = a;
  trB = b - 4.0 * c;
  trC = 4.0 * c;

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrAlg::fitTRMinuit()
{
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
    if(file.fail()) return Warning("Failed to open file: " + fileName, StatusCode::FAILURE);

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
      file << "      " << 2.7 << "\n"; // actualy one have to use value from the minuit fit
      file << "    </paramVector>\n";
      file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
      file << "      " << 0.0 << " " << trB << " " << trC << "\n";
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

#include <boost/spirit/include/classic.hpp>
using namespace boost::spirit::classic;

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
        rule<> paramVectorRule = *space_p >> *(real_p[push_back_a(values)] >> +space_p);
        parse(paramVectorContent.c_str(), paramVectorRule);

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
      LHCb::OTChannelID id(s+1, l, q, m, 1);
      DeOTModule* module = detector->findModule(id);
      module->setStrawT0s(t0s);
      OTDet::RtRelation rt(2.45, cRt, cRtErr, 50);
      module->setRtRelation(rt);

      pos = conditionPosEnd;
    }
  }

  return StatusCode::SUCCESS;
}
