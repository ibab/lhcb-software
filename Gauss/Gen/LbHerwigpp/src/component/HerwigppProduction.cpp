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

// ThePEG.
#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/Repository/Repository.h"
#include "ThePEG/Persistency/PersistentIStream.h"
#include "ThePEG/Utilities/DynamicLoader.h"
#include "ThePEG/Utilities/Debug.h"
#include "ThePEG/EventRecord/Event.h"
#include "ThePEG/EventRecord/SubProcess.h"
#include "ThePEG/Handlers/XComb.h"
#include "ThePEG/Handlers/EventHandler.h"
#include "ThePEG/PDF/PartonExtractor.h"
#include "ThePEG/PDF/PDF.h"
#include "ThePEG/Vectors/HepMCConverter.h"
#include "ThePEG/Repository/UseRandom.h"

// Generators .
#include "Generators/IProductionTool.h"
#include "Generators/IBeamTool.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

// STL.
#include "limits.h"

// Local.
#include "HerwigppProduction.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: HerwigppProduction
//
//  2010-03-30 : Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of tool factory.
DECLARE_TOOL_FACTORY(HerwigppProduction);


//=============================================================================
// ThePEG structure used for PDF info.
//=============================================================================
namespace ThePEG {
  template<> struct HepMCTraits<HepMC::GenEvent> : 
    public HepMCTraitsBase<HepMC::GenEvent, HepMC::GenParticle,
			   HepMC::GenVertex, HepMC::Polarization,
			   HepMC::PdfInfo>
  {};
}

//=============================================================================
// Default constructor.
//=============================================================================
HerwigppProduction::HerwigppProduction(const std::string& type,
				       const std::string& name,
				       const IInterface* parent)
  : GaudiTool (type, name, parent) {
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings);
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams");
  declareProperty("PrintEvent", m_printEvent = false);
  declareProperty("WriteEvent", m_writeEventOutput = "");
  declareProperty("ReadEvent", m_readEventInput = "");
  
  always() << "============================================================="
           << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
           << endmsg;
  
  // Create default Herwig++ settings.
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:PrintEvent 0");
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:MaxErrors 10000");
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:NumberOfEvents 100000");

  // Create minbias Herwig++ settings.
  m_minbiasSettings.push_back("insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEMinBias");
  m_minbiasSettings.push_back("set /Herwig/Cuts/JetKtCut:MinKT 0.0*GeV");
  m_minbiasSettings.push_back("set /Herwig/Cuts/QCDCuts:MHatMin 0.0*GeV");
  m_minbiasSettings.push_back("set /Herwig/Cuts/QCDCuts:X1Min 0.055");
  m_minbiasSettings.push_back("set /Herwig/Cuts/QCDCuts:X2Min 0.055");
  m_minbiasSettings.push_back("set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0");
  m_minbiasSettings.push_back("set /Herwig/UnderlyingEvent/MPIHandler:softInt Yes");
  m_minbiasSettings.push_back("set /Herwig/UnderlyingEvent/MPIHandler:twoComp Yes");
  m_minbiasSettings.push_back("set /Herwig/UnderlyingEvent/MPIHandler:DLmode 3");
}

//=============================================================================
// Default destructor.
//=============================================================================
HerwigppProduction::~HerwigppProduction( ) { }

