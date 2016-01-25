// $Id: $

//-----------------------------------------------------------------------------
// Implementation file for class : RivetAnalysisHandler
//
// Responsible: Alex Grecu <alexandru.grecu@nipne.ro>
//-----------------------------------------------------------------------------

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/SystemOfUnits.h>
#include <GaudiKernel/IAppMgrUI.h>
#include <GaudiKernel/Bootstrap.h>
#include "GaudiKernel/Vector3DTypes.h"

// from GenEvent/Event
#include <Event/HepMCEvent.h>
#include <Event/BeamParameters.h>
#include <GenEvent/BeamForInitialization.h>

// from HepMC
#include <HepMC/GenEvent.h>
#ifdef HEPMC_HAS_UNITS
#include <HepMC/Units.h>
#endif
#ifdef HEPMC_HAS_CROSS_SECTION
#include <HepMC/GenCrossSection.h>
#endif

// disable unused parameter warning (appears in libRivet)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

// from Rivet (LCG)
#include <Rivet/Rivet.hh>
//#include <Rivet/AnalysisHandler.hh>
#include <Rivet/Analysis.hh>
#include <Rivet/Tools/RivetPaths.hh>
#include <Rivet/Tools/Utils.hh>
#include <Rivet/Math/MathUtils.hh>

// from boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

// local
#include "RivetAnalysisHandler.h"
#include "LogLevelMap.h"

using namespace std;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RivetAnalysisHandler )

