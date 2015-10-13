// Gaudi
#include "GaudiUtils/HistoLabels.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
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
  for (unsigned int i = 0; i < nStations; ++i) {
    // Book histograms for ADC sum distributions for each station.
    const unsigned int bins = m_variableBins ? 256 : 4096;
    const double low = -0.5;
    const double high = 4095.5;
    const std::string st = stations[i];
    std::string name = "ADC/Sum/" + st;
    m_hAdcSum.push_back(book1D(name, st, low, high, bins));
    m_hAdcSumEven.push_back(book1D("ADC/Sum/Even/" + st, st, low, high, bins));
    m_hAdcSumOdd.push_back(book1D("ADC/Sum/Odd/" + st, st, low, high, bins));
    m_hAdcSumNoBeam.push_back(
        book1D("ADC/Sum/NoBeam/" + st, st, low, high, bins));
    m_hAdcSumBeam.push_back(book1D("ADC/Sum/Beam/" + st, st, low, high, bins));
    setAxisLabels(m_hAdcSum[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumEven[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumOdd[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumNoBeam[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumBeam[i], "Sum ADC", "Entries");
    // Book profile histograms of average ADC vs. quadrant for each station.
    name = "ADC/" + st + "/Average";
    m_hAdcVsQuadrant.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Even/Average";
    m_hAdcVsQuadrantEven.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Odd/Average";
    m_hAdcVsQuadrantOdd.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/NoBeam/Average";
    m_hAdcVsQuadrantNoBeam.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Beam/Average";
    m_hAdcVsQuadrantBeam.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    setAxisLabels(m_hAdcVsQuadrant[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantEven[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantOdd[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantNoBeam[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantBeam[i], "Quadrant", "ADC");
  }

  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    // Book histograms for ADC distributions for each channel.
    const std::string ch = "Channel" + std::to_string(i);
    //if (m_variableBins) {
      m_hAdcB.push_back(book1D("ADC/B/" + ch, ch, m_edges));
      m_hAdcF.push_back(book1D("ADC/F/" + ch, ch, m_edges));
      m_hAdcEvenB.push_back(book1D("ADC/B/Even/" + ch, ch, m_edges));
      m_hAdcEvenF.push_back(book1D("ADC/F/Even/" + ch, ch, m_edges));
      m_hAdcOddB.push_back(book1D("ADC/B/Odd/" + ch, ch, m_edges));
      m_hAdcOddF.push_back(book1D("ADC/F/Odd/" + ch, ch, m_edges));
      /*} else {
      const double low = -0.;
      const double high = 1023.5;
      const unsigned int bins = 1024;
      m_hAdcB.push_back(book1D("ADC/B/" + ch, ch, low, high, bins));
      m_hAdcF.push_back(book1D("ADC/F/" + ch, ch, low, high, bins));
      m_hAdcEvenB.push_back(book1D("ADC/B/Even/" + ch, ch, low, high, bins));
      m_hAdcEvenF.push_back(book1D("ADC/F/Even/" + ch, ch, low, high, bins));
      m_hAdcOddB.push_back(book1D("ADC/B/Odd/" + ch, ch, low, high, bins));
      m_hAdcOddF.push_back(book1D("ADC/F/Odd/" + ch, ch, low, high, bins));

      }*/
    setAxisLabels(m_hAdcB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddF[i], "ADC", "Entries");
  }

  for (unsigned int i = 0; i < 4; ++i) {
    // Histogram range and number of bins (in case of uniform binning).
    const double low = -0.5;
    const double high = 1023.5;
    const unsigned int bins = 1024;
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
      const std::string name = "ADC/" + stations[j] + "/" + qu;
      const std::string nameEven = "ADC/" + stations[j] + "/Even/" + qu;
      const std::string nameOdd = "ADC/" + stations[j] + "/Odd/" + qu;
      const std::string nameNoBeam = "ADC/" + stations[j] + "/NoBeam/" + qu;
      const std::string nameBeam = "ADC/" + stations[j] + "/Beam/" + qu;
      const std::string nameBx = "ADCvsBX/" + stations[j] + "/" + qu;
      // TODO: check! Should avoid variable binning if not needed.
      if (m_variableBins) {
        m_hAdcQuadrant.push_back(book1D(name, qu, m_edges));
        m_hAdcQuadrantEven.push_back(book1D(nameEven, qu, m_edges));
        m_hAdcQuadrantOdd.push_back(book1D(nameOdd, qu, m_edges));
        m_hAdcQuadrantNoBeam.push_back(book1D(nameNoBeam, qu, m_edges));
        m_hAdcQuadrantBeam.push_back(book1D(nameBeam, qu, m_edges));
        m_hAdcVsBx.push_back(book2D(nameBx, qu, bxedges, m_edges));
      } else {
        m_hAdcQuadrant.push_back(book1D(name, qu, low, high, bins));
        m_hAdcQuadrantEven.push_back(book1D(nameEven, qu, low, high, bins));
        m_hAdcQuadrantOdd.push_back(book1D(nameOdd, qu, low, high, bins));
        m_hAdcQuadrantNoBeam.push_back(book1D(nameNoBeam, qu, low, high, bins));
        m_hAdcQuadrantBeam.push_back(book1D(nameBeam, qu, low, high, bins));
        m_hAdcVsBx.push_back(
            book2D(nameBx, qu, bxlow, bxhigh, bxbins, low, high, bins));
      }
      const unsigned int index = i * nStations + j;
      setAxisLabels(m_hAdcQuadrant[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantEven[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantOdd[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantNoBeam[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantBeam[index], "ADC", "Entries");
      setAxisLabels(m_hAdcVsBx[index], "BX", "ADC");
    }
  }

  // Book profile histograms of average ADC vs. channel number.
  m_hAdcVsChannelB = bookProfile1D("ADC/B/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelF = bookProfile1D("ADC/F/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenB =
      bookProfile1D("ADC/B/Even/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenF =
      bookProfile1D("ADC/F/Even/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelOddB = bookProfile1D("ADC/B/Odd/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelOddF = bookProfile1D("ADC/F/Odd/Average", "F", -0.5, 63.5, 64);
  setAxisLabels(m_hAdcVsChannelB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddF, "Channel", "ADC");

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
  const bool even = (bxid % 2 == 0);
  // Get the bunch-crossing type.
  const auto bxtype = odin->bunchCrossingType();

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }

  // Loop over all digits.
  for (LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const double adc = fadc(digit->adc());
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
    } else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
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
        warning() << "Cannot retrieve digit for " << st << j << endmsg; 
        continue;
      }
      unsigned int adc = digit->adc();
      const unsigned int index = j * nStations + i;
      m_hAdcVsQuadrant[i]->fill(j, adc);
      m_hAdcQuadrant[index]->fill(adc);
      m_hAdcVsBx[index]->fill(bxid, adc);
      if (even) {
        m_hAdcVsQuadrantEven[i]->fill(j, adc);
        m_hAdcQuadrantEven[index]->fill(adc);
      } else {
        m_hAdcVsQuadrantOdd[i]->fill(j, adc);
        m_hAdcQuadrantOdd[index]->fill(adc);
      }
      if (bxtype == LHCb::ODIN::NoBeam) {
        m_hAdcVsQuadrantNoBeam[i]->fill(j, adc);
        m_hAdcQuadrantNoBeam[index]->fill(adc);
      } else if (bxtype == LHCb::ODIN::BeamCrossing) {
        m_hAdcVsQuadrantBeam[i]->fill(j, adc);
        m_hAdcQuadrantBeam[index]->fill(adc);
      }
      sum[i] += adc;
    }
    m_hAdcSum[i]->fill(sum[i]);
    if (even) {
      m_hAdcSumEven[i]->fill(sum[i]);
    } else {
      m_hAdcSumOdd[i]->fill(sum[i]);
    }
    if (bxtype == LHCb::ODIN::NoBeam) {
      m_hAdcSumNoBeam[i]->fill(sum[i]);
    } else if (bxtype == LHCb::ODIN::BeamCrossing) {
      m_hAdcSumBeam[i]->fill(sum[i]);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCDigitMonitor::finalize() {

  if (!m_variableBins) return HCMonitorBase::finalize();
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
      scale(m_hAdcQuadrantNoBeam[index]);
      scale(m_hAdcQuadrantBeam[index]);
    }
  }
  return HCMonitorBase::finalize();
}
