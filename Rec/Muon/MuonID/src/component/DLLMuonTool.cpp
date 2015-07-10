/** Implementation of DLLMuonTool
 *
 * 2015-01-12: Ricardo Vazquez Gomez
 */
#include "DLLMuonTool.h"

// Let's define constants we do not need to calculate
// ATanh(0.5)
#define ATANH05   0.5493061443340548

#define  iM2   1
#define  iM3   2
#define  iM4   3
#define  iM5   4

DECLARE_TOOL_FACTORY(DLLMuonTool)

/** C'tor
 * Also declares an interface to that the tool can be obtained in Gaudi via
 * tool<...>(...)
 */
DLLMuonTool::DLLMuonTool(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : GaudiTool(type, name, parent), foiFactor_{1.0} {
  declareInterface<DLLMuonTool>(this);

  // some hardcoded parameters. Need to be fixed
  declareProperty("OverrideDB", m_OverrideDB = false);
  declareProperty("PreSelMomentum", preSelMomentum_);
  declareProperty("MomentumCuts", momentumCuts_);
  declareProperty("FOIfactor", foiFactor_);
  // Same parameters for muons and non muons but different names
  // Muons
  declareProperty("MupBinsR1", m_MupBinsR1);
  declareProperty("MupBinsR2", m_MupBinsR2);
  declareProperty("MupBinsR3", m_MupBinsR3);
  declareProperty("MupBinsR4", m_MupBinsR4);
  // Non muons
  declareProperty("NonMupBinsR1", m_MupBinsR1);
  declareProperty("NonMupBinsR2", m_MupBinsR2);
  declareProperty("NonMupBinsR3", m_MupBinsR3);
  declareProperty("NonMupBinsR4", m_MupBinsR4);

  // Landau parameters Non-Muons - Region 1-2-3-4:
  declareProperty("NonMuLandauParameterR1", m_NonMuLanParR1);
  declareProperty("NonMuLandauParameterR2", m_NonMuLanParR2);
  declareProperty("NonMuLandauParameterR3", m_NonMuLanParR3);
  declareProperty("NonMuLandauParameterR4", m_NonMuLanParR4);

  // hyperbolic tangent mapping of distances:
  // tanh scale factors for muons necessary for
  declareProperty("tanhScaleFactorsMuonR1", m_tanhScaleFactorsMuonR1);
  declareProperty("tanhScaleFactorsMuonR2", m_tanhScaleFactorsMuonR2);
  declareProperty("tanhScaleFactorsMuonR3", m_tanhScaleFactorsMuonR3);
  declareProperty("tanhScaleFactorsMuonR4", m_tanhScaleFactorsMuonR4);

  // tanh scale factors for non-muons
  declareProperty("tanhScaleFactorsNonMuonR1", m_tanhScaleFactorsNonMuonR1);
  declareProperty("tanhScaleFactorsNonMuonR2", m_tanhScaleFactorsNonMuonR2);
  declareProperty("tanhScaleFactorsNonMuonR3", m_tanhScaleFactorsNonMuonR3);
  declareProperty("tanhScaleFactorsNonMuonR4", m_tanhScaleFactorsNonMuonR4);

  // tanh(dist2) histograms contents for muons  // DLLflag = 3 and 4
  declareProperty("tanhCumulHistoMuonR1_1", m_tanhCumulHistoMuonR1_1);
  declareProperty("tanhCumulHistoMuonR1_2", m_tanhCumulHistoMuonR1_2);
  declareProperty("tanhCumulHistoMuonR1_3", m_tanhCumulHistoMuonR1_3);
  declareProperty("tanhCumulHistoMuonR1_4", m_tanhCumulHistoMuonR1_4);
  declareProperty("tanhCumulHistoMuonR1_5", m_tanhCumulHistoMuonR1_5);
  declareProperty("tanhCumulHistoMuonR1_6", m_tanhCumulHistoMuonR1_6);
  declareProperty("tanhCumulHistoMuonR1_7", m_tanhCumulHistoMuonR1_7);

  declareProperty("tanhCumulHistoMuonR2_1", m_tanhCumulHistoMuonR2_1);
  declareProperty("tanhCumulHistoMuonR2_2", m_tanhCumulHistoMuonR2_2);
  declareProperty("tanhCumulHistoMuonR2_3", m_tanhCumulHistoMuonR2_3);
  declareProperty("tanhCumulHistoMuonR2_4", m_tanhCumulHistoMuonR2_4);
  declareProperty("tanhCumulHistoMuonR2_5", m_tanhCumulHistoMuonR2_5);

  declareProperty("tanhCumulHistoMuonR3_1", m_tanhCumulHistoMuonR3_1);
  declareProperty("tanhCumulHistoMuonR3_2", m_tanhCumulHistoMuonR3_2);
  declareProperty("tanhCumulHistoMuonR3_3", m_tanhCumulHistoMuonR3_3);
  declareProperty("tanhCumulHistoMuonR3_4", m_tanhCumulHistoMuonR3_4);
  declareProperty("tanhCumulHistoMuonR3_5", m_tanhCumulHistoMuonR3_5);

  declareProperty("tanhCumulHistoMuonR4_1", m_tanhCumulHistoMuonR4_1);
  declareProperty("tanhCumulHistoMuonR4_2", m_tanhCumulHistoMuonR4_2);
  declareProperty("tanhCumulHistoMuonR4_3", m_tanhCumulHistoMuonR4_3);
  declareProperty("tanhCumulHistoMuonR4_4", m_tanhCumulHistoMuonR4_4);
  declareProperty("tanhCumulHistoMuonR4_5", m_tanhCumulHistoMuonR4_5);

  // tanh(dist2) histograms contents for non-muons // DLLflag = 3
  declareProperty("tanhCumulHistoNonMuonR1_1", m_tanhCumulHistoNonMuonR1_1);
  declareProperty("tanhCumulHistoNonMuonR1_2", m_tanhCumulHistoNonMuonR1_2);
  declareProperty("tanhCumulHistoNonMuonR1_3", m_tanhCumulHistoNonMuonR1_3);
  declareProperty("tanhCumulHistoNonMuonR1_4", m_tanhCumulHistoNonMuonR1_4);
  declareProperty("tanhCumulHistoNonMuonR1_5", m_tanhCumulHistoNonMuonR1_5);
  declareProperty("tanhCumulHistoNonMuonR1_6", m_tanhCumulHistoNonMuonR1_6);
  declareProperty("tanhCumulHistoNonMuonR1_7", m_tanhCumulHistoNonMuonR1_7);

  declareProperty("tanhCumulHistoNonMuonR2_1", m_tanhCumulHistoNonMuonR2_1);
  declareProperty("tanhCumulHistoNonMuonR2_2", m_tanhCumulHistoNonMuonR2_2);
  declareProperty("tanhCumulHistoNonMuonR2_3", m_tanhCumulHistoNonMuonR2_3);
  declareProperty("tanhCumulHistoNonMuonR2_4", m_tanhCumulHistoNonMuonR2_4);
  declareProperty("tanhCumulHistoNonMuonR2_5", m_tanhCumulHistoNonMuonR2_5);

  declareProperty("tanhCumulHistoNonMuonR3_1", m_tanhCumulHistoNonMuonR3_1);
  declareProperty("tanhCumulHistoNonMuonR3_2", m_tanhCumulHistoNonMuonR3_2);
  declareProperty("tanhCumulHistoNonMuonR3_3", m_tanhCumulHistoNonMuonR3_3);
  declareProperty("tanhCumulHistoNonMuonR3_4", m_tanhCumulHistoNonMuonR3_4);
  declareProperty("tanhCumulHistoNonMuonR3_5", m_tanhCumulHistoNonMuonR3_5);

  declareProperty("tanhCumulHistoNonMuonR4_1", m_tanhCumulHistoNonMuonR4_1);
  declareProperty("tanhCumulHistoNonMuonR4_2", m_tanhCumulHistoNonMuonR4_2);
  declareProperty("tanhCumulHistoNonMuonR4_3", m_tanhCumulHistoNonMuonR4_3);
}

/** Initialises the tool.
 * Loads helpers and extracts detector information and variables from the
 * CondDB.
 */
StatusCode DLLMuonTool::initialize() {
  auto sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  hitManager_ = tool<CommonMuonHitManager>("CommonMuonHitManager");
  muonTool_ = tool<ICommonMuonTool>("CommonMuonTool");
  det_ = getDet<DeMuonDetector>(DeMuonLocation::Default);

  // Load station information from detector description
  for (unsigned s = 0; s != nStations; ++s) {
    regionInner_[s] = std::make_pair(det_->getInnerX(s), det_->getInnerY(s));
    regionOuter_[s] = std::make_pair(det_->getOuterX(s), det_->getOuterY(s));
    stationZ_[s] = det_->getStationZ(s);
    for (unsigned r = 0; r != nRegions; ++r) {
      padSize_[s * nRegions + r] =
          std::make_pair(det_->getPadSizeX(s, r), det_->getPadSizeY(s, r));
    }
  }

  if (m_OverrideDB) {
    if (msgLevel(MSG::INFO))
      info() << "Initialise: OverrideDB=true. Data from Conditions database "
                "will be ignored." << endmsg;
    if (msgLevel(MSG::INFO))
      info()
          << "Initialise:                  Using values read from options file"
          << endmsg;
  } else {  // Read values from database
    // Prepare to load information from conditions data base
    Condition* condFoiFactor = nullptr;
    Condition* condPreSelMomentum = nullptr;
    Condition* condMomentumCuts = nullptr;
    Condition* condNumberMupBins = nullptr;
    Condition* condMupBins = nullptr;

    Condition* condNonMuLandauParameterR1 = nullptr;
    Condition* condNonMuLandauParameterR2 = nullptr;
    Condition* condNonMuLandauParameterR3 = nullptr;
    Condition* condNonMuLandauParameterR4 = nullptr;
    // For DLL flag 4
    Condition* condtanhScaleFactors = nullptr;
    // For DLL flag 5
    //Condition* condtanhScaleFactorsMuon = nullptr;
    // Condition* condtanhScaleFactorsNonMuon = nullptr;
    Condition* condtanhCumulHistoMuonR1 = nullptr;
    Condition* condtanhCumulHistoMuonR2 = nullptr;
    Condition* condtanhCumulHistoMuonR3 = nullptr;
    Condition* condtanhCumulHistoMuonR4 = nullptr;
    // Condition* condtanhCumulHistoNonMuonR1 = nullptr;
    // Condition* condtanhCumulHistoNonMuonR2 = nullptr;
    // Condition* condtanhCumulHistoNonMuonR3 = nullptr;
    // Condition* condtanhCumulHistoNonMuonR4 = nullptr;

    registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/FOIfactor",
                                   condFoiFactor);
    registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/PreSelMomentum",
                                   condPreSelMomentum);
    registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/MomentumCuts",
                                   condMomentumCuts);
    registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/nMupBins",
                                   condNumberMupBins);
    registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/MupBins",
                                   condMupBins);

    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/NonMuLandauParameterR1",
        condNonMuLandauParameterR1);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/NonMuLandauParameterR2",
        condNonMuLandauParameterR2);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/NonMuLandauParameterR3",
        condNonMuLandauParameterR3);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/NonMuLandauParameterR4",
        condNonMuLandauParameterR4);
    // for DLL flag 4
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/tanhScaleFactors",
        condtanhScaleFactors);
    // for DLL flag 5
    //registerCondition<DLLMuonTool>(
    //    "Conditions/ParticleID/Muon/tanhScaleFactorsMuon",
    //    condtanhScaleFactorsMuon);
    
    // registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/tanhScaleFactorsNonMuon",
    // condtanhScaleFactorsNonMuon);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/tanhCumulHistoMuonR1",
        condtanhCumulHistoMuonR1);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/tanhCumulHistoMuonR2",
        condtanhCumulHistoMuonR2);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/tanhCumulHistoMuonR3",
        condtanhCumulHistoMuonR3);
    registerCondition<DLLMuonTool>(
        "Conditions/ParticleID/Muon/tanhCumulHistoMuonR4",
        condtanhCumulHistoMuonR4);
    // registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/tanhCumulHistoNonMuonR1",
    // condtanhCumulHistoNonMuonR1);
    // registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/tanhCumulHistoNonMuonR2",
    // condtanhCumulHistoNonMuonR2);
    // registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/tanhCumulHistoNonMuonR3",
    // condtanhCumulHistoNonMuonR3);
    // registerCondition<DLLMuonTool>("Conditions/ParticleID/Muon/tanhCumulHistoNonMuonR4",
    // condtanhCumulHistoNonMuonR4);

    // Update conditions
    sc = runUpdate();
    if (sc.isFailure()) {
      Error("Could not update conditions from the CondDB.");
      return sc;
    }
    // Extract parameters
    foiFactor_ = condFoiFactor->param<double>("FOIfactor");
    preSelMomentum_ = condPreSelMomentum->param<double>("PreSelMomentum");
    momentumCuts_ = condMomentumCuts->paramVect<double>("MomentumCuts");

    // m_nMupBinsR1 = condNumberMupBins->param<int>("nMupBinsR1");
    // m_nMupBinsR2 = condNumberMupBins->param<int>("nMupBinsR2");
    // m_nMupBinsR3 = condNumberMupBins->param<int>("nMupBinsR3");
    // m_nMupBinsR4 = condNumberMupBins->param<int>("nMupBinsR4");
    m_MupBinsR1 = condMupBins->paramVect<double>("MupBinsR1");
    m_MupBinsR2 = condMupBins->paramVect<double>("MupBinsR2");
    m_MupBinsR3 = condMupBins->paramVect<double>("MupBinsR3");
    m_MupBinsR4 = condMupBins->paramVect<double>("MupBinsR4");

    // Non-Muons - Region 1-2-3-4: // flag 4
    m_NonMuLanParR1 =
        condNonMuLandauParameterR1->paramVect<double>("NonMuLandauParameterR1");
    m_NonMuLanParR2 =
        condNonMuLandauParameterR2->paramVect<double>("NonMuLandauParameterR2");
    m_NonMuLanParR3 =
        condNonMuLandauParameterR3->paramVect<double>("NonMuLandauParameterR3");
    m_NonMuLanParR4 =
        condNonMuLandauParameterR4->paramVect<double>("NonMuLandauParameterR4");

    // For DLL flag = 4. The name of the parameter changes for flag = 5
    m_tanhScaleFactorsMuonR1 =
        condtanhScaleFactors->paramVect<double>("tanhScaleFactorsR1");
    m_tanhScaleFactorsMuonR2 =
        condtanhScaleFactors->paramVect<double>("tanhScaleFactorsR2");
    m_tanhScaleFactorsMuonR3 =
        condtanhScaleFactors->paramVect<double>("tanhScaleFactorsR3");
    m_tanhScaleFactorsMuonR4 =
        condtanhScaleFactors->paramVect<double>("tanhScaleFactorsR4");

    // m_tanhScaleFactorsNonMuonR1 =
    // condtanhScaleFactorsNonMuon->paramVect<double>("tanhScaleFactorsNonMuonR1");
    // m_tanhScaleFactorsNonMuonR2 =
    // condtanhScaleFactorsNonMuon->paramVect<double>("tanhScaleFactorsNonMuonR2");
    // m_tanhScaleFactorsNonMuonR3 =
    // condtanhScaleFactorsNonMuon->paramVect<double>("tanhScaleFactorsNonMuonR3");
    // m_tanhScaleFactorsNonMuonR4 =
    // condtanhScaleFactorsNonMuon->paramVect<double>("tanhScaleFactorsNonMuonR4");

    m_tanhCumulHistoMuonR1_1 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_1");
    m_tanhCumulHistoMuonR1_2 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_2");
    m_tanhCumulHistoMuonR1_3 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_3");
    m_tanhCumulHistoMuonR1_4 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_4");
    m_tanhCumulHistoMuonR1_5 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_5");
    m_tanhCumulHistoMuonR1_6 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_6");
    m_tanhCumulHistoMuonR1_7 =
        condtanhCumulHistoMuonR1->paramVect<double>("tanhCumulHistoMuonR1_7");

    m_tanhCumulHistoMuonR2_1 =
        condtanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_1");
    m_tanhCumulHistoMuonR2_2 =
        condtanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_2");
    m_tanhCumulHistoMuonR2_3 =
        condtanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_3");
    m_tanhCumulHistoMuonR2_4 =
        condtanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_4");
    m_tanhCumulHistoMuonR2_5 =
        condtanhCumulHistoMuonR2->paramVect<double>("tanhCumulHistoMuonR2_5");

    m_tanhCumulHistoMuonR3_1 =
        condtanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_1");
    m_tanhCumulHistoMuonR3_2 =
        condtanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_2");
    m_tanhCumulHistoMuonR3_3 =
        condtanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_3");
    m_tanhCumulHistoMuonR3_4 =
        condtanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_4");
    m_tanhCumulHistoMuonR3_5 =
        condtanhCumulHistoMuonR3->paramVect<double>("tanhCumulHistoMuonR3_5");

    m_tanhCumulHistoMuonR4_1 =
        condtanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_1");
    m_tanhCumulHistoMuonR4_2 =
        condtanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_2");
    m_tanhCumulHistoMuonR4_3 =
        condtanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_3");
    m_tanhCumulHistoMuonR4_4 =
        condtanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_4");
    m_tanhCumulHistoMuonR4_5 =
        condtanhCumulHistoMuonR4->paramVect<double>("tanhCumulHistoMuonR4_5");

    /*
    m_tanhCumulHistoNonMuonR1_1 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_1");
    m_tanhCumulHistoNonMuonR1_2 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_2");
    m_tanhCumulHistoNonMuonR1_3 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_3");
    m_tanhCumulHistoNonMuonR1_4 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_4");
    m_tanhCumulHistoNonMuonR1_5 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_5");
    m_tanhCumulHistoNonMuonR1_6 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_6");
    m_tanhCumulHistoNonMuonR1_7 =
    condtanhCumulHistoNonMuonR1->paramVect<double>("tanhCumulHistoNonMuonR1_7");

    m_tanhCumulHistoNonMuonR2_1 =
    condtanhCumulHistoNonMuonR2->paramVect<double>("tanhCumulHistoNonMuonR2_1");
    m_tanhCumulHistoNonMuonR2_2 =
    condtanhCumulHistoNonMuonR2->paramVect<double>("tanhCumulHistoNonMuonR2_2");
    m_tanhCumulHistoNonMuonR2_3 =
    condtanhCumulHistoNonMuonR2->paramVect<double>("tanhCumulHistoNonMuonR2_3");
    m_tanhCumulHistoNonMuonR2_4 =
    condtanhCumulHistoNonMuonR2->paramVect<double>("tanhCumulHistoNonMuonR2_4");
    m_tanhCumulHistoNonMuonR2_5 =
    condtanhCumulHistoNonMuonR2->paramVect<double>("tanhCumulHistoNonMuonR2_5");

    m_tanhCumulHistoNonMuonR3_1 =
    condtanhCumulHistoNonMuonR3->paramVect<double>("tanhCumulHistoNonMuonR3_1");
    m_tanhCumulHistoNonMuonR3_2 =
    condtanhCumulHistoNonMuonR3->paramVect<double>("tanhCumulHistoNonMuonR3_2");
    m_tanhCumulHistoNonMuonR3_3 =
    condtanhCumulHistoNonMuonR3->paramVect<double>("tanhCumulHistoNonMuonR3_3");
    m_tanhCumulHistoNonMuonR3_4 =
    condtanhCumulHistoNonMuonR3->paramVect<double>("tanhCumulHistoNonMuonR3_4");
    m_tanhCumulHistoNonMuonR3_5 =
    condtanhCumulHistoNonMuonR3->paramVect<double>("tanhCumulHistoNonMuonR3_5");

    m_tanhCumulHistoNonMuonR4_1 =
    condtanhCumulHistoNonMuonR4->paramVect<double>("tanhCumulHistoNonMuonR4_1");
    m_tanhCumulHistoNonMuonR4_2 =
    condtanhCumulHistoNonMuonR4->paramVect<double>("tanhCumulHistoNonMuonR4_2");
    m_tanhCumulHistoNonMuonR4_3 =
    condtanhCumulHistoNonMuonR4->paramVect<double>("tanhCumulHistoNonMuonR4_3");
    */
  }
  m_tanhScaleFactorsMuon.push_back(&m_tanhScaleFactorsMuonR1);
  m_tanhScaleFactorsMuon.push_back(&m_tanhScaleFactorsMuonR2);
  m_tanhScaleFactorsMuon.push_back(&m_tanhScaleFactorsMuonR3);
  m_tanhScaleFactorsMuon.push_back(&m_tanhScaleFactorsMuonR4);

  m_tanhScaleFactorsNonMuon.push_back(&m_tanhScaleFactorsNonMuonR1);
  m_tanhScaleFactorsNonMuon.push_back(&m_tanhScaleFactorsNonMuonR2);
  m_tanhScaleFactorsNonMuon.push_back(&m_tanhScaleFactorsNonMuonR3);
  m_tanhScaleFactorsNonMuon.push_back(&m_tanhScaleFactorsNonMuonR4);

  // DLLflag = 3 and 4 for muons
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_1);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_2);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_3);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_4);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_5);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_6);
  m_tanhCumulHistoMuonR1.push_back(&m_tanhCumulHistoMuonR1_7);

  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_1);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_2);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_3);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_4);
  m_tanhCumulHistoMuonR2.push_back(&m_tanhCumulHistoMuonR2_5);

  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_1);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_2);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_3);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_4);
  m_tanhCumulHistoMuonR3.push_back(&m_tanhCumulHistoMuonR3_5);

  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_1);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_2);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_3);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_4);
  m_tanhCumulHistoMuonR4.push_back(&m_tanhCumulHistoMuonR4_5);

  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR1);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR2);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR3);
  m_tanhCumulHistoMuon.push_back(&m_tanhCumulHistoMuonR4);
  // DLLFlag = 5 for non-muons
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_1);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_2);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_3);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_4);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_5);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_6);
  m_tanhCumulHistoNonMuonR1.push_back(&m_tanhCumulHistoNonMuonR1_7);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_1);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_2);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_3);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_4);
  m_tanhCumulHistoNonMuonR2.push_back(&m_tanhCumulHistoNonMuonR2_5);

  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_1);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_2);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_3);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_4);
  m_tanhCumulHistoNonMuonR3.push_back(&m_tanhCumulHistoNonMuonR3_5);

  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_1);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_2);
  m_tanhCumulHistoNonMuonR4.push_back(&m_tanhCumulHistoNonMuonR4_3);

  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR1);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR2);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR3);
  m_tanhCumulHistoNonMuon.push_back(&m_tanhCumulHistoNonMuonR4);

  StatusCode scNorm = calcLandauNorm();
  if (scNorm.isFailure()) {
    return Error(" Normalizations of Landaus not properly set ", scNorm);
  }
  
  return sc;
}

