#ifndef __OTModuleClbrMon_h 
#define __OTModuleClbrMon_h 1

#include <GaudiAlg/GaudiTupleAlg.h>

#include <OTDet/DeOTDetector.h>

#include <GaudiKernel/ToolHandle.h>

#include <TrackInterfaces/ITrackFitter.h>
#include <TrackInterfaces/ITrackProjector.h>
#include <TrackInterfaces/ITrackExtrapolator.h>
#include <OTDAQ/IOTRawBankDecoder.h>

namespace LHCb { class Track; }

#include <string>
#include <TH1D.h>
#include <TF1.h>
#include <TFile.h>
#include <TString.h>

/** @class OTModuleClbrMon OTModuleClbrMon.h
 *
 *  This class implements monitoring for module T0's and TR-relation.
 *
 *  @author Alexandr Kozlinskiy
 *  @date   2010-04-12
 */
class OTModuleClbrMon: public GaudiTupleAlg
{
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
private:
  AIDA::IHistogram1D* histModuleDriftTime[3][4][4][9];
  AIDA::IHistogram1D* histQuarterDriftTime[3][4][4];
  AIDA::IHistogram1D* histLayerDriftTime[3][4];
  AIDA::IHistogram1D* histStationDriftTime[3];

  AIDA::IHistogram1D* histOTISResidual[3][4][4][9][4];
  AIDA::IHistogram2D* histOTISResidualVsHitY[3][4][4][9][4];

  AIDA::IHistogram1D* histModuleDriftTimeResidual01L[3][4][4][9]; // otis01 && distance < 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual01R[3][4][4][9]; // otis01 && distance > 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual23L[3][4][4][9]; // otis23 && distance < 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual23R[3][4][4][9]; // otis23 && distance > 0


  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis0_L[3][4][4][9]; // otis0 && distance < 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis1_L[3][4][4][9]; // otis1 && distance < 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis2_L[3][4][4][9]; // otis2 && distance < 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis3_L[3][4][4][9]; // otis3 && distance < 0

  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis0_R[3][4][4][9]; // otis0 && distance > 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis1_R[3][4][4][9]; // otis1 && distance > 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis2_R[3][4][4][9]; // otis2 && distance > 0
  AIDA::IHistogram1D* histModuleDriftTimeResidual_otis3_R[3][4][4][9]; // otis3 && distance > 0

  AIDA::IHistogram1D* histModuleDriftTimeResidual[3][4][4][9];
  AIDA::IHistogram1D* histQuarterDriftTimeResidual[3][4][4];
  AIDA::IHistogram1D* histLayerDriftTimeResidual[3][4];
  AIDA::IHistogram1D* histStationDriftTimeResidual[3];

  AIDA::IHistogram1D* histLayerDriftTimeResidualClbr[3][4][16];

  AIDA::IHistogram2D* histModuleDriftTimeVsDist[3][4][4][9];

  AIDA::IHistogram1D* histDriftTime;

  AIDA::IHistogram2D* histDriftTimeVsDist;
  AIDA::IHistogram2D* histDriftTimeVsDistGood;
  AIDA::IHistogram2D* histDriftTimeVsDist17Good;
  AIDA::IHistogram2D* histDriftTimeVsDist89Good;

  AIDA::IHistogram2D* histDriftTimeResidualVsHitY;
  AIDA::IHistogram2D* histWalkVsHitStrawY;
  AIDA::IHistogram2D* histDriftTimeResidualVsHitYGood;
  AIDA::IHistogram2D* histWalkVsHitStrawYGood;

  AIDA::IHistogram1D* histTrackErrorGood;
  AIDA::IHistogram1D* histTrackError17Good;
  AIDA::IHistogram1D* histTrackError89Good;

  AIDA::IHistogram2D* histResidualPullVsDist17Good;
  AIDA::IHistogram2D* histResidualPullVsDist89Good;

//  AIDA::IHistogram2D* histDriftTimeVsDistGood50;
//  AIDA::IHistogram2D* histDriftTimeVsDistGood25;
private:
  DeOTDetector* detector;

  ToolHandle<ITrackFitter> fitter;
  ToolHandle<ITrackProjector> projector;
  ToolHandle<IOTRawBankDecoder> decoder;

  std::string trackLocation;

  /**
   * Options for drift time histograms (default t = [-30, 70]/200).
   */
  std::vector<double> histDriftTimeOpts;
  /**
   * Options for drift time residual histograms (default t = [-50, 50]/400).
   */
  std::vector<double> histDriftTimeResidualOpts;
  /**
   * Options for drift time VS distance histograms (default r = [-5, 5]/200 and t = [-30, 70]/200).
   */
  std::vector<double> histDriftTimeVsDistOpts;
  /**
   * Options for residual histograms (default r = [-10, 10]/200).
   */
  std::vector<double> histResidualOpts;

  /**
   * Max track Chi^2 (default 16).
   */
  double maxTrackChi2;
  /**
   * Max track unbiased Chi^2 (default 4).
   */
  double maxTrackUbChi2;

  double minNDF;
  double minP;
  int trackType;

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
   * OTIS calibration (instead of 4 sub contributions per module)               
   */
  bool OTIS_calibration;
  /**                                                                                                                                                            
   * OTIS calibration separating left and right tracks               
   */
  bool OTIS_LR_calibration;


public:
  OTModuleClbrMon(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  /**
   * Read condition XMLs.
   */
  StatusCode readCondXMLs(double t0s[3][4][4][9]);

  /**
   * Read condition XMLs form DB.
   */
  StatusCode readCondDB(double t0s[3][4][4][9]);
  /**
   * Read global t0 from online partition of offline DB.
   */
  StatusCode read_Globalt0(double t0s[3][4][4][9]);

 private:

  TH1D* hdt0;
  TH1D* ht0;
  TH1D* hdt0proj;
  
  TH1D* m_histModuleDriftTimeResidual[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual01L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual01R[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual23L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual23R[3][4][4][9];

  TH1D* m_histModuleDriftTimeResidual_otis0_L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis1_L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis2_L[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis3_L[3][4][4][9];

  TH1D* m_histModuleDriftTimeResidual_otis0_R[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis1_R[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis2_R[3][4][4][9];
  TH1D* m_histModuleDriftTimeResidual_otis3_R[3][4][4][9];

  std::string m_xmlFilePath;
  std::string m_xmlFileName;  

  //TF1* m_myFunc;                                                                  
  StatusCode writeCondXMLs(double t0s[3][4][4][9]);
  StatusCode writeCondXMLs(double t0s[3][4][4][9][4]);
  StatusCode writeCondDBXMLs(double t0s[3][4][4][9]);
  StatusCode fit_single_hist(TH1D* hist,int s, int l, int q, int m, double& result);
  StatusCode fit_single_hist(TH1D* hist,int s, int l, int q, int m, std::string contr, double& result, double& result_err, TFile* outFile);

};



#endif // __OTModuleClbrMon_h
