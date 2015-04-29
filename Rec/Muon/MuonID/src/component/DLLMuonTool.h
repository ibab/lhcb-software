#ifndef DLLMUONTOOL_H_
#define DLLMUONTOOL_H_

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/DeMuonDetector.h"

#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/ICommonMuonTool.h"
#include "MakeMuonTool.h"

#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include <gsl/gsl_math.h>

static const InterfaceID IID_DLLMuonTool("DLLMuonTool", 1, 0);

/** @class DLLMuonTool DLLMuonTool.h
 * A tool that provides functionality for all steps in muon identification that
 *are only used offline.: e.g: DLL
 *
 * It is designed to have no state associated with the event. The only members
 *are related to conditions (such as detector information or FoI).
 *
 * @author Ricardo Vazquez Gomez
 * @date 2015-01-12
 */
class DLLMuonTool final : public GaudiTool {
 public:
  static const InterfaceID& interfaceID() { return IID_DLLMuonTool; }
  static const unsigned nStations = 5, nRegions = 4;
  using MuonTrackExtrapolation =
      std::array<std::pair<double, double>, nStations>;
  DLLMuonTool(const std::string& type, const std::string& name,
                 const IInterface* parent);
  ~DLLMuonTool() override = default;
  StatusCode initialize() override;
  bool calcNShared( LHCb::MuonPID* muonid, LHCb::MuonPIDs* pMuids, CommonConstMuonHits&, const MuonTrackExtrapolation&, std::map< LHCb::MuonPID*, CommonConstMuonHits >);
  double calcDist( const MuonTrackExtrapolation&, CommonConstMuonHits& ) const; 
  int GetPbin(double p, int region);
  std::vector<int> findTrackRegions(const MuonTrackExtrapolation&);
  double calc_closestDist(const LHCb::Track&, const MuonTrackExtrapolation&, CommonConstMuonHits&, std::array<unsigned, ICommonMuonTool::nStations>);
  std::vector<double> loadNonMuLandauParam(const MuonTrackExtrapolation&);
  double calc_ProbMu_tanh(const double& tanhdist0, int pBin, int region);
  double calc_ProbNonMu_tanh(const double& tanhdist0, int pBin, int region);
  double calc_ProbNonMu(double& dist, std::vector<double>& parNonMu);
  std::pair<double,double> calcMuonLL_tanhdist_landau(const LHCb::Track&, const MuonTrackExtrapolation&, CommonConstMuonHits&, std::array<unsigned, ICommonMuonTool::nStations>);
  std::pair<double,double> calcMuonLL_tanhdist(const LHCb::Track&, const MuonTrackExtrapolation&, CommonConstMuonHits&, std::array<unsigned, ICommonMuonTool::nStations>);
  
  StatusCode calcLandauNorm();
  double calcNorm_nmu(double *par);
  // compare the coordinates of two MuonPIDs 
  bool compareHits(LHCb::MuonPID* muonid1, LHCb::MuonPID* muonid2, std::map< LHCb::MuonPID*, CommonConstMuonHits >);
  std::map< LHCb::MuonPID*, CommonConstMuonHits > m_muonMap;
  // make muon track function
  LHCb::Track* makeMuonTrack(LHCb::MuonPID* muonid, std::map< LHCb::MuonPID*, CommonConstMuonHits >);


 private:
  // Ignore MuonID info from conditions database.
  bool m_OverrideDB;
  
  // Helper functions
  std::pair<double, double> foi(int, int, double) const;
  // Members
  CommonMuonHitManager* hitManager_;
  DeMuonDetector* det_;
  ICommonMuonTool* muonTool_;
  MakeMuonTool* makeMuonTool_;
  bool m_FindQuality;
  double foiFactor_, preSelMomentum_;
  std::array<double, nStations> stationZ_;
  std::array<std::pair<double, double>, nStations * nRegions> padSize_;
  std::array<std::pair<double, double>, nStations> regionInner_, regionOuter_;
  std::vector<double> momentumCuts_;
  std::vector<double> m_MupBinsR1;
  std::vector<double> m_MupBinsR2;
  std::vector<double> m_MupBinsR3;
  std::vector<double> m_MupBinsR4;
  
  std::vector< double >     m_NonMuLanParR1;
  std::vector< double >     m_NonMuLanParR2;
  std::vector< double >     m_NonMuLanParR3;
  std::vector< double >     m_NonMuLanParR4; 

  // hyperbolic tangent mapping of distances:
  // tanh scale factors for muons
  std::vector< double >  m_tanhScaleFactorsMuonR1;
  std::vector< double >  m_tanhScaleFactorsMuonR2;
  std::vector< double >  m_tanhScaleFactorsMuonR3;
  std::vector< double >  m_tanhScaleFactorsMuonR4;

  typedef std::vector< std::vector< double >* > vectorOfVectors;
  vectorOfVectors m_tanhScaleFactorsMuon;

  // hyperbolic tangent mapping of distances:
  // tanh scale factors for non-muons
  std::vector< double >  m_tanhScaleFactorsNonMuonR1;
  std::vector< double >  m_tanhScaleFactorsNonMuonR2;
  std::vector< double >  m_tanhScaleFactorsNonMuonR3;
  std::vector< double >  m_tanhScaleFactorsNonMuonR4;

  vectorOfVectors m_tanhScaleFactorsNonMuon;

  // tanh(dist2) histograms contents for muons
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

  // tanh(dist2) histograms contents for non-muons
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
  //std::vector< double >     m_tanhCumulHistoNonMuonR4_4;
  //std::vector< double >     m_tanhCumulHistoNonMuonR4_5;
  vectorOfVectors m_tanhCumulHistoNonMuonR4;

  std::vector< vectorOfVectors * > m_tanhCumulHistoNonMuon;

  double Fdist[5];
  double small_dist[5];
  double closest_region[5];
  double closest_x[5];
  double closest_y[5];

};

#endif  // DLLMUONTOOL_H_