/**Method to calculate the distance from the hit to the extrapolated position
*/
double DLLMuonTool::calcDist(
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation,
    CommonConstMuonHits& hits) const {
  double m_dist = 0.;
  double mCoordX[4] = {0., 0., 0., 0.};
  double mCoordY[4] = {0., 0., 0., 0.};
  double mCoordDX[4] = {0., 0., 0., 0.};
  double mCoordDY[4] = {0., 0., 0., 0.};

  for_each( begin(hits), end(hits), [&](const CommonMuonHit* hit) {
        for (unsigned s = 1; s != 5; ++s) { // No M1 ever
          if (s == hit->station()) {  // same station for hit and extrapolation
            if (mCoordX[s] == 0) {  // first coord found. 
              mCoordX[s] = hit->x();
              mCoordY[s] = hit->y();
              // difference between x,y and the extrapolated x,y
              mCoordDX[s] = (hit->x() - extrapolation[s].first) / hit->dx();
              mCoordDY[s] = (hit->y() - extrapolation[s].second) / hit->dy();
              if (msgLevel(MSG::DEBUG))
                debug() << "calcDist : In station " << s << endmsg;
              if (msgLevel(MSG::DEBUG))
                debug() << "calcDist : x = " << hit->x()
                        << ", m_trackX[station] = " << extrapolation[s].first
                        << ", dx = " << hit->dx() << endmsg;
              if (msgLevel(MSG::DEBUG))
                debug() << "calcDist : y = " << hit->y()
                        << ", m_trackY[station] = " << extrapolation[s].second
                        << ", dy = " << hit->dy() << endmsg;
            } else {
              // get best match in X and Y
              if (msgLevel(MSG::DEBUG))
                debug() << "(x - m_trackX[station])*(x - m_trackX[station]) = "
                        << (hit->x() - extrapolation[s].first) *
                               (hit->x() - extrapolation[s].first) << endmsg;
              if (msgLevel(MSG::DEBUG))
                debug() << "(y - m_trackY[station])*(y - m_trackY[station]) = "
                        << (hit->y() - extrapolation[s].second) *
                               (hit->y() - extrapolation[s].second) << endmsg;
              if (msgLevel(MSG::DEBUG))
                debug() << "pow((mCoordX[station] - m_trackX[station]),2) = "
                        << pow((mCoordX[s] - extrapolation[s].first), 2)
                        << endmsg;
              if (msgLevel(MSG::DEBUG))
                debug() << "pow((mCoordY[station] - m_trackY[station]),2) = "
                        << pow((mCoordY[s] - extrapolation[s].second), 2)
                        << endmsg;
              if (pow((hit->x() - extrapolation[s].first), 2) +
                      pow((hit->y() - extrapolation[s].second), 2) <
                  pow((mCoordX[s] - extrapolation[s].first), 2) +
                      pow((mCoordY[s] - extrapolation[s].second), 2)) {
                // this Coord is a better match
                mCoordX[s] = hit->x();
                mCoordY[s] = hit->y();
                mCoordDX[s] = (hit->x() - extrapolation[s].first) / hit->dx();
                mCoordDY[s] = (hit->y() - extrapolation[s].second) / hit->dy();
                if (msgLevel(MSG::DEBUG))
                  debug() << "calcDist : In station " << s << endmsg;
                if (msgLevel(MSG::DEBUG))
                  debug() << "calcDist : x = " << hit->x()
                          << ", m_trackX[station] = " << extrapolation[s].first
                          << ", dx = " << hit->dx() << endmsg;
                if (msgLevel(MSG::DEBUG))
                  debug() << "calcDist : y = " << hit->y()
                          << ", m_trackY[station] = " << extrapolation[s].second
                          << ", dy = " << hit->dy() << endmsg;
              }
            }
          }
        }
      });

  // calculate the square of the distances
  unsigned nstn = 0;
  for (unsigned stn = 0; stn != 4; ++stn) {  // No M1 case: 5 here refers to dimension
                                       // of mCoordDX arrays JHL 12/Oct./2013
    if (msgLevel(MSG::DEBUG)) {
      debug() << "calcDist: station: " << stn << " mCoordDX = " << mCoordDX[stn]
              << " mCoordDY = " << mCoordDY[stn] << endmsg;
    }
    if (mCoordDX[stn] != 0. && mCoordDY[stn] != 0.) {
      if (msgLevel(MSG::DEBUG)) debug() << "nstn = " << nstn << endmsg;
      if (msgLevel(MSG::DEBUG))
        debug() << "mCoordDX[stn]*mCoordDX[stn] = "
                << mCoordDX[stn] * mCoordDX[stn] << endmsg;
      if (msgLevel(MSG::DEBUG))
        debug() << "mCoordDY[stn]*mCoordDY[stn] = "
                << mCoordDY[stn] * mCoordDY[stn] << endmsg;
      nstn++;
      if (msgLevel(MSG::DEBUG)) debug() << "m_dist = " << m_dist << endmsg;
      m_dist += pow(mCoordDX[stn], 2) + pow(mCoordDY[stn], 2);
    }
  }
  if (msgLevel(MSG::DEBUG)) debug() << "m_dist Total = " << m_dist << endmsg;
  m_dist = m_dist / nstn;  // divide by the number of station

  return m_dist;
}

