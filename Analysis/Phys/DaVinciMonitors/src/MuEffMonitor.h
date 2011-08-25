// $Id: $
#ifndef MUEFFMONITOR_H
#define MUEFFMONITOR_H 1

/** @class MuEffMonitor MuEffMonitor.h
 *  monitoring of Muon Chamber Efficiency
 *
 *  @author P. de Simone
 *  @date   2010-06-16
 */

#include <cstdio>
//#include <string>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h" // IOpaqueAddress
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tools for histogramming
#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile2D.h"

#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IANNSvc.h"
#include "Kernel/ReadRoutingBits.h"

// From the MuonDetector:
#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MuonCoord.h"
#include "MuonDet/DeMuonDetector.h"

// from Event:
#include "Event/ODIN.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

#include "Event/L0DUReport.h"
#include "Event/HltSelReports.h"
#include "Event/HltObjectSummary.h"
#include "Event/HltDecReports.h"

#include "Event/MuonDigit.h"
#include "Event/IntLink.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackChi2Calculator.h"

class MuonCoord;
class DeMuonDetector;
class ITrackExtrapolator;

class MuEffMonitor : public DVAlgorithm {
public: 
  /// Standard constructor
  MuEffMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuEffMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Job Options properties:

  bool            m_notOnline;
  bool            m_DoTrigger;    // apply trigger
  double          m_MomentumCut;  // Minimum momentum cut
  double          m_EecalMax, m_EecalMin;  // Asymmetric Ecal Mip enregy window
  double          m_EhcalMax, m_EhcalMin;  // Asymmetric Hcal Mip enregy window
  
  std::vector<double> m_nSigmaX; /// X window hits search
  std::vector<double> m_nSigmaY; /// Y window hits search

  double          m_Chi2Min;
  double          m_CosThetaCut;
  double          m_xyDistCut;
  double          m_PCutEff;

  std::string m_histoLevel;
  
  std::vector<std::string> m_containers ; ///< Containers for associators


  //=========
  // histos:
  //========= 

  AIDA::IHistogram1D *m_nEvts, *m_nTracks, *m_nLDTrks, *m_nTrksP;
  AIDA::IHistogram1D *m_nMuPreS, *m_nMuSel, *m_nMu;
  AIDA::IHistogram1D *m_TrkType, *m_Chi2Hits;
  AIDA::IHistogram1D *m_PpreSel, *m_PSel;
  AIDA::IHistogram1D *m_Hcal, *m_Ecal, *m_Angolo, *m_Distanza;
  AIDA::IHistogram1D *m_SHcal, *m_SEcal;
  AIDA::IHistogram2D *m_ene, *m_Sene, *m_DeltaP;
  
  AIDA::IHistogram1D *m_P_S1, *m_P_S2, *m_P_S3, *m_P_S4, *m_P_S5;
  AIDA::IHistogram1D *m_PP_S1, *m_PP_S2, *m_PP_S3, *m_PP_S4, *m_PP_S5;
  AIDA::IHistogram1D *m_PN_S1, *m_PN_S2, *m_PN_S3, *m_PN_S4, *m_PN_S5;
  AIDA::IHistogram1D *m_P_S1hit, *m_P_S2hit, *m_P_S3hit, *m_P_S4hit, *m_P_S5hit;
  AIDA::IHistogram1D *m_PP_S1hit, *m_PP_S2hit, *m_PP_S3hit, *m_PP_S4hit, *m_PP_S5hit;
  AIDA::IHistogram1D *m_PN_S1hit, *m_PN_S2hit, *m_PN_S3hit, *m_PN_S4hit, *m_PN_S5hit;
  AIDA::IHistogram1D *m_StationsEff_den, *m_StationsEff_num;
  AIDA::IHistogram1D *m_StationsEff_denP, *m_StationsEff_numP, *m_StationsEff_denN, *m_StationsEff_numN;
  AIDA::IHistogram1D *m_RegionsEff_den, *m_RegionsEff_num;
  AIDA::IHistogram1D *m_RegionsEff_denP, *m_RegionsEff_numP, *m_RegionsEff_denN, *m_RegionsEff_numN;

