
/** @file RichRecTimeMonitor.h
 *
 *  Header file for algorithm class : RichRecTimeMonitor
 *
 *  CVS Log :-
 *  $Id: RichRecTimeMonitor.h,v 1.2 2005-06-23 15:14:56 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

#ifndef RICHRECMONITOR_RICHRECTIMEMONITOR_H
#define RICHRECMONITOR_RICHRECTIMEMONITOR_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event
#include "Event/RichPID.h"
#include "Event/RichRecStatus.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Units
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecTimeMonitor RichRecTimeMonitor.h
 *
 *  Monitor class for Rich Reconstruction processing time
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

class RichRecTimeMonitor : public RichRecAlgBase {

public:

  /// Standard constructor
  RichRecTimeMonitor( const std::string& name, 
                    ISvcLocator* pSvcLocator );

  virtual ~RichRecTimeMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Loads the PID data from configured location
  StatusCode loadPIDData();

  /// Book histograms
  StatusCode bookHistograms();

private: // data

  // job options
  std::string m_histPth;     ///< Output histogram path
  std::string m_mcHistPth;   ///< Output MC truth histogram path
  std::string m_PIDLocation; ///< Location of PID results in TES
  std::string m_name;        ///< Name of Algorithm being monitored

  /// Definition of algorithm name list
  typedef std::vector<std::string> AlgorithmNames;
  AlgorithmNames m_algNames; ///< algorithm(s) to include in timing
  bool m_noHists;            ///< Flag to turn off histogramming
  
  // Histograms
  IHistogram1D* m_time;         ///< Overall event processing time
  IHistogram1D* m_timePerPID;   ///< Event processing time per PID
  IHistogram2D* m_timeVnPIDs;   ///< Event processing time versus # PIDs

  // Vector of pointers to RichPIDs
  std::vector<ContainedObject*> m_richPIDs;

  // Data to compute average processing time per event and PID
  unsigned int m_nEvents;
  unsigned int m_nPIDs;
  double m_totTime;
  double m_maxTime;
  double m_maxTimePerPID;

};

#endif // RICHRECMONITOR_RICHRECTIMEMONITOR_H
