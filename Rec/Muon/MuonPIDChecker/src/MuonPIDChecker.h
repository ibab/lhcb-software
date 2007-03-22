// $Id: MuonPIDChecker.h,v 1.3 2007-03-22 09:05:51 cattanem Exp $
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
 *  new version Feb/2004 jtmn
 *
 *  @author M. Gandelman, S. Amato, J. de Mello Neto
 *  @date   25-10-2002
 *
 *  new version Aug/2006 miriam
 *
 *  @author M. Gandelman, E. Polycarpo
 *  @date   08-08-2006
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

  double m_zOriginCut; ///< cut on origin of the muon used to define decays 
  double m_DLLCut; ///< cut on DLL value used to define efficiency      
  double m_NSHCut; ///< cut on  nShared value used to define efficiency      

  long           m_eventNumber,m_runNumber;
  long           m_cpresel[4],m_cisMuon[4], m_cDLL[4], m_cnShared[4];
  long           m_cmisID, m_cmisIDDLL, m_cmisIDnShared;
  long           m_nTr, m_nPSTr, m_nPSGhosts, m_nHit;     // counters 
  long           m_TrIsMuon,m_TrNShared;
  double         m_TrMuonLhd,m_TrNMuonLhd,m_TrzDecay,m_TrzOrigin; 
  double         m_Trp0,m_TrMCp0;
  int            m_TrnLinks, m_TrType;

  int m_neventsTest; ///< count events

  int m_NStation;     // Number of stations
  int m_NRegion;      //Number of regions
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
