// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"
// Local
#include "VPDepositMonitor.h"

using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY(VPDepositMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPDepositMonitor::VPDepositMonitor(const std::string& name, 
                                   ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(nullptr) {

  declareProperty("Detailed", m_detailed = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VPDepositMonitor::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDetIfExists<DeVP>(DeVPLocation::Default);
  if (!m_det) {
    return Error("No detector element at " + DeVPLocation::Default);
  }

  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPDepositMonitor::execute() {

  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(LHCb::MCHitLocation::VP);
  if (hits) monitorHits(hits);
  const LHCb::MCVPDigits* mcdigits = getIfExists<LHCb::MCVPDigits>(LHCb::MCVPDigitLocation::Default);
  if (mcdigits) monitorDigits(mcdigits);
  return StatusCode::SUCCESS;

}

//=============================================================================
// Fill histograms for MC hits
//=============================================================================
void VPDepositMonitor::monitorHits(const LHCb::MCHits* hits) {

  for (const LHCb::MCHit* hit : *hits) {
    plot(hit->pathLength() / Gaudi::Units::micrometer, "PathInSensor",  
         "Path in sensor [um]" , 0.0, 300.0, 60);
    plot(hit->energy() / Gaudi::Units::keV, "EnergyInSensor",
         "Energy deposited in sensor [keV]" , 0.0, 200.0, 40);
  }

}

//=============================================================================
// Fill histograms for MC digits 
//=============================================================================
void VPDepositMonitor::monitorDigits(const LHCb::MCVPDigits* mcdigits) {

  plot(mcdigits->size(), "nDigits", "Number of MCVPDigits / event", 
       0., 10000., 50);
  for (const LHCb::MCVPDigit* mcdigit : *mcdigits) {
    LHCb::VPChannelID channel = mcdigit->channelID();
    plot(channel.module(), "DigitsPerModule",
         "Number of digits / module", -0.5, 51.5, 52);
    // Loop over the deposits.
    double charge = 0.;
    const std::vector<double>& deposits = mcdigit->deposits();
    const unsigned int nDeposits = deposits.size();
    for (unsigned int i = 0; i < nDeposits; ++i) {
      charge += deposits[i];
      plot(deposits[i], "ChargePerDeposit", "Charge/deposit [e]", 0., 20000., 100);
    }
    plot(charge, "ChargePerPixel", "Charge/pixel [e]", 0., 25000.0, 100);
  }

}

