#ifndef LBPOWHEG_POWHEGPRODUCTION_H 
#define LBPOWHEG_POWHEGPRODUCTION_H 1

// LbHard.
#include "LbHard/HardProduction.h"

// Local.
#include "PowhegProcs.h"

/** 
 * Production tool to generate events with PowhegBox.
 * 
 * POWHEG BOX, as taken from http://powhegbox.mib.infn.it/, is a
 * general computer framework for implementing NLO calculations in
 * shower Monte Carlo programs according to the POWHEG method. It is
 * also a library, where previously included processes are made
 * available to the users. It can be interfaced with all modern shower
 * Monte Carlo programs that support the Les Houches Interface for
 * User Generated Processes.
 *
 * The documentation for individual processes can be found at 
 * http://powhegbox.mib.infn.it/. The POWHEG method is outlined in
 * hep-ph/0409146, arXiv:0709.2092, and arXiv:1002.2581.
 *
 * The tool works by loading the POWHEG BOX executables at runtime
 * after wrapping them as plugin library. This method was initially
 * developed for Pythia 8 and has been adapted for LHCb needs. The
 * following POWHEG BOX process libraries are available:
 *
 * hvq: heavy-quark pair production, arXiv:0707.3088.
 *
 * Note that full matching with the parton shower is performed for
 * Pythia 8, but has not been backwards ported for use with Pythia
 * 6. Consequently, while a Pythia 6 parton shower can be applied with
 * this tool it is recommended to only use Pythia 8 showers.
 *
 * @class  PowhegProduction
 * @file   PowhegProduction.h 
 * @author Bartek Rachwal
 * @author Marcin Kucharczyk
 * @author Philip Ilten
 * @date   2013-06-18
 */
class PowhegProduction : public HardProduction {
public:

  /// Default constructor.
  PowhegProduction(const std::string &type, const std::string &name,
		   const IInterface *parent);

  /// Initialize the hard process tool.
  StatusCode hardInitialize();

  /// Initialize the hard process tool.
  StatusCode hardInitializeGenerator();

  /// Finalize the hard process tool.
  StatusCode hardFinalize();

private:

  // Members.
  CommandVector m_defaultSettings; ///< The default settings.
  string m_proc;                   ///< The POWHEG process.
  map<string, int> m_procs;        ///< The POWHEG process map (int is nFinal).
  Pythia8::PowhegProcs *m_powheg;  ///< The actual POWHEG process container.
};

#endif // LBPOWHEG_POWHEGPRODUCTION_H
