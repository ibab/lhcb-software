#ifndef MUONIDALG_H 
#define MUONIDALG_H 1

// Include files
// from STL
#include <string>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "MuonDet/MuonBasicGeometry.h"

// From event packages
#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "Event/MuonCoord.h"

#include "MuonDet/DeMuonDetector.h"

#include "ImuIDTool.h"
#include "IIsMuonCandidateC.h"
#include "INShared.h"

#include "TMath.h"

class MuonPID;
class Track;
class MuonCoord;
class DeMuonDetector;

/** @class MuonIDAlg MuonIDAlg.h
 *
 *  This is an Algorithm to create MuonPID objects starting from tracks and
 *  using the hits in the muon system
 *
 *  @author Erica Polycarpo, Miriam Gandelman
 *  @date   20/03/2006
 *
 *
 */
class MuonIDAlg : public GaudiAlgorithm{
public:
  /// Standard constructor
  MuonIDAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonIDAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:

  /// Do the identification
  StatusCode doID(LHCb::MuonPID* pMuid);

  /// Calculates the Likelihood given a MuonPID
  StatusCode calcMuonLL( LHCb::MuonPID* muonid);

  /// Calculates the number of tracks that share hits
  StatusCode calcSharedHits( LHCb::MuonPID* muonid, LHCb::MuonPIDs* pMuids );

  /// Calculates the distance from the hit to the extrapolated position
  /// Input: a MuonPID object
  StatusCode calcDist( LHCb::MuonPID* muonid );

  /// Compare the coordinates of two MuonPIDs
  bool compareHits( LHCb::MuonPID* muonid1, LHCb::MuonPID* muonid2 );

  /// check the track is in the p and angle acceptance
  StatusCode preSelection(LHCb::MuonPID* pMuid, bool &passed);

  /// Fill the local vector of coord positions
  StatusCode fillCoordVectors();

  /// Empty the coord vectors
  void clearCoordVectors();

  /// Set the MuonCoords used in this ID
  StatusCode setCoords(LHCb::MuonPID *pMuid);

  /// Extract the momentum and extrapolate the track to each station
  StatusCode trackExtrapolate(const LHCb::Track *pTrack);

  // Find out if st myst is in input array of stations
  template <typename Container>
  bool stInStations(const int myst,const Container& stations) {
     return std::find(stations.begin(),stations.end(),myst)!=stations.end();
  }

  //Common IsMuon requirements from set of stations with hits in FOI
  bool IsMuon(const std::vector<int>& stations,double p);

  // Common IsMuonLoose requirements from set of stations with hits in FOI
  bool IsMuonLoose(const std::vector<int>& stations,double p);

  // Calculates MuProb based on DeltaSx (slope difference)
  double calcMuProb(LHCb::MuonPID * pMuid);

  /// return the FOI in (x,y) in a station and region for momentum (in MeV/c)
  std::pair<double,double> foiXY(int station, int region, double p) const;

  /// clear track based local variables
  void resetTrackLocals();

  LHCb::Track* makeMuonTrack(const LHCb::MuonPID& pMuid);

  /// Load ImuIDTool on demand (avoid loading always in initialize())  
  inline ImuIDTool* myMuIDTool() 
  {
    if ( !m_myMuIDTool ) m_myMuIDTool = tool<ImuIDTool>(m_myMuIDToolName,"myMuIDTool",this);
    return m_myMuIDTool;
  }
  
  /// Load ImuIDTool on demand (avoid loading always in initialize())
  inline ImuIDTool* DistMuIDTool() 
  {
    if ( !m_DistMuIDTool ) m_DistMuIDTool = tool<ImuIDTool>("DistMuIDTool","DistMuIDTool",this);
    return m_DistMuIDTool;
  }
  
  /// Load NShared on demand (avoid loading always in initialize())
  inline INShared* NSharedTool() 
  {
    if ( !m_NSharedTool ) m_NSharedTool = tool<INShared>("NShared","NSharedTool",this);
    return m_NSharedTool;
  }


  /// Load IsMuonTool on demand (avoid loading always in initialize())
  inline IIsMuonCandidateC* IsMuonTool() 
  {
    if ( !m_IsMuonTool ) m_IsMuonTool = tool<IIsMuonCandidateC>("IsMuonCandidateC","IsMuonTool",this);
    return m_IsMuonTool;
  }

