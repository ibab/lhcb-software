// $Id: $
#ifndef COMPONENT_RIVETANALYSISHANDLER_H 
#define COMPONENT_RIVETANALYSISHANDLER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include <string>
#include <vector>
#include <pair>

using namespace std;

/** @class RivetAnalysisHandler RivetAnalysisHandler.h component/RivetAnalysisHandler.h
 *  
 *  Component that interfaces Rivet to Gauss in order to run Rivet plug-ins 
 *  inside Gauss jobs. The interface is mainly inspired by the ATLAS API for 
 *  Rivet which is developed and maintained by Andy Buckley and James Monk.
 *  
 *  @author Alex Grecu <alexandru.grecu@nipne.ro>
 *  @date   2011-08-12
 */
class RivetAnalysisHandler : public GaudiAlgorithm {
public: 

  /// Standard constructor
  RivetAnalysisHandler( const string& name, ISvcLocator* pSvcLocator );
  virtual ~RivetAnalysisHandler( );     ///< Destructor
  virtual StatusCode initialize();       ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();      ///< Algorithm finalization

protected:

private:

  std::pair<HepMC::GenParticle*,HepMC::GenParticle*> findBeamParticles(HepMC::GenEvent* hEvent);

  bool detectBeamCrossingAngles(HepMC::GenEvent* hEvent);

  /// The base file name to write results to (prefix of filenames).
  string m_filename;
 
  /// The name of the run (prepended to plot paths).
  string m_runname;
 
  /// A list of names of the analyses to run (set from the job properties)
  vector<string> m_analysisNames;

  /// The main Rivet analysis handler
  Rivet::AnalysisHandler* _analysisManager;

  /// Stream name where output plots are stored (AIDA file) - default: "/Rivet"
  string m_histStreamName;

  /// Location where the HepMC events are read from (on TES) - default: LHCb::HepMCEventLocation::Default
  string m_mcEvtLocation;

  /// Instructs the algorithm to automatically detect and correct for beam crossing angles
  bool m_xAngleDetect;

  /// List of paths where analyses should be looked for
  vector<string> m_analysisPaths;

  /// Indicates whether the cross-section is to be read from data (true)
  bool m_reqCrossSection;

  /// The externally provided cross-section for the present run (mb or mub?); ignored when read from data
  double m_crossSection;

  /// Switch that controls the transformation of status ID of particles (given by EvtGen) to Pythia defaults
  bool m_modStatusID;

  /// When crossing angle presence is detected these flags is true
  bool _xHAngleCorrection;
  bool _xVAngleCorrection;

  // The medium horizontal crossing angle value (Ox)
  double _mHxAngle;
  // The medium vertical crossing angle value (Oy)
  double _mVxAngle;
  
  /// This flag is true only if the component was linked against HepMC 2.03 or when scaling to LHCb units is needed
  bool _needsUnitConv;
  
  /// Toggles after first event processed to allow dynamic setup of flags above
  bool _isFirstEvent;
  
  /// Energy conversion factor
  double _scaleFactorEnergy;
  
  /// Time conversion factor (not really used!)
  double _scaleFactorTime;

  /// Various statistics (debugging mode and not only)
  // Index 0: Counter for debug message supression
  vector<long int> myStats;   

  void compatSetCrossSection();
};

bool cmpGenParticleByEDesc(const HepMC::GenParticle* a, const HepMC::GenParticle* b);

bool fuzzyEq(double a, double b, double eps);

#endif
// COMPONENT_RIVETANALYSISHANDLER_H
