// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event.
#include "Event/GenCollision.h"

// Generators.
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "LbPythia/Pythia.h"
#include "LbBcVegPy/BcVegPy.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

// Local.
#include "BcVegPyProduction.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BcVegPyProduction
//
// 2014-05-08 : Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of the tool factory.
DECLARE_TOOL_FACTORY(BcVegPyProduction);

//=============================================================================
// Default constructor.
//=============================================================================
BcVegPyProduction::BcVegPyProduction(const std::string &type,
				     const std::string &name,
				     const IInterface *parent)
  : GaudiTool(type, name, parent), 
    m_pythia("PythiaProduction", "PythiaProduction", parent) {

  // Interfaces for BcVegPy.
  declareInterface<IProductionTool>(this) ;
  declareProperty("BcVegPyCommands", m_commandSettings);
  declareProperty("PrintEvent", m_printEvent = false);
  declareProperty("WriteEvent", m_writeEventOutput = "");
  declareProperty("ReadEvent", m_readEventInput = "");
  
  // Interfaces for Pythia.
  declareProperty("Commands", m_showerSettings);
  declareProperty("PygiveCommands", m_pygive);
  declareProperty("PDTList", m_pdtlist);
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams");
  declareProperty("WidthLimit", m_widthLimit = 1.5e-6 * Gaudi::Units::GeV);
  declareProperty("SLHADecayFile", m_slhaDecayFile = "empty");
  declareProperty("PDecayList", m_pdecaylist) ;
  declareProperty("SLHASpectrumFile", m_slhaSpectrumFile = "empty") ;
  declareProperty("ValidateHEPEVT", m_validate_HEPEVT ,
		  "The flag to force the validation of HEPEVT.");
  declareProperty("Inconsistencies", m_inconsistencies , 
		  "The file to dump HEPEVT inconsinstencies.");
  declareProperty("UpdatedParticles", m_updatedParticles);
  declareProperty("ParticlesToAdd", m_particlesToAdd);

  // Print the production engine.
  always() << "============================================================="
           << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
           << endmsg;

  // Create default BcVegPy settings.
  m_defaultSettings.push_back("mixevnt imix 0");
  m_defaultSettings.push_back("mixevnt imixtype 1");
  m_defaultSettings.push_back("counter ibcstate 1"); // B_c state.
  m_defaultSettings.push_back("upcom pmb 4.95");     // Mass of b-quark.
  m_defaultSettings.push_back("upcom pmc 1.326");    // Mass of c-quark.
  m_defaultSettings.push_back("upcom pmbc 6.276");   // Mass of B_c = pmB + pmC.
  m_defaultSettings.push_back("upcom ecm 14000.0");  // E.C.M. of LHC.
  m_defaultSettings.push_back("confine ptcut 0.0");
  m_defaultSettings.push_back("confine etacut 1000000000.0");
  m_defaultSettings.push_back("funtrans nq2 3"); 
  m_defaultSettings.push_back("funtrans npdfu 2");
  m_defaultSettings.push_back("loggrade ievntdis 0");
  m_defaultSettings.push_back("loggrade igenerate 0");
  m_defaultSettings.push_back("loggrade ivegasopen 0");
  m_defaultSettings.push_back("loggrade igrade 1");
  m_defaultSettings.push_back("loggrade iusecurdir 0");
  m_defaultSettings.push_back("subopen subenergy 100.0");
  m_defaultSettings.push_back("subopen isubonly 0");
  m_defaultSettings.push_back("usertran ishower 1");
  m_defaultSettings.push_back("usertran idpp 3");
  m_defaultSettings.push_back("vegasinf number 1000000");
  m_defaultSettings.push_back("vegasinf nitmx 20");
  m_defaultSettings.push_back("vegcross iveggrade 0"); 
  m_defaultSettings.push_back("qqbar iqqbar 0");
  m_defaultSettings.push_back("qqbar iqcode 2");
  m_defaultSettings.push_back("outpdf ioutpdf 0");     
  m_defaultSettings.push_back("outpdf ipdfnum 300");   
  m_defaultSettings.push_back("coloct ioctet 0");
  m_defaultSettings.push_back("octmatrix coeoct 0.1");
}

//=============================================================================
// Default destructor. 
//=============================================================================
BcVegPyProduction::~BcVegPyProduction( ) { }