//=============================================================================
// Initialize the class.
//=============================================================================
StatusCode HerwigppProduction::initialize( ) {  

  // Initialize gaudi tool.
  debug() << "Entered initialize." << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("Gaudi Tool failed to initialize");

  // Determine the message level (maps ALWAYS -> Level 9, every step).
  int msgLvl = this->msgLevel() * 1.3;
  debug() << "Herwig++ printing at message level: " << msgLvl << endmsg;
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:DebugLevel " + numToString(msgLvl));

  // Set the HepMC output file if requested.
  if (m_writeEventOutput != "")
    m_hepmcOut = new HepMC::IO_GenEvent(m_writeEventOutput.c_str(),
					std::ios::out);
  else m_hepmcOut = 0;

  // Initialize random number service.
  IRndmGenSvc *i = 0;
  try {
    i = svc<IRndmGenSvc>("RndmGenSvc" , true);
  }
  catch (const GaudiException &exc) {
    Exception( "RndmGenSvc not found to initialize Herwig++ random engine." );
  }

  // Initialize random number generator.
  sc = m_random.initialize(i, Rndm::Flat(0,1));
  if ( sc.isFailure() )
    Exception( "Random seed generator for Herwig++ failed to initialize.");
  
  // Initialize the beam tool.
  m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;
  
  // Initialize the particle property service.
  LHCb::IParticlePropertySvc* m_ppSvc( 0 ) ;
  try {
    m_ppSvc = svc< LHCb::IParticlePropertySvc > ("LHCb::ParticlePropertySvc", true);
  }
  catch (const GaudiException &exc) {
    Exception( "Cannot open ParticlePropertySvc", exc);
  }
  
  // Initialize default beam types.
  m_id1 = 2212; m_id2 = 2212;
  
  // Get the beams from Gaudi
  Gaudi::XYZVector pBeam1;
  Gaudi::XYZVector pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);  
  
  // Find beam masses.
  double mass1 = (m_ppSvc->find( LHCb::ParticleID( m_id1 ) ) )->mass();
  double mass2 = (m_ppSvc->find( LHCb::ParticleID( m_id2 ) ) )->mass();
  
  // Calculate center of mass energy.
  m_cme = (sqrt(pBeam1.Dot(pBeam1)+mass1*mass1) +  
             sqrt(pBeam2.Dot(pBeam2) + mass2*mass2))/Gaudi::Units::GeV;
  
  // Set the generator beams.
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy " + numToString(m_cme));
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:EventHandler:BeamA " + numToString(m_id1));
  m_defaultSettings.push_back("set /Herwig/Generators/LHCGenerator:EventHandler:BeamB " + numToString(m_id2));

  return initializeGenerator();
}