/** Calculate the number of tracks that share hits
 */
bool DLLMuonTool::calcNShared(
    LHCb::MuonPID* muonid, LHCb::MuonPIDs* pMuids, CommonConstMuonHits& hits,
    const ICommonMuonTool::MuonTrackExtrapolation& extr){
  
  unsigned nNSH;
  double dsquare1;
  // Calculate dist only if it is not store already in the distance map
  // TODO: the following should be made a function
  if ( m_muonDistMap.find(muonid) == m_muonDistMap.end() ) {
    // not found
    dsquare1 = calcDist(extr, hits);
    if (msgLevel(MSG::DEBUG))
      debug() << "     For ORIGINAL track with momentum p = "
              << (muonid->idTrack())->p() << ". Dist 1 = " << dsquare1
              << ". The track container has " << pMuids->size() << "tracks."
              << endmsg;
    if (dsquare1 < 0.) {
      muonid->setNShared(100);
      return Warning("calcDist 1 failure", false);
    }
    // Store distance in the muon map 
    m_muonDistMap[muonid] = dsquare1;
  } else {
    // Get distance from the muon map 
    dsquare1 = m_muonDistMap[muonid];
  }
  
  CommonConstMuonHits hits2;

  // loop over the muonPIDs
  LHCb::MuonPIDs::const_iterator iMuon, end(pMuids->end());
  for (iMuon = pMuids->begin(); iMuon != end; ++iMuon) {
    // skip if this muonPID is the same as the input or if IsMuonLoose is false
    if (!(*iMuon)->IsMuonLoose() || *iMuon == muonid) continue;
    if (msgLevel(MSG::DEBUG))
      debug()
          << "---------> Start the computation of Dist 2 for track with p = "
          << (*iMuon)->idTrack()->p() << " <----------" << endmsg;
    // see if there are shared hits between the muonIDs
    if (msgLevel(MSG::DEBUG))
      debug() << "Comparing primary with p = " << (muonid->idTrack())->p()
              << " with secondary of p = " << (*iMuon)->idTrack()->p()
              << endmsg;
    // Get hits from muon hits map 
    hits2 = m_muonMap[*iMuon];
    if (compareHits(hits, hits2)) {
      // get dist for this muonID
      
      double dsquare2;
      // Calculate dist only if it is not store already in the distance map
      if ( m_muonDistMap.find(*iMuon) == m_muonDistMap.end() ) {
        // not found
        const LHCb::Track* track2 =
          (*iMuon)->idTrack();  // get the track of the muonID
        const auto extr2 = muonTool_->extrapolateTrack(*track2);

        dsquare2 = calcDist(extr2, hits2);

        if (msgLevel(MSG::DEBUG))
          debug() << "     For ORIGINAL track with momentum p = "
                  << (muonid->idTrack())->p() << ". Dist 1 = " << dsquare1
                  << ". The track container has " << pMuids->size() << "tracks."
                  << endmsg;
        if (dsquare2 < 0.) {
          muonid->setNShared(100);
          return Warning("calcDist 2 failure", false);
        }
        if (msgLevel(MSG::DEBUG))
          debug() << "            For SECONDARY track with momentum = "
                  << track2->p() << ". Dist 2 = " << dsquare2 << endmsg;
        // Store distance in the muon map 
        m_muonDistMap[*iMuon] = dsquare2;
      } else {
        // Get distance from the muon map 
        dsquare2 = m_muonDistMap[*iMuon];
      }
  
      // the muonID which gets the number of shared hits is the one
      // which has the biggest dist
      if (dsquare2 < dsquare1) {
        nNSH = muonid->nShared();
        if (msgLevel(MSG::DEBUG))
          debug() << "Setting the nShared to the original track. NShared =  "
                  << nNSH << endmsg;
        muonid->setNShared(nNSH + 1);
      } else {
        nNSH = (*iMuon)->nShared();
        if (msgLevel(MSG::DEBUG))
          debug() << "Setting the nShared to the secondary track. NShared =  "
                  << nNSH << endmsg;
        (*iMuon)->setNShared(nNSH + 1);
      }
    }
  }
  if (msgLevel(MSG::DEBUG))
    debug() << "nShared = " << muonid->nShared() << endmsg;

  // Store hits in the muon map for this track
  m_muonMap[muonid] = hits; 
  
  return true;
}

