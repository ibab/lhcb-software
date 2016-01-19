#include "OTModuleClbrMon.h"

#include <GaudiKernel/AlgFactory.h>

#include <Event/ODIN.h>

#include <Event/Track.h>
#include <Event/Measurement.h>
#include <Event/OTMeasurement.h>
#include <Event/State.h>
#include <Event/Node.h>
#include <Event/FitNode.h>

#include <DetDesc/Condition.h>

using namespace LHCb;

#include <iostream>
#include <fstream>

#include <boost/preprocessor/seq.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>

#include <boost/assign.hpp>
using namespace boost::assign;

#define PHOENIX_LIMIT 6
#include <boost/spirit/home/classic.hpp>
#include <boost/spirit/home/phoenix.hpp>

DECLARE_ALGORITHM_FACTORY(OTModuleClbrMon)

OTModuleClbrMon::OTModuleClbrMon(const std::string& name, ISvcLocator* pSvcLocator): GaudiTupleAlg(name, pSvcLocator),
  projector("TrajOTProjector"),
  decoder("OTRawBankDecoder")
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

  declareProperty("HistDriftTimeResidual", histDriftTimeResidualOpts, "Options for drift time residual histograms (default t = [-25, 25]/200)");

  declareProperty("HistDriftTimeVsDist", histDriftTimeVsDistOpts, "Options for drift time VS distance histograms (default r = [-5, 5]/200 and t = [-30, 70]/200)");

  declareProperty("HistResidual", histResidualOpts, "Options for residual histograms (default r = [-2.5, 2.5]/50)");
  declareProperty("Simulation", simulation = false, " Is simulation or data (default false, so data)");
  declareProperty("Apply_Calibration", Apply_Calibration = true, " Apply_Calibration - in xml (by now) or in DB (default false,)");
  declareProperty("Save_Fits", save_fits = true, " Save fitted gaussian (default false,)");

  declareProperty("OTIS_calibration", OTIS_calibration = true, "OTIS calibration (instead of half module left and right)");
  declareProperty("OTIS_LR_calibration", OTIS_LR_calibration = false, "OTIS calibration, left and right contributions");

  declareProperty("Verbose", verbose = true, " Verbose, for debugging (default false,)");
  //  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/group/online/alignment/" );
  // declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online" );
  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online/OT/results.xml" );
  declareProperty("xmlFileName"   ,  m_xmlFileName  = "results.xml" );

  //boost::filesystem::path full_path("/afs/cern.ch/user/l/lgrillo/databases_for_online/OT/results.xml");
  
  // Check options - reset to default if wrong or missing
  #if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<double> dtOpts= list_of(-5.0)(45.0)(200);
  std::vector<double> dtrOpts= list_of(-25.0)(25.0)(200);
  std::vector<double> dtvdOpts= list_of(-3.0)(3.0)(120)(-5.0)(45.0)(200);
  std::vector<double> resOpts= list_of(-5.0)(5.0)(100);
  if(histDriftTimeOpts.size() != 3) histDriftTimeOpts= dtOpts;
  if(histDriftTimeResidualOpts.size() != 3) histDriftTimeResidualOpts = dtrOpts;
  if(histDriftTimeVsDistOpts.size() != 6) histDriftTimeVsDistOpts = dtvdOpts;
  if(histResidualOpts.size() != 3) histResidualOpts = resOpts;
#else
  if(histDriftTimeOpts.size() != 3) histDriftTimeOpts= list_of(-5.0)(45.0)(200);
  if(histDriftTimeResidualOpts.size() != 3) histDriftTimeResidualOpts = list_of(-25.0)(25.0)(200);
  if(histDriftTimeVsDistOpts.size() != 6) histDriftTimeVsDistOpts = list_of(-3.0)(3.0)(120)(-5.0)(45.0)(200);
  if(histResidualOpts.size() != 3) histResidualOpts = list_of(-5.0)(5.0)(100);
