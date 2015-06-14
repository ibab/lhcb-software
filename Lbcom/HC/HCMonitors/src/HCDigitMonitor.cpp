// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/HistoLabels.h"

// LHCb
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
      m_parAdc("", -0.5, 1023.5, 1023) {

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

  // Setup the histograms.
  auto bins = m_parAdc.bins();
  double low = m_parAdc.lowEdge();
  double high = m_parAdc.highEdge();
  m_hAdcSumB0 = book1D("ADC/Sum/B0", "B0", low, 4 * high, 4 * bins);
  m_hAdcSumB1 = book1D("ADC/Sum/B1", "B1", low, 4 * high, 4 * bins);
  m_hAdcSumB2 = book1D("ADC/Sum/B2", "B2", low, 4 * high, 4 * bins);
  m_hAdcSumF1 = book1D("ADC/Sum/F1", "F1", low, 4 * high, 4 * bins);
  m_hAdcSumF2 = book1D("ADC/Sum/F2", "F2", low, 4 * high, 4 * bins);
  setAxisLabels(m_hAdcSumB0, "Sum ADC", "Entries");
  setAxisLabels(m_hAdcSumB1, "Sum ADC", "Entries");
  setAxisLabels(m_hAdcSumB2, "Sum ADC", "Entries");
  setAxisLabels(m_hAdcSumF1, "Sum ADC", "Entries");
  setAxisLabels(m_hAdcSumF2, "Sum ADC", "Entries");

  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    const std::string title = "Channel" + std::to_string(i);
    m_hAdcB.push_back(book1D("ADC/B/" + title, title, low, high, bins));
    m_hAdcF.push_back(book1D("ADC/F/" + title, title, low, high, bins));
    setAxisLabels(m_hAdcB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF[i], "ADC", "Entries");
  }
  for (unsigned int i = 0; i < 4; ++i) {
    const std::string title = "Quadrant" + std::to_string(i);
    m_hAdcB0.push_back(book1D("ADC/B0/" + title, title, low, high, bins));
    m_hAdcB1.push_back(book1D("ADC/B1/" + title, title, low, high, bins));
    m_hAdcB2.push_back(book1D("ADC/B2/" + title, title, low, high, bins));
    m_hAdcF1.push_back(book1D("ADC/F1/" + title, title, low, high, bins));
    m_hAdcF2.push_back(book1D("ADC/F2/" + title, title, low, high, bins));
    setAxisLabels(m_hAdcB0[i], "ADC", "Entries");
    setAxisLabels(m_hAdcB1[i], "ADC", "Entries");
    setAxisLabels(m_hAdcB2[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF1[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF2[i], "ADC", "Entries");
  }
 
  m_hAdcVsChannelB = bookProfile1D("ADC/B/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelF = bookProfile1D("ADC/F/Average", "F", -0.5, 63.5, 64);
  setAxisLabels(m_hAdcVsChannelB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelF, "Channel", "ADC");

  m_hAdcVsQuadrantB0 = bookProfile1D("ADC/B0/Average", "B0", -0.5, 3.5, 4);
  m_hAdcVsQuadrantB1 = bookProfile1D("ADC/B1/Average", "B1", -0.5, 3.5, 4);
  m_hAdcVsQuadrantB2 = bookProfile1D("ADC/B2/Average", "B2", -0.5, 3.5, 4);
  m_hAdcVsQuadrantF1 = bookProfile1D("ADC/F1/Average", "F1", -0.5, 3.5, 4);
  m_hAdcVsQuadrantF2 = bookProfile1D("ADC/F2/Average", "F2", -0.5, 3.5, 4);
  setAxisLabels(m_hAdcVsQuadrantB0, "Quadrant", "ADC");
  setAxisLabels(m_hAdcVsQuadrantB1, "Quadrant", "ADC");
  setAxisLabels(m_hAdcVsQuadrantB2, "Quadrant", "ADC");
  setAxisLabels(m_hAdcVsQuadrantF1, "Quadrant", "ADC");
  setAxisLabels(m_hAdcVsQuadrantF2, "Quadrant", "ADC");

  // Setup the mapping.
  m_mappedB.resize(nChannels, false);
  m_mappedF.resize(nChannels, false);
  m_stationB.resize(nChannels, 0);
  m_stationF.resize(nChannels, 0);
  m_quadrantB.resize(nChannels, 0);
  m_quadrantF.resize(nChannels, 0);
  bool okB0 = true;
  if (m_channelsB0.size() != 4) {
    warning() << "Invalid channel map for station B0." << endmsg;
    okB0 = false;
  }
  bool okB1 = true;
  if (m_channelsB1.size() != 4) {
    warning() << "Invalid channel map for station B1." << endmsg;
    okB1 = false;
  }
  bool okB2 = true;
  if (m_channelsB2.size() != 4) {
    warning() << "Invalid channel map for station B2." << endmsg;
    okB2 = false;
  }
  bool okF1 = true;
  if (m_channelsF1.size() != 4) {
    warning() << "Invalid channel map for station F1." << endmsg;
    okF1 = false;
  }
  bool okF2 = true;
  if (m_channelsF2.size() != 4) {
    warning() << "Invalid channel map for station F2." << endmsg;
    okF2 = false;
  }
  for (unsigned int i = 0; i < 4; ++i) {
    if (okB0) {
      const unsigned int channel = m_channelsB0[i];
      if (m_mappedB[channel]) {
        warning() << "Channel " << channel << " (B) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedB[channel] = true;
      m_stationB[channel] = 0;
      m_quadrantB[channel] = i;
    }
    if (okB1) {
      const unsigned int channel = m_channelsB1[i];
      if (m_mappedB[channel]) {
        warning() << "Channel " << channel << " (B) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedB[channel] = true;
      m_stationB[channel] = 1;
      m_quadrantB[channel] = i;
    }
    if (okB2) {
      const unsigned int channel = m_channelsB2[i];
      if (m_mappedB[channel]) {
        warning() << "Channel " << channel << " (B) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedB[channel] = true;
      m_stationB[channel] = 2;
      m_quadrantB[channel] = i;
    }
    if (okF1) {
      const unsigned int channel = m_channelsF1[i];
      if (m_mappedF[channel]) {
        warning() << "Channel " << channel << " (F) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedF[channel] = true;
      m_stationF[channel] = 1;
      m_quadrantF[channel] = i;
    }
    if (okF2) {
      const unsigned int channel = m_channelsF2[i];
      if (m_mappedF[channel]) {
        warning() << "Channel " << channel << " (F) "
                  << "is mapped to more than one PMT." << endmsg;
      }
      m_mappedF[channel] = true;
      m_stationF[channel] = 2;
      m_quadrantF[channel] = i;
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDigitMonitor::execute() {

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  unsigned int sumB0 = 0;
  unsigned int sumB1 = 0;
  unsigned int sumB2 = 0;
  unsigned int sumF1 = 0;
  unsigned int sumF2 = 0;
  // Loop over the digits.
  for (LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const int adc = digit->adc();
    if (crate == m_crateB) {
      m_hAdcB[channel]->fill(adc);
      m_hAdcVsChannelB->fill(channel, adc); 
      if (!m_mappedB[channel]) continue;
      const unsigned int station = m_stationB[channel];
      const unsigned int quadrant = m_quadrantB[channel];
      if (station == 0) {
        m_hAdcB0[quadrant]->fill(adc);
        m_hAdcVsQuadrantB0->fill(quadrant, adc);
        sumB0 += adc;
      } else if (station == 1) {
        m_hAdcB1[quadrant]->fill(adc);
        m_hAdcVsQuadrantB1->fill(quadrant, adc);
        sumB1 += adc;
      } else if (station == 2) {
        m_hAdcB2[quadrant]->fill(adc);
        m_hAdcVsQuadrantB2->fill(quadrant, adc);
        sumB2 += adc;
      }
    } else if (crate == m_crateF) {
      m_hAdcF[channel]->fill(adc);
      m_hAdcVsChannelF->fill(channel, adc); 
      if (!m_mappedF[channel]) continue;
      const unsigned int station = m_stationF[channel];
      const unsigned int quadrant = m_quadrantF[channel];
      if (station == 1) {
        m_hAdcF1[quadrant]->fill(adc);
        m_hAdcVsQuadrantF1->fill(quadrant, adc);
        sumF1 += adc;
      } else if (station == 2) {
        m_hAdcF2[quadrant]->fill(adc);
        m_hAdcVsQuadrantF2->fill(quadrant, adc);
        sumF2 += adc;
      }
    } else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
  }
  m_hAdcSumB0->fill(sumB0);
  m_hAdcSumB1->fill(sumB1);
  m_hAdcSumB2->fill(sumB2);
  m_hAdcSumF1->fill(sumF1);
  m_hAdcSumF2->fill(sumF2);

  return StatusCode::SUCCESS;
}