//=============================================================================
// Initialize the Herwig++ generator.
//=============================================================================
StatusCode HerwigppProduction::initializeGenerator( ) {
  debug() << "Entered initializeGenerator()"  << endmsg;

  // Print Herwig++ and ThePEG module directory paths (debug only).
  debug() << "There are " 
	  << ThePEG::DynamicLoader::allPaths().size()
	  << " ThePEG search paths." << endmsg;
  for (std::vector<std::string>::const_iterator path = 
	 ThePEG::DynamicLoader::allPaths().begin(); path !=
	 ThePEG::DynamicLoader::allPaths().end(); path++)
    debug() << "Using path: " << *path << endmsg;
  
  // Load the default Herwig++ repository.
  std::string herwigppRepo = System::getEnv("HERWIGPPHOME") 
    + "/share/Herwig++/HerwigDefaults.rpo";
  debug() << "Loading Herwig++ repo from " << herwigppRepo << endmsg;
  ThePEG::Repository::load(herwigppRepo);
  debug() << "Loaded Herwig++ repository." << endmsg;
  
  // Get corrected diffractive PDF paths.
  std::string pomeronPath = System::getEnv("HERWIGPPHOME") 
    + "/share/Herwig++/PDF/diffraction/";
  m_defaultSettings.push_back("set /Herwig/Partons/PomeronPDF:RootName " + pomeronPath);

  // Set the default Herwig++ settings.
  always() << "Processing default settings." << endmsg;
  for (CommandVector::const_iterator cmd = m_defaultSettings.begin();
       cmd != m_defaultSettings.end(); ++cmd) {
    always() << *cmd << endmsg;
    ThePEG::Repository::exec(*cmd, std::cout);
  }
  
  // Set minbias or user settings.
  if (m_userSettings.size() == 0) {
    always() << "Processing minbias settings." << endmsg;
    for (CommandVector::const_iterator cmd = m_minbiasSettings.begin();
	 cmd != m_minbiasSettings.end(); ++cmd) {
      always() << replaceSpecialCharacters(*cmd) << endmsg;
      ThePEG::Repository::exec(replaceSpecialCharacters(*cmd), std::cout);
    }
  }
  else {
    always() << "Processing user settings." << endmsg;
    for (CommandVector::const_iterator cmd = m_userSettings.begin();
	 cmd != m_userSettings.end(); ++cmd) {
      always() << replaceSpecialCharacters(*cmd) << endmsg;
      ThePEG::Repository::exec(replaceSpecialCharacters(*cmd), std::cout);
    }
  }
  
  // Update the repository.
  debug() << "Updating repository." << endmsg;
  ThePEG::Repository::update();

  // Create initial temporary Herwig++ generator.
  debug() << "Getting generator from ThePEG."  << endmsg;
  ThePEG::EGPtr tmpHW;
  ThePEG::EGPtr tmpEG = 
    ThePEG::Repository::GetObject<ThePEG::EGPtr>
    ("/Herwig/Generators/LHCGenerator");
  try {
    debug() << "Reducing repository to single LHC generator run."  << endmsg;
    tmpHW = ThePEG::Repository::makeRun(tmpEG, "LHCb");
  }
  catch (ThePEG::Exception& e) {
    error() << "Exception in ThePEG: " << e.what() << endmsg;
    throw;
  }
  catch (std::exception& e) {
    error() << "STL exception: " << e.what() << endmsg;
    throw;
  }
  
  // Initialize the generator.
  debug() << "Initializing the temporary generator."  << endmsg;
  tmpHW->initialize();

  // Initialize the wrapper for the generator.
  debug() << "Initializing the actual generator object." << endmsg;
  m_herwigpp = new HerwigppGenerator(tmpHW);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode HerwigppProduction::generateEvent( HepMC::GenEvent* theEvent, 
					      LHCb::GenCollision * 
					      /*theCollision*/ ) {
  // Set the random seed.
  int rnd = m_random.shoot() * INT_MAX;
  
  // Flush the Herwig++ random generator.
  m_herwigpp->accessRandom()->flush();

  // Set the Herwig++ random generator seed.
  m_herwigpp->accessRandom()->setSeed(rnd);
  debug() << "Random seed set to " << rnd << "." << endmsg;

  // Check generator object is valid.
  assert(m_herwigpp);

  // Generate the event.
  debug() << "Generating an event."  << endmsg;
  m_event = m_herwigpp->shoot();
  m_nEvents++;

  // Convert the event to HepMC and return.
  return toHepMC(theEvent);
}

//=============================================================================
// Write the HEPMC container (for debugging purposes).
//=============================================================================
StatusCode HerwigppProduction::writeEvent(HepMC::GenEvent* theEvent) {
  m_hepmcOut->write_event(theEvent);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Print the HEPMC container (for debugging purposes).
//=============================================================================
StatusCode HerwigppProduction::printEvent(HepMC::GenEvent* theEvent) {
  // Loop over the particles.
  for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); p++)
    (*p)->print();
  
  // Loop over the vertices.
  for (HepMC::GenEvent::vertex_iterator v = theEvent->vertices_begin();
       v != theEvent->vertices_end(); v++)
    (*v)->print();

  return StatusCode::SUCCESS;
}


//=============================================================================
// Fill the HEPMC container from a file (for debugging purposes).
//=============================================================================
StatusCode HerwigppProduction::readEvent(HepMC::GenEvent* theEvent) {
  std::ifstream file(m_readEventInput.c_str());
  HepMC::IO_GenEvent hepmcio(file);
  hepmcio.fill_next_event(theEvent);
  return StatusCode::SUCCESS;
}