  AIDA::IHistogram2D *m_M1R1_hitP, *m_M1R2_hitP, *m_M1R3_hitP, *m_M1R4_hitP;
  AIDA::IHistogram2D *m_M1R1_hitN, *m_M1R2_hitN, *m_M1R3_hitN, *m_M1R4_hitN;
  AIDA::IHistogram2D *m_M2R1_hitP, *m_M2R2_hitP, *m_M2R3_hitP, *m_M2R4_hitP;
  AIDA::IHistogram2D *m_M2R1_hitN, *m_M2R2_hitN, *m_M2R3_hitN, *m_M2R4_hitN;
  AIDA::IHistogram2D *m_M3R1_hitP, *m_M3R2_hitP, *m_M3R3_hitP, *m_M3R4_hitP;
  AIDA::IHistogram2D *m_M3R1_hitN, *m_M3R2_hitN, *m_M3R3_hitN, *m_M3R4_hitN;
  AIDA::IHistogram2D *m_M4R1_hitP, *m_M4R2_hitP, *m_M4R3_hitP, *m_M4R4_hitP;
  AIDA::IHistogram2D *m_M4R1_hitN, *m_M4R2_hitN, *m_M4R3_hitN, *m_M4R4_hitN;
  AIDA::IHistogram2D *m_M5R1_hitP, *m_M5R2_hitP, *m_M5R3_hitP, *m_M5R4_hitP;
  AIDA::IHistogram2D *m_M5R1_hitN, *m_M5R2_hitN, *m_M5R3_hitN, *m_M5R4_hitN;

  AIDA::IHistogram2D *m_M1R1_nohitP, *m_M1R2_nohitP, *m_M1R3_nohitP, *m_M1R4_nohitP;
  AIDA::IHistogram2D *m_M1R1_nohitN, *m_M1R2_nohitN, *m_M1R3_nohitN, *m_M1R4_nohitN;
  AIDA::IHistogram2D *m_M2R1_nohitP, *m_M2R2_nohitP, *m_M2R3_nohitP, *m_M2R4_nohitP;
  AIDA::IHistogram2D *m_M2R1_nohitN, *m_M2R2_nohitN, *m_M2R3_nohitN, *m_M2R4_nohitN;
  AIDA::IHistogram2D *m_M3R1_nohitP, *m_M3R2_nohitP, *m_M3R3_nohitP, *m_M3R4_nohitP;
  AIDA::IHistogram2D *m_M3R1_nohitN, *m_M3R2_nohitN, *m_M3R3_nohitN, *m_M3R4_nohitN;
  AIDA::IHistogram2D *m_M4R1_nohitP, *m_M4R2_nohitP, *m_M4R3_nohitP, *m_M4R4_nohitP;
  AIDA::IHistogram2D *m_M4R1_nohitN, *m_M4R2_nohitN, *m_M4R3_nohitN, *m_M4R4_nohitN;
  AIDA::IHistogram2D *m_M5R1_nohitP, *m_M5R2_nohitP, *m_M5R3_nohitP, *m_M5R4_nohitP;
  AIDA::IHistogram2D *m_M5R1_nohitN, *m_M5R2_nohitN, *m_M5R3_nohitN, *m_M5R4_nohitN;

  //==================
  // global variables:
  //==================
  //
  // 1) header:
  // ----------
  ulonglong m_event;
  unsigned int m_run, m_BX;
  
  // Trigger lines:

  std::vector<std::string> m_L0LinesNames;
  std::vector<std::string> m_Hlt1LinesNames;
  std::vector<std::string> m_Hlt2LinesNames;
  int m_L0Tis;
  int m_Hlt1Tis;
  int m_Hlt2Tis; 
 
  //--------------------------------
  // variables for the candidates:
  //--------------------------------

  long   m_nTrk; 
  int    m_FlagMCan;
  
  int    m_carica;
  double m_trtype;
  double m_trflag;
  double m_chi2trk;
  
