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
#include "MuonID/CommonMuonTool.h"

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
  double calc_closestDist(const LHCb::Track&, const MuonTrackExtrapolation&, CommonConstMuonHits&, std::array<unsigned, CommonMuonTool::nStations>);
  std::pair<std::vector<double>,std::vector<double>> loadLandauParam(double p, const MuonTrackExtrapolation&);
  std::vector<double> loadNonMuLandauParam(const MuonTrackExtrapolation&);
  double calc_ProbMu_tanh(const double& tanhdist0, int pBin, int region);
  double calc_ProbNonMu(double& dist, std::vector<double>& parNonMu);
  std::pair<double,double> calcMuonLL_tanhdist_landau(const LHCb::Track&, const MuonTrackExtrapolation&, CommonConstMuonHits&, std::array<unsigned, CommonMuonTool::nStations>);
  StatusCode calcLandauNorm();
  double calcNorm_nmu(double *par);
  // compare the coordinates of two MuonPIDs 
  bool compareHits(LHCb::MuonPID* muonid1, LHCb::MuonPID* muonid2, std::map< LHCb::MuonPID*, CommonConstMuonHits >);
  //bool stInStations(const int myst,const std::vector<int>& stations);
  std::map< LHCb::MuonPID*, CommonConstMuonHits > m_muonMap;

 private:
  // Helper functions
  std::pair<double, double> foi(int, int, double) const;
  // Members
  CommonMuonHitManager* hitManager_;
  DeMuonDetector* det_;
  CommonMuonTool* muonTool_;
  double foiFactor_, preSelMomentum_;
  std::array<double, nStations> stationZ_;
  std::array<std::pair<double, double>, nStations * nRegions> padSize_;
  std::array<std::pair<double, double>, nStations> regionInner_, regionOuter_;
  std::array<std::vector<double>, 3> foiParamX_, foiParamY_;
  std::vector<double> momentumCuts_;
  //std::array<std::vector<const LHCb::MuonCoord*>, 5> m_coords;
  std::vector<double> m_MupBinsR1;
  std::vector<double> m_MupBinsR2;
  std::vector<double> m_MupBinsR3;
  std::vector<double> m_MupBinsR4;
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

  int m_nMupBinsR1, m_nMupBinsR2, m_nMupBinsR3, m_nMupBinsR4;
  double Fdist[5];
  double small_dist[5];
  double closest_region[5];
  double closest_x[5];
  double closest_y[5];

};

#endif  // DLLMUONTOOL_H_
