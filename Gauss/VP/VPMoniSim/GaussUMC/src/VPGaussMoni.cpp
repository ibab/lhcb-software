// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/Vector4DTypes.h"

// LHCb
#include "Event/MCParticle.h"

// Local
#include "VPGaussMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VPGaussMoni
//
// 2009-06-05 : Victor Coco, based on VeloGaussMoni
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPGaussMoni)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPGaussMoni::VPGaussMoni(const std::string& name,
                         ISvcLocator* pSvcLocator) :
    GaudiTupleAlg(name, pSvcLocator),
    m_hits(),
    m_nMCH(0.),
    m_nMCH2(0.),
    m_nEvent(0) {

  declareProperty("PrintInfo", m_printInfo = false);
  declareProperty("DetailedMonitor", m_detailedMonitor = true);
  declareProperty("HitLocation", m_hitsLocation = LHCb::MCHitLocation::VP);

}

//=============================================================================
// Destructor
//=============================================================================
VPGaussMoni::~VPGaussMoni() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPGaussMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPGaussMoni::execute() {

  ++m_nEvent;
  // Get data.
  m_hits = getIfExists<LHCb::MCHits>(m_hitsLocation);
  if (!m_hits) {
    error() << "No MCHits at " << m_hitsLocation << endmsg;
  } else {
    if (m_printInfo) {
      info() << "--------------------------------------------------" << endmsg;
      info() << " ==> Found " << m_hits->size() << " VP MCHits" << endmsg;
      info() << "--------------------------------------------------" << endmsg;
    }
    monitor();
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPGaussMoni::finalize() {

  m_nMCH /= m_nEvent;
  m_nMCH2 /= m_nEvent;
  const double err = sqrt((m_nMCH2 - (m_nMCH * m_nMCH)) / m_nEvent);

  info() << "------------------------------------------------------" << endmsg;
  info() << "                - VPGaussMoni table -                 " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  if (m_nMCH > 0) {
    info() << " Number of MCHits/Event: " << m_nMCH << "+/-" << err << endmsg;
  } else {
    info() << " ==> No MCHits found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

void VPGaussMoni::monitor() {

  const double size = m_hits->size();
  m_nMCH += size;
  m_nMCH2 += size * size;

  plot(size, "nMCHits", 
       "Number of hits in VP per event",
       0., 3000., 100);
  if (!m_detailedMonitor) return;
  // Loop over all MCHits in the container.
  LHCb::MCHits::iterator it;
  for (it = m_hits->begin(); it != m_hits->end(); ++it) {
    if (m_printInfo) {
      info() << " ==> Test MCHit:\n"
             << " sensor: " << ((*it)->sensDetID())
             << "\nentry: x= " << ((*it)->entry()).x()/Gaudi::Units::mm << " mm"
             << "\nentry: y= " << ((*it)->entry()).y()/Gaudi::Units::mm << " mm"
             << "\nentry: z= " << ((*it)->entry()).z()/Gaudi::Units::mm << " mm"
             << "\nexit: x= " << ((*it)->exit()).x()/Gaudi::Units::mm << " mm"
             << "\nexit: y= " << ((*it)->exit()).y()/Gaudi::Units::mm << " mm"
             << "\nexit: z= " << ((*it)->exit()).z()/Gaudi::Units::mm << " mm"
             << "energy: " << ((*it)->energy())/Gaudi::Units::eV << " eV"
             << "TOF: " << ((*it)->time())/Gaudi::Units::ns << " ns"
             << endmsg;
    }
    plot((*it)->energy() / Gaudi::Units::eV, "eDepSi",
         "Energy deposited in Si [eV]",
         0., 300000., 100);
    double x = (*it)->entry().x() / Gaudi::Units::cm;
    double y = (*it)->entry().y() / Gaudi::Units::cm;
    double z = (*it)->entry().z() / Gaudi::Units::cm;
    plot2D(z, x, "entryZX",
           "Particle entry point in Si [cm] - ZX plane",
           -30., 80., -5., 5., 1100, 50);
    plot2D(x, y, "entryXY",
           "Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 500, 500);
    x = (*it)->exit().x() / Gaudi::Units::cm;
    y = (*it)->exit().y() / Gaudi::Units::cm;
    z = (*it)->exit().z() / Gaudi::Units::cm;
    plot2D(z, x, "exitZX",
           "Particle exit point in Si [cm] - ZX plane",
           -30., 80., -5., 5., 1100, 50);
    plot2D(x, y, "exitXY",
           "Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 500, 500);
    plot((*it)->time() / Gaudi::Units::ns, "TOF",
         "Time Of Flight [ns]", 
         0., 50., 100);
    // Get access to the MCParticle which made the hit, and write out 4-mom
    const LHCb::MCParticle* particle = (*it)->mcParticle();
    if (!particle) continue;
    Gaudi::LorentzVector fMom = particle->momentum();
    plot(fMom.e() /Gaudi::Units::GeV, "eMCPart",
         "Particle energy [GeV]",
         0., 50., 100);       
    // Get the production vertex position
    const LHCb::MCVertex* vert = particle->originVertex();
    x = vert->position().x() / Gaudi::Units::cm;
    y = vert->position().y() / Gaudi::Units::cm;
    z = vert->position().z() / Gaudi::Units::cm;
    plot2D(x, y, "MCVertexPosXY",
           "Position of production MC Vertex of MCParticles giving hits - XY [cm]",
           -5., 5., -5., 5., 500, 500);      
    plot2D(z, y, "MCVertexPosZX",
           "Position of production MC Vertex of MCParticles giving hits - ZY [cm]",
           -30., 80., -5., 5., 500, 500); 
    plot2D(z, y, "MCVertexPosZY",
           "Position of production MC Vertex of MCParticles giving hits - ZY [cm]",
           -30., 80., -5., 5., 500, 500); 
    if (m_printInfo) {
      info() << " ==> MCHit - MCParticle: "
             << "\np_x = " << fMom.px() / Gaudi::Units::GeV
             << "\np_y = " << fMom.py() / Gaudi::Units::GeV
             << "\np_z = " << fMom.pz() / Gaudi::Units::GeV
             << endmsg;
    }
  }
}