  double m_Mom;
  double m_Mom0;
  double m_TrMom;
  double m_TrMom0;
  double m_TrSlx;
  double m_TrSly;
  double m_TrSlx0;
  double m_TrSly0;

  // risultato del fit lineare   
  double m_sx, m_sy;
  double m_bx, m_by;
  double m_errbx, m_errsx, m_covbsx;
  double m_errby, m_errsy, m_covbsy;
  double m_chi2x, m_chi2y;
  double m_MatchChi2;

 // These are indexed [station] <==>  Tracks extrapolated
  double m_trackX[5];   // x(mm) position of track in each station
  double m_trackY[5];   // y(mm) position of track in each station
  double m_err2X[5];    // error on x(mm) position in each station
  double m_err2Y[5];    // error on y(mm) position in each station
  double m_trackPX[5];  // x momentum (GeV) of track in each station
  double m_trackPY[5];  // y momentum (GeV) of track in each station
  double m_trackPZ[5];  // z momentum (GeV) of track in each station
  int    m_occupancy[5];
  double m_Chisq[5];
  double m_xdista[5];
  double m_ydista[5];
  double m_Dxa[5];
  double m_Dya[5];
  double m_dimx[5];
  double m_dimy[5];
  int    m_reg[5];
  int    m_Sample[5];
 
  // Calo variables:
  double m_Eecal;
  double m_Ehcal;
  double m_Espd;
  double m_Eprs;
  double m_Xecal;
  double m_Yecal;
  double m_Xhcal;
  double m_Yhcal;

  //-------------------------
  // Arrays of the products:
  //-------------------------
  std::vector<int>   m_P_FlagMCan;
  std::vector<int>   m_P_carica;
  std::vector<double> m_P_chi2trk, m_P_trtype, m_P_trflag;
  std::vector<double> m_P_Mom, m_P_Mom0;
  std::vector<double> m_P_TrMom, m_P_TrMom0;
  std::vector<double> m_P_TrSlx, m_P_TrSlx0;
  std::vector<double> m_P_TrSly, m_P_TrSly0;

  std::vector<float> m_P_sx, m_P_sy;
  std::vector<float> m_P_bx, m_P_by;
  std::vector<float> m_P_errbx, m_P_errsx, m_P_covbsx;
  std::vector<float> m_P_errby, m_P_errsy, m_P_covbsy;
  std::vector<float> m_P_chi2x, m_P_chi2y;
  std::vector<float> m_P_MatchChi2;

  std::vector<double> m_P_Xs1, m_P_Ys1;
  std::vector<double> m_P_Xs2, m_P_Ys2;
  std::vector<double> m_P_Xs3, m_P_Ys3;
  std::vector<double> m_P_Xs4, m_P_Ys4;
  std::vector<double> m_P_Xs5, m_P_Ys5;
  std::vector<double> m_P_Xp1, m_P_Yp1, m_P_Zp1;
  std::vector<double> m_P_Xp2, m_P_Yp2, m_P_Zp2;
  std::vector<double> m_P_Xp3, m_P_Yp3, m_P_Zp3;
  std::vector<double> m_P_Xp4, m_P_Yp4, m_P_Zp4;
  std::vector<double> m_P_Xp5, m_P_Yp5, m_P_Zp5;
  std::vector<int>  m_P_occu1;
  std::vector<int>  m_P_occu2;
  std::vector<int>  m_P_occu3;
  std::vector<int>  m_P_occu4;
  std::vector<int>  m_P_occu5; 
  std::vector<double> m_P_dist1, m_P_dist2, m_P_dist3, m_P_dist4, m_P_dist5;
  std::vector<double> m_P_xdist1, m_P_xdist2, m_P_xdist3, m_P_xdist4, m_P_xdist5;
  std::vector<double> m_P_ydist1, m_P_ydist2, m_P_ydist3, m_P_ydist4, m_P_ydist5;
  std::vector<double> m_P_Dx1, m_P_Dx2, m_P_Dx3, m_P_Dx4, m_P_Dx5;
  std::vector<double> m_P_Dy1, m_P_Dy2, m_P_Dy3, m_P_Dy4, m_P_Dy5;
  std::vector<double> m_P_dimx1, m_P_dimx2, m_P_dimx3, m_P_dimx4, m_P_dimx5;
  std::vector<double> m_P_dimy1, m_P_dimy2, m_P_dimy3, m_P_dimy4, m_P_dimy5;
  std::vector<double> m_P_Chisq1, m_P_Chisq2, m_P_Chisq3, m_P_Chisq4, m_P_Chisq5;
  std::vector<int>  m_P_Sample1, m_P_Sample2, m_P_Sample3, m_P_Sample4, m_P_Sample5;
  std::vector<int>  m_P_reg1, m_P_reg2, m_P_reg3, m_P_reg4, m_P_reg5;

