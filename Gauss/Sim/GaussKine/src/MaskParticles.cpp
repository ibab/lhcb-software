// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// LHCb
#include "Event/HepMCEvent.h"
#include "Kernel/ParticleProperty.h"

// Local
#include "MaskParticles.h"

DECLARE_ALGORITHM_FACTORY(MaskParticles)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MaskParticles::MaskParticles(const std::string& name,
                             ISvcLocator* pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator) {

  declareProperty("HepMCEventLocation", 
                  m_inputLocation = LHCb::HepMCEventLocation::Default);
  declareProperty("ParticleID", m_pid = 2212);
  declareProperty("EtaCut", m_etaCut = 8.);
  declareProperty("EnergyCut", m_energyCut = 5. * Gaudi::Units::TeV);
}

//=============================================================================
// Destructor
//=============================================================================
MaskParticles::~MaskParticles() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MaskParticles::execute() {
  
  // Get the generated events.
  LHCb::HepMCEvents* events = getIfExists<LHCb::HepMCEvents>(m_inputLocation);
  if (!events) {
    return Error("No HepMCEvents in " + m_inputLocation);
  }

  // Loop over the events.
  LHCb::HepMCEvents::iterator it;
  for (it = events->begin(); it != events->end(); ++it) {
    HepMC::GenEvent* ev = (*it)->pGenEvt();
    // Loop over the particles.
    HepMC::GenEvent::particle_iterator itp; 
    for (itp = ev->particles_begin(); itp != ev->particles_end(); ++itp) {
      HepMC::GenParticle* particle = *itp;
      if (particle->status() != LHCb::HepMCEvent::StableInProdGen) continue;
      // Check the particle ID.
      LHCb::ParticleID pid(particle->pdg_id());
      if (pid.pid() != m_pid) continue;
      // Check pseudorapidity and energy.
      const double eta = fabs(particle->momentum().pseudoRapidity());
      const double energy = particle->momentum().e();
      if (eta > m_etaCut && energy > m_energyCut) {
        // Set the particle status to DocumentationParticle.
        particle->set_status(LHCb::HepMCEvent::DocumentationParticle);
      }
    }
  }
  return StatusCode::SUCCESS;

}

