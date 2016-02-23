// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/ParticleProperty.h"

// Generators.
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"

// LbBcVegPy.
#include "LHAupBcVegPy.h"
#include "BcVegPyProduction.h"
#include "LbBcVegPy/BcVegPy.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: BcVegPyProduction
//
//  2006-03-02 : Jibo He, Zhenwei Yang, Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of the tool factory.
DECLARE_TOOL_FACTORY(BcVegPyProduction)

//=============================================================================
// Default constructor.
//=============================================================================
BcVegPyProduction::BcVegPyProduction(const std::string &type,
				     const std::string &name,
				     const IInterface *parent)
  : HardProduction (type, name, parent) {

  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("MesonState", m_meson = 541,
		  "The meson to be produced (PDG ID). Only used if "
		  "ViolateGaugeInvariance is set to true.");
  declareProperty("ViolateGaugeInvariance", m_violate = false,
		  "WARNING: if changed to true the validity of the physics "
		  "results cannot be guaranteed. This sets the mass "
		  "parameters via the ParticlePropertyService and PMBC from "
		  "the MesonState.");
  
  // Create the default settings.
  m_meson = abs(m_meson);
  m_defaultSettings.push_back("mixevnt imix 1");
  m_defaultSettings.push_back("mixevnt imixtype 1");
  m_defaultSettings.push_back("counter ibcstate 1");
  m_defaultSettings.push_back("upcom pmb 4.95");      // Mass of the b-quark. 
  m_defaultSettings.push_back("upcom pmc 1.326");     // Mass of the c-quark.  
  // Mass of B_c, note that pmBc = pmB + pCm. 
  m_defaultSettings.push_back("upcom pmbc 6.276");
  m_defaultSettings.push_back("confine ptcut 0.0");
  m_defaultSettings.push_back("confine etacut 1000000000.0");
  m_defaultSettings.push_back("funtrans nq2 3"); 
  m_defaultSettings.push_back("funtrans npdfu 2");
  m_defaultSettings.push_back("loggrade ievntdis 0");
  m_defaultSettings.push_back("loggrade igenerate 1");
  m_defaultSettings.push_back("loggrade ivegasopen 0");
  m_defaultSettings.push_back("loggrade igrade 1");
  m_defaultSettings.push_back("loggrade iusecurdir 0");
  m_defaultSettings.push_back("subopen subenergy 100.0");
  m_defaultSettings.push_back("subopen isubonly 0");
  m_defaultSettings.push_back("usertran ishower 1");
  m_defaultSettings.push_back("usertran idpp 1");
  m_defaultSettings.push_back("vegasinf number 1000000");
  m_defaultSettings.push_back("vegasinf nitmx 20");
  // Should be set to 0.
  m_defaultSettings.push_back("vegcross iveggrade 0"); 
  m_defaultSettings.push_back("qqbar iqqbar 0");
  m_defaultSettings.push_back("qqbar iqcode 2");
  m_defaultSettings.push_back("outpdf ioutpdf 0");     
  // Should be set to 300 for this version.
  m_defaultSettings.push_back("outpdf ipdfnum 300");   
  m_defaultSettings.push_back("coloct ioctet 0");
  m_defaultSettings.push_back("octmatrix coeoct 0.1");
}

//=============================================================================
// Initialize the hard process tool.
//=============================================================================
StatusCode BcVegPyProduction::hardInitialize() {

  // Retrieve the Pythia production tool.
  if (!m_pythia) m_pythia = dynamic_cast<PythiaProduction*>
		   (tool<IProductionTool>("PythiaProduction", this));
  if (!m_pythia) return Error("Failed to retrieve PythiaProduction tool.");
  m_hard = m_pythia;
  m_pythia->m_beamToolName = m_beamToolName;

  // Set Pythia UPEVNT.
  m_pythia->m_userProcess = 2;
  m_pythia->m_frame       = "USER";
  m_pythia->m_beam        = "p+";
  m_pythia->m_target      = "p+";

  // Set Pythia 8 LHAup and UserHooks (no UserHooks needed).
  m_lhaup = new Pythia8::LHAupBcVegPy(this, m_violate ? m_meson : 541);
  
  // Read the BcVegPy settings.
  StatusCode sc = parseSettings(m_defaultSettings, false);
  if (sc.isFailure()) return Error("Failed to parse default settings.");
  sc = parseSettings(m_userSettings);
  if (sc.isFailure()) return Error("Failed to parse settings.");

  // Set the energy.
  if (!m_beamTool) return Error("Beam tool not initialized.");
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);
  double e1(sqrt(pBeam1.Mag2())), e2(sqrt(pBeam2.Mag2())),
    ecm(sqrt((e1 + e2)*(e1 + e2) - (pBeam1 + pBeam2).Mag2())/Gaudi::Units::GeV);
  BcVegPy::upcom().ecm() = ecm;
  BcVegPy::SetParameter();
  return sc;
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode BcVegPyProduction::generateEvent(HepMC::GenEvent *theEvent, 
					    LHCb::GenCollision *theCollision) {
  if (!m_shower) return StatusCode::FAILURE;
  ++m_nEvents;
  StatusCode sc = m_shower->generateEvent(theEvent, theCollision);
  if (m_violate && m_meson != 541 && m_shower == m_pythia && sc.isSuccess()) {
    for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
	 p != theEvent->particles_end(); ++p) {
      if (abs((*p)->pdg_id()) == 541) {
	(*p)->set_pdg_id(((*p)->pdg_id() > 0 ? 1 : -1) * m_meson);
	return sc;
      }
    }
  }
  return sc;
}

