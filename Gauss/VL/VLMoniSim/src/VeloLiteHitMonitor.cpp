// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/Vector4DTypes.h"
// Event/MCEvent
#include "Event/MCParticle.h"
// Det/VeloLiteDet
#include "VeloLiteDet/DeVeloLite.h"
// Local
#include "VeloLiteHitMonitor.h"

/** @file VeloLiteHitMonitor.cpp
 *
 *  Implementation of class : VeloLiteHitMonitor
 *
 */

DECLARE_ALGORITHM_FACTORY(VeloLiteHitMonitor);

//=============================================================================
/// Standard constructor
//=============================================================================
VeloLiteHitMonitor::VeloLiteHitMonitor(const std::string& name,
                                       ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(0),
    m_hits(),
    m_puHits(),
    m_nHits(0.),
    m_nHits2(0.),
    m_nPuHits(0.),
    m_nPuHits2(0.),
    m_nEvents(0) {

  declareProperty("PrintInfo", m_printInfo = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VeloLiteHitMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVeloLite>(DeVeloLiteLocation::Default);
  setHistoTopDir("VeloLite/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VeloLiteHitMonitor::execute() {

  debug() << " ==> execute()" << endmsg;
  ++m_nEvents;
  if (!exist<LHCb::MCHits>(LHCb::MCHitLocation::Velo)) {
    error() << "There are no MCHits at " << LHCb::MCHitLocation::Velo 
            << " in TES!" << endmsg;
  } else {
    m_hits = get<LHCb::MCHits>(LHCb::MCHitLocation::Velo);
    monitorHits();
  }

  if (!exist<LHCb::MCHits>(LHCb::MCHitLocation::PuVeto)) {
    error() << "There are no MCHits at " << LHCb::MCHitLocation::PuVeto 
            << " in TES!" << endmsg;
  } else {
    m_puHits = get<LHCb::MCHits>(LHCb::MCHitLocation::PuVeto);
    monitorPuHits();
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Finalize
//=============================================================================
StatusCode VeloLiteHitMonitor::finalize() {

  debug() << " ==> finalize()" << endmsg;  
  m_nHits  /= m_nEvents;
  m_nHits2 /= m_nEvents;
  const double err = sqrt((m_nHits2 - (m_nHits * m_nHits)) / m_nEvents);
  m_nPuHits  /= m_nEvents;
  m_nPuHits2 /= m_nEvents;
  const double errPu = sqrt((m_nPuHits2 - (m_nPuHits * m_nPuHits)) / m_nEvents);

  info() << "------------------------------------------------------" << endmsg;
  info() << "                 - VeloLiteHitMonitor -               " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  if (m_nHits > 0) {
    info() << " Number of MCHits / event:         " << m_nHits << "+/-"
           << err << endmsg;
  } else {
    info() << " ==> No MCHits found! " << endmsg;
  }
  if (m_nPuHits > 0) {
    info() << " Number of pile-up MCHits / event: " << m_nPuHits << "+/-"
           << errPu << endmsg;
  } else {
    info() << " ==> No pile-up MCHits found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();  

}

StatusCode VeloLiteHitMonitor::monitorHits() {

  debug() << " ==> monitorHits()" << endmsg;
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
    const DeVeloLiteSensor* sensor = m_det->sensor((*it)->sensDetID());
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
                 "Particle entry point in PhiDR [cm] - XY plane",
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
  return StatusCode::SUCCESS;

}

StatusCode VeloLiteHitMonitor::monitorPuHits() {

  debug() << " ==> monitorPuHits()" << endmsg;
  const double size = m_puHits->size();
  m_nPuHits += size;
  m_nPuHits2 += size * size;
  plot(size, "nMCPUHits",
       "Number of pile-up hits per event",
       0., 3000., 100);
  LHCb::MCHits::iterator it;
  for (it = m_puHits->begin(); it != m_puHits->end(); ++it) {
    if (m_printInfo) {
      info() << " ==> Pile-Up MCHit " << it - m_puHits->begin() << endmsg;
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
    plot((*it)->energy() / Gaudi::Units::eV, "eDepSiPU",
         "PileUp: Energy deposited in Si [eV]",
         0., 300000., 100);
    plot2D((*it)->entry().x() / Gaudi::Units::cm,
           (*it)->entry().y() / Gaudi::Units::cm, "entryXYPU",
           "PileUp: Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*it)->entry().z() / Gaudi::Units::cm,
           (*it)->entry().x() / Gaudi::Units::cm, "entryXYPU",
           "PileUp: Particle entry point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot2D((*it)->exit().x() / Gaudi::Units::cm,
           (*it)->exit().y() / Gaudi::Units::cm, "exitXYPU",
           "PileUp: Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*it)->exit().z() / Gaudi::Units::cm,
           (*it)->exit().x() / Gaudi::Units::cm, "exitZXPU",
           "PileUp: Particle exit point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot((*it)->time() / Gaudi::Units::ns, "TOFPU",
         "PileUp: Time Of Flight [ns]",
         0., 50., 100);
    // Get the MCParticle which made the hit, and write out 4-mom
    const LHCb::MCParticle* particle = (*it)->mcParticle();
    if (0 != particle) {
      Gaudi::LorentzVector fMom = particle->momentum();
      plot(fMom.e() / Gaudi::Units::GeV, "eMCPartPU",
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
  return StatusCode::SUCCESS;

}
   