//=============================================================================
// Fill the HEPMC container from the Herwig++ event pointer.
//=============================================================================
StatusCode HerwigppProduction::toHepMC(HepMC::GenEvent* theEvent) {
  // Convert event record to HepMC.
  debug() << "Herwig++ converting event to HepMC."  << endmsg;
  ThePEG::HepMCConverter<HepMC::GenEvent>::convert(*m_event, *theEvent, true,
						   ThePEG::MeV,
						   ThePEG::millimeter);
  debug() << "Herwig++ converted event to HepMC."  << endmsg;

  // Fix vertex positions.
  debug() << "Herwig++ fixing HepMC vertex positions." << endmsg;
  std::vector<HepMC::GenVertex*> vertices;
  for (HepMC::GenEvent::vertex_iterator v = theEvent->vertices_begin();
       v != theEvent->vertices_end(); v++)
      vertices.push_back(*v);
  for (unsigned int vertex = vertices.size() - 1; vertex > 0; vertex--)
    if (vertices[vertex]->position().t() < 
	vertices[vertex - 1]->position().t())
      vertices[vertex - 1]->set_position(vertices[vertex]->position());
  
  // Fix particle codes.
  debug() << "Herwig++ fixing HepMC particle status codes." << endmsg;
  for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); p++) {
    // Set generator specific particles to status 3.
    if ((*p)->pdg_id() > 80 && (*p)->pdg_id() < 101) (*p)->set_status(3);
    // Set all other non-stable particles to status 2.
    else if ((*p)->status() != 1) (*p)->set_status(2);
  }
  
  // Get random seeds.
  debug() << "Herwig++ recording random seeds to HepMC." << endmsg;
  std::vector<long> seeds(1,static_cast<long>(m_seed));
  theEvent->set_random_states(seeds);

  // Manually add PDF to event record.
  debug() << "Herwig++ adding PDF info to HepMC."  << endmsg;
  
  // Determine incoming partons.
  ThePEG::tSubProPtr subprocess = m_event->primarySubProcess();
  int id1 = subprocess->incoming().first ->id(); 
  int id2 = subprocess->incoming().second->id(); 
  
  // Get the event handler.
  ThePEG::tcEHPtr eventHandler = ThePEG::dynamic_ptr_cast<ThePEG::tcEHPtr>
    (m_event->handler());
  
  // Get parton momentum fractions.
  double x1 = eventHandler->lastX1(); 
  double x2 = eventHandler->lastX2(); 
  
  // Get the PDF's.
  ThePEG::pair<ThePEG::PDF,ThePEG::PDF> pdfs;
  pdfs.first  =  eventHandler->pdf<ThePEG::PDF>(subprocess->incoming().first );
  pdfs.second =  eventHandler->pdf<ThePEG::PDF>(subprocess->incoming().second);
  
  // Get the momentum transfer.
  ThePEG::Energy2 scale = eventHandler->lastScale(); 
  double Q = sqrt(scale/ThePEG::GeV2); 
  
  // Get PDF values.
  double pdf1 = pdfs.first.xfx(subprocess->incoming().first ->dataPtr(),
  			       scale, x1); 
  double pdf2 = pdfs.first.xfx(subprocess->incoming().second->dataPtr(),
  			       scale, x2); 
  
  // Add the info to the event record.
  HepMC::PdfInfo pdfInfo(id1, id2, x1, x2, Q, pdf1, pdf2);
  theEvent->set_pdf_info(pdfInfo);
  debug() << "Herwig++ added PDF info to HepMC."  << endmsg;

  // Read the event if requested.
  if (m_readEventInput != "") readEvent(theEvent);
  
  // Write the event if requested.
  if (m_writeEventOutput != "") writeEvent(theEvent);

  // Print the event if requested.
  if (m_printEvent) printEvent(theEvent);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize method.
//=============================================================================
StatusCode HerwigppProduction::finalize( )
{
  // Print summary info.
  assert(m_herwigpp);
  std::cout << "==============================================================================\n";
  std::cout << "Herwig++ Event Summary\n";
  std::cout << "==============================================================================\n";
  m_herwigpp->currentEventHandler()->statistics(std::cout);

  // Finalize the generator.
  m_herwigpp->finalize();

  // Clean up ThePEG.
  ThePEG::Repository::cleanup();

  // Delete the generator object.
  delete m_herwigpp;

  // Delete the HepMC object,
  if (m_hepmcOut) delete m_hepmcOut;

  return GaudiTool::finalize( );
}

//=============================================================================
// Set a particle as stable.
//=============================================================================
void HerwigppProduction::setStable(const LHCb::ParticleProperty *thePP) {

  // Grab the particle data.
  ThePEG::PDPtr pd = m_herwigpp->getParticleData(thePP->pid().pid());

  // If Herwig++ particle exists form the command string and execute.
  if (pd) {
    std::string cmd = "set /Herwig/Particles/" + pd->PDGName()
      + ":Stable Stable";
    debug() << cmd << endmsg;
    ThePEG::Repository::exec(cmd, std::cout);
  }
}