//=============================================================================
// Initialize the class.
//=============================================================================
StatusCode BcVegPyProduction::initialize( ) {

  // Set Pythia variables.
  m_pythia.m_commandVector    = m_showerSettings;
  m_pythia.m_pygive           = m_pygive;	       
  m_pythia.m_pdtlist	      = m_pdtlist;	       
  m_pythia.m_beamToolName     = m_beamToolName;    
  m_pythia.m_widthLimit       = m_widthLimit;      
  m_pythia.m_slhaDecayFile    = m_slhaDecayFile;   
  m_pythia.m_pdecaylist       = m_pdecaylist;      
  m_pythia.m_slhaSpectrumFile = m_slhaSpectrumFile;
  m_pythia.m_validate_HEPEVT  = m_validate_HEPEVT; 
  m_pythia.m_inconsistencies  = m_inconsistencies; 
  m_pythia.m_updatedParticles = m_updatedParticles;
  m_pythia.m_particlesToAdd   = m_particlesToAdd;   
  m_pythia.m_userProcess = 2;
  m_pythia.m_frame       = "USER";
  m_pythia.m_beam        = "p+";
  m_pythia.m_target      = "p+";

  // Read the default and user settings.
  StatusCode sc;
  sc = parseBcVegPyCommands(m_defaultSettings);
  if (sc.isFailure()) return Error("Unable to read default commands.", sc);
  sc = parseBcVegPyCommands(m_commandSettings);
  if (sc.isFailure()) return Error("Unable to read user commands.", sc);
  BcVegPy::SetParameter();

  // Initialize the showering generators.
  sc = m_pythia.initialize();
  if (sc.isFailure()) return sc;

  // Set the HepMC output file if requested.
  if (m_writeEventOutput != "")
    m_hepmcOut = new HepMC::IO_GenEvent(m_writeEventOutput.c_str(),
					std::ios::out);
  else m_hepmcOut = 0;

  return sc ;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode BcVegPyProduction::generateEvent(HepMC::GenEvent *theEvent, 
                                            LHCb::GenCollision *theCollision) {

  StatusCode sc = m_pythia.generateEvent(theEvent, theCollision);
  if (sc.isFailure()) return sc;

  // Read the event if requested.
  if (m_readEventInput != "") readEvent(theEvent);
  
  // Write the event if requested.
  if (m_writeEventOutput != "") writeEvent(theEvent);

  // Print the event if requested.
  if (m_printEvent) printEvent(theEvent);

  return StatusCode::SUCCESS ;
}

//=============================================================================
// All these methods just call Pythia.
//=============================================================================
void BcVegPyProduction::setStable(const LHCb::ParticleProperty *thePP)
{m_pythia.setStable(thePP);}
void BcVegPyProduction::updateParticleProperties(const LHCb::ParticleProperty 
						 *thePP) 
{m_pythia.updateParticleProperties(thePP);}
void BcVegPyProduction::turnOnFragmentation()
{m_pythia.turnOnFragmentation();}
void BcVegPyProduction::turnOffFragmentation()
{m_pythia.turnOffFragmentation();}
StatusCode BcVegPyProduction::hadronize(HepMC::GenEvent *theEvent, 
					LHCb::GenCollision *theCollision)
{return m_pythia.hadronize(theEvent, theCollision);}
void BcVegPyProduction::savePartonEvent(HepMC::GenEvent *theEvent)
{m_pythia.savePartonEvent(theEvent);}
void BcVegPyProduction::retrievePartonEvent(HepMC::GenEvent *theEvent)
{m_pythia.retrievePartonEvent(theEvent);}
void BcVegPyProduction::printRunningConditions()
{m_pythia.printRunningConditions();}
bool BcVegPyProduction::isSpecialParticle(const LHCb::ParticleProperty *thePP)
  const {return m_pythia.isSpecialParticle(thePP);}
StatusCode BcVegPyProduction::setupForcedFragmentation(const int thePdgId)
{return m_pythia.setupForcedFragmentation(thePdgId);}


//=============================================================================
// Parse BcVegPy commands stored in a vector
//=============================================================================
StatusCode BcVegPyProduction::parseBcVegPyCommands( const CommandVector & 
                                                  theCommandVector ) {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ; 
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(3);
    int    int1  = mystring.intpiece(3);
    double fl1   = mystring.numpiece(3);

    
    // Note that BcVegPy needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;

    if ( "mixevnt" == block)
      if      ( "imix"    == entry )BcVegPy::mixevnt().imix()      = int1 ;
      else if ( "imixtype"== entry ) BcVegPy::mixevnt().imixtype() = int1 ;
      else return Error(std::string("BcVegPy error, mixevnt"));

    else if ( "counter" == block)
      if      ( "ibcstate"    == entry ) BcVegPy::counter().ibcstate() = int1 ;
      else return Error(std::string("BcVegPy error, counter"));
			
    else if ( "upcom" == block)
      if      ( "pmb" == entry ) BcVegPy::upcom().pmb()     =fl1;
      else if ( "pmc" == entry ) BcVegPy::upcom().pmc()     =fl1;
      else if ( "pmbc"== entry ) BcVegPy::upcom().pmbc()    =fl1;
      else if ( "ecm" == entry ) BcVegPy::upcom().ecm()     =fl1;
      else return Error (std::string("BcVegPy error, upcom"));

    else if ( "confine" == block )
      if      ( "ptcut" == entry  ) BcVegPy::confine().ptcut() = fl1;
      else if ( "etacut" == entry ) BcVegPy::confine().etacut()= fl1;
      else return Error (std::string("BcVegPy error, confine"));
    
    else if ( "funtrans"== block )
      if      ("nq2"  ==entry)BcVegPy::funtrans().nq2()           =int1;
      else if ("npdfu"==entry)BcVegPy::funtrans().npdfu()         =int1;
      else return Error (std::string("BcVegPy error, funtrans"));
    
    else if ( "loggrade"==block )
      if      ("ievntdis"  ==entry ) BcVegPy::loggrade().ievntdis()   =int1;
      else if ("igenerate" ==entry ) BcVegPy::loggrade().igenerate()  =int1;
      else if ("ivegasopen"==entry ) BcVegPy::loggrade().ivegasopen() =int1;
      else if ("igrade"    ==entry ) BcVegPy::loggrade().igrade()     =int1;
      else if ("iusecurdir"==entry ) BcVegPy::loggrade().iusecurdir() =int1;
      else return Error (std::string("BcVegPy error, loggrade"));
    
    else if ( "subopen"==block )
      if       ( "subenergy"==entry)BcVegPy::subopen().subenergy()   =fl1;
      else if  ( "isubonly" ==entry)BcVegPy::subopen().isubonly()    =int1; 
      else return Error (std::string("BcVegPy error, subopen"));
    
    else if ( "usertran"==block )
      if       ( "ishower"==entry)BcVegPy::usertran().ishower()=int1;
      else if  ( "idpp"   ==entry)BcVegPy::usertran().idpp()=int1; 
      else return Error (std::string("BcVegPy error, usertran"));
    
    else if ( "vegasinf"==block )
      if      ( "number"==entry )BcVegPy::vegasinf().number()         =int1;
      else if ( "nitmx" ==entry )BcVegPy::vegasinf().nitmx()          =int1;
      else return Error (std::string("BcVegPy error, vagasinf"));
    
    else if ( "vegcross"==block )
      if ("iveggrade"==entry)BcVegPy::vegcross().iveggrade()          =int1;
      else return Error (std::string("BcVegPy error, vegcross"));
    
    else if ( "qqbar"==block )
      if       ( "iqqbar"==entry)BcVegPy::qqbar().iqqbar()     =int1;
      else if  ( "iqcode"==entry)BcVegPy::qqbar().iqcode()     =int1; 
      else return Error (std::string("BcVegPy error, qqbar"));
    
    else if ( "outpdf"==block )
      if       ( "ioutpdf"==entry)BcVegPy::outpdf().ioutpdf()  =int1;
      else if  ( "ipdfnum"==entry)BcVegPy::outpdf().ipdfnum()  =int1; 
      else return Error (std::string("BcVegPy error, outpdf"));
    
    else if ( "coloct" == block)
      if      ( "ioctet"    == entry ) BcVegPy::coloct().ioctet()    = int1 ;
      else return Error(std::string("BcVegPy error, coloct"));
    
    else if ( "octmatrix" == block)
      if      ( "coeoct"    == entry ) BcVegPy::octmatrix().coeoct() = fl1 ;
      else return Error(std::string("BcVegPy error, octmatrix"));
    
    else return Error (std::string("BcVegPy error in parse parameters"));
  }
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Write the HEPMC container (for debugging purposes).
//=============================================================================
StatusCode BcVegPyProduction::writeEvent(HepMC::GenEvent* theEvent) {
  m_hepmcOut->write_event(theEvent);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Print the HEPMC container (for debugging purposes).
//=============================================================================
StatusCode BcVegPyProduction::printEvent(HepMC::GenEvent* theEvent) {
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
StatusCode BcVegPyProduction::readEvent(HepMC::GenEvent* theEvent) {
  std::ifstream file(m_readEventInput.c_str());
  HepMC::IO_GenEvent hepmcio(file);
  hepmcio.fill_next_event(theEvent);
  return StatusCode::SUCCESS;
}
