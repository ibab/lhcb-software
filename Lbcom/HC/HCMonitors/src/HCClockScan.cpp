// ROOT (needed?)
#include "TH2D.h"
#include "TProfile.h"

// Gaudi
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"

// Local
#include "HCClockScan.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCClockScan)

//=============================================================================
// Standard constructor
//=============================================================================
HCClockScan::HCClockScan(const std::string& name, ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator) {
  declareProperty("DigitLocations", m_digitLocations);
  declareProperty("MinimumStepNr", m_minStep = 0);
  declareProperty("MaximumStepNr", m_maxStep);
  declareProperty("VFEClkPitch", m_VFEClkPitch = 1);
  declareProperty("ADCClkPitch", m_ADCClkPitch = 2);
  declareProperty("RelativeMax", m_relativeMax = true);

}

//=============================================================================
// Destructor
//=============================================================================
HCClockScan::~HCClockScan() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCClockScan::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Setup the histograms.
  m_stations = {"B0", "B1", "B2", "F1", "F2"};
  m_slots = {"Central", "Previous", "Next"};
  m_quadrants = {"0", "1", "2", "3"};
  const unsigned int nStations = m_stations.size();
  const unsigned int nQuadrants = m_quadrants.size();
  const unsigned int nSlots = m_slots.size();
  m_adcs.resize(nSlots);
  m_adcsPerStep.resize(nSlots);
  for (unsigned int j = 0; j < nSlots; ++j) {
    m_adcs[j].resize(nStations);
    m_adcsPerStep[j].resize(nStations);
    for (unsigned int i = 0; i < nStations; ++i) {
      m_adcs[j][i].resize(nQuadrants);
      m_adcsPerStep[j][i].resize(nQuadrants);
      for (unsigned int k = 0; k < nQuadrants; ++k) {
        const std::string quad = m_quadrants[k];
        const std::string st = m_stations[i];
        const std::string sl = m_slots[j];
        std::string name = "ADC/" + sl + "/" + st + "/" + quad;
        const double low = m_minStep - 0.5;
        const double high = m_maxStep + 0.5;
        const unsigned int bins = m_maxStep - m_minStep + 1;
        m_adcs[j][i][k] = bookProfile1D(name, name, low, high, bins); 
        setAxisLabels(m_adcs[j][i][k], "Step Number", "ADC Mean");
        for (int kk = m_minStep; kk < m_maxStep; kk++) {
          std::string name = "ADC/" + std::to_string(kk) + "/" + st + "/" + quad + "/" + sl;
          m_adcsPerStep[j][i][k].push_back(book1D(name, name, -0.5, 1023.5, 1024));
        }
      }
    }
  }
  m_results.resize(nStations);
  m_offsets.resize(nStations);
  m_resultsStation.resize(nStations);
  for (unsigned int i = 0; i < nStations; ++i) {
    const std::string st = m_stations[i];
    const double low = -0.5;
    const double high = 31.5;
    const unsigned int bins = 32; 
    std::string name = "SCAN/" + st;
    m_resultsStation[i] = book2D(name, name, low, high, bins, low, high, bins);
    m_results[i].resize(nQuadrants);
    m_offsets[i].resize(nQuadrants);
    for (unsigned int k = 0; k < nQuadrants; ++k) {
      const std::string quad = m_quadrants[k];
      std::string name = "SCAN/" + st + quad;
      m_results[i][k] = book2D(name, name, low, high, bins, low, high, bins);
      name = "OFFSET/" + st + quad;
      m_offsets[i][k] = book2D(name, name, low, high, bins, low, high, bins);
      const std::string titlex = "VFE clock delay";
      const std::string titley = "ADC clock delay";
      setAxisLabels(m_results[i][k], titlex, titley);
      setAxisLabels(m_offsets[i][k], titlex, titley);
    }
  }

  m_stepCounter = m_minStep;
  m_stepEvtCounter = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCClockScan::execute() {

  // Get event information from ODIN.
  m_odin->getTime();
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  if (bxid < m_bxMin || bxid > m_bxMax) return StatusCode::SUCCESS;
  const int step = odin->calibrationStep();

  if (step < m_minStep || step > m_maxStep) return StatusCode::SUCCESS;
  /*
  if (step != m_stepCounter) {
    m_stepCounter = step;
    m_stepEvtCounter = 0;
    info() << "Step number " << step << endmsg;
  }
  if (m_stepEvtCounter > m_maxEvtPerStep) return StatusCode::SUCCESS;
  ++m_stepEvtCounter;
  */

  const unsigned int nLocations = m_digitLocations.size();
  for (unsigned int i = 0; i < nLocations; ++i) {
    const std::string location = m_digitLocations[i];
    const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(location);
    if (!digits) {
      return Error("No digits in " + location, StatusCode::FAILURE);
    }
    // Loop over the digits.
    for (const LHCb::HCDigit* digit : *digits) {
      const unsigned int crate = digit->cellID().crate();
      const unsigned int channel = digit->cellID().channel();
      const int adc = digit->adc();
      unsigned int station = 0;
      unsigned int quadrant = 0;
      unsigned int offset = 0;
      if (crate == m_crateB) {
        if (!m_mappedB[channel]) continue;
        station = m_stationB[channel];
        quadrant = m_quadrantB[channel];
      } else if (crate == m_crateF) {
        if (!m_mappedF[channel]) continue;
        station = m_stationF[channel];
        quadrant = m_quadrantF[channel];
        offset = 2;
      } else {
        warning() << "Unexpected crate number (" << crate << ")" << endmsg;
      }
      const unsigned int index = station + offset;
      const double value = fadc(adc);
      m_adcs[i][index][quadrant]->fill(step, value);
      m_adcsPerStep[i][index][quadrant][step - m_minStep]->fill(value);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCClockScan::finalize() {

  const unsigned int nStations = m_stations.size();
  const unsigned int nQuadrants = m_quadrants.size();
  const unsigned int nBins = m_maxStep - m_minStep + 1;
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int k = 0; k < nQuadrants; ++k) {
      for (unsigned int bin = 0; bin < nBins; ++bin) {
        const unsigned int step = m_minStep + bin;
        const int vfe = step % (32 / m_VFEClkPitch);
        const int adc = step / (32 / m_ADCClkPitch);
        double adcMax = -1.;
        double adcSecondMax = -1.;
        int bestslot = -2;
        for (unsigned int j = 0; j < 3; ++j) {
          double val = m_adcs[j][i][k]->binMean(bin);
          if (val > adcMax) {
            adcSecondMax = adcMax;
            adcMax = val;
            bestslot = j - 1;
          } else if (val > adcSecondMax) {
            adcSecondMax = val;
          }
        }
        if (m_relativeMax) {
          if (adcMax <= 0. && adcSecondMax <= 0.) {
            adcMax = 0.;
          } else if (adcSecondMax <= 0.) {
            adcMax = 1.;
          } else {
            adcMax = adcMax / adcSecondMax;
          }
        }
        m_results[i][k]->fill(vfe, adc, adcMax);
        m_offsets[i][k]->fill(vfe, adc, bestslot);
        m_resultsStation[i]->fill(vfe, adc, adcMax);
      }
    }
  }
  return HCMonitorBase::finalize();
}
