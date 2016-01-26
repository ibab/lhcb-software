// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// Generators.
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"

// LbGenXicc.
#include "LHAupGenXicc.h"
#include "GenXiccProduction.h"
#include "LbGenXicc/GenXicc.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: BcVegPyProduction
//
//  2011-04-10 : F. Zhang, G. Graziani, Philip Ilten
//-----------------------------------------------------------------------------

// Declaration of the tool factory.
DECLARE_TOOL_FACTORY(GenXiccProduction)

//=============================================================================
// Default constructor.
//=============================================================================
GenXiccProduction::GenXiccProduction(const std::string &type,
				     const std::string &name,
				     const IInterface *parent)
   : HardProduction (type, name, parent) {

  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("BaryonState", m_baryon= "Xi_cc+",
		  "The baryon to be produced");

  // Create the default settings.
  m_defaultSettings.push_back("mixevnt imix 0");
  m_defaultSettings.push_back("mixevnt imixtype 1");
  // The Xi state.
  m_defaultSettings.push_back("counter ixiccstate 1");
  // The max sampling weight: reducing it improves efficiency but
  // distorts the phase space. The typical max value obtained by vegas
  // is ~1.e7.
  m_defaultSettings.push_back("counter xmaxwgt 1000000."); 
  m_defaultSettings.push_back("confine ptcut 0.0");
  m_defaultSettings.push_back("confine etacut 1000000000.0");
  m_defaultSettings.push_back("funtrans npdfu 2");
  // Switch on/off to get the event number.
  m_defaultSettings.push_back("loggrade ievntdis 0");
  // Generate full events used when IDWTUP = 1.
  m_defaultSettings.push_back("loggrade igenerate 0"); 
  m_defaultSettings.push_back("loggrade ivegasopen 0");
  m_defaultSettings.push_back("loggrade igrade 1");
  // Generate on the GRID.
  m_defaultSettings.push_back("loggrade iusecurdir 0");
  m_defaultSettings.push_back("subopen subenergy 100.0");
  m_defaultSettings.push_back("subopen isubonly 0");
  m_defaultSettings.push_back("subopen ichange 0");
  
  // Specify IDWTUP for weighting.
  m_defaultSettings.push_back("usertran idpp 3");
  m_defaultSettings.push_back("vegasinf number 10000");
  m_defaultSettings.push_back("vegasinf nitmx 2");
  m_defaultSettings.push_back("vegcross iveggrade 0"); 
  m_defaultSettings.push_back("outpdf ioutpdf 1");
  // PDFs used from the set (CTEQ6HQ: 100, GRV98L: 200, MRST2001L: 300).
  m_defaultSettings.push_back("outpdf ipdfnum 300");   
  m_defaultSettings.push_back("vegasbin nvbin 100");   
  m_defaultSettings.push_back("valmatrix cmfactor 1.0");
}




//=============================================================================
// Initialize the hard process tool.
//=============================================================================
StatusCode GenXiccProduction::hardInitialize() {

  // Retrieve the Pythia production tool.
  if (!m_pythia) m_pythia = dynamic_cast<PythiaProduction*>
		   (tool<IProductionTool>("PythiaProduction", this));
  if (!m_pythia) return Error("Failed to retrieve PythiaProduction tool.");
  m_hard = m_pythia;
  m_pythia->m_beamToolName = m_beamToolName;

  // Set Pythia UPEVNT.
  m_pythia->m_userProcess = 7;
  m_pythia->m_frame       = "USER";
  m_pythia->m_beam        = "p+";
  m_pythia->m_target      = "p+";

  // Set Pythia 8 LHAup and UserHooks (no UserHooks needed).
  m_lhaup = new Pythia8::LHAupGenXicc(this);

  // Initialize the GenXicc settings.
  GenXicc::SetXiccDefaultParameters();
  QQqState baryon = m_mapBaryon[m_baryon];
  if (baryon == Xiccpp) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 1");
    m_defaultSettings.push_back("wbstate nbound 1");
  } else if (baryon == Xibcp) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 2");
    m_defaultSettings.push_back("wbstate nbound 1");
    m_defaultSettings.push_back("mixevnt imix 1");
    m_defaultSettings.push_back("vegasbin nvbin 300");
  } else if (baryon == Xibc0) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 2");
    m_defaultSettings.push_back("wbstate nbound 2");
  } else if (baryon == Xibb0) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 3");
    m_defaultSettings.push_back("wbstate nbound 1");
  } else if (baryon == Xibbm) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 3");
    m_defaultSettings.push_back("wbstate nbound 2");
  } else if (baryon == Omegaccp) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 1");
    m_defaultSettings.push_back("wbstate nbound 3");
  } else if (baryon == Omegabc0) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 2");
    m_defaultSettings.push_back("wbstate nbound 3");
  } else if (baryon == Omegabbm) {
    m_defaultSettings.push_back("mtypeofxi mgenxi 3");
    m_defaultSettings.push_back("wbstate nbound 3");
  } else {
    m_defaultSettings.push_back("mtypeofxi mgenxi 1");
    m_defaultSettings.push_back("wbstate nbound 2");
  }

  m_defaultSettings.push_back("subopen iconsbarnum 1");

  StatusCode sc = parseSettings(m_defaultSettings , true);
  if (sc.isFailure()) return Error("Failed to parse default settings.");
  sc = parseSettings(m_userSettings);
  if (sc.isFailure()) return Error("Failed to parse user settings.");

  // Set the energy and initialize.
  if (!m_beamTool) return Error("Beam tool not initialized.");
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool->getMeanBeams(pBeam1, pBeam2);
  double e1(sqrt(pBeam1.Mag2())), e2(sqrt(pBeam2.Mag2())),
    ecm(sqrt((e1 + e2)*(e1 + e2) - (pBeam1 + pBeam2).Mag2())/Gaudi::Units::GeV);
  GenXicc::upcom().ecm() = ecm;
  verbose() << " calling GenXicc::SetXiccConsistentParameters"<<endmsg;
  GenXicc::SetXiccConsistentParameters();
  verbose() << " calling GenXicc::EvntInit"<<endmsg;
  GenXicc::EvntInit();
  return sc;
}

