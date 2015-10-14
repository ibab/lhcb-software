// ROOT
#include "TH1.h"

// Gaudi
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
#include "DetDesc/Condition.h"

// Local
#include "HCMonitorBase.h"

//=============================================================================
// Standard constructor
//=============================================================================
HCMonitorBase::HCMonitorBase(const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiTupleAlg(name, pSvcLocator),
      m_cond(nullptr) {

  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);

  declareProperty("MasksB0", m_masksB0 = {0, 0, 0, 0});
  declareProperty("MasksB1", m_masksB1 = {0, 0, 0, 0});
  declareProperty("MasksB2", m_masksB2 = {0, 0, 0, 0});
  declareProperty("MasksF1", m_masksF1 = {0, 0, 0, 0});
  declareProperty("MasksF2", m_masksF2 = {0, 0, 0, 0});

  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);

  declareProperty("SpareChannelsB0", m_sparesB0);
  declareProperty("SpareChannelsB1", m_sparesB1);
  declareProperty("SpareChannelsB2", m_sparesB2);
  declareProperty("SpareChannelsF1", m_sparesF1);
  declareProperty("SpareChannelsF2", m_sparesF2);

  // Binning options
  declareProperty("VariableBins", m_variableBins = false);
  declareProperty("RandomiseADC", m_randomiseAdc = true);
  declareProperty("MinBX", m_bxMin = 0);
  declareProperty("MaxBX", m_bxMax = 10000);
  declareProperty("Edges", m_edges = {-0.5,1023.5});
}

