// LbBound.
#include "BoundProduction.h"

//-----------------------------------------------------------------------------
// Implementation file for class: BoundProduction
//
// 2016-03-18 : ..., Philip Ilten
//-----------------------------------------------------------------------------

// Declare the BoundProduction tool.
DECLARE_TOOL_FACTORY(BoundProduction)

//=============================================================================
// Default constructor.
//=============================================================================
BoundProduction::BoundProduction(const std::string& type,
				 const std::string& name,
				 const IInterface* parent) 
: GaudiTool(type, name, parent), m_nEvents(0), m_prodTool(0) {

  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings,
		  "The commands to pass to the bound state generator.");
  declareProperty("ProductionToolName", m_prodToolName = "Pythia8Production",
		  "Name of the production tool to use for the actual event "
		  "generation (any production tool except this tool is valid.");
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams",
		  "The beam tool to use.");
}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode BoundProduction::initialize() {  
  
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
  
  // Initialize the production tool.
  if (!m_prodTool) {
    if (m_prodToolName == "BoundProduction") 
      Exception("This tool cannot be used as its own production tool.");
    m_prodTool = tool<IProductionTool>(m_prodToolName, this);
    if (!m_prodTool)
      Exception("Failed to initialize the production tool.");
    if ((dynamic_cast<GaudiTool*>(m_prodTool))->hasProperty("BeamToolName"))
      (dynamic_cast<GaudiTool*>(m_prodTool))->
	setProperty("BeamToolName", m_beamToolName);
    else Exception("Production tool has no BeamToolName property.");
  }
  m_nEvents = 0;
  return sc;
}

//=============================================================================
// Initialize the generator.
//=============================================================================
StatusCode BoundProduction::initializeGenerator() {
  if (!m_prodTool) return StatusCode::FAILURE;
  return m_prodTool->initializeGenerator();;
}

//=============================================================================
// Finalize the tool.
//=============================================================================
StatusCode BoundProduction::finalize() {
  if (m_prodTool) release(m_prodTool);
  return GaudiTool::finalize();
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode BoundProduction::generateEvent(HepMC::GenEvent *theEvent, 
					  LHCb::GenCollision *theCollision) {
  if (!m_prodTool) return StatusCode::FAILURE;
  ++m_nEvents;
  StatusCode sc = m_prodTool->generateEvent(theEvent, theCollision);
  return sc && bindStates(theEvent);
}

//=============================================================================
// Set a particle stable.
//=============================================================================
void BoundProduction::setStable(const LHCb::ParticleProperty *thePP) 
{if (m_prodTool) m_prodTool->setStable(thePP);}

//=============================================================================
// Update a particle.
//=============================================================================
void BoundProduction::updateParticleProperties(const LHCb::ParticleProperty 
					       *thePP)
{if (m_prodTool) m_prodTool->updateParticleProperties(thePP);}  

//=============================================================================
// Turn on and off fragmentation.
//=============================================================================
void BoundProduction::turnOnFragmentation() 
{if (m_prodTool) m_prodTool->turnOnFragmentation();}

void BoundProduction::turnOffFragmentation() 
{if (m_prodTool) m_prodTool->turnOffFragmentation();}

//=============================================================================
// Hadronize an event.
//=============================================================================
StatusCode BoundProduction::hadronize(HepMC::GenEvent* theEvent, 
				      LHCb::GenCollision* theCollision) {
  if (!m_prodTool) return StatusCode::FAILURE;
  StatusCode sc = m_prodTool->hadronize(theEvent, theCollision);
  return sc && bindStates(theEvent);
}

//=============================================================================
// Save the event record.
//=============================================================================
void BoundProduction::savePartonEvent(HepMC::GenEvent* theEvent) 
{if (m_prodTool) m_prodTool->savePartonEvent(theEvent);}

//=============================================================================
// Retrieve the event record.
//=============================================================================
void BoundProduction::retrievePartonEvent(HepMC::GenEvent* theEvent) 
{if (m_prodTool) m_prodTool->retrievePartonEvent(theEvent);}

//=============================================================================
// Print the running conditions.
//=============================================================================
void BoundProduction::printRunningConditions()
{if (m_prodTool) m_prodTool->printRunningConditions();}

//=============================================================================
// Returns whether a particle has special status.
//=============================================================================
bool BoundProduction::isSpecialParticle(const LHCb::ParticleProperty *thePP) 
  const {
  if (!m_prodTool) return false;
  else return m_prodTool->isSpecialParticle(thePP);
}

//=============================================================================
// Setup forced fragmentation.
//=============================================================================
StatusCode BoundProduction::setupForcedFragmentation(const int thePdgId) {
  if (!m_prodTool) return StatusCode::FAILURE;
  return m_prodTool->setupForcedFragmentation(thePdgId);
}

//=============================================================================
// Create the bound states.
//=============================================================================
StatusCode BoundProduction::bindStates(HepMC::GenEvent *theEvent) {
  // TO-DO: implement the coalescence model or whatever physics
  // model(s) is chosen! The HepMC Doxygen is here:
  // http://lcgapp.cern.ch/project/simu/HepMC/206/html/
  return StatusCode::FAILURE;
}

//=============================================================================
// The END.
//=============================================================================
