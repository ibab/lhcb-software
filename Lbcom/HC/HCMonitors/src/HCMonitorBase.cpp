// ROOT
#include "TH1.h"

// Gaudi
#include "GaudiUtils/Aida2ROOT.h"

// Local
#include "HCMonitorBase.h"

//=============================================================================
// Standard constructor
//=============================================================================
HCMonitorBase::HCMonitorBase(const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiTupleAlg(name, pSvcLocator),
      m_odin(NULL) {

  // Mapping which will then go in the condDB
  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);

  declareProperty("MasksB0", m_masksB0);
  declareProperty("MasksB1", m_masksB1);
  declareProperty("MasksB2", m_masksB2);
  declareProperty("MasksF1", m_masksF1);
  declareProperty("MasksF2", m_masksF2);


  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);

  declareProperty("SpareChannelsB0", m_spareChannelsB0);
  declareProperty("SpareChannelsB1", m_spareChannelsB1);
  declareProperty("SpareChannelsB2", m_spareChannelsB2);
  declareProperty("SpareChannelsF1", m_spareChannelsF1);
  declareProperty("SpareChannelsF2", m_spareChannelsF2);

  declareProperty("ChannelCalib", m_channelsCalibConfig);
  declareProperty("Thetas", m_thetaConfig);
  declareProperty("X0", m_x0Config);
  declareProperty("Y0", m_y0Config);


  // Real monitoring staff
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
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  // Get ODIN.
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder", "OdinDecoder", this);
  m_stations = {"B0", "B1", "B2", "F1", "F2"};
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
  for (unsigned int i = 0; i < m_channelsB0.size(); ++i){
    m_masksFromName["B0"+std::to_string(i)] = m_masksB0[i];
    int test =( m_crateB << 6) | m_channelsB0[i];
    m_channelsFromName["B0"+std::to_string(i)] = (m_crateB << 6) | m_channelsB0[i];
    m_refChannelsFromName["B0"+std::to_string(i)] = (m_crateB << 6) | m_spareChannelsB0[i];
  }
  for (unsigned int i = 0; i < m_channelsB1.size(); ++i){
    m_masksFromName["B1"+std::to_string(i)] = m_masksB1[i];
    m_channelsFromName["B1"+std::to_string(i)] = (m_crateB << 6) | m_channelsB1[i];
    m_refChannelsFromName["B1"+std::to_string(i)] = (m_crateB << 6) | m_spareChannelsB1[i];
  }
  for (unsigned int i = 0; i < m_channelsB2.size(); ++i){
    m_masksFromName["B2"+std::to_string(i)] = m_masksB2[i];
    m_channelsFromName["B2"+std::to_string(i)] = (m_crateB << 6) | m_channelsB2[i];
    m_refChannelsFromName["B2"+std::to_string(i)] = (m_crateB << 6) | m_spareChannelsB2[i];
  }
  for (unsigned int i = 0; i < m_channelsF1.size(); ++i){
    m_masksFromName["F1"+std::to_string(i)] = m_masksF1[i];
    m_channelsFromName["F1"+std::to_string(i)] = (m_crateF << 6) | m_channelsF1[i];
    m_refChannelsFromName["F1"+std::to_string(i)] = (m_crateF << 6) | m_spareChannelsF1[i];
  }
  for (unsigned int i = 0; i < m_channelsF2.size(); ++i){
    m_masksFromName["F2"+std::to_string(i)] = m_masksF2[i];
    m_channelsFromName["F2"+std::to_string(i)] = (m_crateF << 6) | m_channelsF2[i];
    m_refChannelsFromName["F2"+std::to_string(i)] = (m_crateF << 6) | m_spareChannelsF2[i];
  }

  for (unsigned int i = 0; i < m_channelsCalibConfig.size() ; ++i){
    std::vector< float > tmpVect;
    m_thetas[m_channelsCalibConfig[i]]= {m_thetaConfig[2*i],m_thetaConfig[2*i+1]};
    m_x0[m_channelsCalibConfig[i]]= {m_x0Config[2*i],m_x0Config[2*i+1]};
    m_y0[m_channelsCalibConfig[i]]= {m_y0Config[2*i],m_y0Config[2*i+1]};
  }

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
float HCMonitorBase::correctChannel( std::string channel , unsigned int adc, unsigned int adc_ref, unsigned int parity) {
  return adc-(adc_ref-m_x0[channel][parity])*tan( m_thetas[channel][parity] )-m_y0[channel][parity];
}
