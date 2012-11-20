// Gaudi.
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "GaudiKernel/PhysicalConstants.h"
 
// Event.
#include "Event/GenCollision.h"

// Pythia.
#include "Pythia.h"
#include "HepMCInterface.h"

// Generators. 
#include "Generators/IBeamTool.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/HEPEVT_Wrapper.h"

// Local.
#include "PowhegProduction.h"
#include "LbPowheg/GaudiRandomForPowheg.h" 

//-----------------------------------------------------------------------------
//  Implementation file for class: PowhegProduction
//
//  2012-11-20 : Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory.
DECLARE_TOOL_FACTORY(PowhegProduction);

//=============================================================================
// Default constructor.
//=============================================================================
PowhegProduction::PowhegProduction(const std::string &type,
				   const std::string &name,
				   const IInterface *parent)
  : GaudiTool (type, name, parent) {
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings);
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams");
  declareProperty("ListAllParticles", m_listAllParticles = false);

  always() << "============================================================="
           << endmsg;
  always() << "Using as production engine  " << this->type() << endmsg;
  always() << "============================================================="
           << endmsg;

  // Create default Pythia settings.
  m_defaultSettings.clear();
  m_defaultSettings.push_back("SpaceShower:pTmaxMatch = 2");
  m_defaultSettings.push_back("TimeShower:pTmaxMatch  = 2");
  m_defaultSettings.push_back("SigmaProcess:alphaSorder = 2");
  m_defaultSettings.push_back("MultipartonInteractions:Kfactor = 3.0");
  m_defaultSettings.push_back("MultipartonInteractions:bProfile = 1");
  m_defaultSettings.push_back("MultipartonInteractions:pTmin = 4.28");
  m_defaultSettings.push_back("MultipartonInteractions:ecmRef = 14000");
  m_defaultSettings.push_back("MultipartonInteractions:ecmPow = 0.162");
  m_defaultSettings.push_back("StringFlav:mesonCL1S0J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J0 = 0.0135");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J2 = 0.0675");
  m_defaultSettings.push_back("StringFlav:mesonBL1S0J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J0 = 0.0135");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J2 = 0.0675");
  m_defaultSettings.push_back("PDF:useLHAPDF = on");
  m_defaultSettings.push_back("PDF:LHAPDFset = cteq6l.LHpdf");
  m_defaultSettings.push_back("PDF:LHAPDFmember = 1");
  m_defaultSettings.push_back("PDF:extrapolateLHAPDF = off");    
}

//=============================================================================
// Destructor.
//=============================================================================
PowhegProduction::~PowhegProduction() {}

//=============================================================================
// Initialize.
//=============================================================================
StatusCode PowhegProduction::initialize() {  

  // Initialize gaudi tool.
  debug() << "Entered initialize." << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("Gaudi Tool failed to initialize");
  
  // Initialize the beam tool.
  m_beamTool = tool<IBeamTool>(m_beamToolName, this);

  // Initialize the particle property service.
  IParticlePropertySvc* m_ppSvc(0);
  try {m_ppSvc = svc< IParticlePropertySvc > ("ParticlePropertySvc", true);}
  catch (const GaudiException &exc) 
    {Exception( "Cannot open ParticlePropertySvc", exc);}

  // Initialize default beam types.
  m_id1 = 2212; m_id2 = 2212;
  
  // Get the beams from Gaudi
  Gaudi::XYZVector pBeam1;
  Gaudi::XYZVector pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);  
  
  // Find beam masses.
  double mass1 = (m_ppSvc->findByStdHepID(m_id1))->mass();
  double mass2 = (m_ppSvc->findByStdHepID(m_id2))->mass();
  
  // Calculate center of mass energy.
  m_cme = (sqrt(pBeam1.Dot(pBeam1)+mass1*mass1) +  
             sqrt(pBeam2.Dot(pBeam2) + mass2*mass2))/Gaudi::Units::GeV;

  // Get Pythia XMLDOC path.
  std::string xmlpath = "";
  if ("UNKNOWN" != System::getEnv("PYTHIA8XML")) 
    xmlpath = System::getEnv("PYTHIA8XML");

  // Initialize Pythia and Powheg.
  m_pythia = new Pythia8::Pythia(xmlpath);
  m_powheg = new Powheg::Powheg();
  m_hooks  = new PowhegHooks();

  // Initialize the random generator.
  IRndmGenSvc *randSvc(0);
  try {randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);}
  catch (const GaudiException & exc) 
    {Exception("RndmGenSvc not found to initialize Powheg random engine.");}
  m_randomEngine = new GaudiRandomForPowheg(randSvc, sc);
  if (!sc.isSuccess()) 
    return Error("Cannot initialize GaudiRandomForPowheg", sc);
  release(randSvc);

  // Set the random generator.
  m_pythia->setRndmEnginePtr(m_randomEngine);
  return initializeGenerator() ;
}

