//=============================================================================
// Include files.
//=============================================================================

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
 
// Event.
#include "Event/GenCollision.h"

// Generators .
#include "Generators/StringParse.h"
#include "Generators/IProductionTool.h"
#include "Generators/IBeamTool.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/IO_HEPEVT.h"

// SuperChic.
#include "SuperChic/SuperChic.h"

// Local.
#include "SuperChicProduction.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: SuperChicProduction
//
//  2015-06-12 : Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of tool factory.
DECLARE_TOOL_FACTORY(SuperChicProduction)

//=============================================================================
// Default constructor.
//=============================================================================
SuperChicProduction::SuperChicProduction(const string& type,
					 const string& name,
					 const IInterface* parent)
: GaudiTool (type, name, parent), m_beamTool(0) {
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings = CommandVector(),
		  "List of commands to pass to SuperChic.");
  declareProperty("Decays", m_decays = true,
		  "Flag to allow SuperChic to perform the decay (true).");
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams",
		  "The beam tool to use.");

  // Create the default settings.
  m_defaultSettings.push_back("xflag jpsi");
  m_defaultSettings.push_back("chiflag 0");
  m_defaultSettings.push_back("decay 2");
  m_defaultSettings.push_back("idh1 211");
  m_defaultSettings.push_back("idh2 -211");
  m_defaultSettings.push_back("idh3 211");
  m_defaultSettings.push_back("idh4 -211");
  m_defaultSettings.push_back("mh 0");
  m_defaultSettings.push_back("mh1 -1");
  m_defaultSettings.push_back("mh2 -1");
  m_defaultSettings.push_back("phdecay mu");
  m_defaultSettings.push_back("eflag 7");
  m_defaultSettings.push_back("pflag mstw");
  m_defaultSettings.push_back("gen fit");
  m_defaultSettings.push_back("photo fit");
  m_defaultSettings.push_back("chiv 2");
  m_defaultSettings.push_back("surv on");
  m_defaultSettings.push_back("corr on");
}

//=============================================================================
// Default destructor.
//=============================================================================
SuperChicProduction::~SuperChicProduction() {}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode SuperChicProduction::initialize() {  

  // Print the initialization banner.
  always() << "============================================================="
	   << "=====" << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
	   << "=====" << endmsg;

  // Initialize the Gaudi tool.
  debug() << "Entered initialize." << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("Gaudi Tool failed to initialize");

  // Initialize the beam tool.
  if (!m_beamTool) m_beamTool = tool<IBeamTool>(m_beamToolName, this);
  if (!m_beamTool) Exception("Failed to initialize the IBeamTool.");

  // Read the SuperChic settings.
  sc = parseSettings(m_defaultSettings);
  if (sc.isFailure()) return Error("Failed to parse default settings.");
  sc = parseSettings(m_userSettings);
  if (sc.isFailure()) return Error("Failed to parse settings.");

  // Set the COM.
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);
  double e1(sqrt(pBeam1.Mag2())), e2(sqrt(pBeam2.Mag2()));
  int ecm(sqrt((e1 + e2)*(e1 + e2) -
  	       (pBeam1 + pBeam2).Mag2())/Gaudi::Units::GeV);
  if (ecm > 1000) ecm /= 1000;
  flags_.eflag[0] = ecm;
  return sc;
}

//=============================================================================
// Finalize method.
//=============================================================================
StatusCode SuperChicProduction::finalize() {

  // Print summary info.
  always() << "==============================================================="
    "===============\n";
  always() << "SuperChic Event Summary\n";
  always() << "==============================================================="
    "===============\n";

  // Finalize SuperChic.
  int entryp(26), exitp(1);
  superchic_(&entryp, &exitp);
  return GaudiTool::finalize( );
}

