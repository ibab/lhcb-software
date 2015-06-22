// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// Generators.
#include "Generators/IBeamTool.h"

// LbPowheg.
#include "PowhegProduction.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: PowhegProduction
//
//  2013-06-18 : Bartek Rachwal, Marcin Kucharczyk, Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of the tool factory.
DECLARE_TOOL_FACTORY(PowhegProduction)

//=============================================================================
// Default constructor.
//=============================================================================
PowhegProduction::PowhegProduction(const std::string &type,
				   const std::string &name,
				   const IInterface *parent)
: HardProduction (type, name, parent) {

  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("Process", m_proc = "hvq", "The POWHEG process to use.");

  // Initialize the available processes.
  m_procs["hvq"] = 2;

  // Create the default settings.
  m_defaultSettings.push_back("ih1 1");   
  m_defaultSettings.push_back("ih2 1");     
  m_defaultSettings.push_back("lhans1 10800");    
  m_defaultSettings.push_back("lhans2 10800");
  m_defaultSettings.push_back("qmass 4.75");
  m_defaultSettings.push_back("facscfact 1");   
  m_defaultSettings.push_back("renscfact 1");   
  m_defaultSettings.push_back("use-old-grid 1");   
  m_defaultSettings.push_back("use-old-ubound 1");
  m_defaultSettings.push_back("ncall1 10000");
  m_defaultSettings.push_back("itmx1 5");
  m_defaultSettings.push_back("ncall2 100000");
  m_defaultSettings.push_back("itmx2 5"); 
  m_defaultSettings.push_back("foldcsi 2");       
  m_defaultSettings.push_back("foldy 5");    
  m_defaultSettings.push_back("foldphi 1");
  m_defaultSettings.push_back("nubound 500000");   
  m_defaultSettings.push_back("iymax 1");
  m_defaultSettings.push_back("ixmax 1");      
  m_defaultSettings.push_back("xupbound 2");       
}

//=============================================================================
// Initialize the hard process tool.
//=============================================================================
StatusCode PowhegProduction::hardInitialize() {

  // Retrieve the Pythia 8 production tool.
  if (!m_pythia8) m_pythia8 = dynamic_cast<Pythia8Production*>
		    (tool<IProductionTool>("Pythia8Production", this));
  if (!m_pythia8) return Error("Failed to retrieve Pythia8Production tool.");
  m_hard = m_pythia8;
  m_pythia8->m_beamToolName = m_beamToolName;

  // Set Pythia 8 LHAup and UserHooks.
  Pythia8::Pythia *pythia = m_pythia8->m_pythia;
  m_powheg = new Pythia8::PowhegProcs(pythia, "hvq");
  m_lhaup  = m_powheg->lhaup;
  m_hooks  = &m_powheg->hooks;
  
  // Configure Pythia 8.
  if (m_procs.find(m_proc) == m_procs.end())
    return Error("POWHEG process " + m_proc + "is not known.");
  pythia->settings.addMode("POWHEG:nFinal", m_procs[m_proc], true, false, 1, 0);
  pythia->settings.addMode("POWHEG:veto",      1, true, true,  0, 2);
  pythia->settings.addMode("POWHEG:vetoCount", 3, true, false, 0, 0);
  pythia->settings.addMode("POWHEG:pThard",    2, true, true,  0, 2);
  pythia->settings.addMode("POWHEG:pTemt",     0, true, true,  0, 2);
  pythia->settings.addMode("POWHEG:emitted",   0, true, true,  0, 3);
  pythia->settings.addMode("POWHEG:pTdef",     1, true, true,  0, 2);
  pythia->settings.addMode("POWHEG:MPIveto",   0, true, true,  0, 1);
  pythia->settings.addMode("POWHEG:QEDveto",   2, true, true,  0, 1);
  
  // Set the energy.
  if (!m_beamTool) return Error("Beam tool not initialized.");
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);
  stringstream eBeam1("ebeam1 "), eBeam2("ebeam2 ");
  eBeam1 << sqrt(pBeam1.Mag2()) / Gaudi::Units::GeV;
  eBeam2 << sqrt(pBeam2.Mag2()) / Gaudi::Units::GeV;
  m_defaultSettings.push_back(eBeam1.str());
  m_defaultSettings.push_back(eBeam2.str());
  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize the hard process generator.
//=============================================================================
StatusCode PowhegProduction::hardInitializeGenerator() {
  for (unsigned int i = 0; i < m_defaultSettings.size(); ++i)
    m_powheg->readString(m_defaultSettings[i]);
  for (unsigned int i = 0; i < m_userSettings.size(); ++i)
    m_powheg->readString(m_userSettings[i]);
  return m_powheg->init();
}

//=============================================================================
// Finalize the hard process tool.
//=============================================================================
StatusCode PowhegProduction::hardFinalize() {
  if (m_powheg) delete m_powheg;
  if (m_pythia8) {m_pythia8->m_lhaup = 0; m_pythia8->m_hooks = 0;}
  return StatusCode::SUCCESS;
}

//=============================================================================
// The END.
//=============================================================================
