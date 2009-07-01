// $Id: MuonPIDChecker.h,v 1.8 2009-07-01 03:33:37 polye Exp $
#ifndef TUNEMUONID_H 
#define TUNEMUONID_H 1

// Include files

// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// From event packages
#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "Event/MuonCoord.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/RecHeader.h"
#include "Event/MuonDigit.h"

#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonBasicGeometry.h"


/** @class MuonPIDChecker MuonPIDChecker.h
 *  Test MuonID efficiency/Mis-ID rates
 *
 *  @author M. Gandelman, S. Amato, J. de Mello Neto
 *  @date   25-10-2002
 *
 *  new version Feb/2004 jtmn
 *
 *  new version Aug/2006 miriam
 *
 *  @author M. Gandelman, E. Polycarpo
 *  @date   08-08-2006
 *
 *  @author M. Gandelman, E. Polycarpo
 *  @date   20-08-2008
 */


class MuonPIDChecker : public GaudiHistoAlg {

public:

  /// Standard constructor
  MuonPIDChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonPIDChecker( );             ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:

  // Fill Histos
  void fillTrHistos(const int Level);
  // Loop over MuonPIDs
  StatusCode getTrPIDInfo(LinkedTo<LHCb::MCParticle, LHCb::Track>* linkToTrack); 
  // Loop over Muon Tracks
  StatusCode getMuTrPIDInfo(LinkedTo<LHCb::MCParticle, LHCb::Track>* linkToTrack); 
  // Extrapolate tracks to the Muon System
  StatusCode trackExtrapolate(const LHCb::Track *pTrack); 

  // Check MC association 
  StatusCode checkMCAss(const LHCb::Track *pTrack, const LHCb::MCParticle* mcP);
  
  // Reset values of track info
  void resetTrInfo();
  void resetMuTrInfo();

  // get Track type according to MC info
  //int getTrType( const LHCb::Track *pTrack, const LHCb::MCParticle* mcP);
  // changed to do everything inside
  int getTrType( const LHCb::Track *pTrack, LinkedTo<LHCb::MCParticle, LHCb::Track>* linkToTrack);
                                            
  
  // Properties
  /// TES path of the tracks to analyse
  std::string m_TracksPath;
  /// TES path of the hits to analyse
  std::string m_HitsPath;
  /// TES path to MuonPIDs
  std::string m_MuonPIDsPath;
  /// TES path to Muon Tracks
  std::string m_MuonTracksPath;

  int    m_IsMuonFlag; ///< swap between IsMuon(=1) and IsMuonLoose(=0)
  int    m_DLLFlag; ///< swap between standard(=0), pbinned(=1) and MuProb(=2)
  double m_DLLlower; ///< lower limit for DLL histo 
  double m_DLLupper; ///< upper limit for DLL histo
  int    m_DLLnbins; ///< number of bins for DLL histo

  double m_DLLCut; ///< cut on DLL value used to define efficiency      
  double m_ProbCut1; ///< cut on Prob_mu value used to define efficiency      
  double m_ProbCut2; ///< cut on Prob_mu value used to define efficiency      
  double m_NSHCut; ///< cut on  nShared value used to define efficiency      
  bool   m_RunningMC; ///< swap between running on data and MC

  // Efficiency and Eff. Error used as reference (for two samples)
  std::vector< double > m_refEff1IM ,m_refdEff1IM ;
  std::vector< double > m_refEff1IML,m_refdEff1IML;
  std::vector< double > m_refEff1DLL,m_refdEff1DLL;
  std::vector< double > m_refEff1NSH,m_refdEff1NSH;
  std::vector< double > m_refEff2IM ,m_refdEff2IM ;
  std::vector< double > m_refEff2IML,m_refdEff2IML;
  std::vector< double > m_refEff2DLL,m_refdEff2DLL;
  std::vector< double > m_refEff2NSH,m_refdEff2NSH;


  unsigned int m_eventNumber,m_runNumber;
               // total number of 
               //preselected tracks
               //tracks  with IsMuon=1
               //tracks  with DLL> m_DLLCut
               //tracks  with NShared> m_NSHCut
               // indices are:
               //  0 = ghost
               //  1 = muons
               //  2 = muons from decays in flight
               //  3 = non-muons  
               //  4 = all (data)
  unsigned int m_nTrPreSel[5], m_nTrIML[5], m_nTrIM[5],m_nTrDLL[5], m_nTrNSH[5];
  unsigned int m_nTrProb[2];// to get efficiency for 2 additional cuts
               // Track info
  unsigned int m_TrIsMuon,m_TrIsMuonLoose;
  unsigned int m_TrNShared;
  double       m_TrMuonLhd,m_TrNMuonLhd,m_TrzDecay; 
  double       m_TrDist2,m_Trquality, m_TrCLquality,m_TrChi2, m_TrCLarrival;
  double       m_Trp0,m_TrMCp0;
  unsigned int m_TrnLinks;
  unsigned int m_TrType;

  int findTrackRegion(const int sta);

  unsigned int m_neventsTest; ///< count events

  unsigned int m_NStation;     // Number of stations
  unsigned int m_NRegion;      //Number of regions
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;

  // These are indexed [station]
  std::vector<double> m_stationZ; // station position
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station  

};

#endif // TUNEMUONID_H
