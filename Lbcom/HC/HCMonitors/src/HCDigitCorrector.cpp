#include <cmath>

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
// Kernel/LHCbKernel
#include "Kernel/HCCellID.h"

// Local
#include "HCDigitCorrector.h"

DECLARE_ALGORITHM_FACTORY(HCDigitCorrector)

//=============================================================================
// Constructor
//=============================================================================
HCDigitCorrector::HCDigitCorrector(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator) {

  declareProperty("InputLocation",
                  m_inputLocation = LHCb::HCDigitLocation::Default);
  declareProperty("OutputLocation",
                  m_outputLocation = "Raw/HC/CorrectedDigits");

  declareProperty("Thetas", m_thetaConfig);
  declareProperty("X0", m_x0Config);
  declareProperty("Y0", m_y0Config);

}

//=============================================================================
// Destructor
//=============================================================================
HCDigitCorrector::~HCDigitCorrector() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDigitCorrector::initialize() {

  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  const unsigned int nStations = 5;
  const unsigned int nQuadrants = 4;
  // Check if the lists of calibration constants have the right size.
  const unsigned int nExpectedSize = nStations * 4 * 2;
  if (m_thetaConfig.size() != nExpectedSize || 
      m_x0Config.size() != nExpectedSize ||
      m_y0Config.size() != nExpectedSize) {
    return Error("List of calibration constants has incorrect size.",
                 StatusCode::FAILURE);
  } 
  // TODO: check if I got this right
  m_tantheta.resize(nStations);
  m_x0.resize(nStations);
  m_y0.resize(nStations);
  unsigned int index = 0;
  for (unsigned int i = 0; i < nStations; ++i) {
    m_tantheta[i].resize(nQuadrants);
    m_x0[i].resize(nQuadrants);
    m_y0[i].resize(nQuadrants);
    for (unsigned int j = 0; j < nQuadrants; ++j) {
      m_tantheta[i][j].resize(2);
      m_x0[i][j].resize(2);
      m_y0[i][j].resize(2);
      for (unsigned int k = 0; k < 2; ++k) {
        m_tantheta[i][j][k] = tan(m_thetaConfig[index]);
        m_x0[i][j][k] = m_x0Config[index];
        m_y0[i][j][k] = m_y0Config[index];
        ++index;
      }
    }
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode HCDigitCorrector::execute() {

  // Get event information from ODIN.
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  // TODO: check! For some reason 0 = odd, 1 = even?
  const unsigned int parity = (bxid + 1) % 2;

  // Grab the digits.
  const LHCb::HCDigits* indigits = getIfExists<LHCb::HCDigits>(m_inputLocation);
  if (!indigits) {
    return Error("No digits in " + m_inputLocation, StatusCode::FAILURE);
  }
  // Create container and pass ownership to the TES.
  LHCb::HCDigits* outdigits = new LHCb::HCDigits();
  put(outdigits, m_outputLocation);

  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    const bool bwd = i < 3;
    for (unsigned int j = 0; j < 4; ++j) {
      // Skip masked quadrants.
      if (m_masked[i][j]) continue;
      LHCb::HCCellID id(m_channels[i][j]);
      const LHCb::HCDigit* digit = indigits->object(id);
      if (!digit) {
        const std::string st = bwd ? "B" + std::to_string(i) :
                                     "F" + std::to_string(i - 2);
        warning() << "Cannot retrieve digit for " << st << j << endmsg;
        continue;
      }
      const unsigned int adc = digit->adc();
      LHCb::HCCellID refid(m_references[i][j]);
      const LHCb::HCDigit* refdigit = indigits->object(refid);
      if (!refdigit) {
        const std::string st = bwd ? "B" + std::to_string(i) :
                                     "F" + std::to_string(i - 2);
        warning() << "Cannot retrieve reference for " << st << j << endmsg;
        continue;
      }
      const unsigned int refadc = refdigit->adc();
      const double tantheta = m_tantheta[i][j][parity];
      const double x0 = m_x0[i][j][parity];
      const double y0 = m_y0[i][j][parity];
      const int corradc = static_cast<int>(adc - (refadc - x0) * tantheta - y0);
      LHCb::HCDigit* outdigit = new LHCb::HCDigit(corradc);
      outdigits->insert(outdigit, id);
    }
  }
  return StatusCode::SUCCESS;
}
