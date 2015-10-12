// Gaudi
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"

// Local
#include "HCDelayScan.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCDelayScan)

//=============================================================================
// Standard constructor
//=============================================================================
HCDelayScan::HCDelayScan(const std::string& name, ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator),
      m_minStep(0),
      m_parAdc("", -0.5, 1023.5, 1023) {

  declareProperty("DigitLocation",
                  m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("ParametersADC", m_parAdc);
  declareProperty("MinimumStepNr", m_minStep);
  declareProperty("MaximumStepNr", m_maxStep);
  declareProperty("BxPrev", m_bxPrev);
  declareProperty("BxCentral", m_bxCentral);
  declareProperty("BxNext", m_bxNext);
}

//=============================================================================
// Destructor
//=============================================================================
HCDelayScan::~HCDelayScan() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDelayScan::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Setup the histograms.
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const std::vector<std::string> slots = {"Central", "Previous", "Next"};
  const unsigned int nStations = stations.size();
  const unsigned int nSlots = slots.size();
  m_adcs.resize(nStations);
  m_adcsPerStep.resize(nStations);
  for (unsigned int i = 0; i < nStations; ++i) {
    const std::string st = stations[i];
    m_adcs[i].resize(4);
    m_adcsPerStep[i].resize(4);
    for (unsigned int j = 0; j < 4; ++j) {
      const std::string qu = std::to_string(j);
      m_adcsPerStep[i][j].resize(nSlots);
      for (unsigned int k = 0; k < nSlots; ++k) {
        const std::string sl = slots[k];
        std::string name = "ADC/" + sl + "/" + st + "/" + qu;
        m_adcs[i][j].push_back(Gaudi::Utils::Aida2ROOT::aida2root(
          bookProfile1D(name, name, m_minStep - 0.5, m_maxStep + 0.5,
                        m_maxStep - m_minStep + 1)));
        for (unsigned int kk = m_minStep; kk < m_maxStep; ++kk) {
          const std::string u = std::to_string(kk);
          name = "ADC/" + u + "/" + st + "/" + qu + "/" + sl;
          m_adcsPerStep[i][j][k].push_back(Gaudi::Utils::Aida2ROOT::aida2root(
            book1D(name, name, -0.5, 1023 + 0.5, 1024)));
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDelayScan::execute() {

  // Get event information from ODIN.
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  if (bxid < m_bxMin || bxid > m_bxMax) return StatusCode::SUCCESS;

  // TODO: always, or only if bxCentral?
  unsigned int slot = 0;
  if (bxid == m_bxPrev) slot = 1;
  if (bxid == m_bxNext) slot = 2;

  const unsigned int step = odin->calibrationStep();
  if (step < m_minStep || step > m_maxStep) return StatusCode::SUCCESS;

  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  // Loop over the digits.
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    const bool bwd = i < 3;
    for (unsigned int j = 0; j < 4; ++j) {
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
      m_adcs[i][j][slot]->Fill(step, fadc(adc));
      m_adcsPerStep[i][j][slot][step - m_minStep]->Fill(fadc(adc));
    }
  }
  return StatusCode::SUCCESS;
}

