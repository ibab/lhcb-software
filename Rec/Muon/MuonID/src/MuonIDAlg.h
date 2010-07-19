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
#include "TMath.h"
#include "TRandom1.h"


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
  bool stInStations(const int myst,const std::vector<int>& stations);

  //Common IsMuon requirements from set of stations with hits in FOI
  bool IsMuon(const std::vector<int>& stations,const double& p, bool *w);

  // Common IsMuonLoose requirements from set of stations with hits in FOI
  bool IsMuonLoose(const std::vector<int>& stations,const double& p, bool *w);

  // Calculates MuProb based on DeltaSx (slope difference)
  double calcMuProb(LHCb::MuonPID * pMuid);

  /// return the FOI in x in a station and region for momentum (in MeV/c)
  double foiX(const int &station, const int &region, const double &p, const double &dx);
  /// return the FOI in y in a station and region for momentum (in MeV/c)
  double foiY(const int &station, const int &region, const double &p, const double &dy);

  /// clear track based local variables
  void resetTrackLocals();

  LHCb::Track* makeMuonTrack(const LHCb::MuonPID& pMuid);

  /// Load ImuIDTool on demand (avoid loading always in initialize())
  inline ImuIDTool* Chi2MuIDTool() 
  {
    if ( !m_Chi2MuIDTool ) m_Chi2MuIDTool = tool<ImuIDTool>(m_myMuIDTool,"myMuIDTool",this);
    return m_Chi2MuIDTool;
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

  /// Preselection momentum (no attempt to ID below this)
  double m_PreSelMomentum;

  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges

  /// muon and pion distributions
  std::vector<double> m_distPion;
  std::vector<double> m_distMuon;


  // GL&SF:
  bool m_weightFlag;// flag to introduce weights in IsMuon/IsMuonLoose
  int m_dllFlag;  // flag to discriminate among the different DLLs

  /// GL&SF: Calculate weights:
  void P_weights(const double& p, bool *w);

  /// GL&SF: Calculates the Distance Likelihood given a MuonPID
  StatusCode calcMuonLL_dist(LHCb::MuonPID* muonid, const double& p);

  /// GL&SF: Get closest hit per station
  StatusCode get_closest(LHCb::MuonPID *pMuid, double *closest_x, double *closest_y, double *closest_region);
  /// GL&SF: Find the region in a given station of teh extrapolated track
  int findTrackRegion(const int sta);
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

  // Number of bins for tanh(dist) histos
  int m_nDistBins;

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
  std::string m_myMuIDTool;

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
  int m_ntotmu;
  // Number of tracks with IsMuonLoose = true which failed likelihood construction
  int m_mullfail;
  // Number of stations
  int m_NStation;
  // Number of regions
  int m_NRegion;
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;

  // muonIDtool
  ImuIDTool* m_Chi2MuIDTool;

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
  std::vector<int> m_occupancy;
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