//=============================================================================
// Update Pythia particle and the meson, c-quark, or b-quark mass.
//=============================================================================
void BcVegPyProduction::hardUpdateParticleProperties
(const LHCb::ParticleProperty* thePP) {

  // Set for Pythia.
  if (!m_pythia->isSpecialParticle(thePP))
    m_pythia->updateParticleProperties(thePP);

  // Set the mass of the meson, c-quark, or b-quark if gauge violation allowed.
  if (m_violate) {
    int absid = abs(thePP->pid().pid());
    double m0(thePP->mass() / Gaudi::Units::GeV);
    if (absid == m_meson) BcVegPy::upcom().pmbc() = m0;
    else if (absid == 4)  BcVegPy::upcom().pmc()  = m0;
    else if (absid == 5)  BcVegPy::upcom().pmb()  = m0;
  }
}

//=============================================================================
// Parse the BcVegPy settings.
//=============================================================================
StatusCode BcVegPyProduction::parseSettings(const CommandVector &settings,
					    bool user) {
  // Loop over the settings.
  for (unsigned int i = 0; i < settings.size(); ++i) {
    debug() << " Command is: " << settings[i] << endmsg ;
    StringParse setting(settings[i]);
    std::string block = setting.piece(1);
    std::string entry = setting.piece(2);
    std::string str   = setting.piece(3);
    int    int1       = setting.intpiece(3);
    double fl1        = setting.numpiece(3);
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;

    // Apply the settings.
    if ("mixevnt" == block)
      if      ("imix"     == entry) BcVegPy::mixevnt().imix()     = int1;
      else if ("imixtype" == entry) BcVegPy::mixevnt().imixtype() = int1;
      else return Error("Unknown mixevnt entry: " + entry);
    else if ("counter" == block)
      if      ("ibcstate" == entry) BcVegPy::counter().ibcstate() = int1 ;
      else return Error("Unknown counter entry: " + entry);
    else if ("upcom" == block)
      if (user) return Warning("All upcom variables should be set via the "
			       "particle property service or the beam tool.");
      else if ("pmb" == entry) BcVegPy::upcom().pmb()  = fl1;
      else if ("pmc" == entry) BcVegPy::upcom().pmc()  = fl1;
      else if ("pmbc"== entry) BcVegPy::upcom().pmbc() = fl1;
      else if ("ecm" == entry) BcVegPy::upcom().ecm()  = fl1;
      else return Error("Unknown upcom entry: " + entry);
    else if ("confine" == block)
      if      ("ptcut"  == entry) BcVegPy::confine().ptcut()  = fl1;
      else if ("etacut" == entry) BcVegPy::confine().etacut() = fl1;
      else return Error("Unknown confine entry: " + entry);
    else if ("funtrans" == block)
      if      ("nq2"  == entry) BcVegPy::funtrans().nq2()   = int1;
      else if ("npdfu"== entry) BcVegPy::funtrans().npdfu() = int1;
      else return Error("Unknown funtrans entry: " + entry);
    else if ("loggrade"==block)
      if      ("ievntdis"   == entry) BcVegPy::loggrade().ievntdis()   = int1;
      else if ("igenerate"  == entry) BcVegPy::loggrade().igenerate()  = int1;
      else if ("ivegasopen" == entry) BcVegPy::loggrade().ivegasopen() = int1;
      else if ("igrade"     == entry) BcVegPy::loggrade().igrade()     = int1;
      else if ("iusecurdir" == entry) BcVegPy::loggrade().iusecurdir() = int1;
      else return Error("Unknown loggrade entry: " + entry);
    else if ("subopen" == block)
      if      ("subenergy" == entry) BcVegPy::subopen().subenergy() = fl1;
      else if ("isubonly"  == entry) BcVegPy::subopen().isubonly()  = int1; 
      else return Error("Unknown subopen entry: " + entry);
    else if ("usertran" == block)
      if      ("ishower" == entry) BcVegPy::usertran().ishower() = int1;
      else if ("idpp"    == entry) BcVegPy::usertran().idpp()    = int1; 
      else return Error("Unknown usertran entry: " + entry);
    else if ("vegasinf" == block)
      if      ("number" == entry) BcVegPy::vegasinf().number() = int1;
      else if ("nitmx"  == entry) BcVegPy::vegasinf().nitmx()  = int1;
      else return Error("Unknown vagasinf entry: " + entry);
    else if ("vegcross" == block)
      if      ("iveggrade" == entry) BcVegPy::vegcross().iveggrade() = int1;
      else return Error("Unknown vegcross entry: " + entry);
    else if ("qqbar" == block)
      if      ("iqqbar" == entry) BcVegPy::qqbar().iqqbar() = int1;
      else if ("iqcode" == entry) BcVegPy::qqbar().iqcode() = int1; 
      else return Error("Unknown qqbar entry: " + entry);
    else if ("outpdf" == block)
      if      ("ioutpdf" == entry) BcVegPy::outpdf().ioutpdf() = int1;
      else if ("ipdfnum" == entry) BcVegPy::outpdf().ipdfnum() = int1; 
      else return Error("Unknown outpdf entry: " + entry);
    else if ("coloct" == block)
      if      ("ioctet" == entry) BcVegPy::coloct().ioctet() = int1 ;
      else return Error("Unknown coloct entry: " + entry);
    else if ( "octmatrix" == block)
      if      ("coeoct" == entry) BcVegPy::octmatrix().coeoct() = fl1 ;
      else return Error("Unknown octmatrix entry: " + entry);
    else return Error("Unknown block: " + block);
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================
// The END.
//=============================================================================
