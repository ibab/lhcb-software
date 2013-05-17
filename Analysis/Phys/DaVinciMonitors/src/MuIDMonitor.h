#ifndef MUIDMONITOR_H 
#define MUIDMONITOR_H 1

// from STL
#include <string>
#include <map>
#include <vector>
#include <cstdio>

// Boost
#include "boost/assign/list_of.hpp"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRegistry.h" // IOpaqueAddress
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IHistoTool.h"

// DaVinci
#include "Kernel/DaVinciHistoAlgorithm.h"

// Local
#include "BasePlotTool.h"

// From event packages
#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "Event/MuonCoord.h"
#include "Event/Particle.h"
#include "Event/ODIN.h"
#include "Event/MuonDigit.h"
#include "Event/IntLink.h"

// Muon Det
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"

// ROOT
#include "TMath.h"
#include "TH1D.h"
#include "TF1.h"
#include "TDirectory.h"

/** @class MuIDMonitor MuIDMonitor.h
 *
 *  This is an Algorithm to create MuonPID objects starting from tracks and
 *  using the hits in the muon system
 *
 *  @author A. Sarti & M. Palutan
 *  @date   10/10/2009
 *
 *
 */
class MuIDMonitor : public DaVinciHistoAlgorithm
{

public:
  /// Standard constructor
  MuIDMonitor( const std::string& name,
               ISvcLocator* pSvcLocator);

  virtual ~MuIDMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Select the track searching hit 2*foi in M2
  bool selectHitInFOI();

  /// Extract the momentum and extrapolate the track to each station
  StatusCode trackExtrapolate(const LHCb::Track *pTrack);

  /// Get closest hit per station
  StatusCode get_closest(const LHCb::MuonPID *pMuid, double *closest_x, double *closest_y, double *closest_region);

  /// Calculate closest distance
  double calc_closestDist(const LHCb::MuonPID *pMuid, const double& p, double *closest_region);

  double closest_region[5];
  double closest_x[5];
  double closest_y[5];
  double Fdist[5];
  double small_dist[5];

  /// return the FOI in x in a station and region for momentum (in MeV/c)
  double foiX(const int &station, const int &region, const double &p, const double &dx);
  /// return the FOI in y in a station and region for momentum (in MeV/c)
  double foiY(const int &station, const int &region, const double &p, const double &dy);

  /// clear track based local variables
  void resetTrackLocals();

  // Properties
  /// TES path to output the MuonPIDs to
  std::string m_MuonPIDsPath;

  /// TES path to output the Track PIDs to
  std::string m_MuonTracksPath;

  /// Preselection momentum (no attempt to ID below this)
  double m_PreSelMomentum;
  double m_MassMean, m_MassWin, m_EffWin;
  int m_JPAna, m_LMAna, m_hitInFoi;
  int m_extrFail;
  GaudiAlg::HistoBinEdges m_edgesJPX,m_edgesLMX;

  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges

  /// muon and pion distributions
  std::vector<double> m_distPion;
  std::vector<double> m_distMuon;

  //Mu probability
  float m_MuProb;
  double m_LklhMu,m_LklhPi;
  // function that defines the field of interest size
  // formula is p(1) + p(2)*exp(-p(3)*momentum)
  std::vector< double >     m_xfoiParam1;
  std::vector< double >     m_xfoiParam2;
  std::vector< double >     m_xfoiParam3;
  std::vector< double >     m_yfoiParam1;
  std::vector< double >     m_yfoiParam2;
  std::vector< double >     m_yfoiParam3;

  // foifactor for MuIDMonitor calibration (in opts file, default = 1.)
  double m_foifactor;

  // Number of stations
  int m_NStation;
  // Number of regions
  int m_NRegion;
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;

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
  int m_region;

  // local track parameters: momentum and linear extrapolation to each station
  double m_dist;
  double m_Momentum; // in MeV/c
  double m_MomM1; // in MeV/c
  double m_trackSlopeX;
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station

  //test if found a hit in the MuonStations
  std::vector<int> m_occupancy;
  std::map< const LHCb::MuonPID*, std::vector<LHCb::MuonCoord*> > m_muonMap;

  // store X of hits for dx/dz matching with track (only need M2/M3)
  std::vector<double> m_CoordX;

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
  std::vector<std::vector<double> > m_coordPosMonx;
  std::vector<std::vector<double> > m_coordPosMony;
  std::vector<std::vector<double> > m_coordPosMondx;
  std::vector<std::vector<double> > m_coordPosMondy;

};
#endif // MUIDMONITOR_H