//=============================================================================
// Destructor
//=============================================================================
HCMonitorBase::~HCMonitorBase() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCMonitorBase::initialize() {

  // Initialise the base class.
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  // Setup the binning and ADC correction.
  m_adcCorrection.resize(1024, 0.);
  m_adcStep.resize(1024, 0);
  unsigned int adc = 0;
  unsigned int step = 1;
  if (m_variableBins){  
    m_edges.clear();
    while (adc < 1024) {
      m_edges.push_back(adc - 0.5);
      if (adc == 128)
        step = 2;
      else if (adc == 256)
        step = 8;
      else if (adc == 512)
        step = 16;
      m_adcCorrection[adc] = 0.5 * (step - 1.);
      m_adcStep[adc] = step;
      adc += step;
    }
    m_edges.push_back(1023.5);
  } else {
    double minVal(m_edges[0]);
    double maxVal(m_edges[1]);
    m_edges.clear();
    for (int i = 0; i < maxVal-minVal +1 ; ++i) {
      m_edges.push_back(minVal+i);
    }
  }
  
  // Setup the mapping.
  m_channels.resize(5);
  m_references.resize(5);
  m_masked.resize(5);
  // Check if the mapping is available in the conditions database.
  const std::string location = "Conditions/ReadoutConf/HC/Mapping";
  if (existDet<Condition>(location)) {
    registerCondition(location, m_cond, &HCMonitorBase::cacheMapping);
    // First update.
    sc = updMgrSvc()->update(this);
    if (sc.isFailure()) {
      return Error("Cannot update mapping.", StatusCode::FAILURE);
    }
  } else {
    warning() << "Cannot find " << location << " in database" << endmsg;
  }
  mapChannels(m_channelsB0, m_sparesB0, m_masksB0, 0, true);
  mapChannels(m_channelsB1, m_sparesB1, m_masksB1, 1, true);
  mapChannels(m_channelsB2, m_sparesB2, m_masksB2, 2, true);
  mapChannels(m_channelsF1, m_sparesF1, m_masksF1, 1, false);
  mapChannels(m_channelsF2, m_sparesF2, m_masksF2, 2, false);
  printMapping();

  // Setup the random number generator.
  if (!m_uniform.initialize(randSvc(), Rndm::Flat(0., 1.))) {
    return Error("Unable to initialize random number generator.",
                 StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Update the channel map using the conditions database.
//=============================================================================
StatusCode HCMonitorBase::cacheMapping() {

  m_crateB = m_cond->param<int>("CrateB");
  m_crateF = m_cond->param<int>("CrateF");

  m_channelsB0 = m_cond->paramVect<int>("ChannelsB0");
  m_channelsB1 = m_cond->paramVect<int>("ChannelsB1");
  m_channelsB2 = m_cond->paramVect<int>("ChannelsB2");
  m_channelsF1 = m_cond->paramVect<int>("ChannelsF1");
  m_channelsF2 = m_cond->paramVect<int>("ChannelsF2");

  m_sparesB0 = m_cond->paramVect<int>("ReferenceChannelsB0");
  m_sparesB1 = m_cond->paramVect<int>("ReferenceChannelsB1");
  m_sparesB2 = m_cond->paramVect<int>("ReferenceChannelsB2");
  m_sparesF1 = m_cond->paramVect<int>("ReferenceChannelsF1");
  m_sparesF2 = m_cond->paramVect<int>("ReferenceChannelsF2");

  m_masksB0 = m_cond->paramVect<int>("MasksB0");
  m_masksB1 = m_cond->paramVect<int>("MasksB1");
  m_masksB2 = m_cond->paramVect<int>("MasksB2");
  m_masksF1 = m_cond->paramVect<int>("MasksF1");
  m_masksF2 = m_cond->paramVect<int>("MasksF2");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Setup the channel map for a given station.
//=============================================================================
bool HCMonitorBase::mapChannels(const std::vector<int>& channels,
                                const std::vector<int>& refs,
                                const std::vector<int>& masks,
                                const unsigned int station,
                                const bool bwd) {

  const unsigned int offset = bwd ? 0 : 2;
  // Check if the input is valid.
  if (channels.size() != 4 || refs.size() != 4 || masks.size() != 4) {
    std::string s = bwd ? "B" : "F";
    s += std::to_string(station);
    warning() << "Invalid channel map for station " << s 
              << ". Masking all quadrants." << endmsg;
    m_channels[station + offset].assign(4, 0);
    m_references[station + offset].assign(4, 0);
    m_masked[station + offset].assign(4, true);
    return false;
  }

  const unsigned int crate = bwd ? m_crateB : m_crateF;
  m_channels[station + offset].resize(4);
  m_references[station + offset].resize(4);
  m_masked[station + offset].resize(4);
  for (unsigned int i = 0; i < 4; ++i) {
    if (channels[i] < 0) {
      std::string s = bwd ? "B" : "F";
      s += std::to_string(station);
      warning() << "Invalid channel number " << channels[i]
                << ". Masking quadrant " << s << i << endmsg;
      m_masked[station + offset][i] = true;
      continue;
    }
    m_channels[station + offset][i] = (crate << 6) | channels[i];
    m_references[station + offset][i] = (crate << 6) | refs[i];
    m_masked[station + offset][i] = masks[i] != 0;
  }
  return true;
}

//=============================================================================
// Print the channel map for information
//=============================================================================
void HCMonitorBase::printMapping() const {

  info() << " Quadrant   ID   Ref. ID    Comment" << endmsg; 
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = stations.size();
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      const std::string comment = m_masked[i][j] ? "Masked" : "";
      info() << format("   %2s/%1d", stations[i].c_str(), j)
             << format("   %4d", m_channels[i][j])
             << format(" %4d", m_references[i][j])
             << format(" %-10s", comment.c_str()) << endmsg;
    }
  }

}

//=============================================================================
// Correct/randomise a given ADC value.
//=============================================================================
double HCMonitorBase::fadc(const int adc) {

  if (adc < 0) return adc;
  double result = adc;
  if (m_randomiseAdc) {
    if (m_adcStep[adc] > 1) result += m_uniform.shoot() * m_adcStep[adc];
  } else {
    result += m_adcCorrection[adc];
  }
  return result;
}

//=============================================================================
// Rescale variable-bin histogram using bin width.
//=============================================================================
void HCMonitorBase::scale(AIDA::IHistogram1D* h) {

  auto hRoot = Gaudi::Utils::Aida2ROOT::aida2root(h);
  // hRoot->Sumw2();
  hRoot->Scale(1., "width");
}
