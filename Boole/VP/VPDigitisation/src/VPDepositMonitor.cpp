// Gaudi
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartRefVector.h"
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
/// Standard constructor, initializes variables
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

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDet<DeVP>(DeVPLocation::Default);
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VPDepositMonitor::execute() {

  LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(LHCb::MCHitLocation::VP);
  if (hits) monitorHits(hits);
  LHCb::MCVPDigits* mcdigits = getIfExists<LHCb::MCVPDigits>(LHCb::MCVPDigitLocation::MCVPDigitLocation);
  if (mcdigits) monitorDigits(mcdigits);
  return StatusCode::SUCCESS;

}

void VPDepositMonitor::monitorHits(LHCb::MCHits* hits) {

  LHCb::MCHits::const_iterator it;
  for (it = hits->begin(); it != hits->end(); ++it) {
    plot((*it)->pathLength() / Gaudi::Units::micrometer, "PathInSensor",  
         "Path in sensor [um]" , 0.0, 300.0, 60);
    plot((*it)->energy() / Gaudi::Units::keV, "EnergyInSensor",
         "Energy deposited in sensor [keV]" , 0.0, 200.0, 40);
  }

}

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
    SmartRefVector<LHCb::MCVPDeposit> deposits = (*it)->mcDeposit();
    SmartRefVector<LHCb::MCVPDeposit>::const_iterator itd;
    for (itd = deposits.begin(); itd != deposits.end(); ++itd) {
      plot((*itd)->depositedCharge(), "ChargePerDeposit",
           "Charge/deposit [e]", 0., 20000., 100);
      charge += (*itd)->depositedCharge();
    }
    plot(charge, "ChargePerPixel", 
         "Charge/pixel [e]", 0., 25000.0, 100);
  }

}

