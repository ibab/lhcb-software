// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Bootstrap.h"

// from Generators
#include "Generators/IBeamTool.h"

// from Event
#include "Event/HepMCEvent.h"
// from HepMC
#include "HepMC/GenEvent.h"
#ifdef HEPMC_HAS_UNITS
#include "HepMC/Units.h"
#endif

// disable unused parameter warning!
#pragma GCC diagnostic ignored "-Wunused-parameter"
// from Rivet (LCG)
#include "Rivet/Rivet.hh"
#include "Rivet/AnalysisHandler.hh"
#include "Rivet/Analysis.hh"
#include "Rivet/Tools/RivetPaths.hh"

// from boost
#include <boost/algorithm/string.hpp>
#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH

// local
#include "RivetAnalysisHandler.h"
#include "LogLevelMap.h"

// From Event
#include "Event/BeamParameters.h"
#include "GenEvent/BeamForInitialization.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : RivetAnalysisHandler
//
// 2011-08-12 : Alex Grecu <alexandru.grecu@nipne.ro>
// 2012-07-12 : Alex Grecu - code clean up!
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RivetAnalysisHandler );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RivetAnalysisHandler::RivetAnalysisHandler( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name, pSvcLocator ),
    _analysisManager (0),
    _mHxAngle(0.0),
    _mVxAngle(0.0),
    _scaleFactorEnergy (1.0),
    _scaleFactorTime (1.0)
{
  _isFirstEvent = true; _xHAngleCorrection = false; _xVAngleCorrection = false; m_xAngleDetect = true;
  myStats.assign(3, 0);
  declareProperty("MCEventLocation", m_mcEvtLocation=LHCb::HepMCEventLocation::Default);
  declareProperty("BaseFileName", m_filename="MyRivet");
  declareProperty("RunName", m_runname="LHCB");
  declareProperty("Analyses", m_analysisNames);
  declareProperty("StreamName", m_histStreamName="/Rivet");
  declareProperty("AnalysisPath", m_analysisPaths);
  declareProperty("CorrectStatusID", m_modStatusID=false);
  declareProperty("CorrectCrossingAngles", m_xAngleDetect=true);
  declareProperty("xSectionNeeded", m_reqCrossSection=false);
  declareProperty("xSectionValue", m_crossSection=-1.);
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;
  debug() << "Using Rivet version: " << Rivet::version() << endmsg;
#ifdef HEPMC_HAS_UNITS
  always() << "Units will be determined automatically from HepMC by RIVET !" << std::endl;
  always() << "Algorithm option 'AdjustUnits' will be ignored..." << endmsg;
#endif
#ifndef HEPMC_HAS_UNITS
  _needsUnitConv = true;
#else 
  _needsUnitConv = false;
#endif
  //Check and select histogram service
  //Check histogram service is available - not implemented further yet!
  if (!(histoSvc())) {
    error() << "Failed to retrieve histogram service!" << endmsg;
    return StatusCode::FAILURE;
  };
  //Create Rivet::AnalysisHandler instance
  _analysisManager = new Rivet::AnalysisHandler(m_runname);
  assert(_analysisManager);
  //Set RIVET_ANALYSIS_PATH based on alg setup. Don't overwrite if already set in env
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

  // Setting log level for analyses according to Gauss (LHCbApp) log level
  MSG::Level jobMsgLvl = (MSG::Level)(*msgSvc()).outputLevel();
  always() << "Gauss log level: " << (int)jobMsgLvl << endmsg;
  Rivet::Log::setLevel("Rivet.Projection.PVertex", rivetLevel(jobMsgLvl));
  always() << "Rivet.Projection.PVertex log level: " << Rivet::Log::getLog("Rivet.Projection.PVertex").getLevel() << endmsg;
  // Set Rivet native log level to match LHCbApp (Gauss)
  Rivet::Log::setLevel("Rivet", rivetLevel(jobMsgLvl));
  // List all available analysis names - verbose mode only!
  if (msgLevel(MSG::VERBOSE)) {
    vector<string> analysisNames = Rivet::AnalysisLoader::analysisNames();
    verbose() << "Listing available Rivet analyses:" << std::endl;
    foreach (const string& a, analysisNames) verbose() << " " << a << std::endl;
    verbose() << endmsg;
  };
  // Set cross-section if needed
  m_reqCrossSection = false;
  foreach (const string& a, m_analysisNames) {
    info() << "Loading Rivet analysis " << a << endmsg;
    Rivet::Analysis* analysis = Rivet::AnalysisLoader::getAnalysis(a);
    if (!(analysis))
    {
      error() << "Cannot find/load analysis " << a << ". Skipping..." << endmsg;
      continue;
    };
    _analysisManager->addAnalysis(a);
    string logName = "Rivet.Analysis." + a;
    Rivet::Log::setLevel(logName, rivetLevel(jobMsgLvl));
    if (analysis->needsCrossSection()) {
      m_reqCrossSection = true;
      if (m_crossSection < 0.0) {
        fatal() << "Analysis " << a << " requires the cross section to be set in the job options" << endmsg;
        return StatusCode::FAILURE;
      };
    }; 
  }; // end foreach
  // Initialize Rivet
  _analysisManager->init();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RivetAnalysisHandler::execute()
{
  bool printOn = false;
  debug() << "==> Execute" << endmsg;
  /// get HepMC event from TES 
  LHCb::HepMCEvent::Container* mcEvents 
    = get<LHCb::HepMCEvent::Container>( m_mcEvtLocation );
  if (NULL == mcEvents) {
    error() << "No HepMC events on TES at " << m_mcEvtLocation << endmsg;
    return StatusCode::FAILURE;
  };
  /// take only the first event and thus ignore pile-up
  LHCb::HepMCEvent::Container::iterator ievent = mcEvents->begin();
  LHCb::HepMCEvent* lhcbEvent = (*ievent);
  if (lhcbEvent == NULL)
  {
    warning() << "Skipping NULL primary HepMC event..." << endmsg;
    return StatusCode::SUCCESS;
  };
  // deep copy constructor!
  HepMC::GenEvent* gevEvent = new HepMC::GenEvent(*lhcbEvent->pGenEvt());
  // determine needed scaling factors and xangles from first event
  if (_isFirstEvent)
  {
  #ifdef HEPMC_HAS_UNITS
    // proper unit conversion is done in Rivet::Event by rebuilding GenEvent (deep copy)
    always() << "HepMC event units are  " << HepMC::Units::name(gevEvent->momentum_unit()) \
           << ", " << HepMC::Units::name(gevEvent->length_unit()) << "." << endmsg;
    if ((HepMC::Units::conversion_factor(gevEvent->momentum_unit(),HepMC::Units::GEV) != 1.0) || \
        (HepMC::Units::conversion_factor(gevEvent->length_unit(),HepMC::Units::MM) != 1.0)) \
      warning() << "HepMC event units not [GEV, MM]. Conversion will be done automatically in RIVET." << endmsg;
  #else
    // assume units are consistent with LHCb system (MEV, MM, NS)
    // RIVET units are (GEV, MM, NS)
    _scaleFactorEnergy = Gaudi::Units::MeV/Gaudi::Units::GeV;
    _scaleFactorTime = 1.0;
    _needsUnitConv = true;
  #endif
    debug() << "Internal conversion of units: [" << (_needsUnitConv ? "enabled" : "disabled") << "]." << endmsg; 
    /// set beam particles if not present in event (when reading from external sources)
    pair<HepMC::GenParticle*, HepMC::GenParticle*> beamParticles = findBeamParticles(gevEvent);
    /// if detection is requested and it fails then algorithm fails as well!
    if ( (m_xAngleDetect) && (detectBeamCrossingAngles(gevEvent)) ) return StatusCode::FAILURE;
    _isFirstEvent = false;
  }; // end preprocessing first event
  if (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection || m_modStatusID) {
    double px, py, pz, e;
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
      tmom = new HepMC::FourVector(px, py, pz, e);
      if (m_modStatusID) {
        int psid = (*p)->status();
        if (psid == LHCb::HepMCEvent::StableInDecayGen) (*p)->set_status(1);
        if (psid > LHCb::HepMCEvent::DocumentationParticle) (*p)->set_status(2);
      };
      // detect if particle rest mass is negative
      printOn = ( (myStats[0] < 30) || (myStats[0] % 5 == 0) );
      if ( printOn && ( ( (*p)->momentum().rho() - e ) < 0.0 ) ) {
        error() << "Particle " << (*p)->pdg_id() << " has negative rest mass and P = ("  \
                   << px << ", " << py << ", " << pz << ", " << e << ")...";
        myStats[0] ++;
      };
      if (_needsUnitConv) {
        //modify when length(time) unit conversion also needed (HepMC 2.03)
        //see GeneratorUtils::scale in LHCb framework if needed!
        tmom->setPx(px * _scaleFactorEnergy);
        tmom->setPy(py * _scaleFactorEnergy);
        tmom->setPz(pz * _scaleFactorEnergy);
        tmom->setE(e * _scaleFactorEnergy);
      };
      if (_xHAngleCorrection && _xVAngleCorrection) {
        printOn = ( (myStats[3] < 30 ) || (myStats[3] % 5 == 0) );
        tmom->setE(e - px * _mHxAngle - py * _mVxAngle);
        tmom->setPx(px - e * _mHxAngle);
        tmom->setPy(py - e * _mVxAngle);
        if (printOn) verbose() << "After XY-boost P' = (" << tmom->px() << ", " << tmom->py() \
             << ", " << tmom->pz() << ", " << tmom->e() << ")..." << endmsg;
        myStats[3]++;
      } else {
        if (_xHAngleCorrection) {
          printOn = ( (myStats[1] < 30 ) || (myStats[1] % 5 == 0) );
          // _mHxAngle is negative !
          tmom->setE(e - px * _mHxAngle);
          tmom->setPx(px - e * _mHxAngle);
          if (printOn) verbose() << "After X-boost P' = (" << tmom->px() << ", " << tmom->py() \
               << ", " << tmom->pz() << ", " << tmom->e() << ")..." << endmsg;
          myStats[1] ++;
        };
        if (_xVAngleCorrection) {
          bool printOn = ( (myStats[2] < 30 ) || (myStats[2] % 5 == 0) );
          // _mVxAngle is negative !
          tmom->setE(e - py * _mVxAngle);
          tmom->setPy(py - e * _mVxAngle);
          if (printOn) verbose() << "After Y-boost P' = (" << tmom->px() << ", " << tmom->py() \
               << ", " << tmom->pz() << ", " << tmom->e() << ")..." << endmsg;
          myStats[2] ++;
        };
      };
      if (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection) {
        const HepMC::FourVector qboost(tmom->px(), tmom->py(), tmom->pz(), tmom->e());
        (*p)->set_momentum(qboost);
      };
      delete tmom;
    }; // end particle iteration
  }; //end big if (corrections/transformations needed)
  #ifndef HEPMC_HAS_CROSS_SECTION
  compatSetCrossSection();
  #endif  
  // Analyze HepMC GenEvent in debugging mode
    if (msgLevel(MSG::DEBUG) && (_needsUnitConv || _xHAngleCorrection || _xVAngleCorrection || m_modStatusID)) { 
      debug() << "HepMC event after internal modifications:" << std::endl;
      gevEvent->print(debug().stream());
      debug() << endmsg;
    };
  _analysisManager->analyze(*gevEvent);
  delete gevEvent;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RivetAnalysisHandler::finalize()
{
  debug() << "==> Finalize" << endmsg;
  #ifndef HEPMC_HAS_CROSS_SECTION
    compatSetCrossSection();
  #endif
  //print debugging counters
  if (msgLevel(MSG::DEBUG))
  {
  debug() << "Internal counter values:" << std::endl;
  for (uint i = 0; i < myStats.size(); i++ ) debug() << "Counter[" << i << "] = " << myStats[i] << std::endl;
  debug() << endmsg;
  };
  string aidaFileName = (m_filename + ".aida");
  _analysisManager->finalize();
  _analysisManager->writeData(aidaFileName);
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

/// Helper functions:

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

/// sets manager cross section value when it cannot be read from data automatically
void RivetAnalysisHandler::compatSetCrossSection() {
  if (m_reqCrossSection) _analysisManager->setCrossSection(m_crossSection);
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
    // here Andy's second condition is pid != 0 which is too lax!?
    if ( !(*pi)->production_vertex() && ( (pid == 22) || (pid > 100) ) ) {
      if (pid != 2212)
      {
        warning() << "Beam particle " << pid << " detected but not proton. Skipping..." << endmsg;
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

/// Detect beam crossing angles if they exist
bool RivetAnalysisHandler::detectBeamCrossingAngles(HepMC::GenEvent* hEvent) {
  if ( 0 == hEvent ) {
    error() << "NULL GenEvent pointer. Skip beam crossing angles detection..." << endmsg;
    return true;
  };
  LHCb::BeamParameters * beamParams = get< LHCb::BeamParameters >(LHCb::BeamParametersLocation::Default);
  _mHxAngle = 0.0; _mVxAngle = 0.0;
  if ( 0 == beamParams ) {
    error() << "No beam parameters found in TES." << endmsg;
    return true;
  } else {
    if ( 0.0 != beamParams->angleSmear() )  debug()  \
              << "Please, set BeamBetaStar = 0.0 to eliminate smearing if enabled!" \
              << endmsg;
    _mHxAngle = beamParams->horizontalCrossingAngle();
    _mVxAngle = beamParams->verticalCrossingAngle();
  };
  if ( 0.0 != _mHxAngle ) {
    _xHAngleCorrection = true;
    always() << "Horizontal crossing angle: " \
              << (_mHxAngle/Gaudi::Units::mrad)  << " mrad." << endmsg;
  };
  if ( 0.0 != _mVxAngle ) {
    _xVAngleCorrection = true;
    always() << "Vertical crossing angle: " \
              << (_mVxAngle/Gaudi::Units::mrad) << " mrad." << endmsg; 
  };
  return false;
}

