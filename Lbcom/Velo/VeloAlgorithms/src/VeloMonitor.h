#ifndef VELOMONITOR_H
#define VELOMONITOR_H 1


// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Event/MCVeloFE.h"
#include "Event/VeloFullDigit.h"
#include "Event/VeloCluster.h"

class IHistogram1D;
class IHistogram2D;

class VeloChannelID;
class VeloCluster;
class DeVelo;
//--------------------------------------------------------------------
//
//	VeloMonitor: simple example to illustrate the functionality
//                     of the Velo classes
//
//	Author :   Chris Parkes
//
//--------------------------------------------------------------------

class VeloMonitor : public Algorithm {

 public:
  // Constructor of this form must be provided
  VeloMonitor(const std::string& name, ISvcLocator* pSvcLocator); 

  // Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StatusCode checkTests();
  StatusCode bookHistograms();
  StatusCode retrieveData();
  StatusCode testMCVeloHit();
  StatusCode testPileUpMCVeloHit();
  StatusCode testMCVeloFE();
  StatusCode testVeloFullDigit();
  StatusCode testVeloCluster();
  StatusCode testDetElement();

  StatusCode resolution();

  StatusCode basicMonitor();

  StatusCode clusterType(VeloCluster* aCluster,bool& signal, bool& noise, 
                         bool& other);
  StatusCode FEType(MCVeloFE* FE,bool& signal, bool& noise, bool& other);

  VeloChannelID weightedMean(VeloCluster*, double &);

  StatusCode detElement();

  // histograms

  IHistogram1D*         m_Thomas1;
  IHistogram1D*         m_Thomas2;

  IHistogram1D*         m_MCVHEnergy;
  IHistogram2D*         m_MCVHEntryXY;
  IHistogram2D*         m_MCVHEntryZX;
  IHistogram2D*         m_MCVHExitXY;
  IHistogram2D*         m_MCVHExitZX;
  IHistogram2D*         m_MCVHSensorZ;
  IHistogram1D*         m_MCVHTOF;
  IHistogram1D*         m_MCVHNHits;

  IHistogram2D*         m_MCVHRRighXY;
  IHistogram2D*         m_MCVHRLeftXY;
  IHistogram2D*         m_MCVHPhiULXY;
  IHistogram2D*         m_MCVHPhiDLXY;
  IHistogram2D*         m_MCVHPhiURXY;
  IHistogram2D*         m_MCVHPhiDRXY;

  IHistogram1D*         m_MCPEnergy;

  IHistogram1D*         m_MCVHPEnergy;
  IHistogram2D*         m_MCVHPEntryXY;
  IHistogram2D*         m_MCVHPEntryZX;
  IHistogram2D*         m_MCVHPExitXY;
  IHistogram2D*         m_MCVHPExitZX;
  IHistogram2D*         m_MCVHPSensorZ;
  IHistogram1D*         m_MCVHPTOF;
  IHistogram1D*         m_MCVHPNHits;

  IHistogram1D*         m_MCPUPEnergy;

  IHistogram2D*         m_MCVFESensorStrip;
  IHistogram2D*         m_MCVFESensorStrip_s;
  IHistogram2D*         m_MCVFESensorStrip_n;
  IHistogram2D*         m_MCVFESensorStrip_o;
  IHistogram1D*         m_MCVFECharge;
  IHistogram1D*         m_MCVFECharge_s;
  IHistogram1D*         m_MCVFECharge_n;
  IHistogram1D*         m_MCVFECharge_o;
  IHistogram1D*         m_MCVFESignal;
  IHistogram1D*         m_MCVFESignal_s;
  IHistogram1D*         m_MCVFESignal_n;
  IHistogram1D*         m_MCVFESignal_o;
  IHistogram1D*         m_MCVFENoise;
  IHistogram1D*         m_MCVFENoise_s;
  IHistogram1D*         m_MCVFENoise_n;
  IHistogram1D*         m_MCVFENoise_o;
  IHistogram1D*         m_MCVFECMNoise;
  IHistogram1D*         m_MCVFEPedestal;
  IHistogram1D*         m_MCVFENHits;
  IHistogram1D*         m_MCVFEMCHitsSignal;
  IHistogram1D*         m_MCVFEMCHitsSignal_s;
  IHistogram1D*         m_MCVFEMCHitsSignal_n;
  IHistogram1D*         m_MCVFEMCHitsSignal_o;
  IHistogram1D*         m_MCVFEMCHitsTotalSignal;
  IHistogram1D*         m_MCVFEMCHitsTotalSignal_s;
  IHistogram1D*         m_MCVFEMCHitsTotalSignal_n;
  IHistogram1D*         m_MCVFEMCHitsTotalSignal_o;
  IHistogram1D*         m_MCVFEMCHits;
  IHistogram2D*         m_MCVFEZR0;
  IHistogram2D*         m_MCVFEZR1;
  IHistogram2D*         m_MCVFEZR2;
  IHistogram2D*         m_MCVFEZR3;
  IHistogram2D*         m_MCVFEZPhiInner;
  IHistogram2D*         m_MCVFEZPhiOuter;