// Get the bin for the given P
unsigned DLLMuonTool::GetPbin(double p, unsigned region) {
  std::vector<double>* pBins = 0;
  switch (region + 1) {
    case 1: {
      pBins = &m_MupBinsR1;
      break;
    }
    case 2: {
      pBins = &m_MupBinsR2;
      break;
    }
    case 3: {
      pBins = &m_MupBinsR3;
      break;
    }
    case 4: {
      pBins = &m_MupBinsR4;
      break;
    }
  }
  if (msgLevel(MSG::VERBOSE))
    verbose() << "GetPbin: region+1 " << region + 1 << " p " << p
              << " pBins vector address: " << pBins << endmsg;
  if (0 == pBins)
    throw GaudiException(
        "GetPbin: No match to a pBins vector. Null pBins pointer", "",
        StatusCode::FAILURE);
  unsigned end(pBins->size());
  for (unsigned iBin = 0; iBin != end; ++iBin) {
    if (msgLevel(MSG::VERBOSE))
      verbose() << "GetPbin:\tiBin " << iBin << " pBins[iBin] "
                << (*pBins)[iBin] << endmsg;
    if (p < (*pBins)[iBin]) return iBin;
  }
  return pBins->size();  // last bin. npBins goes from 0 to (Gaias npBin)+1
}