//=============================================================================
// Initialize the SuperChic generator.
//=============================================================================
StatusCode SuperChicProduction::initializeGenerator( ) {

  // Check masses are initialized correctly.
  if (sccom_.mh1[0] < 0 || sccom_.mh2[0] < 0)
    return Error("Unknown PDG IDs provided for idh1 and idh3.");

  // Run up to weighted loop, skip variable definitions.
  int entryp, exitp;
  entryp = 0; exitp = 2;
  superchic_(&entryp, &exitp);
  entryp = 4; exitp = 5;
  superchic_(&entryp, &exitp);
  entryp = 8; exitp = 16;
  superchic_(&entryp, &exitp);

  // Perform weighted loop.
  entryp = 16; exitp = 19;
  superchic_(&entryp, &exitp);

  // Perform un-weighted loop up to event generation.
  init_.ll[0] = -2;
  entryp = 16; exitp = 20;
  superchic_(&entryp, &exitp);
  m_tries = 0;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode SuperChicProduction::generateEvent(HepMC::GenEvent *theEvent, 
					      LHCb::GenCollision * 
					      /*theCollision*/) {

  // Perform event generation.
  int entryp(20), exitp(24), num(sccom_.num[0]), tries(0);
  while (sccom_.num[0] == num && tries < 1000) {
    sccom_.i[0] = -m_tries;
    init_.ll[0] = 2;
    superchic_(&entryp, &exitp);
    ++m_tries; ++tries;
  }

  // Convert the event to HepMC and return.
  return toHepMC(theEvent);
}

//=============================================================================
// Convert the SuperChic HepEvt record to HepMC format.
//=============================================================================
StatusCode SuperChicProduction::toHepMC(HepMC::GenEvent *theEvent) {

  // Remove decay products from the HepEvt block (if requested).
  if (!m_decays) {
    HepMC::HEPEVT_Wrapper hepevt; int nhep(hepevt.number_entries());
    for (int ihep = 1; ihep <= nhep; ++ihep) {
      if (hepevt.status(ihep) == 2) hepevt.set_status(ihep, 1);
      if (hepevt.first_parent(ihep) > 2 || hepevt.last_parent(ihep) > 2) {
	hepevt.set_number_entries(ihep - 1); break;}
    }
  }

  // Convert from HepEvt to HepMC.
  HepMC::IO_HEPEVT hepIO;
  hepIO.set_print_inconsistency_errors(false);
  if (!hepIO.fill_next_event(theEvent)) 
    return Error("Could not convert HepEvt to HepMC.");
  
  // Convert particle four-vectors to LHCb units.
  for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p) {
    (*p)->set_momentum
      (HepMC::FourVector
       ((*p)->momentum().px()*Gaudi::Units::GeV,
	(*p)->momentum().py()*Gaudi::Units::GeV, 
	(*p)->momentum().pz()*Gaudi::Units::GeV, 
	(*p)->momentum().e() *Gaudi::Units::GeV)) ;
    (*p)->set_generated_mass((*p)->generated_mass()*Gaudi::Units::GeV);
  }
  
  // Convert vertex positions to LHCb units.
  for (HepMC::GenEvent::vertex_iterator v = theEvent->vertices_begin();
       v != theEvent->vertices_end(); ++v) {
    (*v)->set_position
      (HepMC::FourVector
       ((*v)->position().x(), (*v)->position().y(), (*v)->position().z(),
	(*v)->position().t()*Gaudi::Units::mm / Gaudi::Units::c_light));
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Update particles properties.
//=============================================================================
void SuperChicProduction::updateParticleProperties(const LHCb::ParticleProperty
						   *thePP) {
  if (sccom_.idh1[0] == thePP->pid().pid() && sccom_.mh1[0] < 0)
    sccom_.mh1[0] = thePP->mass();
  if (sccom_.idh3[0] == thePP->pid().pid() && sccom_.mh2[0] < 0)
    sccom_.mh2[0] = thePP->mass();
}

//=============================================================================
// Dummy functions.
//=============================================================================
void SuperChicProduction::setStable(const LHCb::ParticleProperty */*thePP*/) {}

void SuperChicProduction::savePartonEvent(HepMC::GenEvent */*theEvent*/) {}

void SuperChicProduction::retrievePartonEvent(HepMC::GenEvent */*theEvent*/) {}

StatusCode SuperChicProduction::hadronize(HepMC::GenEvent */*theEvent*/, 
					 LHCb::GenCollision */*theCollision*/)
{return StatusCode::SUCCESS;}

void SuperChicProduction::printRunningConditions() {}

bool SuperChicProduction::isSpecialParticle(const LHCb::ParticleProperty * 
					   /*thePP*/) const {return false;}

StatusCode SuperChicProduction::setupForcedFragmentation(const int 
							 /*thePdgId*/) 
{return StatusCode::SUCCESS;}

void SuperChicProduction::turnOnFragmentation() {}

void SuperChicProduction::turnOffFragmentation() {}

//=============================================================================
// Parse the SuperChic settings.
//=============================================================================
StatusCode SuperChicProduction::parseSettings(const CommandVector &settings) {
  // Loop over the settings.
  for (unsigned int i = 0; i < settings.size(); ++i) {
    debug() << " Command is: " << settings[i] << endmsg ;
    StringParse setting(settings[i]);
    string entry  = setting.piece(1);
    string strval = setting.piece(2);
    int    intval = setting.intpiece(2);
    double dblval = setting.numpiece(2);
    strval.resize(10, ' ');

    // Apply the settings.
    if      (entry == "xflag")   memcpy(sccom_.xflag, strval.c_str(), 10);
    else if (entry == "chiflag") sccom_.chiflag[0] = intval;
    else if (entry == "decay")   sccom_.decay[0] = intval;
    else if (entry == "idh1")    sccom_.idh1[0] = intval;
    else if (entry == "idh2")    sccom_.idh2[0] = intval;
    else if (entry == "idh3")    sccom_.idh3[0] = intval;
    else if (entry == "idh4")    sccom_.idh4[0] = intval;
    else if (entry == "mh")      sccom_.mh[0] = dblval;
    else if (entry == "mh1")     sccom_.mh1[0] = dblval;
    else if (entry == "mh2")     sccom_.mh2[0] = dblval;
    else if (entry == "phdecay") memcpy(sccom_.phdecay, strval.c_str(), 10);
    else if (entry == "eflag")   flags_.eflag[0] = intval;
    else if (entry == "pflag")   memcpy(sccom_.pflag, strval.c_str(), 10);
    else if (entry == "gen")     memcpy(sccom_.gen, strval.c_str(), 10);
    else if (entry == "photo")   memcpy(sccom_.photo, strval.c_str(), 10);
    else if (entry == "chiv")    sccom_.chiv[0] = intval;
    else if (entry == "surv")    memcpy(sccom_.surv, strval.c_str(), 10);
    else if (entry == "corr")    memcpy(sccom_.corr, strval.c_str(), 10);
    else return Error("Unknown entry: " + entry);
  }
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The END 
// ============================================================================