#endif
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
  info() << "Simulation: " << (simulation ? "True" : "False") << endmsg;
  info() << "Apply_Calibration: " << (Apply_Calibration ? "True" : "False") << endmsg;
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

    histModuleDriftTimeResidual_otis0_L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis0_L", "drift time residual (otis0 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis0_R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis0_R", "drift time residual (otis0 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis1_L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis1_L", "drift time residual (otis1 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis1_R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis1_R", "drift time residual (otis1 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis2_L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis2_L", "drift time residual (otis2 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis2_R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis2_R", "drift time residual (otis2 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis3_L[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis3_L", "drift time residual (otis3 && dist < 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);
    histModuleDriftTimeResidual_otis3_R[s][l][q][m] = book(histPath + "/" + "driftTimeResidual_otis3_R", "drift time residual (otis3 && dist > 0)",
      histDriftTimeResidualOpts[0], histDriftTimeResidualOpts[1], (int)histDriftTimeResidualOpts[2]);

    //   std::cout << "book mY module histograms" << std::endl;

    TString name_station [3]= {"station_0", "station_1", "station_2"};
    TString name_layer [4]= {"layer_0", "layer_1", "layer_2", "layer_3"};
    TString name_quarter [4]= {"quarter_0", "quarter_1", "quarter_2", "quarter_3"};
    TString name_module [9]= {"module_0", "module_1", "module_2","module_3", "module_4", "module_5", "module_6", "module_7", "module_8"};

    m_histModuleDriftTimeResidual[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual",200, -25, 25);

    m_histModuleDriftTimeResidual01L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual01L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis01 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual01R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual01R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis01 && dist > 0)",200, -25, 25);
    m_histModuleDriftTimeResidual23L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual23L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis23 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual23R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual23R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis23 && dist > 0)",200, -25, 25);


    m_histModuleDriftTimeResidual_otis0_L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis0_L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis0_R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis0_R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist > 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis1_L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis1_L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis1_R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis1_R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist > 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis2_L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis2_L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis2_R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis2_R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist > 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis3_L[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis3_L"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist < 0)",200, -25, 25);
    m_histModuleDriftTimeResidual_otis3_R[s][l][q][m] = new TH1D("m_histModuleDriftTimeResidual_otis3_R"+name_station[s]+name_layer[l]+name_quarter[q]+name_module[m], "my drift time residual (otis0 && dist > 0)",200, -25, 25);


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


  //-- TMP Monitoring hists ..."                                                                                                                   
           
  hdt0 = new TH1D("hdt0", "hdt0", 432, 0, 432);
  ht0 = new TH1D("ht0", "ht0", 432, 0, 432);
  hdt0proj = new TH1D("hdt0proj", "hdt0proj", 100, -10, 10);

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

  //readCondDB();

  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));
  if(readXMLs) statusCode = readCondXMLs(t0s);
  //else statusCode = readCondDB(t0s);
  else statusCode = read_Globalt0(t0s);

  return statusCode;
}

StatusCode OTModuleClbrMon::execute()
{
  double ts[3][4][4][9]; memset(ts, 0, sizeof(ts));
  if(readXMLs)
  {
    readCondXMLs(ts);
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
	if(OTIS_LR_calibration){
	  if(otis == 0 && dist < 0) fill(histModuleDriftTimeResidual_otis0_L[s][l][q][m], timeResidual, 1.0);
	  if(otis == 1 && dist < 0) fill(histModuleDriftTimeResidual_otis1_L[s][l][q][m], timeResidual, 1.0);
	  if(otis == 2 && dist < 0) fill(histModuleDriftTimeResidual_otis2_L[s][l][q][m], timeResidual, 1.0);
	  if(otis == 3 && dist < 0) fill(histModuleDriftTimeResidual_otis3_L[s][l][q][m], timeResidual, 1.0);

	  if(otis == 0 && dist > 0) fill(histModuleDriftTimeResidual_otis0_R[s][l][q][m], timeResidual, 1.0);
	  if(otis == 1 && dist > 0) fill(histModuleDriftTimeResidual_otis1_R[s][l][q][m], timeResidual, 1.0);
	  if(otis == 2 && dist > 0) fill(histModuleDriftTimeResidual_otis2_R[s][l][q][m], timeResidual, 1.0);
	  if(otis == 3 && dist > 0) fill(histModuleDriftTimeResidual_otis3_R[s][l][q][m], timeResidual, 1.0);

	  if(otis == 0 && dist < 0) m_histModuleDriftTimeResidual_otis0_L[s][l][q][m]->Fill(timeResidual);
	  if(otis == 1 && dist < 0) m_histModuleDriftTimeResidual_otis1_L[s][l][q][m]->Fill(timeResidual);
	  if(otis == 2 && dist < 0) m_histModuleDriftTimeResidual_otis2_L[s][l][q][m]->Fill(timeResidual);
	  if(otis == 3 && dist < 0) m_histModuleDriftTimeResidual_otis3_L[s][l][q][m]->Fill(timeResidual);

	  if(otis == 0 && dist > 0) m_histModuleDriftTimeResidual_otis0_R[s][l][q][m]->Fill(timeResidual);
	  if(otis == 1 && dist > 0) m_histModuleDriftTimeResidual_otis1_R[s][l][q][m]->Fill(timeResidual);
	  if(otis == 2 && dist > 0) m_histModuleDriftTimeResidual_otis2_R[s][l][q][m]->Fill(timeResidual);
	  if(otis == 3 && dist > 0) m_histModuleDriftTimeResidual_otis3_R[s][l][q][m]->Fill(timeResidual);

	}
	else if(OTIS_calibration){
	  if(otis == 0) fill(histModuleDriftTimeResidual01L[s][l][q][m], timeResidual, 1.0);
	  if(otis == 1) fill(histModuleDriftTimeResidual01R[s][l][q][m], timeResidual, 1.0);
	  if(otis == 2) fill(histModuleDriftTimeResidual23L[s][l][q][m], timeResidual, 1.0);
	  if(otis == 3) fill(histModuleDriftTimeResidual23R[s][l][q][m], timeResidual, 1.0);

	  if(otis == 0) m_histModuleDriftTimeResidual01L[s][l][q][m]->Fill(timeResidual);
	  if(otis == 1) m_histModuleDriftTimeResidual01R[s][l][q][m]->Fill(timeResidual);
	  if(otis == 2) m_histModuleDriftTimeResidual23L[s][l][q][m]->Fill(timeResidual);
	  if(otis == 3) m_histModuleDriftTimeResidual23R[s][l][q][m]->Fill(timeResidual);

	}
	else{
	  if(otis <= 1 && dist < 0) fill(histModuleDriftTimeResidual01L[s][l][q][m], timeResidual, 1.0);
	  if(otis <= 1 && dist > 0) fill(histModuleDriftTimeResidual01R[s][l][q][m], timeResidual, 1.0);
	  if(otis >= 2 && dist < 0) fill(histModuleDriftTimeResidual23L[s][l][q][m], timeResidual, 1.0);
	  if(otis >= 2 && dist > 0) fill(histModuleDriftTimeResidual23R[s][l][q][m], timeResidual, 1.0);

	  if(otis <= 1 && dist < 0) m_histModuleDriftTimeResidual01L[s][l][q][m]->Fill(timeResidual);
	  if(otis <= 1 && dist > 0) m_histModuleDriftTimeResidual01R[s][l][q][m]->Fill(timeResidual);
	  if(otis >= 2 && dist < 0) m_histModuleDriftTimeResidual23L[s][l][q][m]->Fill(timeResidual);
	  if(otis >= 2 && dist > 0) m_histModuleDriftTimeResidual23R[s][l][q][m]->Fill(timeResidual);
	}

	m_histModuleDriftTimeResidual[s][l][q][m]->Fill(timeResidual);

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
	  tuple->column("eventNumber", (unsigned int)odin->eventNumber());
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

  TFile * outFile = new TFile("calibration_monitoring.root", "RECREATE");

  /*
  double mtoff[] = {
    27.8546, 27.4911, 27.1707, 26.8945, 26.6638, 26.4802, 26.3445, 26.2568, 26.2233, 27.8478, 27.485, 27.1654, 26.89, 26.66,
    26.4774, 26.3425, 26.2557, 26.2183, 27.8823, 27.5192, 27.1991, 26.9231, 26.6927, 26.5092, 26.3737, 26.2874, 26.2548,
    27.8755, 27.5131, 27.1938, 26.9185, 26.6889, 26.5065, 26.3718, 26.2863, 26.2499, 28.1602, 27.7832, 27.4486, 27.1581,
    26.9129, 26.7143, 26.5635, 26.4614, 26.4162, 27.9127, 27.5628, 27.2564, 26.9949, 26.7797, 26.6116, 26.4919, 26.4203,
    26.3989, 27.948, 27.5975, 27.2905, 27.0285, 26.8123, 26.6437, 26.5232, 26.4515, 26.4309, 28.1797, 27.8039, 27.4705,
    27.1812, 26.9373, 26.7396, 26.5899, 26.4906, 26.4385, 28.1285, 27.7806, 27.4759, 27.2155, 27.0011, 26.8335, 26.7139,
    26.6417, 26.6204, 28.3566, 27.9829, 27.6521, 27.3642, 27.1219, 26.9257, 26.7768, 26.6765, 26.6236, 28.3941, 28.0204,
    27.689, 27.401, 27.158, 26.9613, 26.8118, 26.712, 26.6684, 28.1465, 27.7992, 27.4958, 27.2364, 27.0229, 26.8563,
    26.7376, 26.6672, 26.647, 28.4043, 28.0483, 27.734, 27.4635, 27.2378, 27.0581, 26.9255, 26.8403, 26.8076, 28.3955, 28.0395,
    27.7263, 27.4561, 27.2312, 27.0519, 26.9199, 26.8352, 26.7985, 28.4319, 28.0763, 27.7624, 27.4921, 27.2668, 27.0873, 26.9548,
    26.8708, 26.8392, 28.4231, 28.0676, 27.7548, 27.485, 27.2602, 27.0811, 26.9493, 26.8659, 26.8301, 29.9962, 29.659, 29.3618, 29.1064,
    28.8936, 28.7243, 28.5992, 28.5184, 28.4874, 29.9957, 29.6592, 29.3627, 29.108, 28.8956, 28.727, 28.6025, 28.5224, 28.4876,
    30.024, 29.6872, 29.3902, 29.1352, 28.9228, 28.7535, 28.6285, 28.549, 28.5191, 30.0232, 29.687, 29.3909, 29.1363, 28.9241,
    28.756, 28.6316, 28.5528, 28.5193, 30.2936, 29.9432, 29.6332, 29.3643, 29.1376, 28.9543, 28.8152, 28.7213, 28.6795,
    30.0696, 29.7449, 29.4606, 29.2186, 29.0199, 28.8649, 28.7544, 28.6883, 28.6682, 30.0984, 29.773, 29.489, 29.2466, 29.047,
    28.8915, 28.7804, 28.7145, 28.6956, 30.3194, 29.9702, 29.6607, 29.3929, 29.1675, 28.9852, 28.847, 28.7554, 28.7074,
    30.2737, 29.951, 29.6685, 29.4276, 29.2294, 29.0747, 28.9644, 28.8977, 28.8779, 30.49, 30.1432, 29.8357, 29.5695, 29.3452,
    29.1639, 29.0265, 28.9342, 28.8854, 30.5228, 30.1758, 29.8683, 29.6012, 29.3766, 29.195, 29.0571, 28.9649, 28.925, 30.2967,
    29.9749, 29.6931, 29.4533, 29.256, 29.1021, 28.9927, 28.9281, 28.9096, 30.5439, 30.2125, 29.9213, 29.6709, 29.462,
    29.2961, 29.1738, 29.0951, 29.0647, 30.5397, 30.209, 29.9183, 29.6682, 29.46, 29.2946, 29.1729, 29.0947, 29.0608, 30.5719,
    30.2407, 29.9498, 29.6997, 29.4909, 29.3252, 29.2032, 29.1256, 29.0965, 30.5675, 30.2371, 29.9467, 29.6969, 29.489,
    29.3239, 29.2022, 29.1253, 29.0926, 32.1781, 31.8639, 31.5876, 31.3502, 31.1529, 30.9959, 30.8803, 30.8056, 30.7769, 32.1733,
    31.8596, 31.5844, 31.3472, 31.1507, 30.9942, 30.879, 30.8049, 30.7727, 32.2062, 31.8923, 31.6162, 31.3791, 31.1818,
    31.0251, 30.9096, 30.8361, 30.8087, 32.2014, 31.888, 31.613, 31.3761, 31.1796, 31.0233, 30.9083, 30.8354, 30.8046, 32.468,
    32.142, 31.8531, 31.6031, 31.3927, 31.2227, 31.0939, 31.0069, 30.9683, 32.255, 31.9526, 31.6888, 31.464, 31.2794,
    31.1357, 31.0333, 30.972, 30.9534, 32.2883, 31.9859, 31.7212, 31.496, 31.311, 31.1667, 31.0638, 31.0029, 30.9857, 32.4905,
    32.1651, 31.8774, 31.6284, 31.4187, 31.2495, 31.1213, 31.0365, 30.9923, 32.4635, 32.1625, 31.8993, 31.6756, 31.4915,
    31.3481, 31.2458, 31.1838, 31.1653, 32.6667, 32.343, 32.0568, 31.809, 31.6005, 31.4325, 31.305, 31.2192, 31.1738, 32.6977,
    32.3737, 32.0871, 31.8392, 31.6305, 31.4617, 31.3338, 31.2484, 31.2115, 32.4885, 32.1884, 31.9263, 31.7032, 31.5199,
    31.3773, 31.2756, 31.2157, 31.1986, 32.7279, 32.419, 32.1478, 31.9149, 31.7209, 31.5668, 31.4532, 31.3801, 31.3518, 32.7255,
    32.4177, 32.1464, 31.9141, 31.7206, 31.567, 31.4538, 31.3813, 31.3496, 32.7558, 32.4473, 32.1763, 31.9435, 31.7497,
    31.5959, 31.4825, 31.4105, 31.3837, 32.7534, 32.446, 32.175, 31.9429, 31.7497, 31.5962, 31.4832, 31.4118, 31.3815
  };
  */

  double test[3][4][4][9]; memset(test, 0.0, sizeof(test));
  //double newT0s[3][4][4][9]; memset(newT0s, 0.0, sizeof(newT0s));

  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));

  double test_OTIS[3][4][4][9][4]; memset(test_OTIS, 0.0, sizeof(test_OTIS));

  //double fake_t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));
  //readCondXMLs(t0s);
  if(readXMLs) readCondXMLs(t0s);
  //else readCondDB(t0s);
  else read_Globalt0(t0s);

  std::cout<< "READ t0s"<< std::endl;
  if(verbose){
    for(int s = 0; s < 3; s++){
      for(int l = 0; l < 4; l++){
	for(int q = 0; q < 4; q++){
	  for(int m = 8; m >= 0; m--){
	    std::cout<< "s = "<<s<< " l = "<< l<< " q = "<< q << " m = "<< m << "t0 = " << t0s[s][l][q][m]<<std::endl;
	  }
	}
      }
    }
  }
    
  //Loop over the station layer quarter module                                                                                                    

  std::cout << "Loop ..." << std::endl;
  for(int s = 0; s < 3; s++){
    for(int l = 0; l < 4; l++){
      for(int q = 0; q < 4; q++){

	// used only for Alex's monitorning                                                                                                                                
	//double t0_ = 0.0;
	//double dt0_ = 0.0;
	//double dt0err_ = 0.1;
	

        for(int m = 8; m >= 0; m--){

	  std::string histName = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual";
	  std::string histName01L = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual01L";
	  std::string histName01R = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual01R";
	  std::string histName23L = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual23L";
	  std::string histName23R = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual23R";

          TH1D* hist = (TH1D*)m_histModuleDriftTimeResidual[s][l][q][m]->Clone("hist");
          TH1D* hist01L = (TH1D*)m_histModuleDriftTimeResidual01L[s][l][q][m]->Clone("hist01L");
          TH1D* hist01R = (TH1D*)m_histModuleDriftTimeResidual01R[s][l][q][m]->Clone("hist01R");
          TH1D* hist23L = (TH1D*)m_histModuleDriftTimeResidual23L[s][l][q][m]->Clone("hist23L");
          TH1D* hist23R = (TH1D*)m_histModuleDriftTimeResidual23R[s][l][q][m]->Clone("hist23R");

	  TH1D* hist_otis0_L;
	  TH1D* hist_otis0_R;
	  TH1D* hist_otis1_L;
	  TH1D* hist_otis1_R;
	  TH1D* hist_otis2_L;
	  TH1D* hist_otis2_R;
	  TH1D* hist_otis3_L;
	  TH1D* hist_otis3_R;

	  // if(OTIS_LR_calibration){

	  //   hist_otis0_L = (TH1D*)m_histModuleDriftTimeResidual_otis0_L[s][l][q][m]->Clone("hist_otis0_L");
	  //   hist_otis1_L = (TH1D*)m_histModuleDriftTimeResidual_otis1_L[s][l][q][m]->Clone("hist_otis1_L");
	  //   hist_otis2_L = (TH1D*)m_histModuleDriftTimeResidual_otis2_L[s][l][q][m]->Clone("hist_otis2_L");
	  //   hist_otis3_L = (TH1D*)m_histModuleDriftTimeResidual_otis3_L[s][l][q][m]->Clone("hist_otis3_L");

	  //   hist_otis0_R = (TH1D*)m_histModuleDriftTimeResidual_otis0_R[s][l][q][m]->Clone("hist_otis0_R");
	  //   hist_otis1_R = (TH1D*)m_histModuleDriftTimeResidual_otis1_R[s][l][q][m]->Clone("hist_otis1_R");
	  //   hist_otis2_R = (TH1D*)m_histModuleDriftTimeResidual_otis2_R[s][l][q][m]->Clone("hist_otis2_R");
	  //   hist_otis3_R = (TH1D*)m_histModuleDriftTimeResidual_otis3_R[s][l][q][m]->Clone("hist_otis3_R");
	  // }

	  int modulen = m + 9 * (q + 4 * (l + 4 * s));

          //if(hist == 0 || hist->GetEntries() < 1000 || (s == 0 && m == 0))//this is the normal condition  
          //if(hist == 0 || hist->GetEntries() < 1 || (s == 0 && m == 0))//this is the normal condition  
	  if(s == 0 && m == 0) //this is for debug                                                                                                                            
            {
              if(hist != 0 && !(s == 0 && m == 0)) std::cout << histName << " :: N = " << hist->GetEntries() << std::endl;
              hdt0->SetBinContent(hdt0->FindBin(modulen), 0.0);
              hdt0->SetBinError(hdt0->FindBin(modulen), 0.0);
              ht0->SetBinContent(ht0->FindBin(modulen), t0s[s][l][q][m]);
              ht0->SetBinError(ht0->FindBin(modulen), 0.0);
              continue;
            }
          double dt0err = 0.0;

	  std::cout<<"Now fitting the 4 sub contributions"<<std::endl;

          double residual_01L=0.0;
          double residual_01R=0.0;
          double residual_23L=0.0;
          double residual_23R=0.0;

          double residual_01L_err=0.0;
          double residual_01R_err=0.0;
          double residual_23L_err=0.0;
          double residual_23R_err=0.0;

          double residual_0L=0.0;
          double residual_1L=0.0;
          double residual_2L=0.0;
          double residual_3L=0.0;

          double residual_0R=0.0;
          double residual_1R=0.0;
          double residual_2R=0.0;
          double residual_3R=0.0;

          double residual_0L_err=0.0;
          double residual_1L_err=0.0;
          double residual_2L_err=0.0;
          double residual_3L_err=0.0;

          double residual_0R_err=0.0;
          double residual_1R_err=0.0;
          double residual_2R_err=0.0;
          double residual_3R_err=0.0;


          StatusCode sc_01L = fit_single_hist(hist01L,s,l,q, m, "01L", residual_01L, residual_01L_err, outFile);
          StatusCode sc_01R = fit_single_hist(hist01R,s,l,q, m, "01R", residual_01R, residual_01R_err, outFile);
          StatusCode sc_23L = fit_single_hist(hist23L,s,l,q, m, "23L", residual_23L, residual_23L_err, outFile);
          StatusCode sc_23R = fit_single_hist(hist23R,s,l,q, m, "23R", residual_23R, residual_23R_err, outFile);

	  StatusCode sc_0_L;
	  StatusCode sc_1_L;
	  StatusCode sc_2_L;
	  StatusCode sc_3_L;

	  StatusCode sc_0_R;
	  StatusCode sc_1_R;
	  StatusCode sc_2_R;
	  StatusCode sc_3_R;

	  if(OTIS_LR_calibration){

	    hist_otis0_L = (TH1D*)m_histModuleDriftTimeResidual_otis0_L[s][l][q][m]->Clone("hist_otis0_L");
	    hist_otis1_L = (TH1D*)m_histModuleDriftTimeResidual_otis1_L[s][l][q][m]->Clone("hist_otis1_L");
	    hist_otis2_L = (TH1D*)m_histModuleDriftTimeResidual_otis2_L[s][l][q][m]->Clone("hist_otis2_L");
	    hist_otis3_L = (TH1D*)m_histModuleDriftTimeResidual_otis3_L[s][l][q][m]->Clone("hist_otis3_L");

	    hist_otis0_R = (TH1D*)m_histModuleDriftTimeResidual_otis0_R[s][l][q][m]->Clone("hist_otis0_R");
	    hist_otis1_R = (TH1D*)m_histModuleDriftTimeResidual_otis1_R[s][l][q][m]->Clone("hist_otis1_R");
	    hist_otis2_R = (TH1D*)m_histModuleDriftTimeResidual_otis2_R[s][l][q][m]->Clone("hist_otis2_R");
	    hist_otis3_R = (TH1D*)m_histModuleDriftTimeResidual_otis3_R[s][l][q][m]->Clone("hist_otis3_R");

	    sc_0_L= fit_single_hist(hist_otis0_L,s,l,q, m, "0L", residual_0L, residual_0L_err, outFile);
	    sc_1_L= fit_single_hist(hist_otis1_L,s,l,q, m, "1L", residual_1L, residual_1L_err, outFile);
	    sc_2_L= fit_single_hist(hist_otis2_L,s,l,q, m, "2L", residual_2L, residual_2L_err, outFile);
	    sc_3_L= fit_single_hist(hist_otis3_L,s,l,q, m, "3L", residual_3L, residual_3L_err, outFile);
	    
	    sc_0_R= fit_single_hist(hist_otis0_R,s,l,q, m, "0L", residual_0R, residual_0R_err, outFile);
	    sc_1_R= fit_single_hist(hist_otis1_R,s,l,q, m, "1L", residual_1R, residual_1R_err, outFile);
	    sc_2_R= fit_single_hist(hist_otis2_R,s,l,q, m, "2L", residual_2R, residual_2R_err, outFile);
	    sc_3_R= fit_single_hist(hist_otis3_R,s,l,q, m, "3L", residual_3R, residual_3R_err, outFile);
	    
	    test_OTIS[s][l][q][m][0] = 0.5*(residual_0L + residual_0R);
	    test_OTIS[s][l][q][m][1] = 0.5*(residual_1L + residual_1R);
	    test_OTIS[s][l][q][m][2] = 0.5*(residual_2L + residual_2R);
	    test_OTIS[s][l][q][m][3] = 0.5*(residual_3L + residual_3R);
	    
	    if(Apply_Calibration){
	      for (int a = 0; a<4;a++){
		test_OTIS[s][l][q][m][a] = test_OTIS[s][l][q][m][a]+t0s[s][l][q][m];
	      }
	    }
	    
	    
	  }
          else if(OTIS_calibration){
	    test_OTIS[s][l][q][m][0] = residual_01L;
	    test_OTIS[s][l][q][m][1] = residual_01R;
	    test_OTIS[s][l][q][m][2] = residual_23L;
	    test_OTIS[s][l][q][m][3] = residual_23R;

	    if(Apply_Calibration){
	      for (int a = 0; a<4;a++){
		test_OTIS[s][l][q][m][a] = test_OTIS[s][l][q][m][a]+t0s[s][l][q][m];
	      }
	    }
	    
          }
	  else{
	    if( m==8 && (q == 0 || q == 2) && hist23L->GetEntries()==0 && hist23R->GetEntries()==0){ //only 2 half monlayer contributions 
	      test[s][l][q][m] = 0.5*(residual_01L + residual_01R);
	      
	      dt0err = 0.5*(sqrt((residual_01L_err*residual_01L_err)+(residual_01R_err*residual_01R_err)));
	      
	    }
	    else{
	      test[s][l][q][m] = 0.25*(residual_01L + residual_01R + residual_23L+ residual_23R);
	      
	      
	      dt0err = 0.25*(sqrt((residual_01L_err*residual_01L_err)+(residual_01R_err*residual_01R_err)
				  +(residual_23L_err*residual_23L_err)+(residual_23R_err*residual_23R_err)));
	    }

	    double dt0 =  test[s][l][q][m];
	    double t0 = t0s[s][l][q][m] + test[s][l][q][m];
	    hdt0->SetBinContent(hdt0->FindBin(modulen),  dt0);
	    hdt0->SetBinError(hdt0->FindBin(modulen), dt0err);
	    ht0->SetBinContent(ht0->FindBin(modulen), t0);
	    ht0->SetBinError(ht0->FindBin(modulen), dt0err);


	    //if(fabs(dt0) > 1) std::cout << histName << " :: dt0 = " << dt0 << std::endl; 
	    std::cout << histName << " :: dt0 = " << dt0 << std::endl;

	    if(Apply_Calibration){
	      t0s[s][l][q][m]= t0s[s][l][q][m] + test[s][l][q][m];
	    }
	    else
	      t0s[s][l][q][m]= t0s[s][l][q][m] + 0.0;
	  }
        }
      }
    }
  }

  /*
  for(int s = 0; s < 3; s++){
      for(int l = 0; l < 4; l++){
	for(int q = 0; q < 4; q++){	  
	  for(int m = 8; m >= 0; m--){

	    double t0_ = 0.0;
	    double dt0_ = 0.0;
	    double dt0err_ = 0.1;

	    int modulen = m + 9 * (q + 4 * (l + 4 * s));

	    //std::cout<< "inner looop"<<std::endl;//debug
	    
	    //    if(m_histModuleDriftTimeResidual[s][l][q][m] == 0 || m_histModuleDriftTimeResidual[s][l][q][m]->GetEntries() < 1000  || (s == 0 && m == 0))
	    if(m_histModuleDriftTimeResidual[s][l][q][m] == 0 || m_histModuleDriftTimeResidual[s][l][q][m]->GetEntries() < 0  || (s == 0 && m == 0))//for debug only
	      {
		if (verbose)
		  std::cout<< "no data enough"<<std::endl;
		
		if(m == 8) t0_ = -mtoff[modulen];
		if(verbose){
		  if(!(s == 0 && m == 0)){
		    std::cout<<"not enough events "<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual01L[s][l][q][m]->GetEntries() << std::endl;
		    std::cout<<"not enough events "<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual01R[s][l][q][m]->GetEntries() << std::endl;
		    std::cout<<"not enough events "<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual23L[s][l][q][m]->GetEntries() << std::endl;
		    std::cout<<"not enough events "<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual23R[s][l][q][m]->GetEntries() << std::endl;
		  }
		}

		t0s[s][l][q][m] = t0_ + mtoff[modulen];

		hdt0->SetBinContent(hdt0->FindBin(modulen), dt0_);
		hdt0->SetBinError(hdt0->FindBin(modulen), dt0err_);
		ht0->SetBinContent(ht0->FindBin(modulen), t0_ + (28.0 + 2.0 * s));
		ht0->SetBinError(ht0->FindBin(modulen), dt0err_);
		
		if(verbose)
		  std::cout << "if cond"<< modulen<<" "<< dt0_<< " "<< dt0err_ << std::endl;

		//fake_t0s[s][l][q][m]= 0.0;
		fake_t0s[s][l][q][m]=  t0_ + mtoff[modulen] +0.0;
	    
		if(verbose){
		  std::cout<< "FAKE t0s = " << fake_t0s[s][l][q][m] << " s= "<< s << " l = "<< l<< " q = "<< q <<" m = "<< m <<std::endl; 
		  std::cout<< "READ t0s = " << t0s[s][l][q][m] << " s= "<< s << " l = "<< l<< " q = "<< q <<" m = "<< m <<std::endl; 
		}

		continue;
	      }
	      
	    if(verbose){
	      std::cout<< "good module, enough data"<<std::endl;
	      std::cout<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual01L[s][l][q][m]->GetEntries() << std::endl;
	      std::cout<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual01R[s][l][q][m]->GetEntries() << std::endl;
	      std::cout<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual23L[s][l][q][m]->GetEntries() << std::endl;
	      std::cout<<s<<" "<<l<<" "<<q<<" "<<m<<" "<< m_histModuleDriftTimeResidual23R[s][l][q][m]->GetEntries() << std::endl;
	    }

            double left = m_histModuleDriftTimeResidual[s][l][q][m]->GetXaxis()->GetXmin();
            double right = m_histModuleDriftTimeResidual[s][l][q][m]->GetXaxis()->GetXmax();
            double dt0err = 0.1;
            for(int i = 0; i < 5; i++)
              {
		//std::cout<< "still alive "<< i <<std::endl;
                m_histModuleDriftTimeResidual[s][l][q][m]->Fit("gaus", "QRLL", "", left, right);
                left = m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParameter(1) - 2.0 * m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParameter(2);
                right = m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParameter(1) + 2.0 * m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParameter(2);
              }

	    std::cout<<"Now fitting the 4 sub contributions"<<std::endl;

	    double residual_01L=0.0;
            double residual_01R=0.0;
            double residual_23L=0.0;
            double residual_23R=0.0;

	    double residual_01L_err=0.0;
            double residual_01R_err=0.0;
            double residual_23L_err=0.0;
            double residual_23R_err=0.0;

            //StatusCode sc_01L = fit_single_hist(m_histModuleDriftTimeResidual01L[s][l][q][m],s,l,q, m, residual_01L);
            //StatusCode sc_01R = fit_single_hist(m_histModuleDriftTimeResidual01R[s][l][q][m],s,l,q, m, residual_01R);
            //StatusCode sc_23L = fit_single_hist(m_histModuleDriftTimeResidual23L[s][l][q][m],s,l,q, m, residual_23L);
            //StatusCode sc_23R = fit_single_hist(m_histModuleDriftTimeResidual23R[s][l][q][m],s,l,q, m, residual_23R);

	    StatusCode sc_01L = fit_single_hist(m_histModuleDriftTimeResidual01L[s][l][q][m],s,l,q, m, "01L", residual_01L, residual_01L_err, outFile);
	    StatusCode sc_01R = fit_single_hist(m_histModuleDriftTimeResidual01R[s][l][q][m],s,l,q, m, "01R", residual_01R, residual_01R_err, outFile);
	    StatusCode sc_23L = fit_single_hist(m_histModuleDriftTimeResidual23L[s][l][q][m],s,l,q, m, "23L", residual_23L, residual_23L_err, outFile);
	    StatusCode sc_23R = fit_single_hist(m_histModuleDriftTimeResidual01L[s][l][q][m],s,l,q, m, "23R", residual_23R, residual_23R_err, outFile);

            test[s][l][q][m]= 0.25*(residual_01L+
                                    residual_01R+
                                    residual_23L+
                                    residual_23R);


	    //std::cout<<"assigning dt0 "<< test[s][l][q][m] << std::endl;
	    //std::cout<<"OTIS residuals "<< residual_01L<<" "<<residual_01R<<" "<<residual_23L<<" "<<residual_23R << std::endl;

            double dt0 =  test[s][l][q][m];
	    //std::cout<<"assigning dt0 -2nd time "<< dt0 << std::endl;

	    //std::cout<<"assigning dt0err "<<std::endl;

            //double dt0err = m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParError(1);                                                
            dt0err = m_histModuleDriftTimeResidual[s][l][q][m]->GetFunction("gaus")->GetParError(1);
            //double dt0err = 0.1;                                                                                                                           

	    //std::cout<<"calculating t0 for monitoring"<<  t0s[s][l][q][m] + test[s][l][q][m] <<std::endl;

            double t0 = t0s[s][l][q][m] + test[s][l][q][m];
	    std::cout<<"calculating t0 for monitoring - 2nd time"<<  t0 <<std::endl;

	    //std::cout<<"subtracting toff for monitoring"<<std::endl;

            t0 -= mtoff[modulen];

	    //std::cout<<"filling ndt0"<<std::endl;

            hdt0->SetBinContent(hdt0->FindBin(modulen), dt0);
            hdt0->SetBinError(hdt0->FindBin(modulen), dt0err);

	    //std::cout<<"filling nt0"<<std::endl;

            ht0->SetBinContent(ht0->FindBin(modulen), t0 + (28.0 + 2.0 * s));
            ht0->SetBinError(ht0->FindBin(modulen), dt0err);

	    //std::cout << "fit cond"<< modulen<<" "<< dt0<< " "<< dt0err << std::endl;
            t0_ = t0;
            dt0_ = dt0;
            dt0err_ = dt0err;

	    std::cout<<"warning t0 too big"<<std::endl;

            if(fabs(dt0) > 1) std::cout << s <<" "<< l <<" "<< q <<" "<< m << " :: dt0 = " << dt0 <<" "<<residual_01L<<" "<< residual_01R << " "<< residual_23L<< " "<< residual_23R << std::endl;

	    //comment the following line if you want to NOT include the calibration
	    if(Apply_Calibration){
	      t0s[s][l][q][m]= t0s[s][l][q][m] + test[s][l][q][m];
	    }

	    //fake_t0s[s][l][q][m]= 0.0;
	    fake_t0s[s][l][q][m]= t0s[s][l][q][m] +0.0;

	    if(verbose)
	      std::cout<< "newT0s[s][l][q][m] = "<<t0s[s][l][q][m]<<std::endl;
	      std::cout<< "test[s][l][q][m] = "<<test[s][l][q][m]<<std::endl;

	    // std::cout<< "FAKE t0s = " << fake_t0s[s][l][q][m]<<std::endl; 
	    //std::cout<< "FAKE t0s = " << fake_t0s[s][l][q][m] << " s= "<< s << " l = "<< l<< " q = "<< q <<" m = "<< m <<std::endl; 
	    //std::cout<< "READ t0s = " << t0s[s][l][q][m] << " s= "<< s << " l = "<< l<< " q = "<< q <<" m = "<< m <<std::endl; 
		
	  }
	}
      }
    }
  */

  for(int i = 0; i < 432; i++) if(hdt0->GetBinContent(i+1) != 0) hdt0proj->Fill(hdt0->GetBinContent(i+1));

  if(verbose)
    for(int i = 0; i < 432; i++) std::cout<< i <<" "<< hdt0->GetBinContent(i+1)<< " "<< ht0->GetBinContent(i+1)<<std::endl;


  if(OTIS_calibration){
    if(Apply_Calibration){
      writeCondXMLs(test_OTIS);
    }
    else
      writeCondXMLs(t0s);
  }
  else{
    writeCondXMLs(t0s);
    writeCondDBXMLs(t0s);
  }

  // if(Apply_Calibration){
  //   writeCondXMLs(t0s);
  //   writeCondDBXMLs(t0s);
  // }
  // //just to change the t0s 2ns to everyone, to see how much the track eff. and so change
  // else{
  //   writeCondXMLs(fake_t0s);
  //   writeCondDBXMLs(fake_t0s);
  // }

  return GaudiTupleAlg::initialize();
}

StatusCode OTModuleClbrMon::readCondDB(double read_t0s[3][4][4][9])
{
   std::string subDet = "OT";

   double Module_t0=0;

   for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
   {
     for(int m = 8; m >= 0; m--){

     std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

     std::string alignLoc ="/dd/Conditions/Calibration/OT/CalibrationModules"+stationNames[s]+layerNames[l]+quarterNames[q]+"/"+stationNames[s]+layerNames[l]+quarterNames[q]+moduleNames[m];

     Condition *myCond = get<Condition>( detSvc(), alignLoc );
  
 
     //if(simulation){
	std::vector<double> TZeroVec = myCond->paramAsDoubleVect( "TZero" );
	Module_t0=0;
	for(size_t i = 0; i<TZeroVec.size();i++){
	  //std::cout << "t0 per straw = "<<TZeroVec.at(i)<<std::endl; // for check
	  Module_t0 +=TZeroVec.at(i); 
	}
	read_t0s[s][l][q][m] = Module_t0/(TZeroVec.size()*1.0);//in simcond the t0 are on straw bases, I make an average - for now - to make it module basis
	//}
	//else{
	//	read_t0s[s][l][q][m] = myCond->paramAsDouble( "TZero" );
	//}

     }
   }

  
  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::read_Globalt0(double read_t0s[3][4][4][9])
{
   std::string subDet = "OT";

   //double Module_t0=0;

   for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
   {
     for(int m = 8; m >= 0; m--){
       
       std::string alignLoc ="/dd/Conditions/Calibration/OT/CalibrationGlobal";
       
       Condition *myCond = get<Condition>( detSvc(), alignLoc );
       
       //double global_t0 = myCond->paramAsDouble( "TZero" );
       read_t0s[s][l][q][m] = myCond->paramAsDouble( "TZero" );
       
     }
   }

  
  return StatusCode::SUCCESS;
}


StatusCode OTModuleClbrMon::readCondXMLs(double t0s[3][4][4][9])
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
                          >> *(anychar_p - 'M') >> 'M' >> int_p[boost::phoenix::ref(m) = arg1 - 1]
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
                              [ boost::phoenix::push_back(boost::phoenix::ref(cT0)   [boost::phoenix::ref(m)], arg1) ],
                          if_(boost::phoenix::ref(param) == "WalkParameters")
                              [ boost::phoenix::push_back(boost::phoenix::ref(cWalk) [boost::phoenix::ref(m)], arg1) ]
                      ]
                  >> "</paramVector>"
              )[boost::phoenix::ref(param) = ""]
              >> "</condition>"
          )[boost::phoenix::ref(m) = -1]
          >> "</catalog>"
          >> "</DDDB>"
          >> end_p;
      
      /*
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
      */

      if(parse(xml.c_str(), xmlRule, space_p).full != true) return StatusCode::FAILURE;
    }

    for(int m = 0; m < 9; m++)
    {
      LHCb::OTChannelID id(s + 1, l, q, m + 1, 1);
      DeOTModule* module = detector->findModule(id);
      module->setStrawT0s(cT0[m]);
      OTDet::RtRelation rt(2.45, cRt[m], cRtErr[m]);
      module->setRtRelation(rt);
      if(cWalk[m].size() == 4)
      {
        OTDet::WalkRelation walk(cWalk[m][0], cWalk[m][1], cWalk[m][2], cWalk[m][3]);
        module->setWalkRelation(walk);
      }
      else std::cout << "skip walk initialization: npars != 4" << std::endl;
    }


    for(int m = 0; m < 9; m++)
      {
        double t0 = cT0[m][0];
        t0s[s][l][q][m] = t0;
	std::cout<< "t0 for module " << s <<" "<< l << " "<< q << " "<< m <<" = "<< t0s[s][l][q][m] << std::endl;
      }

  }

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::writeCondXMLs(double t0s[3][4][4][9])
{
  std::string prefix = "CalibrationModules";

  std::cout<< "WRITING XMLs" <<std::endl;       

  for(int s = 0; s < 3; s++)
    for(int l = 0; l < 4; l++)
      for(int q = 0; q < 4; q++)
        {
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

	  std::string fileName = quarterId + "@" + prefix + ".xml";

	  std::ofstream file(fileName.c_str());
          if(file.fail())
            {
	      printf("Can't open file: '%s'\n", fileName.c_str());
              
	      continue;
            }

          file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
          file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
	  file << "\n";
          file << "<DDDB>\n";
          file << "<catalog name=\"" << prefix << quarterId << "\">\n";


	  //for SIMCOND only
	  if(simulation){
	    for(int m = 0; m < 9; m++)
	      {
		if(verbose)
		  std::cout<< "WRITING m = "<< m<< "t0 = "<< t0s[s][l][q][m]<< "written num"<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<<std::endl;
		
		std::string moduleId = quarterId + moduleNames[m];
		
		file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";
		
		file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
		if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
		else      file << "     " << 2.6 << " " << 0 << " " << 4.0 * 0.15 << "\n";
		//else      file << "     " << 2.6 << " " << (3.2 - 2.6 - 4.0 * 0.15) << " " << 4.0 * 0.15 << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
		file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
		//file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
		/*
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		*/
                for(int a = 0; a<16;a++){
                  file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
                }

		file << "    </paramVector>\n";
		//              file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
		//              file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
		//file << "    </paramVector>\n";
		
		file << "  </condition>\n";
	      }
	  }
	  else{	 
	    for(int m = 0; m < 9; m++)
	      {
		std::string moduleId = quarterId + moduleNames[m];
		
		file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";
		
		file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
		if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
		else      file << "     " << 2.6 << " " << 0  << " " << 4.0 * 0.15 << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
		file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
		file << "    </paramVector>\n";
		file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
		file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
		file << "    </paramVector>\n";
		
		file << "  </condition>\n";
	      }
	  }

          file << "</catalog>\n";
          file << "</DDDB>\n";
	  
          file.flush();
          file.close();
        }
  
  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::writeCondXMLs(double t0s[3][4][4][9][4])
{
  std::string prefix = "CalibrationModules";

  std::cout<< "WRITING XMLs" <<std::endl;

  for(int s = 0; s < 3; s++)
    for(int l = 0; l < 4; l++)
      for(int q = 0; q < 4; q++)
	{
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

	  std::string fileName = quarterId + "@" + prefix + ".xml";

	  std::ofstream file(fileName.c_str());
          if(file.fail())
            {
              printf("Can't open file: '%s'\n", fileName.c_str());

              continue;
            }
          file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
          file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
          file << "\n";
          file << "<DDDB>\n";
          file << "<catalog name=\"" << prefix << quarterId << "\">\n";


          //for SIMCOND only
          if(simulation){
            for(int m = 0; m < 9; m++)
              {
                if(verbose){
		  for(int b = 0; b<4;b++){
		  std::cout<< "WRITING m = "<< m<< "t0 = "<< t0s[s][l][q][m][b]<< "written num"<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<<std::endl;
		  }
		}

		std::string moduleId = quarterId + moduleNames[m];

                file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

                file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
                if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
                else      file << "     " << 2.6 << " " << 0 << " " << 4.0 * 0.15 << "\n";
                //else      file << "     " << 2.6 << " " << (3.2 - 2.6 - 4.0 * 0.15) << " " << 4.0 * 0.15 << "\n";
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
                file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
                for(int b = 0; b<4;b++){
		  for(int a = 0; a<4;a++){
                  file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< " " << 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< "\n";
		  }
                }
                file << "    </paramVector>\n";
                file << "  </condition>\n";
              }
          }
	  else{
	    for(int m = 0; m < 9; m++)
	      {
		std::string moduleId = quarterId + moduleNames[m];
		
		file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";
		
		file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
		if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
		else      file << "     " << 2.6 << " " << 0  << " " << 4.0 * 0.15 << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
		file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
		file << "    </paramVector>\n";
		
		file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
		file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m][0] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m][1] + 0.5) << " "
		     << 0.001 * (int)(1000.0 * t0s[s][l][q][m][2] + 0.5) << " " << 0.001 * (int)(1000.0 * t0s[s][l][q][m][3] + 0.5) << "\n";
		file << "    </paramVector>\n";
		file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
		file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
		file << "    </paramVector>\n";
		file << "  </condition>\n";
	      }
	  }
	  
	  file << "</catalog>\n";
	  file << "</DDDB>\n";
	  
	  file.flush();
	  file.close();
	}

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::writeCondDBXMLs(double t0s[3][4][4][9])
{

  std::string prefix = "CalibrationModules";

  //  boost::filesystem::path dir( m_xmlFilePath + "/" +subDet + "/v" + std::to_string(version) ) ;
  //  boost::filesystem::path dir( m_xmlFilePath + "/OT" ) ;
  //  boost::filesystem::path dir( m_xmlFilePath ) ;
  //boost::filesystem::path filen( m_xmlFileName );
  //boost::filesystem::path full_path = dir/filen ;
  
  //boost::filesystem::path dir("/afs/cern.ch/user/l/lgrillo/databases_for_online/OT") ;
  // boost::filesystem::path filen("results.xml");
  //  boost::filesystem::path full_path = dir/filen ;
  // boost::filesystem::path full_path("/afs/cern.ch/user/l/lgrillo/databases_for_online/OT/results.xml");
  boost::filesystem::path full_path(m_xmlFilePath);
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "Writing new XML for online to " << full_path << endmsg ;
  
  std::ofstream file;
  
  // if( !boost::filesystem::exists( full_path ) ) {     
  //   warning() << "full path doesn not exist!?!" << endmsg;
  //   boost::filesystem::create_directories( dir ) ;
  // }
  
  file.open( full_path.string().c_str(), std::ios::app ) ;  // always in append mode
  
  for(int s = 0; s < 3; s++)
    for(int l = 0; l < 4; l++)
      for(int q = 0; q < 4; q++)
        {
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];
	  
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
	      if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
	      else      file << "     " << 2.6 << " " << 0  << " " << 4.0 * 0.15 << "\n";
	      file << "    </paramVector>\n";
	      
	      file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
	      file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
	      file << "    </paramVector>\n";
	      
	      file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
	      file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
	      file << "    </paramVector>\n";
	      file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
	      file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
	      file << "    </paramVector>\n";
	      
	      file << "  </condition>\n";
	    }
	}
  
  file << "</DDDB>\n";
  
  //logging << "<condition classID=\"6\" name=\"AverageHPDOccupancies\">\n";
  //logging << "<paramVector name=\"Occupancies\" type=\"std::string\" comment=\"Average HPD occupancy\">"
  //          << occS << "</paramVector>\n";
  //logging << "</condition>\n";
  //logging << "\n" ;
  
  file.close();
  

  return StatusCode::SUCCESS;
}