//=============================================================================
// Parse GenXicc settings.
//=============================================================================
StatusCode GenXiccProduction::parseSettings(const CommandVector &settings, 
					    bool change) {
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
      if      ("imix"     == entry) GenXicc::mixevnt().imix()     = int1;
      else if ("imixtype" == entry) GenXicc::mixevnt().imixtype() = int1;
      else return Error("Unknown mixevnt entry: " + entry);
    else if ( "counter" == block)
      if      ("ixiccstate" == entry) GenXicc::counter().ixiccstate() = int1;
      else if ("xmaxwgt"    == entry) GenXicc::counter().xmaxwgt()    = fl1;
      else return Error("Unknown counter entry: " + entry);
    else if ("upcom" == block)
      if      ("pmb"    == entry) GenXicc::upcom().pmb()    = fl1;
      else if ("pmc"    == entry) GenXicc::upcom().pmc()    = fl1;
      else if ("pmxicc" == entry) GenXicc::upcom().pmxicc() = fl1;
      else if ("ecm" == entry ) {
        if (change) GenXicc::upcom().ecm() = fl1;
        else warning() << "The ecm is set from the BeamTool." <<endmsg;
      }
      else return Error("Unknown upcom entry: " + entry);
    else if ("confine" == block)
      if      ("ptcut"    == entry) GenXicc::confine().ptcut()    = fl1;
      else if ("etacut"   == entry) GenXicc::confine().etacut()   = fl1;
      else if ("pscutmin" == entry) GenXicc::confine().pscutmin() = fl1;
      else if ("pscutmax" == entry) GenXicc::confine().pscutmax() = fl1;
      else return Error("Unknown confine entry: " + entry);
    else if ("funtrans" == block)
      if      ("npdfu" == entry) GenXicc::funtrans().npdfu() =int1;
      else return Error("Unknown funtrans entry: " + entry);
    else if ("loggrade" == block)
      if      ("ievntdis"   == entry) GenXicc::loggrade().ievntdis()   = int1;
      else if ("igenerate"  == entry) GenXicc::loggrade().igenerate()  = int1;
      else if ("ivegasopen" == entry) GenXicc::loggrade().ivegasopen() = int1;
      else if ("igrade"     == entry) GenXicc::loggrade().igrade()     = int1;
      else if ("iusecurdir" == entry) GenXicc::loggrade().iusecurdir() = int1;
      else return Error("Unknown loggrade entry: " + entry);
    else if ("subopen" == block)
      if      ("subenergy" == entry) GenXicc::subopen().subenergy() = fl1;
      else if ("isubonly"  == entry) GenXicc::subopen().isubonly()  = int1; 
      else if ("ichange"   == entry) GenXicc::subopen().ichange()   = int1; 
      else if ("iconsbarnum" == entry) GenXicc::subopen().iconsbarnum() = int1;
      else return Error("Unknown subopen entry: " + entry);
    else if ("usertran" == block)
      if       ("idpp" == entry) GenXicc::usertran().idpp() = int1; 
      else return Error("Unknown usertran entry: " + entry);
    else if ("vegasinf" == block)
      if      ("number"== entry) GenXicc::vegasinf().number() = int1;
      else if ("nitmx" == entry) GenXicc::vegasinf().nitmx()  = int1;
      else return Error("Unknown vagasinf entry: " + entry);
    else if ("vegcross" == block)
      if ("iveggrade" == entry) GenXicc::vegcross().iveggrade() = int1;
      else return Error("Unknown vegcross entry: " + entry);
    else if ("outpdf" == block)
      if      ("ioutpdf" == entry) GenXicc::outpdf().ioutpdf() = int1;
      else if ("ipdfnum" == entry) GenXicc::outpdf().ipdfnum() = int1; 
      else return Error("Unknown outpdf entry: " + entry);
    else if ("vegasbin" == block)
      if  ("nvbin" == entry) GenXicc::vegasbin().nvbin() = int1; 
      else return Error("Unknown vegasbin entry: " + entry);
    else if ("valmatrix" ==block)
      if    ("cmfactor" == entry) GenXicc::valmatrix().cmfactor() = fl1; 
      else return Error("Unknown valmatrix entry: " + entry);
    else if ("mtypeofxi" == block) {
      if (change) {
        if ("mgenxi" == entry) GenXicc::mtypeofxi().mgenxi()=int1; 
        else return Error("Unknown mtypeofxi entry: " + entry);
      } else warning() << "Use the BaryonState option to set mgenxi." <<endmsg;
    }
    else if ("wbstate" == block) {
      if (change) {
        if ("nbound" == entry) GenXicc::wbstate().nbound() = int1;
        else return Error("Unknown wbstate entry: " + entry);
      }
      else warning() << "Use the BaryonState option to set nbound." <<endmsg;
    }
    else return Error("Unknown block: " + block);
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================
// The END.
//=============================================================================