  std::vector<double> m_P_Eecal, m_P_Ehcal;
  std::vector<double> m_P_Espd, m_P_Eprs;
  std::vector<double> m_P_Xecal, m_P_Yecal;
  std::vector<double> m_P_Xhcal, m_P_Yhcal;

  std::vector<int> m_P_L0Tis; 
  std::vector<int> m_P_Hlt1Tis;
  std::vector<int> m_P_Hlt2Tis; 

  //--------------------------
  // Muon Detector variables:
  //--------------------------
  
  double m_momentum0;
  const LHCb::State * m_stateP;
  const LHCb::State * m_stateP0;

  int m_NStation;       /// Number of stations
  int m_NRegion;        /// Number of regions
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;

  std::vector<double> m_stationZ; // station position

  // local array of pad sizes in mm
  // all std::vectors here are indexed: [station * m_NRegion + region]
  std::vector<double> m_padSizeX;
  std::vector<double> m_padSizeY;
  
  // local array of region sizes
  std::vector<double> m_regionInnerX; // inner edge in abs(x)
  std::vector<double> m_regionOuterX; // outer edge in abs(x) 
  std::vector<double> m_regionInnerY; // inner edge in abs(y)
  std::vector<double> m_regionOuterY; // outer edge in abs(y) 

  // Store x,dx,y,dy of each coord 
  class coordExtent_{
  public:
    coordExtent_(double x, double dx, double y, double dy,  double z, double dz,
    LHCb::MuonCoord *pCoord) :
    m_x(x), m_dx(dx), m_y(y), m_dy(dy), m_z(z), m_dz(dz), m_pCoord(pCoord)  {};
    double m_x;
    double m_dx;
    double m_y;
    double m_dy;
    double m_z;
    double m_dz;
    LHCb::MuonCoord *m_pCoord;
  };
  // vector of positions of coords (innner vector coords, 
  // outer is [station* m_NRegion + region ]
  std::vector<std::vector<coordExtent_> > m_coordPos;

  // LHCbID hits associated to the Selected Track
  std::vector< LHCb::LHCbID > m_seleids;

  std::string m_extrapolatorName;
  ITrackExtrapolator* m_extrapolator; ///< extrapolator

  std::string m_nChi2Calculator;
  ITrackChi2Calculator* m_chi2Calculator;

  ITriggerTisTos* m_TriggerTisTosTool;
  ITriggerTisTos* m_L0TriggerTisTosTool;

  //------------
  // functions:
  //------------
  StatusCode LoadMuonGeometry();
  
  StatusCode DoEvent();

  StatusCode lookTisTos(const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids, const std::vector<std::string> Lines
                                 , int &Hlttis );

  StatusCode lookL0TisTos(const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids, const std::vector<std::string> Lines
				   , int &L0tis );
 
  void resetTrkVariables();
  
  bool DoTrackSelection(const LHCb::Track *pTrack);

  bool DoAccCheck();
  bool DoHitsInPad();
  bool DoCaloMIP(const LHCb::Track *pTrack);
  bool estrapola(const LHCb::Track *pTrack);

  StatusCode fillCoordVectors();
  void linFit(bool &passed);

  void ClearSeleVecs();
  void fillTrkVecs();
  void fillHistos();

};
#endif 
