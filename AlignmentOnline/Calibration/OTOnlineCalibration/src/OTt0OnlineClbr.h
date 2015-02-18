#ifndef __OTt0OnlineClbr_h
#define __OTt0OnlineClbr_h

#include <GaudiAlg/GaudiTupleAlg.h>

#include <OTDet/DeOTDetector.h>

#include <GaudiKernel/ToolHandle.h>

// Include files
// from OMALib
#include "OMAlib/AnalysisTask.h"

//ROOT includes
#include <string>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TString.h>

DeOTDetector* detector;

#include <boost/filesystem.hpp>
#include <fstream>      // std::ofstream
#include <iostream>

//#include <string>
//#include <TH1D.h>
//#include <TF1.h>
//#include <TFile.h>
//#include <TString.h>
//
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
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  virtual StatusCode analyze   (std::string& SaveSet,                
				std::string Task); ///< Algorithm analyze

  typedef int FileVersion ;
  std::string xmldirname( FileVersion v ) const ;

  StatusCode readCondXMLs(double t0s[3][4][4][9], bool TestBeam = false); 
  StatusCode readCondDB(double t0s[3][4][4][9]);
  StatusCode readCondDB_Globalt0(double& read_t0);

 protected:
  
  //private:
  //if you want to write the functions you can do it here
  
 private:
  /**                                                                                                                       
   * Station names: T1, T2, T3.                                                                                             
   */
  std::vector<std::string> stationNames;
  /**                                                                                                                       
   * Layer names: X1, U, V, X2                                                                                              
   */
  std::vector<std::string> layerNames;
  /**                                                                                                                       
   * Quarter names: Q0, Q1, Q2, Q3.                                                                                         
   */
  std::vector<std::string> quarterNames;
  /**                                                                                                                       
   * Module names: M1, M2, ... , M9.                                                                                        
   */
  std::vector<std::string> moduleNames;

  DeOTDetector* detector;

  /**                                                                                                                                                    
   * Read condition XML files (default false).                                                                                                           
   */
  bool readXMLs;
  /**                                                                                                                                                    
   * Simulation or real data (default false, so data).                                                                                                   
   */
  bool simulation;
  /**                                                                                                                                                    
   * Verbose, gives a lot of output, could be useful for debugging purposes (default false).                                                             
   */
  bool verbose;
  /**                                                                                                                                                    
   * Apply_Calibration (default false, if set to true it writes the new XML files - will write DB - with the new, calibrated t0s).                       
   */
  bool Apply_Calibration;
  /**                                                                                                                                                    
   * save_fits (default false, I hope to mange to save the fits, ONLY FOR STUDIES).                       
   */
  bool save_fits;
  /**                                                                                                                                                    
   * fit_module_contributions (default it is true: as done until 2012: fitting 4 half mono layers to get a better shape for the fit.
   * Hopefully not necessary thanks to monolayer alingment    
   */
  bool fit_module_contributions;
  /**                                                                                                                                                    
   * getmean_instead_of_fit  
   */
  bool getmean_instead_of_fit;
  /**
   * calibrate_only_globalt0. This should become the default. We don't want to calibrate all the modules all the time, only global t0 
   */
  bool calibrate_only_globalt0;  
  /**
   * read_global_t0_from_db. At the moment option, online will be the only running way
   */
  bool read_global_t0_from_db;

  /*
   * if the |global t0| exceedes this threshold, we want to update the global t0 condition
   */
  double globalt0_threshold;
  /*
   * Run online or offline - the difference is that offline I read a test .root file,
   * while online it should get the last EoR file from the monitoring?
   */
  bool m_RunOnline;

  FileVersion m_version;

  TH1D* hdt0;
  TH1D* ht0;
  TH1D* hdt0proj;

  TH1D* m_histModuleDriftTimeResidual[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual01L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual01R[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual23L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual23R[3][4][4][9];

  std::string m_xmlFilePath;
  std::string m_xmlFileName;
  std::string m_CondStructure;

  std::string m_InputFileName;
  std::string m_InputFileName_2d;
  std::string m_InputFilePath;

  std::string m_EoRSignal;
  std::pair<unsigned int, std::string> m_mergedRun ;  // to store run number and file path
  std::pair<unsigned int, std::string> m_lastRun ;    // to store run number and file path of last run

  //TF1* m_myFunc;                                                                                                                                             
  StatusCode writeCondXMLs(double t0s[3][4][4][9]);
  StatusCode writeCondDBXMLs(double t0s[3][4][4][9]);
  StatusCode write_Globalt0_XML(double global_t0);
  StatusCode fit_single_hist(TH1D* hist,int s, int l, int q, int m, double& result, double& result_err, TFile* outFile);
  

  std::string m_PubString;

 private:
  IPublishSvc* m_pPublishSvc; ///< Online Gaucho Monitoring Service
  
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
  


};

#endif // OTt0OnlineClbr_H