// Initializing static members
const char* const RivetAnalysisHandler::_statDescriptors[] = {
	"Particles with negative rest mass (in DEBUG mode only)", 
	"Boost angle corrections", 
	"Unit conversions", 
	"ParticleID adjustments",
        "MC generator interaction XS changes"};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RivetAnalysisHandler::RivetAnalysisHandler( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name, pSvcLocator ),
    _analysisManager(0),
    _reqCrossSection(false),
    _xsectionSource(0),
    _mHxAngle(0.0),
    _mVxAngle(0.0),
    _scaleFactorEnergy(1.0),
    _scaleFactorTime(1.0)
{
  _isFirstEvent = true; _xHAngleCorrection = false; _xVAngleCorrection = false; m_xAngleDetect = true;
  _myStats.assign(COUNTERS_NB, 0); m_analysisNames.clear(); m_analysisPaths.clear();
  declareProperty("MCEventLocation", m_mcEvtLocation=LHCb::HepMCEventLocation::Default, "Location on TES where the HepMC events are read from (LHCb::HepMCEventLocation::Default)");
  declareProperty("BaseFileName", m_filename="MyRivet", "The base file name (prefix of filenames) to write results to (\"MyRivet\")");
  declareProperty("RunName", m_runname="", "The name of the run to prepended to YODA histogram paths (keep empty string as it breaks helper scripts in Rivet 2.4.0)");
  declareProperty("Analyses", m_analysisNames, "A list of names of the analyses to run ([])");
  declareProperty("AnalysisPath", m_analysisPaths, "List of additional file paths where analysis plugins should be looked for, e.g. add os.path.abspath('.') when analysis lib(*.so) is in the option file directory ([])");
  declareProperty("CorrectStatusID", m_modStatusID=false, "Switch that controls the transformation of status ID of particles (given by EvtGen) back to Pythia defaults (False)");
  declareProperty("CorrectCrossingAngles", m_xAngleDetect=true, "Instructs the algorithm to automatically detect and correct for beam crossing angles (True)");
  declareProperty("xSectionValue", m_crossSection=-1., "The externally provided cross-section for the present run (mb or mub!?); ignored when read from data (-1.)");
  declareProperty("forceXSection", m_forceCrossSection=false, "Forces GenTune to set the provided cross-section value for each event (False)");
  declareProperty("LogSuppressionSoftLimit", m_logSoftLimit=30, "Internal statistical messages print-out suppression soft limit (30)");
  declareProperty("LogSuppressionHardLimit", m_logHardLimit=200, "Internal statistical message print-out suppression hard limit (200)");
  declareProperty("LogSuppressedOutputFrequency", m_logSuppressFreq=10, "Internal statistical message print-out suppression frequency (10)");
}
//=============================================================================
// Destructor
//=============================================================================
RivetAnalysisHandler::~RivetAnalysisHandler() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RivetAnalysisHandler::initialize()
{
  unsigned int nAna = 0;
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  info() << this->name() << " initialize using Rivet v. " << Rivet::version() << endmsg;
#ifdef HEPMC_HAS_UNITS
  info() << "Units will be determined automatically from HepMC by RIVET! " \
         << "Algorithm option 'AdjustUnits' will be ignored..." << endmsg;
  _needsUnitConv = false;
#else
  _needsUnitConv = true;
#endif
  //Check and select histogram service - for future self-monitoring features - disabled for now!
  //Check histogram service is available - not implemented further yet!
  //if (!(histoSvc())) {
  //  error() << "Failed to retrieve histogram service!" << endmsg;
  //  return StatusCode::FAILURE;
  //};
  //Create Rivet::AnalysisHandler instance
  warning() << "Creating Rivet analysis handler instance..." << endmsg; 
  //_analysisManager = new Rivet::AnalysisHandler(m_runname);
  // for run name to be an empty string to avoid problems with helper scripts until problem solved
  _analysisManager = new Rivet::AnalysisHandler("");
  assert(_analysisManager);
  warning() << "Rivet analysis handler object created..." << endmsg; 
  //Set RIVET_ANALYSIS_PATH based on alg setup. Don't overwrite if already set in env
  //TODO: Schimbat pentru a lua in considerare setup-ul LHCb si a pune calea curenta daca se dezvolta un plugin adica pachetul se afla sub cmtuser!
  vector<string> all_paths = Rivet::getAnalysisLibPaths();
  if (m_analysisPaths.size() > 0)
  {
    //use maybe boost filesystem library to improve checking?!
    foreach(const string& ps, m_analysisPaths) if (ps.size() > 0) all_paths.push_back(ps);
  };
  //read the CMTPATH and add it to RIVET_ANALYSIS_PATH
  char* cmtPath = getenv("CMTPATH");
  vector<string> cmtpaths;
  if (cmtPath == NULL)
  {
    warning() << "$CMTPATH env variable not set: main analysis plugin directory may not be found..." << endmsg;
  }
  else
  {
    boost::split(cmtpaths, cmtPath, boost::is_any_of(string(":")));
    cmtPath = getenv("CMTCONFIG");
    if (cmtPath == NULL)
    {
      warning() << "$CMTCONFIG variable not set: main analysis plugin directory may not be found..." << endmsg;
    }
    else
    {
      boost::split(cmtpaths, cmtPath, boost::is_any_of(string(":")));
      const string cmtcfg = cmtPath;
      const string libPath = "/InstallArea/" + cmtcfg + "/lib";
      foreach(const string& p, cmtpaths) {
        const string cmtlibPath = p + libPath;
        debug() << "Appending " << cmtlibPath << "to default Rivet analysis search path" << std::endl;
        all_paths.push_back(cmtlibPath);
      };
    };
  };
  Rivet::setAnalysisLibPaths(all_paths);
  // read back RIVET_ANALYSIS_PATH from environment and print-out when MSG::DEBUG
  // TODO: Read other environment variables and print them is defined!
  if (msgLevel(MSG::DEBUG))
  {
    string env_rap = getenv("RIVET_ANALYSIS_PATH");
    if (env_rap.size() > 0) 
    {
      debug() << "RIVET_ANALYSIS_PATH = " << env_rap << endmsg;
    }
    else
    {
      debug() << "RIVET_ANALYSIS_PATH missing or empty!" << endmsg;
    };
  };
  // Setting log level for Rivet & analyses according to Gauss (LHCbApp) log level
  // TODO: De separat si introdus o optiune speciala pentru controlul log-level al Rivet
  MSG::Level jobMsgLvl = (MSG::Level)(*msgSvc()).outputLevel();
  always() << "Gauss log level: " << (int)jobMsgLvl << endmsg;
  Rivet::Log::setLevel("Rivet.Projection.PVertex", rivetLevel(jobMsgLvl));
  always() << "Rivet.Projection.PVertex log level: " << Rivet::Log::getLog("Rivet.Projection.PVertex").getLevel() << endmsg;
  Rivet::Log::setLevel("Rivet", rivetLevel(jobMsgLvl));
  if (msgLevel(MSG::VERBOSE)) { //list all available analysis names
    vector<string> analysisNames = Rivet::AnalysisLoader::analysisNames();
    verbose() << "Listing available Rivet analyses:" << std::endl;
    foreach (const string& a, analysisNames) verbose() << " " << a << std::endl;
    verbose() << endmsg;
  };
  // Determine if cross-section is needed
  _reqCrossSection = false;
  foreach (const string& a, m_analysisNames) {
    info() << "Loading Rivet analysis " << a << " ... ";
    Rivet::Analysis* analysis = Rivet::AnalysisLoader::getAnalysis(a);
    if ( 0 == analysis ) {
      info() << "[ \x1B[31mFAILED\x1B[0m; Skipping... ]" << endmsg;
      continue;
    } else {
      info() << "[ \x1B[32mOK\x1B[0m ]" << endmsg;
      nAna ++;
    };
    if ( analysis->needsCrossSection() ) {
      _reqCrossSection = true;
      info() << "Analysis " << a << " requires valid production cross section value." << endmsg;
    };
    _analysisManager->addAnalysis(a);
    string logName = "Rivet.Analysis." + a;
    // TODO: Sincronizat cu nivelul de logging de la Rivet odata ce s-a produs separarea
    Rivet::Log::setLevel(logName, rivetLevel(jobMsgLvl));
    delete analysis;
  }; // end analysis foreach loop
  if (nAna == 0) { // verify non-zero analysis plugins loaded
    fatal() << "No analysis plugin left to run in current Rivet job. Exiting..." << endmsg;
    return StatusCode::FAILURE;
  };
  if ( m_forceCrossSection && (m_crossSection < 0.0) ) { // fail if user tries to force invalid cross-section value
    if (_reqCrossSection) {
      fatal() << "Invalid external cross-section value was forced." << endmsg;
      return StatusCode::FAILURE;
    } else {
      warning() << "Forced cross-section value is invalid, but not needed." << endmsg;
    };
  };
  // Initialize Rivet 2.x before processing first event
  debug() << "<== Initialize done." << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RivetAnalysisHandler::execute()
{
  bool negmm = false;
  stringstream dess;
  debug() << "==> Execute";
  if (_isFirstEvent) debug() << " first event";
  debug() << endmsg;
  /// get HepMC event from TES 
  LHCb::HepMCEvent::Container* mcEvents 
    = get<LHCb::HepMCEvent::Container>( m_mcEvtLocation );
  if ( NULL == mcEvents ) {
    fatal() << "Cannot retrieve HepMC events from '" << m_mcEvtLocation << "' on TES." << endmsg;
    return StatusCode::FAILURE;
  };
  if ( mcEvents->size() == 0 ) {
    warning() << "No HepMC events in TES container. Skipping execute sequence..." << endmsg;
    return StatusCode::SUCCESS;
  };
  /// take only the first event and thus ignore pile-up ?!
  if ( mcEvents->size() > 1 ) warning() << "Multiple {" << mcEvents->size() << "} HepMC events in container. Pile-up should be avoided. Taking first HepMC event only..." << endmsg;
  LHCb::HepMCEvent::Container::iterator ievent = mcEvents->begin();
  LHCb::HepMCEvent* lhcbEvent = (*ievent);
  if ( NULL == lhcbEvent )
  {
    warning() << "Skipping NULL primary HepMC event and execute sequence..." << endmsg;
    return StatusCode::SUCCESS;
  };
  // using GenEvent's deep copy constructor
  HepMC::GenEvent* gevEvent = new HepMC::GenEvent(*lhcbEvent->pGenEvt());
  // determine needed scaling factors and xangles from first event
  if (_isFirstEvent)
  {
  #ifdef HEPMC_HAS_UNITS
    // proper unit conversion is done in Rivet::Event by rebuilding GenEvent (deep copy)
    info() << "HepMC event units are  [" << HepMC::Units::name(gevEvent->momentum_unit()) \
           << ", " << HepMC::Units::name(gevEvent->length_unit()) << "]." << endmsg;
    if ((HepMC::Units::conversion_factor(gevEvent->momentum_unit(),HepMC::Units::GEV) != 1.0) || \
        (HepMC::Units::conversion_factor(gevEvent->length_unit(),HepMC::Units::MM) != 1.0)) \
      info() << "Conversion to [GEV, MM] will be done automatically in RIVET." << endmsg;
  #else
    warning() << "Old HepMC library detected! Assuming LHCb units [MeV, mm, ns] for conversion to RIVET units [GeV, mm, ns]." << endmsg;
    _scaleFactorEnergy = Gaudi::Units::MeV/Gaudi::Units::GeV;
    _scaleFactorTime = 1.0;
    _needsUnitConv = true;
  #endif
    info() << "Internal conversion of units... [\x1B[1m" << (_needsUnitConv ? "enabled" : "disabled") << "\x1B[0m]." << endmsg; 
    // Cross-section info follows:
  #ifdef HEPMC_HAS_CROSS_SECTION
    if ( 0 == gevEvent->cross_section()) { // useful info for MC generators!
      warning() << "MC event generator provides NULL cross-section in HepMC." << endmsg;
    } else {
      info() << "HepMC cross-section: " << gevEvent->cross_section()->cross_section() << " +/- " << gevEvent->cross_section()->cross_section_error() << " picobarn(s)." << endmsg;
      if (! m_forceCrossSection) {
        _xsectionSource = 1;
        m_crossSection = gevEvent->cross_section()->cross_section(); // copy value so that is not default
      };
    };
  #else
    warning() << "HepMC does not support cross-section entries." << endmsg;
  #endif
    if ( _reqCrossSection ) {
      if ( m_forceCrossSection ) {
        _xsectionSource = 2;
        info() << "Forced cross-section: " << m_crossSection << " pb." << endmsg;
      } else {
        if ( (_xsectionSource == 0) && (m_crossSection < 0.0) ) {
          warning() << "Invalid external cross-section value [pb]...Trying to use total cross section value." << endmsg;
          m_crossSection = getTotalXSection();
          if ( m_crossSection < 0.0 ) {
            fatal() << "Invalid total cross-section value read from BeamParameters. No valid cross-section value available." << endmsg;
            return StatusCode::FAILURE;
          } else {
            warning() << "Using total cross-section value from BeamParameters: " << m_crossSection << " pb." << endmsg;
            _xsectionSource = 3;
          };
        };
      };
    } else {
      info() << "Cross-section value not needed by any of the currently selected plug-ins." << endmsg;
    }; // end cross-section source selection
    /// set beam particles if not present in Event (e.g. when reading from external sources)
    pair<HepMC::GenParticle*, HepMC::GenParticle*> beamParticles = findBeamParticles(gevEvent);
    if ((0 != beamParticles.first) && (0 != beamParticles.second)) {
      Gaudi::XYZVector b1, b2;
      b1.SetXYZ(beamParticles.first->momentum().x(), 
                beamParticles.first->momentum().y(), 
                beamParticles.first->momentum().z());
      b2.SetXYZ(beamParticles.second->momentum().x(), 
                beamParticles.second->momentum().y(), 
                beamParticles.second->momentum().z());
      b1 += b2;
      double hangle = b1.x()/b2.z()/Rivet::sign(b2.z())/2.0/Gaudi::Units::mrad;
      double vangle = b1.y()/b2.z()/Rivet::sign(b2.Z())/2.0/Gaudi::Units::mrad;
      info() << "Smeared horizontal crossing angle: " << hangle << " mrad." << endmsg;
      info() << "Smeared vertical crossing angle: " << vangle << " mrad." << endmsg;
    };
    /// if detection is requested and it fails then algorithm fails!
    if ( (m_xAngleDetect) && (detectBeamCrossingAngles(gevEvent)) ) return StatusCode::FAILURE;
  }; // end preprocessing first event
  if (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection || m_modStatusID) {
    double px, py, pz, e;
    double mm = 0.0;
    HepMC::FourVector* tmom;
    if (msgLevel(MSG::DEBUG)) { 
      debug() << "HepMC event before internal modifications:" << std::endl;
      gevEvent->print(debug().stream());
      debug() << endmsg;
    };
    for (HepMC::GenEvent::particle_iterator p = gevEvent->particles_begin(); \
         p != gevEvent->particles_end(); ++p) {
      px = (*p)->momentum().px();
      py = (*p)->momentum().py();
      pz = (*p)->momentum().pz();
      e  = (*p)->momentum().e();
      if (msgLevel(MSG::DEBUG)) {
        dess.str(std::string());
        // from Rivet::FourMomentum::mass(); HepMC::FourVector::m() not optimised for Oz beam line
        mm = invariantMass((*p)->momentum());
        negmm = (mm < 0.0);
      };
      tmom = new HepMC::FourVector(px, py, pz, e);
      if (m_modStatusID) { //erases trace-back to particles decayed by EvtGen
        int psid = (*p)->status();
        if (psid > LHCb::HepMCEvent::DocumentationParticle) { //following HepMC 2.05 particle status code recommendation
          if ((*p)->is_beam()) {
            ; // beam particle (status code == 4)
          } else if (psid == LHCb::HepMCEvent::StableInDecayGen) {
            (*p)->set_status(1);
          } else if ((psid>10) && (psid < 201)) {
            (*p)->set_status(0);
          } else {
            (*p)->set_status(2);
          };
          _myStats[3] ++;
        };
      };
      // detect if particle rest mass is negative (ignore documentation particles)
      if ( msgLevel(MSG::DEBUG) && ( (*p)->status() != 3 ) && negmm ) {
        dess << "Negative rest mass particle: " << std::endl << "id=" << (*p)->pdg_id() << "; status=" << (*p)->status() << ";" << std::endl;
        dess << "Before boost: " << std::endl << "P = (" << px << ", " << py << ", " << pz << ", " << e << ") --> m0 = " 
             << mm << " [MeV]." <<  std::endl;
      };
      if (_needsUnitConv) {
        //modify when length(time) unit conversion also needed (HepMC 2.03)
        //see GeneratorUtils::scale in LHCb framework if needed!
        tmom->setPx(px * _scaleFactorEnergy);
        tmom->setPy(py * _scaleFactorEnergy);
        tmom->setPz(pz * _scaleFactorEnergy);
        tmom->setE(e * _scaleFactorEnergy);
        _myStats[nbUnitConversions] ++;
      };
      if (_xHAngleCorrection && _xVAngleCorrection) {
        tmom->setE(e - px * _mHxAngle - py * _mVxAngle);
        tmom->setPx(px - e * _mHxAngle);
        tmom->setPy(py - e * _mVxAngle);
        if (statLogEnabled(nbXAngleBoosts)) debug() << "After XY-boost P' = (" << tmom->px() << ", " << tmom->py() \
             << ", " << tmom->pz() << ", " << tmom->e() << ") [MeV]..." << endmsg;
        _myStats[nbXAngleBoosts]++;
      } else {
        if (_xHAngleCorrection) {
          tmom->setE(e - px * _mHxAngle);
          tmom->setPx(px - e * _mHxAngle);
          if (statLogEnabled(nbXAngleBoosts)) debug() << "After X-boost P' = (" << tmom->px() << ", " << tmom->py() \
               << ", " << tmom->pz() << ", " << tmom->e() << ") [MeV]..." << endmsg;
          _myStats[nbXAngleBoosts] ++;
        };
        if (_xVAngleCorrection) {
          tmom->setE(e - py * _mVxAngle);
          tmom->setPy(py - e * _mVxAngle);
          if (statLogEnabled(nbXAngleBoosts)) debug() << "After Y-boost P' = (" << tmom->px() << ", " << tmom->py() \
               << ", " << tmom->pz() << ", " << tmom->e() << ") [MeV]..." << endmsg;
          _myStats[nbXAngleBoosts] ++;
        };
      };
      if (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection) (*p)->set_momentum((const HepMC::FourVector&)(*tmom));
      if (msgLevel(MSG::DEBUG) && negmm) { // re-test for negative rest mass
        if (_xHAngleCorrection || _xVAngleCorrection) {
          mm = invariantMass((*p)->momentum());
          if (mm < 0.) {
            if (statLogEnabled(negMassParticles)) debug() << dess.str() << "After boost: " << std::endl \
                                           << "P' = (" << tmom->px() << ", " << tmom->py() << ", " \
                                           << tmom->pz() << ", " << tmom->e() << ") --> m0 = " << mm << " [MeV]."<< endmsg;
       	    _myStats[negMassParticles] ++;
          };
        } else {
          debug() << dess.str() << endmsg;
          _myStats[negMassParticles] ++;
        };
      }; // end 2nd test negative squared mass
      delete tmom;
    }; // end particle iteration
  }; //end big if (corrections/transforms needed)
  compatSetCrossSection(gevEvent);
  // Analyze HepMC GenEvent in debugging mode
  if (msgLevel(MSG::DEBUG) && (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection || m_modStatusID)) { 
    debug() << "HepMC event after internal modifications:" << std::endl;
    gevEvent->print(debug().stream());
    debug() << endmsg;
  };
  if (_isFirstEvent) {
    _analysisManager->init(*gevEvent);
    _isFirstEvent = false;
  };
  _analysisManager->analyze(*gevEvent);
  delete gevEvent;
  debug() << "<== Execute" << endmsg; 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RivetAnalysisHandler::finalize()
{
  debug() << "==> Finalize" << endmsg;
  compatSetCrossSection(NULL);
  if (_myStats[nbGenXSChanges] >= m_logHardLimit) info() << "Last xsection value received from generator is: " << m_crossSection << " pb." << endmsg;
  //print debugging counters
  info() << "Internal counter values:" << std::endl;
  for (uint i = 0; i < this->_myStats.size(); i++ ) info() << "Counter[" << i << "] <<\"" << RivetAnalysisHandler::_statDescriptors[i] << "\">> = " << this->_myStats[i] << std::endl;
  info() << endmsg;
  string histFileName = (m_filename + ".yoda");
  _analysisManager->finalize();
  _analysisManager->writeData(histFileName);
  debug() << "<== Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Helper functions and additional non-public methods
//=============================================================================

/// sorts GenParticles by descending energy
bool cmpGenParticleByEDesc(const HepMC::GenParticle* a, const HepMC::GenParticle* b)
{
  return (a->momentum().e() > b->momentum().e());
}

/// taken from RIVET framework
bool fuzzyEq(double a, double b, double eps=1.E-6)
{
    const double absavg = fabs(a + b)/2.0;
    const double absdiff = fabs(a - b);
    const bool rtn = ( (absavg == 0.0 && absdiff == 0.0) || (absdiff < eps*absavg) );
    return rtn;
}

/// computes invariant mass for particles boosted along z-axis
double invariantMass(const HepMC::FourVector& mom) {
  double mass = (mom.e()-mom.pz()) * (mom.e()+mom.pz()) - mom.px() * mom.px() - mom.py() * mom.py();
  mass = Rivet::sign(mass) * sqrt(fabs(mass));
  return mass;
}

/// sets manager cross section value when it cannot be read from data automatically or a specific value is forced
void RivetAnalysisHandler::compatSetCrossSection(HepMC::GenEvent* pEvent) {
  if (!(_reqCrossSection || m_forceCrossSection)) return;
  if (m_crossSection < 0.0) { 
    error() << "Not setting required negative cross-section value. Set a valid value and/or enforce it in options." << endmsg;
    return;
  };
  if (NULL == pEvent) {
    _analysisManager->setCrossSection(m_crossSection);
    return;
  };
#ifdef HEPMC_HAS_CROSS_SECTION
  HepMC::GenCrossSection *pxs = pEvent->cross_section();
  if ( (0 == pxs) || m_forceCrossSection ) { // try setting the cross-section to "externally" provided value
    pxs = new HepMC::GenCrossSection();
    pxs->set_cross_section(m_crossSection);
    pEvent->set_cross_section(*pxs);
  } else {
    if ( !fuzzyEq(m_crossSection, pxs->cross_section()) ) {
      _myStats[nbGenXSChanges]++;
      if ( statLogEnabled(nbGenXSChanges, MSG::INFO) ) info() << "Generator provided xsection has changed from " << m_crossSection << " to " << pxs->cross_section() << endmsg;
      m_crossSection = pxs->cross_section(); // (re)set xsection to value given by generator (backup value!)
    };
  };
#else
  _analysisManager->setCrossSection(m_crossSection);
#endif
}

/// verifies if the log messages for a specific internal statistics flag is suppressed
bool RivetAnalysisHandler::statLogEnabled(idStatusLog  statId) {
  return statLogEnabled(statId, MSG::DEBUG);
}
bool RivetAnalysisHandler::statLogEnabled(idStatusLog  statId, MSG::Level lvl) {
  if (!msgLevel(lvl)) return false;
  bool printOn = ( (_myStats[statId] < m_logSoftLimit) || ( (_myStats[statId] <= m_logHardLimit) && (_myStats[statId] % m_logSuppressFreq == 0) ) );
  if ( _myStats[statId] == m_logSoftLimit ) always() << "Messages for internal stat flag \"" << _statDescriptors[statId] << "\" [id:" << statId << "] are softly suppressed from now on." << endmsg;
  if ( _myStats[statId] == m_logHardLimit ) always() << "Messages for internal stat flag \"" << _statDescriptors[statId] << "\" [id:" << statId << "] are totally supressed from now on." << endmsg;
  return printOn;
}

/// verifies that HepMC event has valid beam particles and if not try to detect them
std::pair<HepMC::GenParticle*,HepMC::GenParticle*> RivetAnalysisHandler::findBeamParticles(HepMC::GenEvent* hEvent)
{
  if (hEvent->valid_beam_particles()) return hEvent->beam_particles();
  HepMC::GenParticle* pBeam1 = 0;
  HepMC::GenParticle* pBeam2 = 0;
  int pid = 0;
  for (HepMC::GenEvent::particle_const_iterator pi = hEvent->particles_begin();
         pi != hEvent->particles_end(); ++pi) {
    pid = (*pi)->pdg_id();
    //consider only gamma and non partons as beam particles (enforce p--><--p ?)
    // second condition pid != 0 was too lax!
    if ( !(*pi)->production_vertex() && ( (pid == 22) || (pid > 100) ) ) {
      if (pid != 2212)
      {
        warning() << "Beam particle " << pid << " (status=" << (*pi)->status() << ") detected but not proton. Skipping..." << endmsg;
        continue;
      };
      if (pBeam2) {
        error() << "More than 2 beams detected!" << endmsg;
        break;
       };
      if (pBeam1) {
        pBeam2 = *pi;
      } else {
        pBeam1 = *pi;
      };
    };
  };
  if ( (pBeam2 == 0) || (pBeam1 == 0) )
  {
    error() << "Not all proton beams were found in HepMC event:" << std::endl;
    hEvent->print(error().stream());
    error() << endmsg;
    pBeam1 = 0; pBeam2 = 0;
    return make_pair(pBeam1, pBeam2);
  };
  // is this really needed for two protons of same energy?!
  if (!cmpGenParticleByEDesc(pBeam1,pBeam2))
  {
    HepMC::GenParticle* tt = pBeam2;
    pBeam2 = pBeam1; pBeam1 = tt;
  };
  hEvent->set_beam_particles(pBeam1, pBeam2);
  return std::make_pair(pBeam1, pBeam2);
}

/// Read total cross-section from beam parameters. PYTHIA & LHCb use mb while HepMC uses pb!
double RivetAnalysisHandler::getTotalXSection() {
  LHCb::BeamParameters * beamParams = get<LHCb::BeamParameters>(LHCb::BeamParametersLocation::Default);
  if ( 0 == beamParams ) return -1.0;
  return (beamParams->totalXSec()/Gaudi::Units::picobarn);
}

/// Detect beam crossing angles if they exist
bool RivetAnalysisHandler::detectBeamCrossingAngles(HepMC::GenEvent* hEvent) {
  if ( 0 == hEvent ) {
    error() << "NULL GenEvent pointer. Skip beam crossing angles detection..." << endmsg;
    return true;
  };
  LHCb::BeamParameters * beamParams = get<LHCb::BeamParameters>(LHCb::BeamParametersLocation::Default);
  _mHxAngle = 0.0; _mVxAngle = 0.0;
  if ( 0 == beamParams ) {
    error() << "No beam parameters found in TES." << endmsg;
    return true;
  } else {
    info() << "Maximal angle smearing: " << (beamParams->angleSmear()/Gaudi::Units::mrad) << " mrad." << endmsg;
    if ( 0.0 != beamParams->angleSmear() )  debug()  \
              << "Please, use Gauss().BeamBetaStar = 0.0 to eliminate smearing when nedeed." \
              << endmsg;
    _mHxAngle = beamParams->horizontalCrossingAngle();
    _mVxAngle = beamParams->verticalCrossingAngle();
  };
  if ( 0.0 != _mHxAngle ) {
    _xHAngleCorrection = true;
    info() << "Horizontal crossing angle: " \
              << (_mHxAngle/Gaudi::Units::mrad)  << " mrad." << endmsg;
  };
  if ( 0.0 != _mVxAngle ) {
    _xVAngleCorrection = true;
    info() << "Vertical crossing angle: " \
              << (_mVxAngle/Gaudi::Units::mrad) << " mrad." << endmsg; 
  };
  return false;
}

