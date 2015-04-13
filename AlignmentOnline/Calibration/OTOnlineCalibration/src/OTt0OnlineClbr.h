#ifndef __OTt0OnlineClbr_h
#define __OTt0OnlineClbr_h

// STD & STL
#include <string>
#include <memory>
#include <fstream>
#include <iostream>

// BOOST
#include <boost/filesystem.hpp>

// Gaudi
#include <GaudiAlg/GaudiTupleAlg.h>
#include <GaudiKernel/ToolHandle.h>
#include <OTDet/DeOTDetector.h>

// OMALib
#include "OMAlib/AnalysisTask.h"

// ROOT 
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TString.h>

/** @class OTt0OnlineClbr OTt0OnlineClbr.h
 *
 *
 *  @author Lucia Grillo
 *  @date   2015-01-08
 */

class OTt0OnlineClbr : public AnalysisTask {

public:

   /// Standard constructor
   OTt0OnlineClbr( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~OTt0OnlineClbr( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode start     ();    ///< Algorithm start
   virtual StatusCode execute   ();    ///< Algorithm execution
   virtual StatusCode finalize  ();    ///< Algorithm finalization
   virtual StatusCode analyze   (std::string& SaveSet,
                                 std::string Task); ///< Algorithm analyze

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
    * path/names of histograms to perform online calibration on.
    */
   std::vector<std::string> m_histogramNames;
   /*
    * Minimum number of entries required for calibration.
    */
   unsigned int m_minEntries;

   std::string m_xmlFilePath;
   std::string m_xmlFileName;
   std::string m_CondStructure;

   std::string m_InputFileName;
   std::string m_InputFilePath;

   std::string m_pubString;
   std::string m_pubStatus;
   IPublishSvc* m_pPublishSvc; ///< Online Gaucho Monitoring Service

   std::set<unsigned int> m_calibratedRuns;
   std::map<unsigned int, std::vector<std::string>> m_calibrating;

   //=============================================================================
   // Function to get run number from fileName, rely on the name convention here:
   // https://lbtwiki.cern.ch/bin/view/Online/MonitoringExpertGuide#Savesets
   //=============================================================================
   inline unsigned int getRunNumber( std::string fileName )
   {
      std::string firstName = fileName.substr(fileName.find_first_of("-")+1);
      std::string runString = firstName.substr(0, firstName.find_first_of("-"));

      unsigned int runNumber = atoi( runString.c_str() );

      return runNumber;
   }

   boost::filesystem::path xmlFileName( FileVersion v ) const ;
   double readCondDB();
   std::pair<unsigned int, double> readXML(const boost::filesystem::path& xmlFile);

   boost::filesystem::path writeXML(FileVersion previous, unsigned int run, double global_t0);
   void fitHistogram(TH1D* hist, double& result, double& result_err, TFile* outFile);

   unsigned int latestVersion() const;
   void publish(const std::vector<unsigned int> runs, const FileVersion version,
                const std::string status);
   std::unique_ptr<TH1D> getHistogram() const;

};

#endif // OTt0OnlineClbr_H