std::vector<double> DLLMuonTool::loadNonMuLandauParam(
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation) {
  std::vector<double> parNonMu;
  int region = findTrackRegions(extrapolation)[1];  // M2 JHLJHL Check M2 and M3
                                                    // indices if no M1
                                                    // 30/08/2013
  if (region < 0) region = findTrackRegions(extrapolation)[2];  // M3
  if (region == 0) {                                            // Region 1
    for (unsigned i = 0; i != 3; ++i) {
      parNonMu[i] = m_NonMuLanParR1[i];
    }
  } else if (region == 1) {  // Region 2
    for (unsigned i = 0; i != 3; ++i) {
      parNonMu[i] = m_NonMuLanParR2[i];
    }
  } else if (region == 2) {  // Region 3
    for (unsigned i = 0; i != 3; ++i) {
      parNonMu[i] = m_NonMuLanParR3[i];
    }
  } else if (region == 3) {  // Region 4
    for (unsigned i = 0; i != 3; ++i) {
      parNonMu[i] = m_NonMuLanParR4[i];
    }
  } else
    throw GaudiException("Not valid region", "", StatusCode::FAILURE);
  return parNonMu;
}

std::vector<int> DLLMuonTool::findTrackRegions(
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation) {
  //=====================================================================
  // comment: Returns the muon detector region of the extrapolated track;
  //=====================================================================

  std::vector<int> trackRegion(nStations, -1);
  // Region of the track extrapolated:

  for (unsigned sta = 0; sta != nStations; ++sta) {
    int chnum = -1;
    int regnum = -1;
    det_->Pos2StChamberNumber(extrapolation[sta].first,
                              extrapolation[sta].second, sta, chnum,
                              regnum).ignore();
    trackRegion[sta] = regnum;
    if (trackRegion[sta] < 0 && msgLevel(MSG::DEBUG)) {
      debug() << format(
                     " Track extrapolation in station %d gives not-existent "
                     "region ",
                     sta) << endmsg;
    }
  }

  if (msgLevel(MSG::DEBUG)) debug() << "TrackRegions" << trackRegion << endmsg;
  return trackRegion;
}

