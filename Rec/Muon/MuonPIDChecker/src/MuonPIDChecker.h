// $Id: MuonPIDChecker.h,v 1.10 2010-03-22 02:54:25 polye Exp $
#ifndef MUONPIDCHECKER_H 
#define MUONPIDCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from STL
#include <string>

// From event packages
#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/RecHeader.h"

#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonBasicGeometry.h"



/** @class MuonPIDChecker MuonPIDChecker.h
 *  Monitor MuonPID multiplicities and spectrum 
 *
 *  @author Erica Polycarpo Macedo
 *  @date   2010-03-20
 */
class MuonPIDChecker : public GaudiHistoAlg{
public: 
  /// Standard constructor
  MuonPIDChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonPIDChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  // Properties 
  // TES path of the tracks to analyse
  std::string m_TracksPath;
  // TES path to MuonPIDs
  std::string m_MuonPIDsPath;
  // TES path of the muon tracks to analyse
  std::string m_MuonTracksPath;

  int m_HistosOutput; ///< Different levels of histos
  int m_TrackType;
  bool   m_RunningMC; ///< swap between running on data and MC
  std::vector< double > m_monitCutValues;

  double m_DLLlower; ///< lower limit for DLL histo 
  double m_DLLupper; ///< upper limit for DLL histo
  int    m_DLLnbins; ///< number of bins for DLL histo

  //Number of cuts to be monitored
  int m_nMonitorCuts;
  // Muon Detector geometry 
  unsigned int m_NStation;     // Number of stations
  unsigned int m_NRegion;      //Number of regions

  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;
  // These are indexed [station]
  std::vector<std::string> m_stationNames;  //Names of the station
  std::vector<double> m_stationZ; // station position

  // Track Info
  double  m_Trp0,m_TrpT;
  int m_TrRegionM2;
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station  

  unsigned int m_TrIsPreSel,m_TrIsMuon,m_TrIsMuonLoose,m_TrNShared;
  // MuonPID   
  double  m_TrMuonLhd,m_TrNMuonLhd;
  // MuonTrack
  double m_TrDist2, m_Trquality, m_TrCLquality, m_TrChi2, m_TrCLarrival;
  unsigned int m_Trnhitsfoi[20];

  // MC info
  double       m_TrMCp0, m_TrzDecay;
  unsigned int m_TrnLinks;
  unsigned int m_TrType;

  // methods
  StatusCode trackExtrapolate(const LHCb::Track& track);
  int findTrackRegion(const int sta);
  void resetTrInfo();
  void getMuonPIDInfo(const LHCb::Track& track, const LHCb::MuonPIDs* pMuid);
  void getMuonTrackInfo(const LHCb::Track& track, const LHCb::Tracks* muTracks);
  StatusCode getTrackInfo(const LHCb::Track& track);
  void fillPreSelPlots(int level);
  void fillIMLPlots(int level);
  void fillIMPlots(int level);
  void fillMultiplicityPlots(int level);
  void fillHitMultPlots(int level);
  double getRateError( double rate , int den);

  // Event  counters
  unsigned int m_nTr;
  unsigned int m_nTrPreSel;
  unsigned int m_nTrIsMuonLoose;
  unsigned int m_nTrIsMuon;
  
  // Run counters
  std::vector<unsigned int> m_ntotTr;

};
#endif // MUONPIDCHECKER_H