  /// Load IsMuonLooseTool on demand (avoid loading always in initialize())
  inline IIsMuonCandidateC* IsMuonLooseTool() 
  {
    if ( !m_IsMuonLooseTool ) m_IsMuonLooseTool = tool<IIsMuonCandidateC>("IsMuonCandidateC","IsMuonLooseTool",this);
    return m_IsMuonLooseTool;
  }



  // Properties

  /// TES path of the tracks to analyse
  std::string m_TracksPath;

  bool m_useTtrack;

  /// TES path to output the MuonPIDs to
  std::string m_MuonPIDsPath;

  /// TES path to output the Track PIDs to
  std::string m_MuonTracksPath;

  /// TES path to output the all Track PIDs to
  std::string m_MuonTracksPathAll;

  /// Ignore MuonID info from conditions database.
  bool m_OverrideDB;

  /// use dist as muon quality (track extra info)
  bool m_use_dist;

  // Use or not uncrossed logical channels in the non-pad detector areas
  bool m_use_uncrossed;

  /// Preselection momentum (no attempt to ID below this)
  double m_PreSelMomentum;

  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges

  /// muon and pion distributions
  std::vector<double> m_distPion;
  std::vector<double> m_distMuon;
  
  // muon track for KalmanFoI
  LHCb::Track m_mutrack;

  // GL&SF:
  int m_dllFlag;  // flag to discriminate among the different DLLs

  // Use KalmanFoi?
  bool m_kalman_foi;

  /// GL&SF: Calculates the Distance Likelihood given a MuonPID
  StatusCode calcMuonLL_dist(LHCb::MuonPID* muonid, const double& p);

  /// GL&SF: Get closest hit per station
  StatusCode get_closest(LHCb::MuonPID *pMuid, double *closest_x, double *closest_y, double *closest_region);
  /// GL&SF&XCV: Find the region in a given station of teh extrapolated track
  std::vector<int> findTrackRegions();
  /// GL&SF: Calculate closest distance
  double calc_closestDist(LHCb::MuonPID *pMuid, const double& p, double *closest_region);
  /// GL&SF: Find parameters for hypothesis test
  StatusCode find_LandauParam(const double& p, const std::vector<int>& trackRegion, double *parMu, double *parNonMu);
  /// GL&SF: Calculate the compatibility with the Muon hypothesis
  double calc_ProbMu(const double& dist, const double *parMu);
  /// GL&SF: Calculate the compatibility with the Non-Muon hypothesis
  double calc_ProbNonMu(const double& dist, const double *parNonMu);

  /// GL&SF: Normalizations of the Landaus
  StatusCode calcLandauNorm();
  double calcNorm(double *par);
  double calcNorm_nmu(double *par);

  /// Return the momentum bin corresponding to p, given the region
  int GetPbin(double p, int region);

  /// Determine probabilities for DLL_flag=3
  StatusCode calcMuonLL_tanhdist(LHCb::MuonPID * pMuid, const double& p);
  double     calc_ProbMu_tanh(const double& tanhdist0, int pBin, int region);
  double     calc_ProbNonMu_tanh(const double& tanhdist0, int pBin, int region);

  /// Determine probabilities for DLL_flag=4 
  StatusCode calcMuonLL_tanhdist_landau(LHCb::MuonPID * pMuid, const double& p);
  
  double Fdist[5];
  double small_dist[5];
  double closest_region[5];
  double closest_x[5];
  double closest_y[5];


  /// GL&SF: define parameters for the hypothesis test

  std::vector<double> m_MupBinsR1;
  std::vector<double> m_MupBinsR2;
  std::vector<double> m_MupBinsR3;
  std::vector<double> m_MupBinsR4;
  int m_nMupBinsR1, m_nMupBinsR2, m_nMupBinsR3, m_nMupBinsR4;

  double m_parLandauMu;
  double m_parLandauNonMu;
  std::vector< double >     m_MuLanParR1_1;
  std::vector< double >     m_MuLanParR1_2;
  std::vector< double >     m_MuLanParR1_3;
  std::vector< double >     m_MuLanParR1_4;
  std::vector< double >     m_MuLanParR1_5;
  std::vector< double >     m_MuLanParR1_6;
  std::vector< double >     m_MuLanParR1_7;

  std::vector< double >     m_MuLanParR2_1;
  std::vector< double >     m_MuLanParR2_2;
  std::vector< double >     m_MuLanParR2_3;
  std::vector< double >     m_MuLanParR2_4;
  std::vector< double >     m_MuLanParR2_5;

