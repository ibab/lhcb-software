// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/Vector4DTypes.h"
// Event/MCEvent
#include "Event/MCParticle.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "VLHitMonitor.h"

/** @file VLHitMonitor.cpp
 *
 *  Implementation of class : VLHitMonitor
 *
 */

DECLARE_ALGORITHM_FACTORY(VLHitMonitor)

//=============================================================================
/// Standard constructor
//=============================================================================
VLHitMonitor::VLHitMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(0),
    m_hits(),
    m_nHits(0.),
    m_nHits2(0.),
    m_nEvents(0) {

  declareProperty("Detailed",  m_detailed = false);
  declareProperty("PrintInfo", m_printInfo = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VLHitMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << " ==> initialize()"  << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  setHistoTopDir("VL/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VLHitMonitor::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> execute()" << endmsg;
  ++m_nEvents;
  if (!exist<LHCb::MCHits>(LHCb::MCHitLocation::VL)) {
    error() << "There are no MCHits at " << LHCb::MCHitLocation::VL << endmsg;
    return StatusCode::FAILURE; 
  }
  m_hits = get<LHCb::MCHits>(LHCb::MCHitLocation::VL);
  monitor();
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Finalize
//=============================================================================
StatusCode VLHitMonitor::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> finalize()" << endmsg;  
  m_nHits  /= m_nEvents;
  m_nHits2 /= m_nEvents;
  const double err = sqrt((m_nHits2 - (m_nHits * m_nHits)) / m_nEvents);

  info() << "------------------------------------------------------" << endmsg;
  info() << "                      VLHitMonitor                    " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  if (m_nHits > 0) {
    info() << " Number of MCHits / event:         " << m_nHits << "+/-"
           << err << endmsg;
  } else {
    info() << " ==> No MCHits found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();  

}

void VLHitMonitor::monitor() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> monitor()" << endmsg;
  const double size = m_hits->size();
  m_nHits += size;
  m_nHits2 += size * size;
  plot(size, "nMCHits", 
       "Number of hits per event",
       0., 3000., 100);
  // Loop over all MCHits stored in the container.
  LHCb::MCHits::iterator it;
  for (it = m_hits->begin(); it != m_hits->end(); ++it) {
    if (m_printInfo) {
      info() << " ==> MCHit " << it - m_hits->begin() << endmsg;
      info() << " sensor: " << ((*it)->sensDetID()) << endmsg;
      info() << " entry (mm): x = " << ((*it)->entry()).x() / Gaudi::Units::mm 
             << ", y = " << ((*it)->entry()).y() / Gaudi::Units::mm 
             << ", z = " << ((*it)->entry()).z() / Gaudi::Units::mm << endmsg;
      info() << " exit (mm): x = " << ((*it)->exit()).x() / Gaudi::Units::mm 
             << ", y = " << ((*it)->exit()).y() / Gaudi::Units::mm 
             << ", z = " << ((*it)->exit()).z() / Gaudi::Units::mm << endmsg;
      info() << " energy: " << ((*it)->energy()) / Gaudi::Units::eV << " eV"
             << ", TOF: " << ((*it)->time()) / Gaudi::Units::ns << " ns" 
             << endmsg;
    }
    plot((*it)->energy() / Gaudi::Units::eV, "eDepSi",
         "Energy deposited in Si [eV]",
         0., 300000., 100);
    plot((*it)->sensDetID(), "detId", 
         -0.5, 199.5, 200);
    plot2D((*it)->entry().z() / Gaudi::Units::cm,
           (*it)->entry().x() / Gaudi::Units::cm, "entryZX",
           "Particle entry point in Si [cm] - ZX plane",
           -20., 80., -5., 5., 1000, 50);
    plot2D((*it)->entry().x() / Gaudi::Units::cm,
           (*it)->entry().y() / Gaudi::Units::cm, "entryXY",
           "Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*it)->exit().z() / Gaudi::Units::cm,
           (*it)->exit().x() / Gaudi::Units::cm, "exitZX",
           "Particle exit point in Si [cm] - ZX plane",
           -20., 80., -5., 5., 1000, 50);
    plot2D((*it)->exit().x() / Gaudi::Units::cm,
           (*it)->exit().y() / Gaudi::Units::cm, "exitXY",
           "Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot((*it)->time() / Gaudi::Units::ns, "TOF",
         "Time Of Flight [ns]", 
         0., 50., 100);
    if (!m_detailed) continue;
    const DeVLSensor* sensor = m_det->sensor((*it)->sensDetID());
    const double x = (*it)->entry().x() / Gaudi::Units::cm;
    const double y = (*it)->entry().y() / Gaudi::Units::cm;
    const double z = (*it)->entry().z() / Gaudi::Units::cm;
    if (sensor->isRight()) {
      if (sensor->isR()) {
        plot2D(x, y, "entryRRXY",
               "Particle entry point in RRight [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, "entryRRZX",
               "Particle entry point in RRight [cm] - ZX plane",
               -20., 80., -5., 5., 1000, 50);
      } else if (sensor->isPhi()) {
        if (sensor->isDownstream()) {
          plot2D(x, y, "entryPDRXY",
                 "Particle entry point in PhiDR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        } else {
          plot2D(x, y, "entryPURXY",
                 "Particle entry point in PhiUR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }
      }
    } else {
      if (sensor->isR()) {
        plot2D(x, y, "entryRLXY",
               "Particle entry point in RLeft [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, "entryRLZX",
               "Particle entry point in RLeft [cm] - ZX plane",
               -20, 80, -5., 5., 1000, 50);
      } else if (sensor->isPhi()) {
        if (sensor->isDownstream()) {
          plot2D(x, y, "entryPDLXY",
                 "Particle entry point in PhiDL [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        } else {    
          plot2D(x, y, "entryPULXY",
                 "Particle entry point in PhiUL [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }
      }
    }
    // Get the MCParticle which made the hit, and write out 4-mom
    const LHCb::MCParticle* particle = (*it)->mcParticle();
    if (particle) {
      Gaudi::LorentzVector fMom = particle->momentum();
      plot(fMom.e() / Gaudi::Units::GeV, "eMCPart",
           "Particle energy [GeV]",
           0., 50., 100);        
      if (m_printInfo) {
        info() << " ==> MCParticle four-momentum: " << endmsg;
        info() << " px = " << fMom.px() / Gaudi::Units::GeV
               << ", py = " << fMom.py() / Gaudi::Units::GeV
               << ", pz = " << fMom.pz() / Gaudi::Units::GeV
               << ", E = " << fMom.e() / Gaudi::Units::GeV
               << endmsg;
      }
    }
  }

}
   