//=====================================================================
double DLLMuonTool::calc_closestDist(
    const LHCb::Track& track,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation,
    CommonConstMuonHits& hits,
    std::array<unsigned, ICommonMuonTool::nStations>& occupancies) {
  //=====================================================================
  // comment: Returns the squared distance calculated with closest hit
  //=====================================================================

  double closest_dist = 0;

  // Get closest hits
  double foiXDim, foiYDim;

  for (unsigned ista = 0; ista != 5;
       ++ista) {  // No M1 case: 5 here is OK, these arrays have dimension 5 JHL
                  // 12/Oct./2013
    closest_x[ista] = -1;
    closest_y[ista] = -1;
    small_dist[ista] = 10000000.;
    Fdist[ista] = 0;
    closest_region[ista] = -1; 

  }
  double p = track.p();
  for_each(
      std::begin(hits), std::end(hits),
      [&](const CommonMuonHit* hit) {  // hits in FOI
        unsigned s = hit->tile().station();
        unsigned r = hit->tile().region();
        foiXDim = hit->dx() * foiFactor_ * muonTool_->foi(s, r, p).first;
        foiYDim = hit->dy() * foiFactor_ * muonTool_->foi(s, r, p).second;
        if (msgLevel(MSG::DEBUG))
          debug() << "The p = " << p << ", with foiFactor = " << foiFactor_
                  << ", foiX = " << muonTool_->foi(s, r, p).first
                  << ", and foiY = " << muonTool_->foi(s, r, p).second
                  << endmsg;
        if (msgLevel(MSG::DEBUG))
          debug() << "For hit in x = " << hit->x() << ", and y= " << hit->y()
                  << ". Extrapolation in x = " << extrapolation[s].first
                  << ", y = " << extrapolation[s].second << endmsg;
        if (msgLevel(MSG::DEBUG))
          debug() << "The FOI dimension is, foiXDim  = " << foiXDim
                  << ", foiYDim = " << foiYDim << endmsg;
        if (s >=
            1) {  // JHL: station == 0 is OK in the case of no M1  12/Oct./2013
          if ((fabs(hit->x() - extrapolation[s].first) < foiXDim) &&
              (fabs(hit->y() - extrapolation[s].second) < foiYDim)) {
            if (msgLevel(MSG::DEBUG))
              debug() << "residX = " << fabs(hit->x() - extrapolation[s].first)
                      << ", and residY = "
                      << fabs(hit->y() - extrapolation[s].second) << endmsg;

            Fdist[s] = pow((hit->x() - extrapolation[s].first) / hit->dx(), 2) +
                       pow((hit->y() - extrapolation[s].second) / hit->dy(), 2);
            if (msgLevel(MSG::DEBUG))
              debug() << "FDist in station " << s << " = " << Fdist[s]
                      << endmsg;
            if (Fdist[s] < small_dist[s]) {
              small_dist[s] = Fdist[s];
              closest_x[s] = (hit->x() - extrapolation[s].first) / hit->dx();
              closest_y[s] = (hit->y() - extrapolation[s].second) / hit->dy();
              closest_region[s] = r;
            }  // Fdist
          }    // foi
        }      // station
      });      // hit

  if (msgLevel(MSG::DEBUG)) {
    debug()
        << "calc_closestDist: station closest_region \t closest_x \t closest_y"
        << endmsg;
    for (unsigned ist = 0; ist != 5; ++ist) {
      debug() << ist << " " << closest_region[ist] << "\t" << closest_x[ist]
              << "\t" << closest_y[ist] << endmsg;
    }
  }


  if (p > preSelMomentum_ &&
      p < momentumCuts_[0]) {  // JHLJHL: Check station indices whith no M1
                               // configuration 30/08/2013
    // 3 or 2 stations
    if (occupancies[iM2] > 0 && occupancies[iM3] > 0 &&
        occupancies[iM4] > 0) {  // M2 &&M3 && M4
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2);
      closest_dist = closest_dist / 3.;
      return closest_dist;
    }
    if (occupancies[iM2] > 0 && occupancies[iM3] > 0) {  // M2 &&M3
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2);
      closest_dist = closest_dist / 2.;
      return closest_dist;
    }
    if (occupancies[iM2] > 0 && occupancies[iM4] > 0) {  // M2 &&M4
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2);
      closest_dist = closest_dist / 2.;
      return closest_dist;
    }
    if (occupancies[iM3] > 0 && occupancies[iM4] > 0) {  // M3 &&M4
      closest_dist = pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2);
      closest_dist = closest_dist / 2.;
      return closest_dist;
    }
  }  // 3-6

  if (p > momentumCuts_[0]) {
    if (occupancies[iM2] > 0 && occupancies[iM3] > 0 && occupancies[iM4] > 0 &&
        occupancies[iM5] > 0) {
      // M2 &&M3&&M4&&M5
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2) +
                     pow(closest_x[iM5], 2) + pow(closest_y[iM5], 2);
      closest_dist = closest_dist / 4.;
      return closest_dist;
    }
    if (occupancies[iM2] > 0 && occupancies[iM3] > 0 &&
        occupancies[iM4]) {  // M2 && M3 && M4
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2);
      closest_dist = closest_dist / 3.;
      return closest_dist;
    }
    if (occupancies[iM2] > 0 && occupancies[iM3] > 0 &&
        occupancies[iM5]) {  // M2 && M3 && M5
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM5], 2) + pow(closest_y[iM5], 2);
      closest_dist = closest_dist / 3.;
      return closest_dist;
    }
    if (occupancies[iM3] > 0 && occupancies[iM4] > 0 &&
        occupancies[iM5] > 0) {  // M3 &&M4 && M5
      closest_dist = pow(closest_x[iM3], 2) + pow(closest_y[iM3], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2) +
                     pow(closest_x[iM5], 2) + pow(closest_y[iM5], 2);
      closest_dist = closest_dist / 3.;
      return closest_dist;
    }
    if (occupancies[iM2] > 0 && occupancies[iM4] > 0 &&
        occupancies[iM5] > 0) {  // M2 &&M4 && M5
      closest_dist = pow(closest_x[iM2], 2) + pow(closest_y[iM2], 2) +
                     pow(closest_x[iM4], 2) + pow(closest_y[iM4], 2) +
                     pow(closest_x[iM5], 2) + pow(closest_y[iM5], 2);
      closest_dist = closest_dist / 3.;
      return closest_dist;
    }
  }
  if (!closest_dist) return 0;
  return closest_dist;
}