  std::vector< double >     m_MuLanParR3_1;
  std::vector< double >     m_MuLanParR3_2;
  std::vector< double >     m_MuLanParR3_3;
  std::vector< double >     m_MuLanParR3_4;
  std::vector< double >     m_MuLanParR3_5;

  std::vector< double >     m_MuLanParR4_1;
  std::vector< double >     m_MuLanParR4_2;
  std::vector< double >     m_MuLanParR4_3;
  std::vector< double >     m_MuLanParR4_4;
  std::vector< double >     m_MuLanParR4_5;

  std::vector< double >     m_NonMuLanParR1;
  std::vector< double >     m_NonMuLanParR2;
  std::vector< double >     m_NonMuLanParR3;
  std::vector< double >     m_NonMuLanParR4;


  double m_x; // x-width for the integral
  int m_nMax;// number of steps

  // hyperbolic tangent mapping of distances:

  // tanh scale factors
  std::vector< double >  m_tanhScaleFactorsR1;
  std::vector< double >  m_tanhScaleFactorsR2;
  std::vector< double >  m_tanhScaleFactorsR3;
  std::vector< double >  m_tanhScaleFactorsR4;

  typedef std::vector< std::vector< double >* > vectorOfVectors;
  vectorOfVectors m_tanhScaleFactors;

  // tanh(dist2) histograms contents
  std::vector< double >     m_tanhCumulHistoMuonR1_1;
  std::vector< double >     m_tanhCumulHistoMuonR1_2;
  std::vector< double >     m_tanhCumulHistoMuonR1_3;
  std::vector< double >     m_tanhCumulHistoMuonR1_4;
  std::vector< double >     m_tanhCumulHistoMuonR1_5;
  std::vector< double >     m_tanhCumulHistoMuonR1_6;
  std::vector< double >     m_tanhCumulHistoMuonR1_7;
  vectorOfVectors m_tanhCumulHistoMuonR1;


  std::vector< double >     m_tanhCumulHistoMuonR2_1;
  std::vector< double >     m_tanhCumulHistoMuonR2_2;
  std::vector< double >     m_tanhCumulHistoMuonR2_3;
  std::vector< double >     m_tanhCumulHistoMuonR2_4;
  std::vector< double >     m_tanhCumulHistoMuonR2_5;
  vectorOfVectors m_tanhCumulHistoMuonR2;

  std::vector< double >     m_tanhCumulHistoMuonR3_1;
  std::vector< double >     m_tanhCumulHistoMuonR3_2;
  std::vector< double >     m_tanhCumulHistoMuonR3_3;
  std::vector< double >     m_tanhCumulHistoMuonR3_4;
  std::vector< double >     m_tanhCumulHistoMuonR3_5;
  vectorOfVectors m_tanhCumulHistoMuonR3;

  std::vector< double >     m_tanhCumulHistoMuonR4_1;
  std::vector< double >     m_tanhCumulHistoMuonR4_2;
  std::vector< double >     m_tanhCumulHistoMuonR4_3;
  std::vector< double >     m_tanhCumulHistoMuonR4_4;
  std::vector< double >     m_tanhCumulHistoMuonR4_5;
  vectorOfVectors m_tanhCumulHistoMuonR4;

  std::vector< vectorOfVectors * > m_tanhCumulHistoMuon;

  // tanh(dist2) histograms contents
  std::vector< double >     m_tanhCumulHistoNonMuonR1_1;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_2;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_3;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_4;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_5;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_6;
  std::vector< double >     m_tanhCumulHistoNonMuonR1_7;
  vectorOfVectors m_tanhCumulHistoNonMuonR1;


  std::vector< double >     m_tanhCumulHistoNonMuonR2_1;
  std::vector< double >     m_tanhCumulHistoNonMuonR2_2;
  std::vector< double >     m_tanhCumulHistoNonMuonR2_3;
  std::vector< double >     m_tanhCumulHistoNonMuonR2_4;
  std::vector< double >     m_tanhCumulHistoNonMuonR2_5;
  vectorOfVectors m_tanhCumulHistoNonMuonR2;

  std::vector< double >     m_tanhCumulHistoNonMuonR3_1;
  std::vector< double >     m_tanhCumulHistoNonMuonR3_2;
  std::vector< double >     m_tanhCumulHistoNonMuonR3_3;
  std::vector< double >     m_tanhCumulHistoNonMuonR3_4;
  std::vector< double >     m_tanhCumulHistoNonMuonR3_5;
  vectorOfVectors m_tanhCumulHistoNonMuonR3;

