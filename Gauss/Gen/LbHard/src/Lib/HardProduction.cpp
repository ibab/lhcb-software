// LbHard.
#include "LbHard/HardProduction.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: HardProduction
//
//  2015-05-01 : Philip Ilten
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor.
//=============================================================================
HardProduction::HardProduction(const std::string& type, const std::string& name,
			       const IInterface* parent) 
  : GaudiTool(type, name, parent),  m_pythia(0), m_pythia8(0), m_nEvents(0),
   m_shower(0), m_hard(0), m_beamTool(0), m_hooks(0), m_lhaup(0) {

  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings,
		  "The commands to pass to the hard process generator.");
  declareProperty("ShowerToolName", m_showerToolName = "PythiaProduction",
		  "The shower tool to use: PythiaProduction, Pythia8Production,"
		  " or HerwigppProduction.");
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams",
		  "The beam tool to use.");
}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode HardProduction::initialize() {  

  // Print the initialization banner.
  debug() << "Entered initialize()." << endmsg;
  always() << "============================================================="
	   << "=====" << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
	   << "=====" << endmsg;

  // Initialize the Gaudi tool.
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) Exception("Failed to initialize the Gaudi tool.");
 
  // Initialize the beam tool.
  if (!m_beamTool) m_beamTool = tool<IBeamTool>(m_beamToolName, this);
  if (!m_beamTool) Exception("Failed to initialize the IBeamTool.");

  // Initialize the hard process tool.
  sc = hardInitialize();
  if (sc.isFailure()) Exception("Failed to initialize the generator.");

  // Initialize the shower tool.
  if (!m_shower) {
    m_shower = tool<IProductionTool>(m_showerToolName, this);
    if (m_showerToolName == "PythiaProduction") {
      if (!m_pythia) m_pythia = dynamic_cast<PythiaProduction*>(m_shower);
      m_pythia->m_beamToolName = m_beamToolName;
    } else if (m_showerToolName == "Pythia8Production") {
      if (!m_pythia8) m_pythia8 = dynamic_cast<Pythia8Production*>(m_shower);
      m_pythia8->m_beamToolName = m_beamToolName;
      if (m_hooks) m_pythia8->m_hooks = m_hooks;
      if (m_lhaup) m_pythia8->m_lhaup = m_lhaup;
    } else Exception("Unavailable shower generator requested.");
    if (!m_shower) Exception("Failed to initialize the shower generator.");
  }
  m_nEvents = 0;
  return sc;
}

//=============================================================================
// Initialize the generator.
//=============================================================================
StatusCode HardProduction::initializeGenerator() {
  if (!m_shower) return StatusCode::FAILURE;
  StatusCode sc = hardInitializeGenerator();
  if (m_shower) sc = sc && m_shower->initializeGenerator();
  return sc;
}

//=============================================================================
// Finalize the tool.
//=============================================================================
StatusCode HardProduction::finalize() {
  StatusCode sc = hardFinalize();
  sc = sc && GaudiTool::finalize();
  return sc;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode HardProduction::generateEvent(HepMC::GenEvent *theEvent, 
					 LHCb::GenCollision *theCollision) {
  if (!m_shower) return StatusCode::FAILURE;
  ++m_nEvents;
  return m_shower->generateEvent(theEvent, theCollision);
}

//=============================================================================
// Set a particle stable.
//=============================================================================
void HardProduction::setStable(const LHCb::ParticleProperty *thePP) {
  hardSetStable(thePP);
  if (m_shower) m_shower->setStable(thePP);
}

//=============================================================================
// Update a particle.
//=============================================================================
void HardProduction::updateParticleProperties(const LHCb::ParticleProperty 
					      *thePP) { 
  hardUpdateParticleProperties(thePP);
  if (m_shower) m_shower->updateParticleProperties(thePP);
}  

//=============================================================================
// Turn on and off fragmentation.
//=============================================================================
void HardProduction::turnOnFragmentation() 
{if (m_shower) m_shower->turnOnFragmentation();}

void HardProduction::turnOffFragmentation() 
{if (m_shower) m_shower->turnOffFragmentation();}

//=============================================================================
// Hadronize an event.
//=============================================================================
StatusCode HardProduction::hadronize(HepMC::GenEvent* theEvent, 
				     LHCb::GenCollision* theCollision) {
  if (!m_shower) return StatusCode::FAILURE;
  return m_shower->hadronize(theEvent, theCollision);
}

//=============================================================================
// Save the event record.
//=============================================================================
void HardProduction::savePartonEvent(HepMC::GenEvent* theEvent) 
{if (m_shower) m_shower->savePartonEvent(theEvent);}

//=============================================================================
// Retrieve the event record.
//=============================================================================
void HardProduction::retrievePartonEvent(HepMC::GenEvent* theEvent) 
{if (m_shower) m_shower->retrievePartonEvent(theEvent);}

//=============================================================================
// Print the running conditions.
//=============================================================================
void HardProduction::printRunningConditions() {
  hardPrintRunningConditions();
  if (m_shower) m_shower->printRunningConditions();
}

//=============================================================================
// Returns whether a particle has special status.
//=============================================================================
bool HardProduction::isSpecialParticle(const LHCb::ParticleProperty *thePP)
  const {
  bool special = hardIsSpecialParticle(thePP);
  if (m_shower) special = special || m_shower->isSpecialParticle(thePP);
  return special;
}

//=============================================================================
// Setup forced fragmentation.
//=============================================================================
StatusCode HardProduction::setupForcedFragmentation(const int thePdgId) {
  if (!m_shower) return StatusCode::FAILURE;
  return m_shower->setupForcedFragmentation(thePdgId);
}

//=============================================================================
// The hard process methods.
//=============================================================================
StatusCode HardProduction::hardInitialize() {return StatusCode::SUCCESS;}

StatusCode HardProduction::hardInitializeGenerator() {
  if (!m_hard || m_hard == m_shower) return StatusCode::SUCCESS;
  return m_hard->initializeGenerator();
}

StatusCode HardProduction::hardFinalize() {return StatusCode::SUCCESS;}

void HardProduction::hardSetStable(const LHCb::ParticleProperty *thePP) 
{if (m_hard && m_hard != m_shower) m_hard->setStable(thePP);}

void HardProduction::hardUpdateParticleProperties(const LHCb::ParticleProperty	
						  *thePP) 
{if (m_hard && m_hard != m_shower) m_hard->updateParticleProperties(thePP);}

void HardProduction::hardPrintRunningConditions()
{if (m_hard && m_hard != m_shower) m_hard->printRunningConditions();}

bool HardProduction::hardIsSpecialParticle(const LHCb::ParticleProperty* thePP)
  const {
  if (m_hard) return m_hard->isSpecialParticle(thePP);
  return false;
}

//=============================================================================
// The END.
//=============================================================================
