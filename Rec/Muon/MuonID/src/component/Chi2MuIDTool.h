#ifndef CHI2MUIDTOOL_H
#define CHI2MUIDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "ICLTool.h"
#include "IGetArrival.h"
#include "ImuIDTool.h"
#include "IIsMuonCandidateC.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MuonPID.h"
#include "Event/Track.h"

#include "MuonDet/DeMuonDetector.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "SmartMuonMeasProvider.h"


/** @class Chi2MuIDTool Chi2MuIDTool.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */
class Chi2MuIDTool : public extends1<GaudiTool, ImuIDTool> {
public:

  /// Standard constructor
  Chi2MuIDTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  ~Chi2MuIDTool( ) override; ///< Destructor

  StatusCode initialize() override;


  StatusCode isGoodSeed(const LHCb::Track& seed) override;

  StatusCode muonCandidate(const LHCb::Track& seed, LHCb::Track& muTrack,
                           const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>()) override;

  StatusCode muonQuality(LHCb::Track& muTrack, double& Quality) override;

  StatusCode muonArrival(const LHCb::Track& muTrack, double& Arrival) const override;

  StatusCode muonDLL(const LHCb::Track& muTrack, const double& Quality, double& CLQuality,
                     const double& Arrival, double& CLArrival, double& DLL) const override;




  StatusCode muonID(const LHCb::Track& seed,LHCb::Track& muTrack,
                    double& Quality, double& CLQuality,
                    double& Arrival, double& CLArrival, double& DLL,
                    const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>()) override;



  bool isTrackInAcceptance(const LHCb::Track& seed) override;




  StatusCode findTrackRegions(const LHCb::Track& muTrack,  std::vector<int>& trackRegion) override;


protected:
  std::vector < double > m_zstations;
  std::vector < LHCb::State > m_states;
  IMeasurementProvider*  m_measProvider;
  SmartMuonMeasProvider* m_muonProvider;
  StatusCode makeStates(const LHCb::Track& seed);
  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges
  /// Preselection momentum (no attempt to ID below this)
  double m_PreSelMomentum;

private:
  bool isTrackInsideStation(int istation) const ;
  void addLHCbIDsToMuTrack(LHCb::Track& muTrack,double mom) const;
  StatusCode search(const LHCb::Track& seed, LHCb::Track& muTrack);

  ITrackFitter* m_fitter;
  ITrackExtrapolator* m_extrapolator;

  IIsMuonCandidateC* m_IsMuonTool;
  ICLTool* m_CLQuality;
  //ICLTool* m_CLArrival;
  IGetArrival* m_GetArrival;

  DeMuonDetector*  m_mudet;

  double m_nsigmas;
  double m_nsigmasUsed;
  int m_discrval;
  int m_NStation;

  bool m_useBkg;

  LHCb::State* m_mySeedState;

  double m_2hits;
  double m_3hits;
  double m_chi2cut;

  bool m_arrivalCuts;



  bool m_applyIsMuon;


  std::vector<double> m_MomRangeIsMuon;


  std::vector < int > m_stations_acceptance;



  //GP check if M1 exists
  int m_isM1defined;
  //
};

#endif // MUIDTOOL_H