  std::vector< double >     m_tanhCumulHistoNonMuonR4_1;
  std::vector< double >     m_tanhCumulHistoNonMuonR4_2;
  std::vector< double >     m_tanhCumulHistoNonMuonR4_3;
  std::vector< double >     m_tanhCumulHistoNonMuonR4_4;
  std::vector< double >     m_tanhCumulHistoNonMuonR4_5;
  vectorOfVectors m_tanhCumulHistoNonMuonR4;

  std::vector< vectorOfVectors * > m_tanhCumulHistoNonMuon;

  //want to find quality?
  bool m_FindQuality;
  //Create container with all muonTracks (even if not in acceptance or !IsMuon)
  bool m_DoAllMuonTracks;

  //Which MuIDTool should be used
  std::string m_myMuIDToolName;

  // function that defines the field of interest size
  // formula is p(1) + p(2)*exp(-p(3)*momentum)
  std::vector< double >     m_xfoiParam1;
  std::vector< double >     m_xfoiParam2;
  std::vector< double >     m_xfoiParam3;
  std::vector< double >     m_yfoiParam1;
  std::vector< double >     m_yfoiParam2;
  std::vector< double >     m_yfoiParam3;

  // foifactor for MuonID calibration (in opts file, default = 1.)
  double m_foifactor;

  // Number of tracks with IsMuon = True (monitoring)
  int m_nmu;
  // Number of tracks with IsMuonLoose = true which failed likelihood construction
  int m_mullfail;
  // Number of stations
  int m_NStation;
  // Number of regions
  int m_NRegion;
  //Names of the station
  std::vector<std::string> m_stationNames;
  // Index of M2. Normally m_iM2=1. But m_iM2=0 in case of no M1. Assuming iM3=iM2+1
  int m_iM2;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;

  // myMuIDtool
  ImuIDTool* m_myMuIDTool;
  
  // DistMuonIDtool
  ImuIDTool* m_DistMuIDTool;

  // NSharedtool
  INShared* m_NSharedTool;

  // IsMuonTool
  IIsMuonCandidateC* m_IsMuonTool;

  // IsMuonLooseTool
  IIsMuonCandidateC* m_IsMuonLooseTool;

  // local array of pad sizes in mm
  // all std::vectors here are indexed: [station * m_NRegion + region]
  std::vector<double> m_padSizeX;
  std::vector<double> m_padSizeY;

  // local array of region sizes
  std::vector<double> m_regionInnerX; // inner edge in abs(x)
  std::vector<double> m_regionOuterX; // outer edge in abs(x)

  std::vector<double> m_regionInnerY; // inner edge in abs(y)
  std::vector<double> m_regionOuterY; // outer edge in abs(y)

  // These are indexed [station]
  //std::vector<double> m_stationZ; // station position
  double m_stationZ[5]; // station position

  // local track parameters: momentum and linear extrapolation to each station
  double m_dist;
  double m_dist_out;
  double m_Momentum; // in MeV/c
  double m_MomentumPre; // in MeV/c
  double m_trackSlopeX;
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station

  //test if found a hit in the MuonStations
  std::vector<int> m_occupancyAll;
  std::vector<int> m_occupancyWithCrossing;  // Requires x,y crossing for IsMuonTight
  std::map< LHCb::MuonPID*, std::vector<LHCb::MuonCoord*> > m_muonMap;

  // store X of hits for dx/dz matching with track (only need M2/M3)
  std::vector<double> m_CoordX;
  int m_xMatchStation; // first station to calculate slope (M2)

  // OK nasty optimisation here, store x,dx,y,dy of each coord to test against
  // track extrapolation
  class coordExtent_{
  public:
    coordExtent_(double x, double dx, double y, double dy, LHCb::MuonCoord *pCoord) :
      m_x(x), m_dx(dx), m_y(y), m_dy(dy), m_pCoord(pCoord)  {};
    double m_x;
    double m_dx;
    double m_y;
    double m_dy;
    LHCb::MuonCoord *m_pCoord;
  };

  // vector of positions of coords (innner vector coords,
  // outer is [station* m_NRegion + region ]
  std::vector<std::vector<coordExtent_> > m_coordPos;

};
#endif // MUONID_H