StatusCode OTModuleClbrMon::fit_single_hist(TH1D* hist, int s, int l, int q, int m, double& result)
{
  //char histName[256];                                                                                      
  //sprintf(histName, "OTModuleClbrMon/%s/%s/%s/%s/%s", stationNames[s].c_str(), layerNames[l].c_str(), quarterNames[q].c_str(), moduleNames[m].c_str(), name);
  //TH1D* hist = (TH1D*)file->Get(histName);                                                                 
 
  //  if(hist == 0 || hist->GetEntries() < 100)
    if(hist == 0 || hist->GetEntries() < 1)//to debug
    {
      if(!(q % 2 == 0 && m == 8))
        {
	  std::cout << "FAIL: " << s<< " "<<l <<" "<<q<< " "<<m << std::endl;
        }
      return 0.0;
    }
  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();
  for(int i = 0; i < 5; i++)
    {
      hist->Fit("gaus", "QRLL", "", left, right);
      left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
      right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
    }
  std::cout << "FITTING: " << s<< " "<<l <<" "<<q<< " "<<m <<" "<<hist->GetFunction("gaus")->GetParameter(1)<< " "<< hist->GetFunction("gaus")->GetParameter(2)<<std::endl;

  result =  hist->GetFunction("gaus")->GetParameter(1);

  return StatusCode::SUCCESS;
}

