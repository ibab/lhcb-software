#ifndef MakeMuonTool_H 
#define MakeMuonTool_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "ICLTool.h"
//#include "IGetArrival.h"
//#include "ImuIDTool.h"
#include "IIsMuonCandidateC.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MuonPID.h"
#include "Event/Track.h"

#include "MuonDet/DeMuonDetector.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "IIsMuonCandidateC.h"
#include "ICLTool.h"
#include "IGetArrival.h"
#include "SmartMuonMeasProvider.h"
#include "ImuIDTool.h"


static const InterfaceID IID_MakeMuonTool("MakeMuonTool", 1, 0);

/** @class MakeMuonTool MakeMuonTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */
class MakeMuonTool final: public GaudiTool {
public:
  static const InterfaceID& interfaceID() { return IID_MakeMuonTool; }  
  /// Standard constructor
  MakeMuonTool( const std::string& type, 
            const std::string& name,
            const IInterface* parent);

  virtual ~MakeMuonTool( ); ///< Destructor

  virtual StatusCode initialize();

  
//  StatusCode isGoodSeed(const LHCb::Track& seed);
  
  StatusCode muonCandidate(const LHCb::Track& seed, LHCb::Track& muTrack,
                           const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>());

  virtual StatusCode muonQuality(LHCb::Track& muTrack, double& Quality);

//  StatusCode muonArrival(LHCb::Track& muTrack, double& Arrival);
  
//  StatusCode muonDLL(LHCb::Track& muTrack, const double& Quality, double& CLQuality, 
//                     const double& Arrival, double& CLArrival, double& DLL);
 
 
  
  
//  StatusCode muonID(const LHCb::Track& seed,LHCb::Track& muTrack, 
//                    double& Quality, double& CLQuality, 
//                    double& Arrival, double& CLArrival, double& DLL,
//                    const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>());


//  bool isTrackInsideStation(const int& istation);

//  bool isTrackInAcceptance(const LHCb::Track& seed);

  StatusCode makeStates(const LHCb::Track& seed);
  
  void addLHCbIDsToMuTrack(LHCb::Track& muTrack,double mom);
  
  StatusCode search(const LHCb::Track& seed, LHCb::Track& muTrack);

//  StatusCode findTrackRegions(const LHCb::Track& muTrack,  std::vector<int>& trackRegion);


  
protected:
  
  ITrackFitter* m_fitter;
  ITrackExtrapolator* m_extrapolator;
  IMeasurementProvider*  m_measProvider;

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

  /// Preselection momentum (no attempt to ID below this)
  double m_PreSelMomentum;

  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges

  bool m_applyIsMuon;


  std::vector<double> m_MomRangeIsMuon;

  std::vector < double > m_zstations;

  std::vector < int > m_stations_acceptance;

  std::vector < LHCb::State > m_states;

  SmartMuonMeasProvider* m_muonProvider;

  //GP check if M1 exists
  int m_isM1defined;
  //
};

#endif // MUIDTOOL_H
