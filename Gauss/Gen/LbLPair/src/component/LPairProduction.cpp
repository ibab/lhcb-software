//=============================================================================
// Include files.
//=============================================================================

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/PhysicalConstants.h"
 
// Event.
#include "Event/GenCollision.h"

// Generators .
#include "Generators/StringParse.h"
#include "Generators/IProductionTool.h"
#include "Generators/IBeamTool.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/IO_HEPEVT.h"

// LPair.
#include "LPair/LPair.h"

// Local.
#include "LPairProduction.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: LPairProduction
//
//  2015-08-10 : Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of tool factory.
DECLARE_TOOL_FACTORY(LPairProduction)

//=============================================================================
// Default constructor.
//=============================================================================
LPairProduction::LPairProduction(const string& type,
					 const string& name,
					 const IInterface* parent)
: GaudiTool (type, name, parent), m_beamTool(0), m_photos(3) {
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings = CommandVector(),
		  "List of commands to pass to LPair.");
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams",
		  "The beam tool to use.");

  // Create the default settings.
  m_defaultSettings.push_back("ibeg 1");
  m_defaultSettings.push_back("iend 3");
  m_defaultSettings.push_back("ntrt 1"); 
  m_defaultSettings.push_back("prvg 1");
  m_defaultSettings.push_back("ncvg 60000");
  m_defaultSettings.push_back("itvg 6");
  m_defaultSettings.push_back("ncsg 100");
  m_defaultSettings.push_back("inpp 7000");
  m_defaultSettings.push_back("pmod 2");
  m_defaultSettings.push_back("gpdf 5");
  m_defaultSettings.push_back("spdf 4");
  m_defaultSettings.push_back("inpe 7000");
  m_defaultSettings.push_back("emod 2");
  m_defaultSettings.push_back("pair 13");
  m_defaultSettings.push_back("qpdf 12");
  m_defaultSettings.push_back("mcut 2");
  m_defaultSettings.push_back("thmx 30");
  m_defaultSettings.push_back("thmn 0.85");
  m_defaultSettings.push_back("ecut 7.5");
  m_defaultSettings.push_back("ptct 0.9");
  m_defaultSettings.push_back("q2mn 0");
  m_defaultSettings.push_back("q2mx 1E5");
  m_defaultSettings.push_back("mxmn 1.07");
  m_defaultSettings.push_back("mxmx 320.0");
  m_defaultSettings.push_back("phts 3");
  m_defaultSettings.push_back("frmf 1");
  m_defaultSettings.push_back("vacp 1");
}

//=============================================================================
// Default destructor.
//=============================================================================
LPairProduction::~LPairProduction() {}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode LPairProduction::initialize() {  

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

  // Read the LPair settings.
  sc = parseSettings(m_defaultSettings);
  if (sc.isFailure()) return Error("Failed to parse default settings.");
  sc = parseSettings(m_userSettings);
  if (sc.isFailure()) return Error("Failed to parse settings.");
  float omegamin(0);
  lpair_photos__(&m_photos, &omegamin);
  ludat1_.mstu[22 - 1] = 1000000;
  ludat1_.mstu[12 - 1] = 0;

  // Set the beam momenta.
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);
  beam_.inpp[0] = sqrt(pBeam1.Mag2()) / 1000.0;
  beam_.inpe[0] = sqrt(pBeam1.Mag2()) / 1000.0;
  return sc;
}

//=============================================================================
// Finalize method.
//=============================================================================
StatusCode LPairProduction::finalize() {
  return GaudiTool::finalize();
}

