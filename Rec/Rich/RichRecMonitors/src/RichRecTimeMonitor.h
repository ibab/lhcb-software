
//-----------------------------------------------------------------------------
/** @file RichRecTimeMonitor.h
 *
 *  Header file for algorithm class : RichRecTimeMonitor
 *
 *  CVS Log :-
 *  $Id: RichRecTimeMonitor.h,v 1.5 2006-12-01 16:34:07 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHRECTIMEMONITOR_H
#define RICHRECMONITOR_RICHRECTIMEMONITOR_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Event
#include "Event/RichPID.h"
#include "Event/RichRecStatus.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//-----------------------------------------------------------------------------
/** @class RichRecTimeMonitor RichRecTimeMonitor.h
 *
 *  Monitor class for Rich Reconstruction processing time
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecTimeMonitor : public RichRecHistoAlgBase
{

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

private: // data

  // job options
  std::string m_PIDLocation; ///< Location of PID results in TES
  std::string m_name;        ///< Name given to algorithm or group of algorithms being monitored

  /// Definition of algorithm name list
  typedef std::vector<std::string> AlgorithmNames;
  AlgorithmNames m_algNames; ///< List of algorithm(s) to include in timing

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
