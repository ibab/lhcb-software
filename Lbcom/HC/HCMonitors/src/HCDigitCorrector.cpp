#include <cmath>

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
// Kernel/LHCbKernel
#include "Kernel/HCCellID.h"
// Det/DetDesc
#include "DetDesc/Condition.h"

// Local
#include "HCDigitCorrector.h"

DECLARE_ALGORITHM_FACTORY(HCDigitCorrector)

//=============================================================================
// Constructor
//=============================================================================
HCDigitCorrector::HCDigitCorrector(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator),
      m_condCM(nullptr) {

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

  // Check if the parameters are available in the conditions database.
  const std::string location = "Conditions/Calibration/HC/CommonMode";
  if (existDet<Condition>(location)) {
    registerCondition(location, m_condCM, &HCDigitCorrector::cacheParameters);
    // First update.
    sc = updMgrSvc()->update(this);
    if (sc.isFailure()) {
      return Error("Cannot update calibration constants.");
    }
  } else {
    warning() << "Cannot find " << location << " in database." << endmsg;
    sc = setParameters();
    if (sc.isFailure()) {
      return Error("Cannot set calibration constants.");
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
  const unsigned int parity = bxid % 2;

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
      const int adc = digit->adc();
      LHCb::HCCellID refid(m_references[i][j]);
      const LHCb::HCDigit* refdigit = indigits->object(refid);
      if (!refdigit) {
        const std::string st = bwd ? "B" + std::to_string(i) :
                                     "F" + std::to_string(i - 2);
        warning() << "Cannot retrieve reference for " << st << j << endmsg;
        continue;
      }
      const int refadc = refdigit->adc();
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

//=============================================================================
// Retrieve the calibration constants from the conditions database.
//=============================================================================
StatusCode HCDigitCorrector::cacheParameters() {

  info() << "Updating common mode calibration constants." << endmsg;
  m_thetaConfig.clear();
  m_x0Config.clear();
  m_y0Config.clear();
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  for (const auto& st : stations) {
    const auto thetaEven = m_condCM->paramVect<double>("Theta" + st + "Even");
    const auto thetaOdd = m_condCM->paramVect<double>("Theta" + st + "Odd");
    const auto x0Even = m_condCM->paramVect<double>("X0" + st + "Even");
    const auto x0Odd = m_condCM->paramVect<double>("X0" + st + "Odd");
    const auto y0Even = m_condCM->paramVect<double>("Y0" + st + "Even");
    const auto y0Odd = m_condCM->paramVect<double>("Y0" + st + "Odd");
    for (unsigned int i = 0; i < 4; ++i) {
      m_thetaConfig.push_back(thetaEven[i]);
      m_thetaConfig.push_back(thetaOdd[i]);
      m_x0Config.push_back(x0Even[i]);
      m_x0Config.push_back(x0Odd[i]);
      m_y0Config.push_back(y0Even[i]);
      m_y0Config.push_back(y0Odd[i]);
    }
  }  
  return setParameters();

}

//=============================================================================
// Update the calibration constants.
//=============================================================================
StatusCode HCDigitCorrector::setParameters() {

  const unsigned int nStations = 5;
  const unsigned int nQuadrants = 4;
  // Check if the lists of calibration constants have the right size.
  const unsigned int nExpectedSize = nStations * 4 * 2;
  if (m_thetaConfig.size() != nExpectedSize || 
      m_x0Config.size() != nExpectedSize ||
      m_y0Config.size() != nExpectedSize) {
    return Error("List of calibration constants has incorrect size.");
  } 
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
 