//=============================================================================
// Generator specific initializer.
//=============================================================================
StatusCode PowhegProduction::initializeGenerator() {
  
  // Set the default Pythia settings.
  always() << "Processing default settings." << endmsg;
  for (CommandVector::const_iterator cmd = m_defaultSettings.begin();
       cmd != m_defaultSettings.end(); ++cmd) {
    always() << *cmd << endmsg;
    m_powheg->readString(*cmd);
    m_pythia->readString(*cmd);
  }

  // Set the user settings.
  always() << "Processing user settings." << endmsg;
  for (CommandVector::const_iterator cmd = m_userSettings.begin();
       cmd != m_userSettings.end(); ++cmd) {
    always() << *cmd << endmsg;
    m_powheg->readString(*cmd);
    m_pythia->readString(*cmd);
  }

  // Initialize Pythia.
  m_powheg->init();
  m_pythia->setUserHooksPtr(m_hooks);
  m_pythia->init(m_powheg->matrixElement);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode PowhegProduction::generateEvent(HepMC::GenEvent *theEvent, 
					   LHCb::GenCollision *theCollision) {
  // Generate the event.
  m_pythia->next();
  m_event = m_pythia->event;  

  // Update the event counter.
  ++m_nEvents;
  return toHepMC( theEvent, theCollision ) ;
}

//=============================================================================
// Set a particle as stable.
//=============================================================================
void PowhegProduction::setStable(const LHCb::ParticleProperty *thePP) {
  int pdgId = thePP->pid().pid();
  m_pythia->particleData.mayDecay(pdgId, false);
}

//=============================================================================
// Update the properties for a particle.
//=============================================================================
void PowhegProduction::updateParticleProperties(const LHCb::ParticleProperty  
						*thePP) {
  int pythiaId = thePP->pythiaID();
  int pdgId    = thePP->pid().pid();
  double width, lifetime;

  // Set only if known Pythia particle.
  if (0 != pythiaId) {

    // Get width.
    if (thePP ->lifetime() == 0) width = 0;
    else
      width = Gaudi::Units::hbarc / (thePP->lifetime()*Gaudi::Units::c_light);
    if (width < (1.5e-6 * Gaudi::Units::GeV)) width = 0;
    
    // Get lifetime.
    lifetime = thePP->lifetime() * Gaudi::Units::c_light;
    if (lifetime <= 1.e-4*Gaudi::Units::mm || 
	lifetime >= 1.e16*Gaudi::Units::mm) lifetime = 0;
    

    // Set properties.
    m_pythia->particleData.m0(pdgId, thePP->mass()/Gaudi::Units::GeV);
    if (pdgId != 6 && pdgId != 23 && pdgId != 24 && pdgId != 25) {
      m_pythia->particleData.mWidth(pdgId, width/Gaudi::Units::GeV) ;
      m_pythia->particleData.mMax(pdgId, thePP->maxWidth()/Gaudi::Units::GeV);
      m_pythia->particleData.tau0(pdgId, lifetime/Gaudi::Units::mm);
    }
  }
}

//=============================================================================
// Retrieve the hard scatter information.
//=============================================================================
void PowhegProduction::hardProcessInfo(LHCb::GenCollision *theCollision) {
  theCollision->setProcessType(m_pythia->info.codeSub()) ;
  theCollision->setSHat(m_pythia->info.sHat());
  theCollision->setTHat(m_pythia->info.tHat());
  theCollision->setUHat(m_pythia->info.uHat());
  theCollision->setPtHat(m_pythia->info.pTHat());
  theCollision->setX1Bjorken(m_pythia->info.x1());
  theCollision->setX2Bjorken(m_pythia->info.x2());
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode PowhegProduction::finalize( ) {
  m_powheg->cleanRun();
  m_pythia->statistics();
  delete m_randomEngine;
  delete m_pythia;
  delete m_hooks;
  delete m_powheg;
  return GaudiTool::finalize();
}

//=============================================================================
// Print parameters.
//=============================================================================
void PowhegProduction::printPythiaParameter() {m_pythia->statistics();}

//=============================================================================
// Turn on fragmentation.
//=============================================================================
void PowhegProduction::turnOnFragmentation() {}

//=============================================================================
// Turn off fragmentation.
//=============================================================================
void PowhegProduction::turnOffFragmentation() {}

//=============================================================================
// Save parton event.
//=============================================================================
void PowhegProduction::savePartonEvent( HepMC::GenEvent */*theEvent*/) {
  m_event = m_pythia->event;
}

//=============================================================================
// Load parton event
//=============================================================================
void PowhegProduction::retrievePartonEvent( HepMC::GenEvent */*theEvent*/) {
  m_pythia->event = m_event;
}

//=============================================================================
// Hadronize event.
//=============================================================================
StatusCode PowhegProduction::hadronize( HepMC::GenEvent *theEvent, 
					LHCb::GenCollision *theCollision) {
  if (!m_pythia->forceHadronLevel()) return StatusCode::FAILURE;
  return toHepMC(theEvent, theCollision);
}

//=============================================================================
// Print running conditions.
//=============================================================================
void PowhegProduction::printRunningConditions() { 
  if (m_nEvents==0 && m_listAllParticles==true && msgLevel(MSG::DEBUG))
    m_pythia->particleData.listAll();
  else if (msgLevel(MSG::VERBOSE)) m_pythia->settings.listAll();
  else if (msgLevel(MSG::DEBUG)) m_pythia->settings.listChanged();
}

//=============================================================================
// Retrieve the process code.
//=============================================================================
int PowhegProduction::processCode( ) { 
  if (m_pythia->info.hasSub()) return m_pythia->info.codeSub();
  else return m_pythia->info.code();
}

//=============================================================================
// Return if special particle.
//=============================================================================
bool PowhegProduction::isSpecialParticle(const LHCb::ParticleProperty *thePP) 
  const {
  int id = thePP->pid().abspid();
  if (id == 1 || id == 2 || id == 3 || id == 4 || id == 5 || id == 6 || 
      id == 7 || id == 8 || id == 17 || id == 18 || id == 21 || id == 110 || 
      id == 990 || id == 33 || id == 34 || id == 37 || id == 39 || id == 41 || 
      id == 42 || id == 43 || id == 44 || id == 81 || id == 82 || id == 83 || 
      id == 84 || id == 85 || id == 88 || id == 89 || id == 90 || id == 91 || 
      id == 92 || id == 93 || id == 94 || id == 95 || id == 96 || id == 97 || 
      id == 98 || id == 99 || id == 9900110 || id == 9900210 || id == 9900220 
      || id == 9900330 || id == 9900440 || id == 9902110 || id == 9902210 
      || id == 1101 || id == 1103 || id == 2101 || id == 2103 || id == 2201 
      || id == 2203 || id == 3101 || id == 3103 || id == 3201 || id == 3203 
      || id == 3301 || id == 3303 || id == 4101 || id == 4103 || id == 4201 
      || id == 4203 || id == 4301 || id == 4303 || id == 4401 || id == 4403 
      || id == 5101 || id == 5103 || id == 5201 || id == 5203 || id == 5301 
      || id == 5303 || id == 5401 || id == 5403 || id == 5501 || id == 5503 
      || id == 1000022 || id == 1000024) return true;
  return false;    
}

//=============================================================================
// Force fragmentation.
//=============================================================================
StatusCode PowhegProduction::setupForcedFragmentation(const int /*thePdgId*/) {
  m_pythia->readString("PartonLevel:all = off");
  return StatusCode::SUCCESS;
}

// ============================================================================
/// Pythia to HepMC format. 
// ============================================================================
StatusCode PowhegProduction::toHepMC ( HepMC::GenEvent*     theEvent    , 
                                        LHCb::GenCollision * theCollision ){
  

  // Convert from Pythia format to HepMC format.
  HepMC::I_Pythia8 conversion;

  if (!(conversion.fill_next_event(*m_pythia, theEvent))) 
    return Error("Cannot convert Pythia event to HepMC.");
  
  // Convert to LHCb status codes.
  for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); p++) {
    
    int status = (*p)->status();
    if (status > 3 && status < 20)
      (*p)->set_status(LHCb::HepMCEvent::DocumentationParticle);
    else if (status > 19 && status < 80)
      (*p)->set_status(LHCb::HepMCEvent::DecayedByProdGen);
    else if (status == 91 || status == 92 || (status > 99 && status < 110)) {
      if ((*p)->end_vertex() != 0)
        (*p)->set_status(LHCb::HepMCEvent::DecayedByProdGen);
      else
	(*p)->set_status(LHCb::HepMCEvent::StableInProdGen);
    }
    else if (status == 93 || status == 94)
      (*p)->set_status(LHCb::HepMCEvent::DecayedByProdGen);
    else if (status == 99)
      (*p)->set_status(LHCb::HepMCEvent::DocumentationParticle);
    else if (status != LHCb::HepMCEvent::DecayedByProdGen &&
             status != LHCb::HepMCEvent::StableInProdGen &&
             status != LHCb::HepMCEvent::DocumentationParticle)
      warning() << "Unknown status rule " << status 
		<< " for particle" << (*p)->pdg_id() << endmsg;
  }
  
  // Convert to LHCb units.
  for (HepMC::GenEvent::vertex_iterator v = theEvent->vertices_begin();
       v != theEvent->vertices_end() ; v++)
    (*v)->set_position(HepMC::FourVector((*v)->position().x(),
					 (*v)->position().y(),
					 (*v)->position().z(),
					 (*v)->position().t() 
					 * Gaudi::Units::mm \
					 / Gaudi::Units::c_light));    
  theEvent->set_signal_process_id(processCode());
  hardProcessInfo(theCollision);
  return StatusCode::SUCCESS;
}

// ============================================================================
// The END. 
// ============================================================================
