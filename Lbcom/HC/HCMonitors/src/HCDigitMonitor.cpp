// ROOT
#include "TH1.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"

// Local
#include "HCDigitMonitor.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCDigitMonitor)

//=============================================================================
// Standard constructor
//=============================================================================
HCDigitMonitor::HCDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiHistoAlg(name, pSvcLocator),
      m_odin(NULL),
      m_parAdc("", -0.5, 1023.5, 64) {

  declareProperty("DigitLocation", m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);
  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);
  declareProperty("ParametersADC", m_parAdc);
}

//=============================================================================
// Destructor
//=============================================================================
HCDigitMonitor::~HCDigitMonitor() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDigitMonitor::initialize() {

  // Initialise the base class.
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return sc;

  // Get ODIN.
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder", "OdinDecoder", this);

  // Setup the binning and ADC correction.
  m_adcCorrection.resize(1024, 0.);
  unsigned int adc = 0;
  unsigned int step = 1;
  m_edges.clear();
  while (adc < 1024) {
    m_edges.push_back(adc - 0.5);
    if (adc == 128) step = 2;
    else if (adc == 256) step = 8;
    else if (adc == 512) step = 16; 
    m_adcCorrection[adc] = 0.5 * (step - 1.);
    adc += step;   
  } 
  m_edges.push_back(1023.5);

  // Setup the histograms.
  std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    // Book histograms for ADC sum distributions for each station.
    auto bins = 4 * m_parAdc.bins();
    const double low = m_parAdc.lowEdge();
    const double high = 4 * m_parAdc.highEdge();
    const std::string st = stations[i];
    std::string name = "ADC/Sum/" + st;
    m_hAdcSum.push_back(book1D(name, st, low, high, bins));
    m_hAdcSumEven.push_back(book1D("ADC/Sum/Even/" + st, st, low, high, bins));
    m_hAdcSumOdd.push_back(book1D("ADC/Sum/Odd/" + st, st, low, high, bins));
    setAxisLabels(m_hAdcSum[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumEven[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumOdd[i], "Sum ADC", "Entries");
    // Book profile histograms of average ADC vs. quadrant for each station.
    name = "ADC/" + st + "/Average";
    m_hAdcVsQuadrant.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Even/Average";
    m_hAdcVsQuadrantEven.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Odd/Average";
    m_hAdcVsQuadrantOdd.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    setAxisLabels(m_hAdcVsQuadrant[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantEven[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantOdd[i], "Quadrant", "ADC");
  }

  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    // Book histograms for ADC distributions for each channel.
    const std::string ch = "Channel" + std::to_string(i);
    m_hAdcB.push_back(book1D("ADC/B/" + ch, ch, m_edges));
    m_hAdcF.push_back(book1D("ADC/F/" + ch, ch, m_edges));
    m_hAdcEvenB.push_back(book1D("ADC/B/Even/" + ch, ch, m_edges));
    m_hAdcEvenF.push_back(book1D("ADC/F/Even/" + ch, ch, m_edges));
    m_hAdcOddB.push_back(book1D("ADC/B/Odd/" + ch, ch, m_edges));
    m_hAdcOddF.push_back(book1D("ADC/F/Odd/" + ch, ch, m_edges));
    setAxisLabels(m_hAdcB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddF[i], "ADC", "Entries");
  }
  for (unsigned int i = 0; i < 4; ++i) {
    // Book histograms for ADC distributions for each quadrant.
    const std::string qu = "Quadrant" + std::to_string(i);
    for (unsigned int j = 0; j < nStations; ++j) {
      std::string name = "ADC/" + stations[j] + "/" + qu;
      m_hAdcQuadrant.push_back(book1D(name, qu, m_edges));
      name = "ADC/" + stations[j] + "/Even/" + qu;
      m_hAdcQuadrantEven.push_back(book1D(name, qu, m_edges));
      name = "ADC/" + stations[j] + "/Odd/" + qu;
      m_hAdcQuadrantOdd.push_back(book1D(name, qu, m_edges));
      const unsigned int index = i * nStations + j;
      setAxisLabels(m_hAdcQuadrant[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantEven[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantOdd[index], "ADC", "Entries");
    }
  }
 
  // Book profile histograms of average ADC vs. channel number.
  m_hAdcVsChannelB = bookProfile1D("ADC/B/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelF = bookProfile1D("ADC/F/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenB = bookProfile1D("ADC/B/Even/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenF = bookProfile1D("ADC/F/Even/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelOddB = bookProfile1D("ADC/B/Odd/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelOddF = bookProfile1D("ADC/F/Odd/Average", "F", -0.5, 63.5, 64);
  setAxisLabels(m_hAdcVsChannelB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddF, "Channel", "ADC");

  // Setup the mapping.
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
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDigitMonitor::execute() {

  // Get event information from ODIN.
  m_odin->getTime();
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  const bool even = (bxid % 2 == 0);

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  std::vector<unsigned int> sum = {0, 0, 0, 0, 0};
  const unsigned int nStations = 5;
  // Loop over the digits.
  for (LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const double adc = digit->adc() + m_adcCorrection[digit->adc()];
    unsigned int station = 0;
    unsigned int quadrant = 0;
    unsigned int offset = 0;
    if (crate == m_crateB) {
      m_hAdcB[channel]->fill(adc);
      m_hAdcVsChannelB->fill(channel, adc); 
      if (even) {
        m_hAdcEvenB[channel]->fill(adc);
        m_hAdcVsChannelEvenB->fill(channel, adc);
      } else {
        m_hAdcOddB[channel]->fill(adc);
        m_hAdcVsChannelOddB->fill(channel, adc);
      } 
      if (!m_mappedB[channel]) continue;
      station = m_stationB[channel];
      quadrant = m_quadrantB[channel];
    } else if (crate == m_crateF) {
      m_hAdcF[channel]->fill(adc);
      m_hAdcVsChannelF->fill(channel, adc); 
      if (even) {
        m_hAdcEvenF[channel]->fill(adc);
        m_hAdcVsChannelEvenF->fill(channel, adc);
      } else {
        m_hAdcOddF[channel]->fill(adc);
        m_hAdcVsChannelOddF->fill(channel, adc);
      } 
      if (!m_mappedF[channel]) continue;
      station = m_stationF[channel];
      quadrant = m_quadrantF[channel];
      offset = 2;
    } else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
    const unsigned int index = quadrant * nStations + station + offset;
    m_hAdcVsQuadrant[station + offset]->fill(quadrant, adc);
    m_hAdcQuadrant[index]->fill(adc);
    if (even) {
      m_hAdcVsQuadrantEven[station + offset]->fill(quadrant, adc);
      m_hAdcQuadrantEven[index]->fill(adc);
    } else { 
      m_hAdcVsQuadrantOdd[station + offset]->fill(quadrant, adc);
      m_hAdcQuadrantOdd[index]->fill(adc);
    }
    sum[station + offset] += adc;
  }
  for (unsigned int i = 0; i < nStations; ++i) {
    m_hAdcSum[i]->fill(sum[i]);
    if (even) {
      m_hAdcSumEven[i]->fill(sum[i]);
    } else {
      m_hAdcSumOdd[i]->fill(sum[i]);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCDigitMonitor::finalize() {

  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    scale(m_hAdcB[i]);
    scale(m_hAdcF[i]);
    scale(m_hAdcEvenB[i]);
    scale(m_hAdcEvenF[i]);
    scale(m_hAdcOddB[i]);
    scale(m_hAdcOddF[i]);
  }
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < 4; ++i) {
    for (unsigned int j = 0; j < nStations; ++j) {
      const unsigned int index = i * nStations + j;
      scale(m_hAdcQuadrant[index]);
      scale(m_hAdcQuadrantEven[index]);
      scale(m_hAdcQuadrantOdd[index]);
    }
  }
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Rescale variable-bin histogram using bin width.
//=============================================================================
void HCDigitMonitor::scale(AIDA::IHistogram1D* h) {

  auto hRoot = Gaudi::Utils::Aida2ROOT::aida2root(h);
  // hRoot->Sumw2();
  hRoot->Scale(1., "width");
}

//=============================================================================
// Setup channel map for a given station.
//=============================================================================
void HCDigitMonitor::mapChannels(const std::vector<unsigned int>& channels,
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