  IHistogram2D*         m_VDSensorStrip;
  IHistogram1D*         m_VDNHits; 
  IHistogram1D*         m_VDADC;
  IHistogram1D*         m_VDRawADC;
  IHistogram1D*         m_VDPedestal;
  IHistogram1D*         m_VDCMNoise;
  IHistogram1D*         m_VDNoise;
  IHistogram1D*         m_VDRawNoise;

  IHistogram2D*         m_VCSensorStrip;
  IHistogram1D*         m_VCNHits; 
  IHistogram1D*         m_VCNHits_s;
  IHistogram1D*         m_VCNHits_n;
  IHistogram1D*         m_VCNHits_o;
  IHistogram1D*         m_VCADCSum;
  IHistogram1D*         m_VCADCSum_s;
  IHistogram1D*         m_VCADCSum_n;
  IHistogram1D*         m_VCADCSum_o;
  IHistogram1D*         m_VCSize;
  IHistogram1D*         m_VCSize_s;
  IHistogram1D*         m_VCSize_n;
  IHistogram1D*         m_VCSize_o;
  IHistogram2D*         m_VCZR0;
  IHistogram2D*         m_VCZR1;
  IHistogram2D*         m_VCZR2;
  IHistogram2D*         m_VCZR3;
  IHistogram2D*         m_VCZPhiInner;
  IHistogram2D*         m_VCZPhiOuter;

  IHistogram1D*         m_RRes;
  IHistogram1D*         m_RRes1s;
  IHistogram1D*         m_RRes2s;
  IHistogram1D*         m_RRes3s;
  IHistogram1D*         m_RCluster;
  IHistogram1D*         m_RResd;
  IHistogram1D*         m_PhiRes;
  IHistogram1D*         m_PhiResd;
  IHistogram1D*         m_RAngle;
  IHistogram1D*         m_RAngle1s;
  IHistogram1D*         m_RAngle2s;
  IHistogram1D*         m_RAngle3s;
  IHistogram2D*         m_RResAngle;
  IHistogram1D*         m_randomTest;

  IHistogram2D*         m_PhivsStrip;
  IHistogram2D*         m_RvsStrip;
  IHistogram2D*         m_PhiPlot;

  // test detector element
  // draw sensor
  IHistogram2D* m_detRRigh;
  IHistogram2D* m_detRLeft;
  IHistogram2D* m_detPhiUL;
  IHistogram2D* m_detPhiDL;
  IHistogram2D* m_detPhiUR;
  IHistogram2D* m_detPhiDR;
  // plot strip number
  IHistogram1D* m_stripRRigh;
  IHistogram1D* m_stripRLeft;
  IHistogram1D* m_stripPhiUL;
  IHistogram1D* m_stripPhiDL;
  IHistogram1D* m_stripPhiUR;
  IHistogram1D* m_stripPhiDR;

  // plot residuals
  IHistogram1D* m_resRRigh;
  IHistogram1D* m_resRLeft;
  IHistogram1D* m_resPhiUL;
  IHistogram1D* m_resPhiDL;
  IHistogram1D* m_resPhiUR;
  IHistogram1D* m_resPhiDR;
  // Radius of strip
  IHistogram1D* m_rOfStrips;

  Rndm::Numbers m_uniformDist;

  DeVelo* m_velo;

  MCVeloHits* m_mchits;
  MCVeloHits* m_pumchits;
  MCVeloFEs*  m_mcfes;
  VeloFullDigits* m_digits;
  VeloClusters* m_clusters;

  int m_printout;
  // control switches
  bool m_detailedMonitor;
  bool m_testMCVeloHit;
  bool m_testPUMCVeloHit;
  bool m_testMCVeloFE;
  bool m_testVeloFullDigit;
  bool m_testVeloCluster;
  bool m_resolution;
  bool m_detElement;
  bool m_2DHist;
 
  double m_nMCVH2;
  double m_nMCPH;
  double m_nMCPH2;
  double m_nMCFE;
  double m_nMCFE2;
  double m_nFD;
  double m_nFD2;
  double m_nVC;
  double m_nVC2;
  double m_nMCFEs;
  double m_nMCFEn;
  double m_nMCFEo;
  double m_nVCs;
  double m_nVCn;
  double m_nVCo;
  double m_nMCVH;

  double m_nMCFDs;
  double m_MCFD;
  double m_nMCFDn;
  double m_nMCFDo;
  double m_nMCVCs;
  double m_MCVC;
  double m_nMCVCn;
  double m_nMCVCo;
  double m_nMCFD;
  double m_nMCFD2;
  double m_nMCVC;
  double m_nMCVC2;

  int m_NEvent;
};


# endif    // HISTOGRAMS_VELOMONITOR_H