//=====================================================================
double DLLMuonTool::calc_ProbNonMu_tanh(double tanhdist0, unsigned pBin,
                                        unsigned region) {
  unsigned nDistBins = (*(*m_tanhCumulHistoNonMuon[region])[pBin]).size();
  unsigned itanhdist = unsigned(tanhdist0 * nDistBins);

  if (itanhdist >= nDistBins) itanhdist = nDistBins - 1;

  if (msgLevel(MSG::DEBUG))
    debug() << "calc_ProbNonMu_tanh: region " << region << " pBin " << pBin
            << " tanh(dist) " << tanhdist0 << " itanhdist " << itanhdist
            << " ProbNonMu "
            << (*((*(m_tanhCumulHistoNonMuon[region]))[pBin]))[itanhdist]
            << endmsg;
  return (*((*(m_tanhCumulHistoNonMuon[region]))[pBin]))[itanhdist];
}

//=====================================================================
double DLLMuonTool::calc_ProbMu_tanh(double tanhdist0, unsigned pBin,
                                     unsigned region) {
  unsigned nDistBins = (*(*m_tanhCumulHistoMuon[region])[pBin]).size();
  unsigned itanhdist = unsigned(tanhdist0 * nDistBins);

  if (itanhdist >= nDistBins) itanhdist = nDistBins - 1;

  if (msgLevel(MSG::DEBUG))
    debug() << "calc_ProbMu_tanh: region " << region << " pBin " << pBin
            << " tanh(dist) " << tanhdist0 << " itanhdist " << itanhdist
            << " ProbMu "
            << (*((*(m_tanhCumulHistoMuon[region]))[pBin]))[itanhdist]
            << endmsg;
  return (*((*(m_tanhCumulHistoMuon[region]))[pBin]))[itanhdist];
}

double DLLMuonTool::calc_ProbNonMu(double dist0,
                                   std::vector<double>& parNonMu) {
  //=====================================================================
  // comment: Returns for a given track of dist0 the probability to be a
  // non-muon;
  //=====================================================================

  double Prob = ROOT::Math::landau_cdf(dist0, parNonMu[1], parNonMu[0]) -
                ROOT::Math::landau_cdf(0, parNonMu[1], parNonMu[0]);
  if (msgLevel(MSG::DEBUG))
    debug() << "ProbNonMu, parNonMu[0], parNonMu[2] = " << Prob << ", "
            << parNonMu[0] << ", " << parNonMu[2] << endmsg;
  if (parNonMu[2] > 0) {
    if (msgLevel(MSG::DEBUG))
      debug() << "probnmu, parNonMu[2] : " << Prob << "," << parNonMu[2]
              << endmsg;
    return Prob = Prob / parNonMu[2];
  } else {
    Warning("ProbNonMu: normalization out of control ", StatusCode::FAILURE)
        .ignore();
    return -1.;
  }
}

StatusCode DLLMuonTool::calcLandauNorm() {
  //=====================================================================
  // comment: Normalizations for Landaus for Muons and Non-Muons
  //=====================================================================
  
  double parnmu[3];
  for (unsigned i = 1; i != 3; ++i) parnmu[i] = 0;
  double Norm = -1;

  //=============
  //  Non-Muons
  //=============
  
  if (m_NonMuLanParR1.size() < 3) {
    return Error("NonMuLandauParameterR1 not filled properly");
  }
  for (unsigned i = 0; i != 3; ++i) parnmu[i] = m_NonMuLanParR1[i];
  Norm = calcNorm_nmu(parnmu);
  if (Norm < 0 || Norm == 0)
    return Error("normalization of Non-Muon R1 out of control");
  m_NonMuLanParR1[2] = Norm;

  if (m_NonMuLanParR2.size() < 3) {
    return Error("NonMuLandauParameterR2 not filled properly");
  }
  for (unsigned i = 0; i != 3; ++i) parnmu[i] = m_NonMuLanParR2[i];
  Norm = calcNorm_nmu(parnmu);
  if (Norm < 0 || Norm == 0)
    return Error("normalization of Non-Muon R2 out of control");
  m_NonMuLanParR2[2] = Norm;

  if (m_NonMuLanParR3.size() < 3) {
    return Error("NonMuLandauParameterR3 not filled properly");
  }
  for (unsigned i = 0; i != 3; ++i) parnmu[i] = m_NonMuLanParR3[i];
  Norm = calcNorm_nmu(parnmu);
  if (Norm < 0 || Norm == 0)
    return Error("normalization of Non-Muon R3 out of control");
  m_NonMuLanParR3[2] = Norm;

  if (m_NonMuLanParR4.size() < 3) {
    return Error("NonMuLandauParameterR4 not filled properly");
  }
  for (unsigned i = 0; i != 3; ++i) parnmu[i] = m_NonMuLanParR4[i];
  Norm = calcNorm_nmu(parnmu);
  if (Norm < 0 || Norm == 0)
    return Error("normalization of Non-Muon R4 out of control");
  m_NonMuLanParR4[2] = Norm;

  return StatusCode::SUCCESS;
}

double DLLMuonTool::calcNorm_nmu(double* par) {
  //=====================================================================
  // comment: Calculate Normalizations for non-muons
  //=====================================================================

  double m_x = 0.2;
  double m_nMax = 4000;
  double Norm = ROOT::Math::landau_cdf(m_x * m_nMax, par[1], par[0]) -
                ROOT::Math::landau_cdf(0., par[1], par[0]);

  return Norm;
}