StatusCode OTModuleClbrMon::fit_single_hist(TH1D* hist, int s, int l, int q, int m, std::string contr, double& result, double& result_error,  TFile* outFile)
{
  debug() << hist << " " << s << " " << l << " " << q << " "<< m << endmsg;

  stationNames = {"T1","T2","T3"};
  layerNames = {"X1", "U", "V", "X2"};
  quarterNames = {"Q0", "Q1", "Q2", "Q3"};
  moduleNames = {"M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9"};


  if(!hist){
    Error("Histogram not found " );
    return StatusCode::FAILURE;

  }else if(hist->GetEntries() < 100) 
    //}else if(hist->GetEntries() < 1)//to debug  
    {
      if(!(q % 2 == 0 && m == 8))
        {
	  std::cout << "FAIL: " << s<< " "<<l <<" "<<q<< " "<<m << std::endl;
        }
      return StatusCode::FAILURE;
    }

  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();


  for(int i = 0; i < 5; i++)
    {
      hist->Fit("gaus", "QRLL", "", left, right);
      left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
      right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
    }
  debug() << "FITTING: " << s<< " "<<l <<" "<<q<< " "<<m <<" "<<hist->GetFunction("gaus")->GetParameter(1)<< " "<< hist->GetFunction("gaus")->GetParameter(2)<< endmsg ;
  result =  hist->GetFunction("gaus")->GetParameter(1);
  result_error =  hist->GetFunction("gaus")->GetParError(1);
  //only to save stuff

  if(save_fits){
    outFile->cd();

    std::string hist_name;

    if(s==-1 && l==-1 && q==-1 && m==-1)
      hist_name = "Global_hist";
    else
      hist_name = stationNames[s] + "_" + layerNames[l] + "_" + quarterNames[q] + "_"+ moduleNames[m] + contr;

    hist->SetName(hist_name.c_str());
    hist->Write(); 

  }

  return StatusCode::SUCCESS;
}
