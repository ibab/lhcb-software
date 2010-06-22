// $Id: $
#ifndef MUEFFMONITOR_H
#define MUEFFMONITOR_H 1

/** @class MuEffMonitor MuEffMonitor.h
 *  monitoring of Muon Chamber Efficiency
 *
 *  @author P. DeSimone
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
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

#include "Event/MuonDigit.h"
#include "Event/IntLink.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
     
class MuonCoord;
class DeMuonDetector;
class ITrackExtrapolator;

class MuEffMonitor : public GaudiHistoAlg {
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

  AIDA::IHistogram1D *m_nEvts, *m_nHLT1mu, *m_nTracks, *m_nLDTrks, *m_nTrksP;
  AIDA::IHistogram1D *m_nMuPreS, *m_nMuSel, *m_nMu;
  AIDA::IHistogram1D *m_TrkType, *m_Chi2Hits;
  AIDA::IHistogram1D *m_PpreSel, *m_PSel;
  AIDA::IHistogram1D *m_Hcal, *m_Ecal, *m_Angolo, *m_Distanza;
  AIDA::IHistogram2D *m_ene, *m_DeltaP;
  
  AIDA::IHistogram1D *m_P_S1, *m_P_S2, *m_P_S3, *m_P_S4, *m_P_S5, *m_P_S;
  AIDA::IHistogram1D *m_P_S1hit, *m_P_S2hit, *m_P_S3hit, *m_P_S4hit, *m_P_S5hit, *m_P_Shit;
  AIDA::IHistogram1D *m_StationsEff_den, *m_StationsEff_num;
  AIDA::IHistogram1D *m_RegionsEff_den, *m_RegionsEff_num;

  //==================
  // global variables:
  //==================
  //
  // 1) header:
  // ----------
  double m_event, m_run, m_BX;
  
  // Trigger:
  int  m_L0Decision;
  int  m_HLTMuon;

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

  int m_SeleTIS;
  int m_SeleTOS;
  int m_SeleTOB;

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
  std::vector<float> m_P_chi2trk, m_P_trtype, m_P_trflag;
  std::vector<float> m_P_Mom, m_P_Mom0;
  std::vector<float> m_P_TrMom, m_P_TrMom0;
  std::vector<float> m_P_TrSlx, m_P_TrSlx0;
  std::vector<float> m_P_TrSly, m_P_TrSly0;

  std::vector<int> m_P_SeleTIS, m_P_SeleTOS, m_P_SeleTOB;

  std::vector<float> m_P_Xs1, m_P_Ys1;
  std::vector<float> m_P_Xs2, m_P_Ys2;
  std::vector<float> m_P_Xs3, m_P_Ys3;
  std::vector<float> m_P_Xs4, m_P_Ys4;
  std::vector<float> m_P_Xs5, m_P_Ys5;
  std::vector<float> m_P_Xp1, m_P_Yp1, m_P_Zp1;
  std::vector<float> m_P_Xp2, m_P_Yp2, m_P_Zp2;
  std::vector<float> m_P_Xp3, m_P_Yp3, m_P_Zp3;
  std::vector<float> m_P_Xp4, m_P_Yp4, m_P_Zp4;
  std::vector<float> m_P_Xp5, m_P_Yp5, m_P_Zp5;
  std::vector<int>  m_P_occu1;
  std::vector<int>  m_P_occu2;
  std::vector<int>  m_P_occu3;
  std::vector<int>  m_P_occu4;
  std::vector<int>  m_P_occu5; 
  std::vector<float> m_P_dist1, m_P_dist2, m_P_dist3, m_P_dist4, m_P_dist5;
  std::vector<float> m_P_xdist1, m_P_xdist2, m_P_xdist3, m_P_xdist4, m_P_xdist5;
  std::vector<float> m_P_ydist1, m_P_ydist2, m_P_ydist3, m_P_ydist4, m_P_ydist5;
  std::vector<float> m_P_Dx1, m_P_Dx2, m_P_Dx3, m_P_Dx4, m_P_Dx5;
  std::vector<float> m_P_Dy1, m_P_Dy2, m_P_Dy3, m_P_Dy4, m_P_Dy5;
  std::vector<float> m_P_dimx1, m_P_dimx2, m_P_dimx3, m_P_dimx4, m_P_dimx5;
  std::vector<float> m_P_dimy1, m_P_dimy2, m_P_dimy3, m_P_dimy4, m_P_dimy5;
  std::vector<float> m_P_Chisq1, m_P_Chisq2, m_P_Chisq3, m_P_Chisq4, m_P_Chisq5;
  std::vector<int>  m_P_Sample1, m_P_Sample2, m_P_Sample3, m_P_Sample4, m_P_Sample5;
  std::vector<int>  m_P_reg1, m_P_reg2, m_P_reg3, m_P_reg4, m_P_reg5;

  std::vector<float> m_P_Eecal, m_P_Ehcal;
  std::vector<float> m_P_Espd, m_P_Eprs;
  std::vector<float> m_P_Xecal, m_P_Yecal;
  std::vector<float> m_P_Xhcal, m_P_Yhcal;

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

  ITriggerTisTos* m_TriggerTisTosTool;

  // LHCbID hits associated to the Selected Track
  std::vector< LHCb::LHCbID > m_seleids;

  std::string m_extrapolatorName;
  ITrackExtrapolator* m_extrapolator; ///< extrapolator

  //------------
  // functions:
  //------------
  StatusCode LoadMuonGeometry();
  
  StatusCode DoEvent();
  StatusCode DoTrigger();
 
  void resetTrkVariables();
  
  StatusCode DoTrackSelection(bool &passed,const LHCb::Track *pTrack);

  void DoAccCheck(bool &passed,const LHCb::Track *pTrack);
  void DoHitsInPad(bool &passed);
  StatusCode DoCaloMIP(bool &passed,const LHCb::Track *pTrack);

  StatusCode fillCoordVectors();

  void ClearSeleVecs();
  void fillTrkVecs();
  void fillHistos();

};
#endif 