std::tuple<double, double, double> DLLMuonTool::calcMuonLL_tanhdist(
    const LHCb::Track& track,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation,
    CommonConstMuonHits& hits,
    std::array<unsigned, ICommonMuonTool::nStations>& occupancies) {
  //=============================================================================
  // comment: Calculate the muon DLL using cumulative histos of the hyperbolic
  //          tangent of the closest distance tuned on runI data for muon and
  //          non-muon
  //          hypothesis per region and momentum bins:
  //=============================================================================

  double p = track.p();

  // Initialize some variables:
  double myDist = -1.;
  double ProbMu = -1.;
  double ProbNonMu = -1.;

  std::vector<int> regions = findTrackRegions(extrapolation);

  // Calculate Distance using the closest hit:
  myDist = calc_closestDist(track, extrapolation, hits, occupancies);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of myDist is = " << myDist << endmsg;

  int region = regions[1];  // M2   JHLJHL Check indices ... 30/08/2013
  if (region < 0) region = regions[2];  // M3

  // Determine the momentum bin for this region
  unsigned pBin = GetPbin(p, region);
  double tanhdistMu, tanhdistNonMu;
  // Calculate tanh(dist). The effetive scale factor is after dividing by arc
  // tanh(0.5)
  if (msgLevel(MSG::DEBUG))
    debug() << "m_tanhScaleFactorsMuon[region] = "
            << (*(m_tanhScaleFactorsMuon[region]))[pBin] << endmsg;
  tanhdistMu =
      tanh(myDist / (*(m_tanhScaleFactorsMuon[region]))[pBin] * ATANH05);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of Tanhdist Muon is = " << tanhdistMu << endmsg;
  tanhdistNonMu =
      tanh(myDist / (*(m_tanhScaleFactorsNonMuon[region]))[pBin] * ATANH05);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of Tanhdist NonMuon is = " << tanhdistNonMu << endmsg;

  // Calculate Prob(mu)  for a given track using tanh(dist);
  ProbMu = calc_ProbMu_tanh(tanhdistMu, pBin, region);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of ProbMu is = " << ProbMu << endmsg;

  // Calculate ProbNonMu using Landau fits
  ProbNonMu = calc_ProbNonMu_tanh(tanhdistNonMu, pBin, region);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of ProbNonMu is = " << ProbNonMu << endmsg;

  // Using histograms it's not unexpected that some bins are empty. Use very
  // small prob as a protection for the log
  if (ProbMu < 1.e-30) {
    if (msgLevel(MSG::DEBUG))
      debug() << "calcMuonLL_tanhdist: Found null ProbMu: " << ProbMu << endmsg;
    ProbMu = 1.e-30;
  }
  if (ProbNonMu < 1.e-30) {
    if (msgLevel(MSG::DEBUG))
      debug() << "calcMuonLL_tanhdist: Found null ProbNonMu: " << ProbNonMu
              << endmsg;
    ProbNonMu = 1.e-30;
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "calcMuonLL_tanhdist: region: " << region << " momentum: " << p
            << " pBin: " << pBin << " dist " << myDist
            << " tanh(dist2) for Mu: " << tanhdistMu
            << " tanh(dist2) for Non Mu: " << tanhdistNonMu
            << " ProbMu: " << ProbMu << " ProbNonMu: " << ProbNonMu
            << " DLL: " << log(ProbMu / ProbNonMu) << endmsg;
  }

  return std::make_tuple(ProbMu, ProbNonMu, myDist);
}

std::tuple<double, double, double> DLLMuonTool::calcMuonLL_tanhdist_landau(
    const LHCb::Track& track,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation,
    CommonConstMuonHits& hits,
    std::array<unsigned, ICommonMuonTool::nStations>& occupancies) {
  //=============================================================================
  // comment: Calculate the muon DLL using cumulative histos of the hyperbolic
  //          tangent of the closest distance tuned on 2010 data for muon
  //          hypothesis
  //          and previous landau fittings to 2009 MC for non-muon hypothesis,
  //          per region and momentum bins:
  //=============================================================================

  double p = track.p();

  // Initialize some variables:
  double myDist = -1.;
  double ProbMu = -1.;
  double ProbNonMu = -1.;
  std::vector<int> regions = findTrackRegions(extrapolation);

  // Calculate Distance using the closest hit:
  myDist = calc_closestDist(track, extrapolation, hits, occupancies);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of myDist is = " << myDist << endmsg;

  int region = regions[1];  // M2   JHLJHL Check indices ... 30/08/2013
  if (region < 0) region = regions[2];  // M3

  // Find Landau's parameters for a given track:
  std::vector<double> parNonMu(3, 0);

  // RVG TEST. HARDCODED IMPLEMENTATION
  // parNonMu = loadNonMuLandauParam(extrapolation);
  if (region == 0)
    parNonMu = {36.5, 23.8, 0.919259};  //# region 1
  else if (region == 1)
    parNonMu = {40.5, 33.3, 0.867921};  //# region 2
  else if (region == 2)
    parNonMu = {22.0, 30.0, 0.800642};  //# region 3
  else if (region == 3)
    parNonMu = {10.0, 17.0, 0.795441};  //# region 4

  // Determine the momentum bin for this region
  unsigned pBin = GetPbin(p, region);
  double tanhdist;
  // Calculate tanh(dist). The effetive scale factor is after dividing by arc
  // tanh(0.5)
  tanhdist = tanh(myDist / (*(m_tanhScaleFactorsMuon[region]))[pBin] * ATANH05);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of Tanhdist is = " << tanhdist << endmsg;

  // Calculate Prob(mu)  for a given track using tanh(dist);
  ProbMu = calc_ProbMu_tanh(tanhdist, pBin, region);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of ProbMu is = " << ProbMu << endmsg;

  // Calculate ProbNonMu using Landau fits
  ProbNonMu = calc_ProbNonMu(myDist, parNonMu);
  if (msgLevel(MSG::DEBUG))
    debug() << "The value of ProbNonMu is = " << ProbNonMu << endmsg;

  // Using histograms it's not unexpected that some bins are empty. Use very
  // small prob as a protection for the log
  if (ProbMu < 1.e-30) {
    if (msgLevel(MSG::DEBUG))
      debug() << "calcMuonLL_tanhdist_landau: Found null ProbMu: " << ProbMu
              << endmsg;
    ProbMu = 1.e-30;
  }
  if (ProbNonMu < 1.e-30) {
    if (msgLevel(MSG::DEBUG))
      debug() << "calcMuonLL_tanhdist_landau: Found null ProbNonMu: "
              << ProbNonMu << endmsg;
    ProbNonMu = 1.e-30;
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "calcMuonLL_tanhdist_landau: region: " << region
            << " momentum: " << p << " pBin: " << pBin << " dist " << myDist
            << " tanh(dist2): " << tanhdist << " ProbMu: " << ProbMu
            << " ProbNonMu: " << ProbNonMu
            << " DLL: " << log(ProbMu / ProbNonMu) << endmsg;
  }

  return std::make_tuple(ProbMu, ProbNonMu, myDist);
}

/**  Method to compare the Coords of the muonIDs
* modified from the original MuonIDAlg.cpp
*/
bool DLLMuonTool::compareHits(CommonConstMuonHits& hits1,
                              CommonConstMuonHits& hits2){
  bool theSame = false;
  //unsigned m_iM2 = 1;  // this is to start from M2

  for_each(
      std::begin(hits1), std::end(hits1),
      [&](const CommonMuonHit* hit1) {  // first hits
        for_each(
            std::begin(hits2), std::end(hits2),
            [&](const CommonMuonHit* hit2) {  // second hits
              if (msgLevel(MSG::DEBUG))
                debug() << "compareHits:   The hit1 is = " << hit1->tile().key()
                        << ". The hit2 is = " << hit2->tile().key() << endmsg;
              if (hit1->tile().key() == hit2->tile().key()) {
                if (msgLevel(MSG::DEBUG))
                  debug() << "For hits that are the same, the station is = "
                          << hit1->tile().station() << endmsg;
                if (hit1->tile().station() >= iM2) theSame = true;
              }
          });
      });

  return theSame;
}