//=============================================================================
// Initialize the LPair generator.
//=============================================================================
StatusCode LPairProduction::initializeGenerator() {
  zduini_();
  m_tries = 0;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode LPairProduction::generateEvent(HepMC::GenEvent *theEvent, 
					  LHCb::GenCollision * 
					  /*theCollision*/) {
  int iwant(0);
  zduevt_(&iwant);
  ++m_tries;
  return toHepMC(theEvent);
}

//=============================================================================
// Convert the LPair HepEvt record to HepMC format.
//=============================================================================
StatusCode LPairProduction::toHepMC(HepMC::GenEvent *theEvent) {

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
	(*p)->momentum().e() *Gaudi::Units::GeV));
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
// Dummy functions.
//=============================================================================
void LPairProduction::updateParticleProperties(const LHCb::ParticleProperty
					       */*thePP*/) {}

void LPairProduction::setStable(const LHCb::ParticleProperty */*thePP*/) {}

void LPairProduction::savePartonEvent(HepMC::GenEvent */*theEvent*/) {}

void LPairProduction::retrievePartonEvent(HepMC::GenEvent */*theEvent*/) {}

StatusCode LPairProduction::hadronize(HepMC::GenEvent */*theEvent*/, 
					 LHCb::GenCollision */*theCollision*/)
{return StatusCode::SUCCESS;}

void LPairProduction::printRunningConditions() {}

bool LPairProduction::isSpecialParticle(const LHCb::ParticleProperty * 
					   /*thePP*/) const {return false;}

StatusCode LPairProduction::setupForcedFragmentation(const int 
							 /*thePdgId*/) 
{return StatusCode::SUCCESS;}

void LPairProduction::turnOnFragmentation() {}

void LPairProduction::turnOffFragmentation() {}

//=============================================================================
// Parse the LPair settings.
//=============================================================================
StatusCode LPairProduction::parseSettings(const CommandVector &settings) {
  // Loop over the settings.
  for (unsigned int i = 0; i < settings.size(); ++i) {
    debug() << " Command is: " << settings[i] << endmsg ;
    StringParse setting(settings[i]);
    string entry  = setting.piece(1);
    int    intval = setting.intpiece(2);
    double dblval = setting.numpiece(2);

    // Apply the settings.
    if      (entry == "ibeg") vegpar_.ibeg[0]        = intval;
    else if (entry == "iend") vegpar_.iend[0]        = intval;
    else if (entry == "ntrt") vegpar_.ntreat[0]      = intval; 
    else if (entry == "prvg") vegpar_.nprin[0]       = intval;
    else if (entry == "ncvg") vegpar_.ncvg[0]        = intval;
    else if (entry == "itvg") vegpar_.itmx[0]        = intval;
    else if (entry == "ncsg") vegpar_.npoin[0]       = intval;
    else if (entry == "inpp") beam_.inpp[0]          = dblval;
    else if (entry == "pmod") beam_.pmod[0]          = intval;
    else if (entry == "gpdf") beam_.gpdf[0]          = intval;
    else if (entry == "spdf") beam_.spdf[0]          = intval;
    else if (entry == "inpe") beam_.inpe[0]          = dblval;
    else if (entry == "emod") beam_.emod[0]          = intval;
    else if (entry == "pair") beam_.ipair[0]         = intval;
    else if (entry == "qpdf") beam_.nquark[0]        = intval;
    else if (entry == "mcut") cuts_.modcut[0]        = intval;
    else if (entry == "thmx") cuts_.thmax[0]         = dblval;
    else if (entry == "thmn") cuts_.thmin[0]         = dblval;
    else if (entry == "ecut") cuts_.ecut[0]          = dblval;
    else if (entry == "ptct") cuts_.ptcut[0]         = dblval;
    else if (entry == "q2mn") cuts_.q2mn[0]          = dblval;
    else if (entry == "q2mx") cuts_.q2mx[0]          = dblval;
    else if (entry == "mxmn") cuts_.mxmn[0]          = dblval;
    else if (entry == "mxmx") cuts_.mxmx[0]          = dblval;
    else if (entry == "phts") m_photos               = intval;
    else if (entry == "frmf") mgmge_.mff[0]          = intval;
    else if (entry == "vacp") vacpol_cb__.mvacpol[0] = intval;
    else return Error("Unknown entry: " + entry);
  }
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The END 
// ============================================================================
