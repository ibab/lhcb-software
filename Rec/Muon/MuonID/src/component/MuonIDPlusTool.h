#ifndef MUONIDPLUSTOOL_H
#define MUONIDPLUSTOOL_H 1


#include <map>

#include "GaudiAlg/GaudiTool.h"
#include "MuonID/CommonMuonHit.h"
#include "MuonID/IMuonIDTool.h"
#include "MuonID/IMuonMatchTool.h"
#include "Kernel/ParticleID.h"

// Forward declarations
namespace LHCb {
  class Particle;
  class MuonCoord;
  class MuonTileID;
  class State;
}
class DeMuonDetector;
class ITrackExtrapolator;
class IMuonFastPosTool;
class IMuonPadRec;
class IMuonClusterRec;



/** @class MuonIDPlusTool MuonIDPlusTool.h
 *
 *  /brief Generic tool for MuonID, returning a MuonID object from a Track object
 *
 *  on the first call per event, this tool makes association tables between all tracks passing
 *  some preselction cuts (defined by options MinTrackMomentum, MaxGhostProb, MaxTrackChi2) and all muon hits.
 *  Matches within Nsigma < SearchSigmaCut are kept
 *
 *  Subsequent steps in muonID (association of best muon hits, chi^2 calculations, DLL, etc) are 
 *  delegated to dedicated tools
 *
 *  Use ReleaseObjectOwnership = false
 *  if you want the tool to delete all created MuonID and Track objects (by default the tool assumes these objects will  
 *  be stored in a container)
 *
 *
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-10
 *
 */
class MuonIDPlusTool : public GaudiTool, public IMuonIDTool {
public:
  /// Standard constructor
  MuonIDPlusTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~MuonIDPlusTool(){ clearPIDs();}; ///< Destructor
  virtual StatusCode initialize();
  
  virtual StatusCode eventInitialize();

  virtual LHCb::MuonPID* getMuonID(const LHCb::Track* track);
  
  virtual double muonIDPropertyD(const LHCb::Track* track, const char* propertyName, int station=-1);
  virtual int muonIDPropertyI(const LHCb::Track* track, const char* propertyName, int station=-1);

private:
  DeMuonDetector*  m_mudet;
  IMuonFastPosTool* m_posTool;
  IMuonPadRec* m_padrectool;
  IMuonClusterRec* m_clustertool;
  ITrackExtrapolator* m_extrapolator;
  IMuonMatchTool* m_matchTool;

  unsigned int m_lastRun;
  unsigned int m_lastEvent;
  LHCb::ParticleID theMuon;
  const LHCb::Track* m_lasttrack;

  static const unsigned int MAXHITS = 1024;
  std::vector<unsigned int> m_hitCounter;
  std::vector<CommonMuonHits> m_muhits;                 // muon hits grouped by stations
  std::map<const CommonMuonHit*, std::vector< std::pair< const LHCb::Track*,float> > > m_mutrkmatchTable;
  std::map<const LHCb::Track*, std::vector< TrackMuMatch > > m_trkmumatchTable;
  std::map<const LHCb::Track*, std::vector< TrackMuMatch > > m_trkmumatchTableSpares;
  std::map<const LHCb::Track*, std::vector<bool>> m_trkInAcceptance;

  std::vector<LHCb::MuonPID*> m_muonPIDs;
  LHCb::MuonPID* m_lastPID;

  bool m_largeClusters;
  std::vector<bool> m_stationHasLargeCluster;
  int m_nStationsWithMatch;

  void initVariables();
  void clearPIDs();
  void setIsMuon(double momentum, double chi2perdof);
  void checkMuIsolation(const LHCb::Track *pTrack, std::vector<CommonConstMuonHits>* mucoord);

  StatusCode getMuonHits();
  StatusCode matchHitsToTracks();
  bool isTrackInsideStation(const LHCb::State& state, unsigned int istation) const;

  double medianClusize();

  // variables for tuple:
  std::vector<int> m_matchM;
  std::vector<double> m_matchSigma;
  std::vector<int> m_matchX;
  std::vector<int> m_matchY;
  std::vector<double> m_matchT;
  std::vector<double> m_matchdT;
  std::vector<int> m_clusize;
  double m_medianClusize;
  std::vector<double> m_isoM;
  double m_maxisoM;
  int m_friendShares;
  int m_nSmatched;
  int m_nVmatched;

  bool m_isMuon;
  bool m_isMuonLoose;
  bool m_isMuonTight;


  // options:
  bool m_useM1;
  double m_searchSigmaCut;
  double m_maxTrackChi2;
  double m_maxGhostProb;
  double m_minTrackMomentum;
  int m_maxCluSize;
  std::string m_matchToolName;
  std::string m_BestTrackLocation;
  bool m_ReleaseObjectOwnership;
  
};
#endif // MUONIDPLUSTOOL_H
