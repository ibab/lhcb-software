// $
#ifndef CHARGEDPROTOPALG_H 
#define CHARGEDPROTOPALG_H 1

// Include files
// from STL
#include <string>
#include <map>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
// LHCbKernel
#include "Relations/IAssociator.h"
#include "Relations/IAssociatorWeighted.h"
// from Event
#include "Event/CaloHypo.h"
#include "Event/TrStoredTrack.h"
// from MuonID
#include "MuonID/IMuonIDDLLTool.h"


/** @class ChargedProtoPAlg ChargedProtoPAlg.h
 *  This algorithms combined information from Tracking,RichPID and
 *  MuonPID to produce charged ProtoParticles
 *
 *  @author Gloria Corti
 *  @date   2002-07-08
 */
class ChargedProtoPAlg : public Algorithm {
public:

  enum TrkRejectType { KeepTrack=0, NoTrack, NoTrackType, Chi2Cut, Other };
  enum ProtoType { TrackProto=0, RichProto, MuonProto, ElectronProto };
  
  /// Standard constructor
  ChargedProtoPAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoPAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  class CombinedDLL {
  public:

    CombinedDLL() : m_eDLL(0), m_muDLL(0), m_piDLL(0), m_kDLL(0), m_pDLL(0) {};
    void reset() { 
      m_eDLL = 0.0;
      m_muDLL = 0.0; 
      m_piDLL = 0.0; 
      m_kDLL = 0.0; 
      m_pDLL = 0.0; 
    }
    
    double m_eDLL;
    double m_muDLL;
    double m_piDLL;
    double m_kDLL;
    double m_pDLL;
  };

  /// Check if track satisfy criterias to make a ProtoParticle.
  /// Forward, Match and Upstream tracks with no error flag and
  /// Chi2 < Max value are taken
  int rejectTrack( const TrStoredTrack* track );
  StatusCode addRich( SmartDataPtr<RichPIDs>& richpids, ProtoParticle* proto,
                      CombinedDLL* combDLL );
  StatusCode caloPIDTools();
  StatusCode muonProbDLL( ProtoParticle* proto, CombinedDLL* combDLL );
  void ProjectSeed2TT(const TrStoredTrack* ttt, double& xAtTT, double& yAtTT);
  
private:

  std::string m_tracksPath;        ///< Location in TES of input tracks
  std::string m_richPath;          ///< Location in TES of input Rich pids
  std::string m_muonPath;          ///< Location in TES of input Muon pids
  std::string m_electronPath;      ///< Location in TES of input Electron pids
  std::string m_photonMatchName;   ///< Name of calo&track match associator
  std::string m_electronMatchName; ///< Name of cluster energy match associator
  std::string m_bremMatchName;     ///< Name of bremsstrahlung match associator
  /// Name of associators for delta log likelyhood for e+/- PID based on Ecal
  std::string m_dLLeEcalName;
  /// Name of associators for delta log likelyhood for e+/- PID based on Prs
  std::string m_dLLePrsName;
  /// Name of associators for delta log likelyhood for e+/- PID based on Brem
  std::string m_dLLeBremName;
  /// Name of associators for delta log likelyhood for e+/- PID based on Hcal
  std::string m_dLLeHcalName;
  /// Name of associators for delta log likelyhood for mu+/- PID based on Ecal
  std::string m_dLLmuEcalName;
  /// Name of associators for delta log likelyhood for mu+/- PID based on Hcal
  std::string m_dLLmuHcalName;

  std::string m_protoPath;    ///< Location in TES of output ProtoParticles
  
  bool   m_upstream;       ///< Use or not unique upstream tracks
  bool   m_velott;         ///< Use or not unique velo-tt tracks
  double m_trackClassCut;  ///< Fraction of IT clusters to separate Tracks types
  double m_chiSqITracks;   ///< Max Chi2/NoF to make ProtoP from IT Tracks
  double m_chiSqOTracks;   ///< Max Chi2/NoF to make ProtoP from IT Tracks
  double m_chiSqVTT   ;    ///< Max Chi2/NoF to make ProtoP from VTT Tracks
  double m_chiSqUps;   ///< Max Chi2/NoF to make ProtoP from upstream Tracks
  double m_minTTx;    ///<  TT hole x cut
  double m_minTTy;    ///<  TT hole y cut
  

 
  int m_idElectron;           ///< PDG id of e+/-
  int m_idMuon;               ///< PDG id of mu+/mu-
  int m_idPion;               ///< PDG id of pi+/pi-
  int m_idKaon;               ///< PDG id of K+/K-
  int m_idProton;             ///< PDG id of p/p~ 

  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> PhotonMatch;
  typedef IAssociatorWeighted<CaloHypo,TrStoredTrack,float>    ElectronMatch;
  typedef IAssociatorWeighted<CaloHypo,TrStoredTrack,float>    BremMatch;
  typedef IAssociator<TrStoredTrack,float>                     ITrkCaloPID;
  typedef PhotonMatch::InverseType    PhotonTable;
  typedef ElectronMatch::InverseType  ElectronTable;
  typedef BremMatch::InverseType      BremTable;
  typedef ITrkCaloPID::DirectType     TrkCaloPIDTable;
  typedef PhotonTable::Range          PhotonRange;
  typedef ElectronTable::Range        ElectronRange;
  typedef BremTable::Range            BremRange;
  typedef TrkCaloPIDTable::Range      TrkCaloPIDRange;

  PhotonMatch*   m_photonMatch;     ///< Associator for photon Match
  ElectronMatch* m_electronMatch;   ///< Associator for electron Match
  BremMatch*     m_bremMatch;       ///< Associator for brem Match
  /// Associator for e+/- delta log likelyhood based on Ecal energy
  ITrkCaloPID*   m_ecalPIDe;
  /// Associator for e+/- delta log likelyhood based on Preshower 
  ITrkCaloPID*   m_prsPIDe;
  /// Associator for e+/- delta log likelyhood based on Brem
  ITrkCaloPID*   m_bremPIDe;
  /// Associator for e+/- delta log likelyhood based on Hcal
  ITrkCaloPID*   m_hcalPIDe;
  /// Associator for mu+/- delta log likelyhood based on Ecal
  ITrkCaloPID*   m_ecalPIDmu; 
  /// Associator for mu+/- delta log likelyhood based on Hcal
  ITrkCaloPID*   m_hcalPIDmu;

  /// Tool for mu+/- delta log likelyhood based on Muon System
  IMuonIDDLLTool*  m_muonIDdll;

  typedef std::map< std::string, int, std::less<std::string> > ErrorTable;
  ErrorTable m_errorCount;          ///< Error counters
  bool m_monitor;                   ///< Fill histograms and special print  

};
#endif // CHARGEDPROTOPALG_H
