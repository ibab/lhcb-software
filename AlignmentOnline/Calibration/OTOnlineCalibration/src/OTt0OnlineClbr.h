#ifndef __OTt0OnlineClbr_h
#define __OTt0OnlineClbr_h

// STD & STL
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <chrono>
#include <mutex>
#include <queue>
#include <deque>
#include <thread>
#include <condition_variable>
#include <unordered_set>
#include <unordered_map>

// BOOST
#include <boost/filesystem.hpp>

// Gaudi
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/IEventProcessor.h>
#include <GaudiKernel/IDetDataSvc.h>

// Detector
#include <OTDet/DeOTDetector.h>

// OMALib
#include <OMAlib/AnalysisTask.h>

// dim
#include <dic.hxx>

// ROOT
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TString.h>

template<class T, class Container = std::deque<T> >
class Queue {
public:
   Queue() : m_mutex{}, m_condition{}, m_queue{} {}

   T get() {
      std::unique_lock<std::mutex> lock{m_mutex};
      m_condition.wait(lock, [this] ()-> bool {return not m_queue.empty();});
      T tmp = std::move(m_queue.front());
      m_queue.pop();
      return tmp;
   }

   template<class ITEM>
   void put(ITEM&& i) {
      std::unique_lock<std::mutex> lock{m_mutex};
      m_queue.push(std::forward<ITEM>(i));
      m_condition.notify_one();
   }

private:

   std::mutex m_mutex;
   std::condition_variable m_condition;
   std::queue<T, Container> m_queue;

};


/** @class OTt0OnlineClbr OTt0OnlineClbr.h
 *
 *
 *  @author Lucia Grillo
 *  @date   2015-01-08
 */

class Condition;

class OTt0OnlineClbr : public AnalysisTask {

public:

   /// Standard constructor
   OTt0OnlineClbr( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~OTt0OnlineClbr( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode start     ();    ///< Algorithm start
   virtual StatusCode finalize  ();    ///< Algorithm finalization
   virtual StatusCode execute   ();    ///< to run offline
   virtual StatusCode analyze   (std::string& saveSet,
                                 std::string task); ///< Algorithm analyze

   StatusCode updateT0();
   
   typedef unsigned int FileVersion ;

protected:

   //private:
   //if you want to write the functions you can do it here

private:

   /**
    * save_fits (default false, I hope to mange to save the fits, ONLY FOR STUDIES).
    */
   bool m_saveFits;
   /**
    * getmean_instead_of_fit
    */
   bool m_useMean;
   /**
    * read_global_t0_from_db. At the moment option, online will be the only running way
    */
   bool m_readFromDB;
   /*
    * if the |global t0| exceedes this threshold, we want to update the global t0 condition
    */
   double m_threshold;
   /*
    * if the |global t0| exceedes this threshold, we don't update and indicate a problem
    */
   double m_maxDifference;
   /*
    * Run online or offline - the difference is that offline I read a test .root file,
    * while online it should get the last EoR file from the monitoring?
    */
   bool m_RunOnline;
   /*
    * Amount of time to sleep between consecutive publishes in case of multiple runs.
    */
   unsigned int m_pubSleep;
   /*
    * Name of the to be published service.
    */
   std::string m_pubName;
   /*
    * path/names of histograms to perform online calibration on.
    */
   std::vector<std::string> m_histogramNames;
   /*
    * Minimum number of entries required for calibration.
    */
   unsigned int m_minEntries;
   /*
    * Minimum number of entries required for calibration.
    */
   unsigned int m_maxTimeDiff;
   /*
    * T0 used in data for testing.
    */
   double m_dataT0;
   /*
    * Take LHCb clock phase into account.
    */
   double m_useClockPhase;
   /*
    * Initial time
    */
   long long m_initialTime;
   /*
    * Check if our previous calibration was used in data.
    */
   bool m_checkDataT0;
   /*
    * Online condition path
    */
   std::string m_runCondPath;

   std::string m_xmlFilePath;
   std::string m_xmlFileName;

   std::string m_InputFileName;
   std::string m_InputFilePath;

   std::string m_pubString;
   std::string m_pubStatus;
   IPublishSvc* m_publishSvc; ///< Online Gaucho Monitoring Service

   //monitoring histogram
   AIDA::IHistogram1D* m_hist_aida;

   // T0 condition
   std::string m_conditionPath;
   Condition*  m_condition;
   
   // Synchronisation
   std::thread m_thread;
   Queue<std::pair<std::string, std::string>> m_queue;

   // Detector
   SmartIF<IIncidentSvc> m_incidentSvc;
   SmartIF<IDetDataSvc> m_detDataSvc;
   SmartIF<IEventProcessor> m_evtProc;

   std::set<unsigned int> m_calibratedRuns;
   std::map<unsigned int, std::vector<std::string>> m_calibrating;

   std::chrono::time_point<std::chrono::system_clock> m_start;

   static constexpr float phase_default = -1000.;

   // Class to listen to clock phase from DIM
   class ClockPhase : public DimInfo {
   public :
      ClockPhase()
         : DimInfo("LHCb/ClockPhase", phase_default),
           m_phase{0.} {}

      boost::optional<double> phase() const {
         boost::optional<double> r;
         if (m_phase != phase_default) {
            r = m_phase;
         }
         return r;
      }

   private:

      void infoHandler()
      {
         m_phase = getFloat();
      }

      float m_phase;
   };

   std::unique_ptr<ClockPhase> m_phase;
   std::unordered_map<unsigned int, double> m_phases;
   std::unordered_set<unsigned int> m_versions;

   boost::filesystem::path xmlFileName( FileVersion v ) const ;
   std::tuple<unsigned int, boost::optional<double>, double>
   readXML(const boost::filesystem::path& xmlFile);

   /* boost::filesystem::path writeXML(const FileVersion previous, const unsigned int run, */
   /*                                  const boost::optional<double> phase, */
   /*                                  const double t0); */
   boost::filesystem::path writeXML(const FileVersion previous, const boost::filesystem::path path,
				    const unsigned int run,
                                    const boost::optional<double> phase,
                                    const double t0);
   void fitHistogram(TH1D* hist, double& result, double& result_err, TFile* outFile);

   boost::optional<unsigned int> latestVersion() const;
   void publish(const std::vector<unsigned int> runs, const FileVersion version,
                const std::string status);
   std::unique_ptr<TH1D> getHistogram() const;

   // This one does all the work.
   StatusCode calibrate(std::string saveSet, std::string task);

};

#endif // OTt0OnlineClbr_H
