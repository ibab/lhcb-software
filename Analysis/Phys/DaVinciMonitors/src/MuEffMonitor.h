
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
#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

// Tools for histogramming
//#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile2D.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// From the MuonDetector:
#include "Event/MuonCoord.h"

// from Event:
#include "Event/RecHeader.h"
#include "Event/ODIN.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
// Raw event:
#include "OTDAQ/IOTRawBankDecoder.h"
#include "L0Interfaces/IL0DUFromRawTool.h"

class DeMuonDetector;
class ITrackExtrapolator;
class ISequencerTimerTool;
class ITriggerTisTos;
class IMuonFastPosTool;

class MuEffMonitor : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  MuEffMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuEffMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // Job Options properties:

  bool            m_Offline;
  double          m_MomentumCut;    // Minimum momentum cut
  double          m_MomentumCutM4;  // Minimum momentum cut for M4
  double          m_MomentumCutM5;  // Minimum momentum cut for M5
  bool            m_useCalo;        // use calorimeters for selection
  double          m_EecalMax, m_EecalMin;  // Asymmetric Ecal Mip enregy window
  double          m_EhcalMax, m_EhcalMin;  // Asymmetric Hcal Mip enregy window
  
  std::vector<double> m_nSigma1X; /// X window hits search M1
  std::vector<double> m_nSigma1Y; /// Y window hits search M1 
  std::vector<double> m_nSigmaX; /// X window hits search M2-M5
  std::vector<double> m_nSigmaY; /// Y window hits search M2-M5

  int             m_TrminDof;
  double          m_nSigmaFidVol; /// fiducial volume cut (in sigma of extrap. track position) 
  double          m_Chi2TrMin;    /// chi2 prob cut on tracker tracks
  double          m_Chi2MuMin;    /// chi2/ndf cut global track to mu matching 
  double          m_Chi2OtherMuMin;  /// max chi2/ndf track to mu matching other stations
  double          m_Chi2MuGlobal; ///  max chi2/ndf for global candidates 
  double          m_nSigmaXother; /// X window constraint for other stations
  double          m_nSigmaYother; /// Y window constraint for other stations
  double          m_CosThetaCut;
  double          m_xyDistCut;
  bool            m_mustCross;
  int             m_nReqStations;
  bool            m_doTrigger;
  bool            m_TagUnBias;
  bool            m_ignoreM1;
  bool            m_measureTime;
  std::string     m_posToolName;

  int             m_origReqStations;

  std::string m_histoLevel;
  

  //=========
  // histos:
  //========= 

  AIDA::IHistogram1D *m_nEvts, *m_nTracks, *m_nLDTrks, *m_nTrksP;
  AIDA::IHistogram1D *m_nMuPreS, *m_nMuSel, *m_nMu, *m_nMuSelTIS;
  AIDA::IHistogram1D *m_TrkType, *m_Chi2Hits;
    
  AIDA::IHistogram1D *m_PpreSel, *m_PSel, *m_PPSel, *m_PNSel;
  
  AIDA::IHistogram1D *m_PPreS, *m_PPPreS, *m_PNPreS;
  AIDA::IHistogram1D *m_PPreSMIP, *m_PPPreSMIP, *m_PNPreSMIP;

  AIDA::IHistogram1D *m_PpreTis, *m_PpreTag, *m_PPTag, *m_PNTag;
  AIDA::IHistogram1D *m_PPTagCheck, *m_PNTagCheck;
  AIDA::IHistogram1D *m_PPTagCheckS, *m_PNTagCheckS;
  AIDA::IHistogram1D *m_Hcal, *m_Ecal;
  AIDA::IHistogram1D *m_Angolo, *m_DeltaP;
  AIDA::IHistogram1D *m_AngoloS, *m_DeltaPS;
  AIDA::IHistogram1D *m_Distanza, *m_DistanzaX, *m_DistanzaY;
  AIDA::IHistogram1D *m_DistanzaS, *m_DistanzaXS, *m_DistanzaYS;
  AIDA::IHistogram1D *m_SHcal, *m_SEcal;
  AIDA::IHistogram2D *m_ene, *m_Sene; 
  AIDA::IHistogram1D *m_trNdof,*m_trExtErrX,*m_trExtErrY;
  AIDA::IHistogram2D *m_trAccuracy;

  AIDA::IHistogram1D *m_PPre[5];
  AIDA::IHistogram1D *m_PPreMIP[5];

  AIDA::IHistogram1D *m_P[5];
  AIDA::IHistogram1D *m_PP[5];
  AIDA::IHistogram1D *m_PN[5];
  AIDA::IHistogram1D *m_P_hit[5];
  AIDA::IHistogram1D *m_PP_hit[5];
  AIDA::IHistogram1D *m_PN_hit[5];
  AIDA::IHistogram1D *m_OccuBck[5][4];
  AIDA::IHistogram1D *m_OccuMu[5][4];
  AIDA::IHistogram1D *m_StationsEff_den, *m_StationsEff_num;
  AIDA::IHistogram1D *m_StatEffNoMIP_den, *m_StatEffNoMIP_num;
  AIDA::IHistogram1D *m_StationsEff_denP, *m_StationsEff_numP, *m_StationsEff_denN, *m_StationsEff_numN;
  AIDA::IHistogram1D *m_RegionsEff_den, *m_RegionsEff_num;
  AIDA::IHistogram1D *m_RegEffNoMIP_den, *m_RegEffNoMIP_num;
  AIDA::IHistogram1D *m_RegionsEff_denP, *m_RegionsEff_numP, *m_RegionsEff_denN, *m_RegionsEff_numN;

  AIDA::IHistogram2D *m_hit_numP[5][4]; 
  AIDA::IHistogram2D *m_hit_numN[5][4]; 
  AIDA::IHistogram2D *m_hit_denN[5][4];
  AIDA::IHistogram2D *m_hit_denP[5][4];
  AIDA::IHistogram2D *m_hitsInW[5][4];
  AIDA::IHistogram1D *m_nDx[5][4];
  AIDA::IHistogram1D *m_nDy[5][4];
  AIDA::IHistogram1D *m_Chi2OtherHits[5][4];
  AIDA::IHistogram1D *m_chamberEff_den[5][4];
  AIDA::IHistogram1D *m_chamberEff_num[5][4];
  AIDA::IHistogram1D *m_chamberEffMIP_den[5][4];
  AIDA::IHistogram1D *m_chamberEffMIP_num[5][4];
  
  AIDA::IHistogram2D *m_TrkP[5][4]; 
  AIDA::IHistogram2D *m_TrkN[5][4]; 
  AIDA::IHistogram2D *m_TrkPMIP[5][4]; 
  AIDA::IHistogram2D *m_TrkNMIP[5][4]; 
  AIDA::IHistogram1D *m_chamberTrk[5][4];
  AIDA::IHistogram1D *m_chamberTrkMIP[5][4];

  //==================
  // global variables:
  //==================
  //
  
  // Trigger lines:
  std::string m_L0LinesNames;
  std::string m_Hlt1LinesNames;
  std::string m_Hlt2LinesNames;
 
  //--------------------------------
  // variables for the candidates:
  //--------------------------------

  bool m_muInCalo;
  long m_nTrk;
  std::vector<const LHCb::ProtoParticle*> m_preCandidate; 
  std::vector<const LHCb::State*> m_preCandState; 
  const LHCb::Track *m_pTrackTag;


  class MuCandidate { 
  public:
    MuCandidate() {}
    ~MuCandidate() {}
    void init() {
      ForseTag = false;
      TracciaIP = false;
      Eecal = -1000.;
      Ehcal = -1000.;
      nMatchedSt =0;
      Chisq =0.;
      for (int i=0; i<5; i++) {
        //VF[i] = false;
        //extrap[i] = false;
        hitInM[i] = false;
        hitInW[i] = 0;
        Dx[i] = -1000.;
        Dy[i] = -1000.;
        reg[i] = -1;
        ChisqM[i]=mtcSigmax[i]=mtcSigmay[i]=0.;
      }      
    }

    const LHCb::ProtoParticle* proto;
    const LHCb::Track *pTrack;
    const LHCb::State * stateP;
    
    bool   ForseTag;
    bool   TracciaIP;
    double Mom;
    double Mom0;
    double Eecal;    
    double Ehcal;     

    // These are indexed [station] <==>  Tracks extrapolated
    //bool   extrap[5];
    //bool   VF[5];
    double trackX[5];   // x(mm) position of track in each station
    double trackY[5];   // y(mm) position of track in each station
    double err2X[5];    // error on x(mm) position in each station
    double err2Y[5];    // error on y(mm) position in each station
    int    nMatchedSt;
    double Chisq;
    double ChisqM[5];
    double mtcSigmax[5];
    double mtcSigmay[5];
    double Dx[5];
    double Dy[5];
    bool   hitInM[5];
    double hitInW[5];
    int    reg[5]; 
  };
    
  MuCandidate m_Muon;

  //--------------------------
  // Muon Detector variables:
  //--------------------------
  int m_NStation;       /// Number of stations
  int m_NRegion;        /// Number of regions
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;
  IMuonFastPosTool* m_posTool;

  std::vector<double> m_stationZ; // station position
  std::vector<int> m_regionNch;   // number of chambers per region
  std::vector<int> m_regionNchCol;// number of chamber column per region
  // local array of pad sizes in mm
  // all std::vectors here are indexed: [station * m_NRegion + region]
  std::vector<double> m_padSizeX;
  std::vector<double> m_padSizeY;  
  // local array of region sizes
  std::vector<double> m_stationInnerX; // inner edge in abs(x)
  std::vector<double> m_stationOuterX; // outer edge in abs(x) 
  std::vector<double> m_stationInnerY; // inner edge in abs(y)
  std::vector<double> m_stationOuterY; // outer edge in abs(y) 

  // Store x,dx,y,dy of each coord 
  class coordExtent_{
  public:
    coordExtent_(double &x, double &dx, double &y, double &dy,  double &z, double &dz,
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

  ITriggerTisTos* m_TriggerTisTosTool;
  ITriggerTisTos* m_L0TriggerTisTosTool;

  const IOTRawBankDecoder* m_rawBankDecoder;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_l0BankDecoder;

  IRndmGenSvc* m_rsvc;
  Rndm::Numbers* m_rand;

  ISequencerTimerTool* m_timer;
  int m_timeExe;
  int m_timePrl;
  int m_timeFiC;
  int m_timePrt;
  int m_timePre;
  int m_timeSel;
  int m_timeCal;
  int m_timeIso;
  int m_timeExr;
  int m_timeMuM;
  int m_timeTis;
  int m_timeFil;
  int m_timeFnl;

  //------------
  // functions:
  //------------
  StatusCode LoadMuonGeometry();  
  StatusCode DoEvent();
  StatusCode lookTisTos(const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids, std::string &LineName
                                 , int &Hlttis );
  StatusCode lookL0TisTos(const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids, std::string &LineName
				   , int &L0tis );   
  bool DoTrackPreSelection(const LHCb::Track *pTrack, const LHCb::State * &muState);
  bool LookForTagMuon();
  bool DoTrackSelection();
  bool DoAccCheck();
  bool DoAccSel();
  bool DoHitsInPad();
  bool DoCaloMIP(const LHCb::ProtoParticle *pProto);
  bool isolatedTrack();
  bool estrapola();
  StatusCode fillCoordVectors();
  StatusCode LookForBck();
  StatusCode LookForMuOccu();
  void fillPreHistos();
  void fillHistos();
  int xy2Chamber(double &x, double &y, int &station, int &region);
};
#endif 
