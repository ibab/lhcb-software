// $Id: MuonPIDChecker.h,v 1.7 2009-05-22 12:47:45 miriamg Exp $
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

  // Check MC association 
  StatusCode checkMCAss(const LHCb::Track *pTrack, const LHCb::MCParticle* mcP);
  
  // Reset values of track info
  void resetTrInfo();

  // get Track type according to MC info
  int getTrType( const LHCb::Track *pTrack, const LHCb::MCParticle* mcP);
                                            
  
  /// TES path of the tracks to analyse
  std::string m_TracksPath;
  /// TES path of the hits to analyse
  std::string m_HitsPath;
  /// TES path to MuonPIDs
  std::string m_MuonPIDsPath;
  /// TES path to Muon Tracks
  std::string m_MuonTracksPath;

  double m_zOriginCut; ///< cut on origin of the muon used to define decays 
  double m_DLLCut; ///< cut on DLL value used to define efficiency      
  double m_NSHCut; ///< cut on  nShared value used to define efficiency      
  bool   m_RunningMC; ///< swap between running on data and MC

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
  unsigned int m_cpresel[5],m_cisMuon[5], m_cDLL[5], m_cnShared[5];
               // total number of tracks mis-id-ed (0+2+3) 
  unsigned int m_cmisID, m_cmisIDDLL, m_cmisIDnShared;
               // Track info
  unsigned int m_TrIsMuon;
  int          m_TrNShared;
  double       m_TrMuonLhd,m_TrNMuonLhd,m_TrzDecay,m_TrzOrigin; 
  double       m_Trp0,m_TrMCp0;
  unsigned int m_TrnLinks;
  int          m_TrType;

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
             // Track info
