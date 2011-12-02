
//-----------------------------------------------------------------------------
/** @file RichRecTimeMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::TimeMonitor
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

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TimeMonitor RichRecTimeMonitor.h
     *
     *  Monitor class for Rich Reconstruction processing time
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class TimeMonitor : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      TimeMonitor( const std::string& name,
                   ISvcLocator* pSvcLocator );

      virtual ~TimeMonitor( ); ///< Destructor

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

      /// Vector of pointers to RichPIDs
      std::vector<ContainedObject*> m_richPIDs;

      // Data to compute average processing time per event and PID
      unsigned long long m_nEvents; ///< Total number of events
      unsigned long long m_nPIDs;   ///< Total PIDs considered
      long double m_totTime;       ///< Log of total processing time

      double m_maxTime;         ///< Maximum overall processing time for plots
      double m_maxTimePerPID;   ///< Maximum processing pre PID time for plots

      unsigned int m_maxTracks;  ///< Maximum number of tracks for plots
      unsigned int m_maxPixels;  ///< Maximum number of pixels for plots
      unsigned int m_maxPhotons; ///< Maximum number of photons for plots
      unsigned int m_maxPhotonsPerPID; ///< Maximum number of photons/PID for plots
      unsigned int m_maxPIDs;    ///< Maximum number of PIDs for plots
      

    };

  }
}

#endif // RICHRECMONITOR_RICHRECTIMEMONITOR_H
