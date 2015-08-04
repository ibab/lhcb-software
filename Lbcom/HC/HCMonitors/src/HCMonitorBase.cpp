// ROOT
#include "TH1.h"

// Gaudi
#include "GaudiUtils/Aida2ROOT.h"

// Local
#include "HCMonitorBase.h"

DECLARE_ALGORITHM_FACTORY(HCMonitorBase)

//=============================================================================
// Standard constructor
//=============================================================================
HCMonitorBase::HCMonitorBase(const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiHistoAlg(name, pSvcLocator),
      m_odin(NULL) {

  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);
  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);
  declareProperty("VariableBins", m_variableBins = false);
  declareProperty("RandomiseADC", m_randomiseAdc = true);
  declareProperty("MinBX", m_bxMin = 0);
  declareProperty("MaxBX", m_bxMax = 10000); 
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
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return sc;

  // Get ODIN.
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder", "OdinDecoder", this);
 
  // Setup the binning and ADC correction.
  m_adcCorrection.resize(1024, 0.);
  m_adcStep.resize(1024, 0);
  unsigned int adc = 0;
  unsigned int step = 1;
  m_edges.clear();
  while (adc < 1024) {
    m_edges.push_back(adc - 0.5);
    if (adc == 128) step = 2;
    else if (adc == 256) step = 8;
    else if (adc == 512) step = 16; 
    m_adcCorrection[adc] = 0.5 * (step - 1.);
    m_adcStep[adc] = step;
    adc += step;   
  } 
  m_edges.push_back(1023.5);

  // Setup the mapping.
  const unsigned int nChannels = 64;
  m_mappedB.resize(nChannels, false);
  m_mappedF.resize(nChannels, false);
  m_stationB.resize(nChannels, 0);
  m_stationF.resize(nChannels, 0);
  m_quadrantB.resize(nChannels, 0);
  m_quadrantF.resize(nChannels, 0);
  mapChannels(m_channelsB0, true, 0);
  mapChannels(m_channelsB1, true, 1);
  mapChannels(m_channelsB2, true, 2);
  mapChannels(m_channelsF1, false, 1);
  mapChannels(m_channelsF2, false, 2);

  // Setup the random number generator.
  if (!m_uniform.initialize(randSvc(), Rndm::Flat(0., 1.))) {
    return Error("Unable to initialize random number generator.", StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Correct/randomise a given ADC value.
//=============================================================================
double HCMonitorBase::fadc(const unsigned int adc) {

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

//=============================================================================
// Setup channel map for a given station.
//=============================================================================
void HCMonitorBase::mapChannels(const std::vector<unsigned int>& channels,
                                 const bool bwd, const unsigned int station) {

  if (channels.size() != 4) {
    std::string s = bwd ? "B" : "F";
    s += std::to_string(station); 
    warning() << "Invalid channel map for station " << s << "." << endmsg;
    return;
  }
  for (unsigned int i = 0; i < 4; ++i) {
    const unsigned int channel = channels[i];
    if (bwd) {
      if (m_mappedB[channel]) {
        warning() << "Channel " << channel << " (B) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedB[channel] = true;
      m_stationB[channel] = station;
      m_quadrantB[channel] = i;
    } else {
      if (m_mappedF[channel]) {
        warning() << "Channel " << channel << " (F) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedF[channel] = true;
      m_stationF[channel] = station;
      m_quadrantF[channel] = i;
    }
  }

}

