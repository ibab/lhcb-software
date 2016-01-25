// $Id: $
#ifndef COMPONENT_RIVETANALYSISHANDLER_H 
#define COMPONENT_RIVETANALYSISHANDLER_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include <sstream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

/** @class RivetAnalysisHandler RivetAnalysisHandler.h component/RivetAnalysisHandler.h
 *  
 *  Component that interfaces Rivet to Gauss in order to run Rivet plug-ins 
 *  inside Gauss jobs. The interface is heavily inspired by the ATLAS API for 
 *  Rivet which is developed and maintained by Andy Buckley and James Monk.
 *  
 *  @author Alex Grecu <alexandru.grecu@nipne.ro>
 *  @date   2011-08-12
 */
class RivetAnalysisHandler : public GaudiAlgorithm {
private:

  std::pair<HepMC::GenParticle*,HepMC::GenParticle*> findBeamParticles(HepMC::GenEvent* hEvent);

  double getTotalXSection();

  bool detectBeamCrossingAngles(HepMC::GenEvent* hEvent);

  /// Property: The base file name (prefix of filenames) to write results to ("MyRivet").
  string m_filename;
 
  /// Property: The name of the run to prepended to YODA plot paths ("LHCB").
  string m_runname;
 
  /// Property: A list of names of the analyses to run ([]).
  vector<string> m_analysisNames;

  /// The main Rivet analysis handler
  Rivet::AnalysisHandler* _analysisManager;

  /// Property: Location on TES where the HepMC events are read from (LHCb::HepMCEventLocation::Default).
  string m_mcEvtLocation;

  /// Property: Instructs the algorithm to automatically detect and correct for beam crossing angles (True). 
  bool m_xAngleDetect;

  /// Property: List of additional file paths where analyses should be looked for, e.g. add os.path.abspath('.') when analysis lib is in option file directory ([]).
  vector<string> m_analysisPaths;

  /// Indicates whether the cross-section is needed by any of the analyses run in job (false).
  bool _reqCrossSection;

  /// Indicates which source should be queried for cross-section value when needed (0 - invalid). Sources are: 1 - HepMC event; 2 - External cross-section value; 3 - BeamParameters total cross-section value
  int _xsectionSource;

  /// Property: Forces GenTune to set the cross-section to the value provided externally in each event (False).
  bool m_forceCrossSection;

  /// Property: The externally provided cross-section for the present run [picobarn]; ignored when read from data (-1.).
  double m_crossSection;

  /// Property: Switch that controls the transformation of status ID of particles (given by EvtGen) back to Pythia defaults (False).
  bool m_modStatusID;

  /// When crossing angle presence is detected these flags are true.
  bool _xHAngleCorrection;
  bool _xVAngleCorrection;

  // The medium horizontal crossing angle value (Ox).
  double _mHxAngle;
  // The medium vertical crossing angle value (Oy).
  double _mVxAngle;
  
  /// This flag is true only if the component was linked against HepMC 2.03 or when scaling to LHCb units is needed.
  bool _needsUnitConv;
  
  /// Toggles to false after first event processed to allow dynamic setup of flags above (true).
  bool _isFirstEvent;
  
  /// Energy conversion factor (1.).
  double _scaleFactorEnergy;
  
  /// Time conversion factor (1.) - not really used!
  double _scaleFactorTime;

  /// Various statistics (debugging mode and not only) -- see idStatusLog
  vector<long unsigned int> _myStats;

  /// The description for each internal statistical counter
  static const char* const _statDescriptors[];

  /// Number of internal statistical counters
  static const unsigned int COUNTERS_NB = 5;

  /// Property: Internal statistical messages print-out suppression soft limit (30).
  long unsigned int m_logSoftLimit;

  /// Property: Internal statistical message print-out suppression frequency (10).
  long unsigned int m_logSuppressFreq;

  /// Property: Internal statistical message print-out suppression hard limit (200).
  long unsigned int m_logHardLimit;

  /// Internal function: Set cross-section for each event when cross-section value provided in options
  void compatSetCrossSection(HepMC::GenEvent*);

public:
  /// Standard constructor
  RivetAnalysisHandler( const string& name, ISvcLocator* pSvcLocator );
  virtual ~RivetAnalysisHandler( );     ///< Destructor
  virtual StatusCode initialize();       ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();      ///< Algorithm finalization

  enum idStatusLog {
    negMassParticles = 0,	// 0: count particles with negative! rest mass
    nbXAngleBoosts,		// 1: times x-Angle boost(s) was done
    nbUnitConversions,		// 2: times internal unit conversion was applied
    nbPIDAdjustments,		// 3: times particleId was adjusted
    nbGenXSChanges		// 4: nb of generator provided xsection changes
  };

private:
  /// Checks whether messages specific to internal flag statId are suppressed or not.
  bool statLogEnabled(idStatusLog statId);
  bool statLogEnabled(idStatusLog statId, MSG::Level lvl);

};

bool cmpGenParticleByEDesc(const HepMC::GenParticle* a, const HepMC::GenParticle* b);

bool fuzzyEq(double a, double b, double eps);

double invariantMass(const HepMC::FourVector& mom);

#endif
// COMPONENT_RIVETANALYSISHANDLER_H
