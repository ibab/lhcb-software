// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"
// Local
#include "VPDepositMonitor.h"

using namespace Gaudi::Units;

/** @file VPDepositMonitor.cpp
 *
 *  Implementation of class : VPDepositMonitor
 *
 */

DECLARE_ALGORITHM_FACTORY(VPDepositMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPDepositMonitor::VPDepositMonitor(const std::string& name, 
                                   ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(NULL) {

  declareProperty("Detailed", m_detailed = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VPDepositMonitor::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDet<DeVP>(DeVPLocation::Default);
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPDepositMonitor::execute() {

  LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(LHCb::MCHitLocation::VP);
  if (hits) monitorHits(hits);
  LHCb::MCVPDigits* mcdigits = getIfExists<LHCb::MCVPDigits>(LHCb::MCVPDigitLocation::Default);
  if (mcdigits) monitorDigits(mcdigits);
  return StatusCode::SUCCESS;

}

//=============================================================================
// Fill histograms for MC hits
//=============================================================================
void VPDepositMonitor::monitorHits(LHCb::MCHits* hits) {

  LHCb::MCHits::const_iterator it;
  for (it = hits->begin(); it != hits->end(); ++it) {
    plot((*it)->pathLength() / Gaudi::Units::micrometer, "PathInSensor",  
         "Path in sensor [um]" , 0.0, 300.0, 60);
    plot((*it)->energy() / Gaudi::Units::keV, "EnergyInSensor",
         "Energy deposited in sensor [keV]" , 0.0, 200.0, 40);
  }

}

//=============================================================================
// Fill histograms for MC digits 
//=============================================================================
void VPDepositMonitor::monitorDigits(LHCb::MCVPDigits* mcdigits) {

  plot(mcdigits->size(), "nDigits", "Number of MCVPDigits / event", 
       0., 10000., 50);
  LHCb::MCVPDigits::iterator it;
  for (it = mcdigits->begin(); it != mcdigits->end(); ++it) {
    LHCb::VPChannelID channel = (*it)->channelID();
    plot(channel.module(), "DigitsPerModule",
         "Number of digits / module", -0.5, 51.5, 52);
    // Loop over the deposits.
    double charge = 0.;
    const std::vector<double>& deposits = (*it)->deposits();
    const unsigned int nDeposits = deposits.size();
    for (unsigned int i = 0; i < nDeposits; ++i) {
      charge += deposits[i];
      plot(deposits[i], "ChargePerDeposit", "Charge/deposit [e]", 0., 20000., 100);
    }
    plot(charge, "ChargePerPixel", "Charge/pixel [e]", 0., 25000.0, 100);
  }

}

