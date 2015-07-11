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
  declareProperty("VFEClkPitch", m_VFEClkPitch);
  declareProperty("ADCClkPitch", m_ADCClkPitch);
  declareProperty("CentralThreshold", m_centralThreshold);
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
  std::vector<std::string> m_stations = {"B0", "B1", "B2", "F1", "F2"};
  std::vector<std::string> m_slots = {"Central", "Previous", "Next"};
  std::vector<std::string> m_quadrants = {"0", "1", "2", "3"};
  const unsigned int nStations = m_stations.size();
  const unsigned int nQuadrants = m_quadrants.size();
  for (unsigned int j = 0; j < 3; ++j) {
    std::vector<std::vector<TProfile*>> myAdcsSt;
    std::vector<std::vector<std::vector<TH1D*>>> myAdcsSt_step;
    for (unsigned int i = 0; i < nStations; ++i) {
      std::vector<TProfile*> myAdcs;
      std::vector<std::vector<TH1D*>> myAdcs_step;
      for (unsigned int k = 0; k < nQuadrants; ++k) {
        // Book histograms for ADC sum distributions for each station.
        const std::string quad = m_quadrants[k];
        const std::string st = m_stations[i];
        const std::string sl = m_slots[j];
        std::string name = "ADC/" + sl + "/" + st + "/" + quad;
        myAdcs.push_back(Gaudi::Utils::Aida2ROOT::aida2root(
            bookProfile1D(name, name, m_minStep - 0.5, m_maxStep + 0.5,
                          m_maxStep - m_minStep + 1)));
        std::vector<TH1D*> perstep;
        for (int kk = m_minStep; kk < m_maxStep; kk++) {
          std::string u = std::to_string(kk);
          std::string name = "ADC/" + u + "/" + st + "/" + quad + "/" + sl;
          perstep.push_back(Gaudi::Utils::Aida2ROOT::aida2root(
              book1D(name, name, -0.5, 1023 + 0.5, 1024)));
        }
        myAdcs_step.push_back(perstep);
        // setAxisLabels(myAdcs[i], "Step Number", "ADC Mean");
      }
      myAdcsSt.push_back(myAdcs);
      myAdcsSt_step.push_back(myAdcs_step);
    }
    m_adcs.push_back(myAdcsSt);
    m_adcsPerStep.push_back(myAdcsSt_step);
  }
  for (unsigned int i = 0; i < nStations; ++i) {
    std::vector<TH2D*> myScan;
    std::vector<TH2D*> myScanOff;
    for (unsigned int k = 0; k < nQuadrants; ++k) {
      // Book histograms for ADC sum distributions for each station.
      const std::string quad = m_quadrants[k];
      const std::string st = m_stations[i];
      const std::string name = "SCAN/" + st + quad;
      myScan.push_back(Gaudi::Utils::Aida2ROOT::aida2root(
          book2D(name, name, -0.5, 31.5, 32, -0.5, 31.5, 32)));
      const std::string nameO = "OFFSET/" + st + quad;
      myScanOff.push_back(Gaudi::Utils::Aida2ROOT::aida2root(
          book2D(nameO, nameO, -0.5, 31.5, 32, -0.5, 31.5, 32)));
      // setAxisLabels(myScan[i], "VFE Clock delay", "ADC Clock delay", "Average");
      // setAxisLabels(myScanOff[i], "VFE Clock delay", "ADC Clock delay","L0 Latency offset");
    }
    m_scanResults.push_back(myScan);
    m_scanOffset.push_back(myScanOff);
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

  // std::vector<bool> hasAboveThresholdCentralB(64, true);
  // std::vector<bool> hasAboveThresholdCentralF(64, true);
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
        // if (m_digitLocations[i].compare("Raw/HC/Digits") == 0 &&
        //     adc < 400) {
        //   hasAboveThresholdCentralB[channel] = false;
        // }
        if (!m_mappedB[channel]) continue;
        // if (!hasAboveThresholdCentralB[channel]) continue;
        station = m_stationB[channel];
        quadrant = m_quadrantB[channel];
      } else if (crate == m_crateF) {
        // if (m_digitLocations[i].compare("Raw/HC/Digits") == 0 &&
        //     adc < 400) {
        //   hasAboveThresholdCentralF[channel] = false;
        // }
        if (!m_mappedF[channel]) continue;
        // if (!hasAboveThresholdCentralF[channel]) continue;
        station = m_stationF[channel];
        quadrant = m_quadrantF[channel];
        offset = 2;
      } else {
        warning() << "Unexpected crate number (" << crate << ")" << endmsg;
      }
      // if (station == 0 && quadrant == 0 && offset == 0)
      //   always() << m_digitLocations[i] << " "
      //            << m_adcs[i][station + offset][quadrant] << " " << adc
      //            << endmsg;
      const unsigned int index = station + offset;
      m_adcs[i][index][quadrant]->Fill(step, fadc(adc));
      m_adcsPerStep[i][index][quadrant][step - m_minStep]->Fill(fadc(adc));
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCClockScan::finalize() {
  /*
  for (unsigned int i = 0; i < m_stations.size(); ++i) {
    for (unsigned int k = 0; k < m_quadrants.size(); ++k) {
      for (int ix = 0; ix < m_adcs[0][i][k]->GetNbinsX() + 1; ++ix) {
        int step = (int)m_adcs[0][i][k]->GetXaxis()->GetBinCenter(ix);
        int vfeclk = (step + m_minStep) % (32 / m_VFEClkPitch);
        int adcclk = (step + m_minStep) / (32 / m_ADCClkPitch);
        double adcmax = -1.;
        int bestslot = -2;
        for (unsigned int j = 0; j < 3; ++j) {
          double val = m_adcs[j][i][k]->GetBinContent(
              m_adcs[j][i][k]->GetXaxis()->FindBin(step));
          if (val > adcmax) {
            adcmax = val;
            bestslot = j - 1;
          }
          m_scanResults[i][k]->Fill(vfeclk, adcclk, adcmax);
          m_scanOffset[i][k]->Fill(vfeclk, adcclk, bestslot);
        }
      }
    }
  }
  */

  return HCMonitorBase::finalize();
}
