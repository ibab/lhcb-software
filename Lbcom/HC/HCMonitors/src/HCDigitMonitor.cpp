// Gaudi
#include "GaudiUtils/HistoLabels.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
// Kernel/LHCbKernel
#include "Kernel/HCCellID.h"

// Local
#include "HCDigitMonitor.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCDigitMonitor)

//=============================================================================
// Standard constructor
//=============================================================================
HCDigitMonitor::HCDigitMonitor(const std::string& name,
                               ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator) {
  declareProperty("DigitLocation",
                  m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("ADCvsBX", m_adcVsBx = false);
  declareProperty("AllChannels", m_allChannels = false); 

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
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Setup the histograms.
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  m_hAdcSum.resize(nStations);
  m_hAdcSumNoBeam.resize(nStations);
  m_hAdcSumBeam.resize(nStations);
  m_hAdcVsQuadrant.resize(nStations);
  m_hAdcVsQuadrantNoBeam.resize(nStations);
  m_hAdcVsQuadrantBeam.resize(nStations);
  for (unsigned int i = 0; i < nStations; ++i) {
    const unsigned int bins = m_variableBins ? 256 : 4 * m_parADC.bins();
    const double low = m_parADC.lowEdge();
    const double high = 4 * (m_parADC.highEdge() + 0.5) - 0.5;
    const std::string st = stations[i];
    const std::vector<std::string> bcs = {"/Even/", "/Odd/", "/"};
    for (const auto& bc : bcs) {
      // Book histograms for ADC sum distributions.
      std::string name = "ADC/Sum" + bc + st;
      m_hAdcSum[i].push_back(book1D(name, st, low, high, bins));
      name = "ADC/Sum/NoBeam" + bc + st;
      m_hAdcSumNoBeam[i].push_back(book1D(name, st, low, high, bins));
      name = "ADC/Sum/Beam" + bc + st;
      m_hAdcSumBeam[i].push_back(book1D(name, st, low, high, bins));
      setAxisLabels(m_hAdcSum[i].back(), "Sum ADC", "Entries");
      setAxisLabels(m_hAdcSumNoBeam[i].back(), "Sum ADC", "Entries");
      setAxisLabels(m_hAdcSumBeam[i].back(), "Sum ADC", "Entries");
      // Book profile histograms of average ADC vs. quadrant.
      name = "ADC/" + st + bc + "Average";
      m_hAdcVsQuadrant[i].push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADC/" + st + "/NoBeam" + bc + "Average";
      m_hAdcVsQuadrantNoBeam[i].push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADC/" + st + "/Beam" + bc + "Average";
      m_hAdcVsQuadrantBeam[i].push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      setAxisLabels(m_hAdcVsQuadrant[i].back(), "Quadrant", "ADC");
      setAxisLabels(m_hAdcVsQuadrantNoBeam[i].back(), "Quadrant", "ADC");
      setAxisLabels(m_hAdcVsQuadrantBeam[i].back(), "Quadrant", "ADC");
    }
  }

  const std::array<std::string, 2> sides = {"B", "F"};
  const unsigned int nChannels = 64;
  m_hAdcChannel.resize(2 * nChannels);
  m_hAdcVsChannel.resize(2); 
  for (unsigned int i = 0; i < 2; ++i) {
    const std::string side = sides[i];
    // Book profile histograms of average ADC vs. channel number.
    m_hAdcVsChannel[i].push_back(
      bookProfile1D("ADC/" + side + "/Even/Average", side, -0.5, 63.5, 64));
    m_hAdcVsChannel[i].push_back(
      bookProfile1D("ADC/" + side + "/Odd/Average", side, -0.5, 63.5, 64));
    m_hAdcVsChannel[i].push_back(
      bookProfile1D("ADC/" + side + "/Average", side, -0.5, 63.5, 64));
    for (auto profile : m_hAdcVsChannel[i]) {
      setAxisLabels(profile, "Channel", "ADC");
    }
    for (unsigned int j = 0; j < nChannels; ++j) {
      const unsigned int index = i * nChannels + j;
      // Book histograms for ADC distributions for each channel.
      const std::string ch = "Channel" + std::to_string(j);
      const std::vector<std::string> bcs = {"/Even/", "/Odd/", "/"};
      for (const auto& bc : bcs) {
        const std::string name = "ADC/" + side + bc + ch;
        if (m_variableBins) {
          m_hAdcChannel[index].push_back(book1D(name, ch, m_edges));
        } else {
          const double low = m_parADC.lowEdge();
          const double high = m_parADC.highEdge();
          const unsigned int bins = m_parADC.bins();
          m_hAdcChannel[index].push_back(book1D(name, ch, low, high, bins));
        }
        setAxisLabels(m_hAdcChannel[index].back(), "ADC", "Entries");
      }
    }
  }

  m_hAdcQuadrant.resize(4 * nStations);
  m_hAdcQuadrantNoBeam.resize(4 * nStations);
  m_hAdcQuadrantBeam.resize(4 * nStations);
  for (unsigned int i = 0; i < 4; ++i) {
    // Histogram range and number of bins (in case of uniform binning).
    const double low = m_parADC.lowEdge();
    const double high = m_parADC.highEdge();
    const unsigned int bins = m_parADC.bins();
    // Book histograms for ADC distributions for each quadrant.
    const std::string qu = "Quadrant" + std::to_string(i);
    // Bx ID binning
    const double bxlow = -0.5;
    const double bxhigh = 4095.5;
    const unsigned int bxbins = 4096;
    std::vector<double> bxedges;
    for (unsigned int j = 0; j < bxbins + 1; ++j) {
      bxedges.push_back(j - 0.5);
    }
    for (unsigned int j = 0; j < nStations; ++j) {
      const unsigned int index = i * nStations + j;
      const std::string st = stations[j];
      const std::vector<std::string> bcs = {"/Even/", "/Odd/", "/"};
      for (const auto& bc : bcs) {
        const std::string name = "ADC/" + st + bc + qu;
        const std::string nameNoBeam = "ADC/" + st + "/NoBeam" + bc + qu;
        const std::string nameBeam = "ADC/" + st + "/Beam" + bc + qu;
        if (m_variableBins) {
          m_hAdcQuadrant[index].push_back(book1D(name, qu, m_edges));
          m_hAdcQuadrantNoBeam[index].push_back(book1D(nameNoBeam, qu, m_edges));
          m_hAdcQuadrantBeam[index].push_back(book1D(nameBeam, qu, m_edges));
        } else {
          m_hAdcQuadrant[index].push_back(book1D(name, qu, low, high, bins));
          m_hAdcQuadrantNoBeam[index].push_back(book1D(nameNoBeam, qu, low, high, bins));
          m_hAdcQuadrantBeam[index].push_back(book1D(nameBeam, qu, low, high, bins));
        }
        setAxisLabels(m_hAdcQuadrant[index].back(), "ADC", "Entries");
        setAxisLabels(m_hAdcQuadrantNoBeam[index].back(), "ADC", "Entries");
        setAxisLabels(m_hAdcQuadrantBeam[index].back(), "ADC", "Entries");
      }
      if (!m_adcVsBx) continue;
      const std::string nameBx = "ADCvsBX/" + stations[j] + "/" + qu;
      if (m_variableBins) {
        m_hAdcVsBx.push_back(book2D(nameBx, qu, bxedges, m_edges));
      } else {
        m_hAdcVsBx.push_back(
          book2D(nameBx, qu, bxlow, bxhigh, bxbins, low, high, bins));
      }
      setAxisLabels(m_hAdcVsBx[index], "BX", "ADC");
    }
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDigitMonitor::execute() {

  // Get event information from ODIN.
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  // Skip events with out-of-range bunch-crossing ID.
  if (bxid < m_bxMin || bxid > m_bxMax) return StatusCode::SUCCESS;
  const unsigned int evenodd = bxid % 2;
  // Get the bunch-crossing type.
  const auto bxtype = odin->bunchCrossingType();

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) return Error("No digits in " + m_digitLocation);

  // Loop over all digits.
  for (const LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const double adc = fadc(digit->adc());
    if (crate != m_crateB && crate != m_crateF) {
      Warning("Unexpected crate (" + std::to_string(crate) + ")").ignore();
    } 
    const unsigned int side = crate == m_crateF ? 1 : 0;
    m_hAdcVsChannel[side][2]->fill(channel, adc);
    m_hAdcVsChannel[side][evenodd]->fill(channel, adc);
    if (!m_allChannels) continue;
    const unsigned int offset = side * 64;
    m_hAdcChannel[offset + channel].back()->fill(adc);
    m_hAdcChannel[offset + channel][evenodd]->fill(adc);
  }

  const unsigned int nStations = 5;
  std::vector<double> sum(nStations, 0.);
  for (unsigned int i = 0; i < nStations; ++i) {
    const bool bwd = i < 3;
    for (unsigned int j = 0; j < 4; ++j) { 
      // Skip masked quadrants.
      if (m_masked[i][j]) continue;
      LHCb::HCCellID id(m_channels[i][j]);
      const LHCb::HCDigit* digit = digits->object(id);
      if (!digit) {
        const std::string st = bwd ? "B" + std::to_string(i) : 
                                     "F" + std::to_string(i - 2); 
        Warning("Cannot retrieve digit for " + st + std::to_string(j)).ignore();
        continue;
      }
      const double adc = fadc(digit->adc());
      const unsigned int index = j * nStations + i;
      m_hAdcVsQuadrant[i][2]->fill(j, adc);
      m_hAdcVsQuadrant[i][evenodd]->fill(j, adc);
      m_hAdcQuadrant[index][2]->fill(adc);
      m_hAdcQuadrant[index][evenodd]->fill(adc);
      if (m_adcVsBx) m_hAdcVsBx[index]->fill(bxid, adc);
      if (bxtype == LHCb::ODIN::NoBeam) {
        m_hAdcVsQuadrantNoBeam[i][2]->fill(j, adc);
        m_hAdcVsQuadrantNoBeam[i][evenodd]->fill(j, adc);
        m_hAdcQuadrantNoBeam[index][2]->fill(adc);
        m_hAdcQuadrantNoBeam[index][evenodd]->fill(adc);
      } else if (bxtype == LHCb::ODIN::BeamCrossing) {
        m_hAdcVsQuadrantBeam[i][2]->fill(j, adc);
        m_hAdcVsQuadrantBeam[i][evenodd]->fill(j, adc);
        m_hAdcQuadrantBeam[index][2]->fill(adc);
        m_hAdcQuadrantBeam[index][evenodd]->fill(adc);
      }
      sum[i] += adc;
    }
    m_hAdcSum[i][2]->fill(sum[i]);
    m_hAdcSum[i][evenodd]->fill(sum[i]);
    if (bxtype == LHCb::ODIN::NoBeam) {
      m_hAdcSumNoBeam[i][2]->fill(sum[i]);
      m_hAdcSumNoBeam[i][evenodd]->fill(sum[i]);
    } else if (bxtype == LHCb::ODIN::BeamCrossing) {
      m_hAdcSumBeam[i][2]->fill(sum[i]);
      m_hAdcSumBeam[i][evenodd]->fill(sum[i]);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCDigitMonitor::finalize() {

  if (!m_variableBins) return HCMonitorBase::finalize();
  for (auto& row : m_hAdcChannel) {
    for (auto histogram : row) scale(histogram);
  }
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < 4; ++i) {
    for (unsigned int j = 0; j < nStations; ++j) {
      const unsigned int index = i * nStations + j;
      for (auto histogram : m_hAdcQuadrant[index]) scale(histogram);
      for (auto histogram : m_hAdcQuadrantNoBeam[index]) scale(histogram);
      for (auto histogram : m_hAdcQuadrantBeam[index]) scale(histogram);
    }
  }
  return HCMonitorBase::finalize();
}