//=============================================================================
// Update the properties for a particle.
//=============================================================================
void HerwigppProduction::updateParticleProperties(const LHCb::ParticleProperty
						  *thePP) {
  // Grab the particle data from the Herwig++ repository.
  ThePEG::PDPtr pd = m_herwigpp->getParticleData(thePP->pid().pid());
  
  if (pd) {
    // Grab particle name.
    std::string name = "set /Herwig/Particles/"+pd->PDGName();
    
    // Convert relevant quantities.
    double width, lifetime, mass, charge;
    if (thePP -> lifetime() == 0)
      width = 0;
    else
      width = Gaudi::Units::hbarc/(thePP->lifetime()
				   *Gaudi::Units::c_light
				   *Gaudi::Units::GeV);
    lifetime = thePP->lifetime()*Gaudi::Units::c_light;
    mass     = thePP->mass()/Gaudi::Units::GeV;
    charge   = thePP->charge()*3;
    
    // Create command vector.
    std::vector<std::string> cmds;
    cmds.push_back(name+":NominalMass " + numToString(mass));
    cmds.push_back(name+":Charge " + numToString(charge));
    cmds.push_back(name+":LifeTime " + numToString(lifetime));
    cmds.push_back(name+":Width " + numToString(width));
    cmds.push_back(name+":WidthUpCut " + numToString(thePP->maxWidth()));
    
    // Run commands on the Herwig++ repository.
    for (std::vector<std::string>::iterator cmd = cmds.begin();
	 cmd != cmds.end(); cmd++) {
      debug() << *cmd << endmsg;
      ThePEG::Repository::exec(*cmd, std::cout);
    }
  }
}

//=============================================================================
// Turn on and off the cluster hadronization model used by Herwig++.
//=============================================================================
void HerwigppProduction::turnOnFragmentation( ) {
  std::string cmd = "set /Herwig/EventHandlers/LHCHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler";
  debug() << cmd << endmsg;
  ThePEG::Repository::exec(cmd, std::cout);
}
void HerwigppProduction::turnOffFragmentation( ) {
  std::string cmd = "set /Herwig/EventHandlers/LHCHandler:HadronizationHandler NULL";
  debug() << cmd << endmsg;
  ThePEG::Repository::exec(cmd, std::cout);
}

//=============================================================================
// Dummy functions.
//=============================================================================
void HerwigppProduction::savePartonEvent(HepMC::GenEvent */* theEvent */) {}

void HerwigppProduction::retrievePartonEvent(HepMC::GenEvent * 
					      /* theEvent */) {}

StatusCode HerwigppProduction::hadronize(HepMC::GenEvent */*theEvent*/, 
					 LHCb::GenCollision
					 */*theCollision*/) {
  return StatusCode::SUCCESS;
}

void HerwigppProduction::printRunningConditions() {}

bool HerwigppProduction::isSpecialParticle(const LHCb::ParticleProperty * 
					   /*thePP*/) const { 
  return false;
}

StatusCode HerwigppProduction::setupForcedFragmentation(const int
							/*thePdgId*/ ) {
  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert double to string.
//=============================================================================
std::string HerwigppProduction::numToString( double num ) {
  std::ostringstream stream;
  stream << num;
  return stream.str();
}

//=============================================================================
// Replace special characters.
//=============================================================================
std::string HerwigppProduction::replaceSpecialCharacters( std::string str ) {
  size_t found;

  // Replace the \s special character.
  found = str.find("\\s");
  while (found != std::string::npos) {
    str.replace(found-1,3,";");
    found = str.find("\\s");
  }

  // Replace the \t special character.
  found = str.find("\\t");
  while (found != std::string::npos) {
    str.replace(found,2," ");
    found = str.find("\\t");
  }
  
  return str;
} 

// ============================================================================
/// The END 
// ============================================================================
