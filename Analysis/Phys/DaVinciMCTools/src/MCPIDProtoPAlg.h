// $Id: MCPIDProtoPAlg.h,v 1.1 2002-07-18 18:38:13 gcorti Exp $
#ifndef MCPIDPROTOPALG_H 
#define MCPIDPROTOPALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from Relations
#include "Relations/IAssociatorWeighted.h"

// from Event
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

/** @class MCPIDProtoPAlg MCPIDProtoPAlg.h
 *  This algorithms combined information from Tracking,RichPID and
 *  MuonPID to produce charged ProtoParticles
 *
 *  @author Gloria Corti
 *  @date   2002-07-08
 */
class MCPIDProtoPAlg : public Algorithm {
public:
  /// Standard constructor
  MCPIDProtoPAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCPIDProtoPAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Check if track satisfy criterias to make a ProtoParticle.
  /// Forward, Match and Upstream tracks with no error flag and
  /// Chi2 < Max value are taken
  bool keepTrack( const TrStoredTrack* track );
  StatusCode addRich( SmartDataPtr<RichPIDs>& richpids, ProtoParticle* proto );
  
private:

  std::string m_tracksPath;     ///< Location in TES of input tracks
  std::string m_richPath;       ///< Location in TES of input Rich pids
  std::string m_muonPath;       ///< Location in TES of input Muon pids
  std::string m_electronPath;   ///< Location in TES of input Electron pids
  std::string m_trkmatchPath;   ///< Location in TES of calo&track match
  std::string m_caloematchPath; ///< Location in TES of cluster energy match
  std::string m_bremmatchPath;  ///< Location in TES of bremsstrahlung match


  std::string m_protoPath;    ///< Location in TES of output ProtoParticles
  
  double m_lastChiSqMax;      ///< Maximum Chi2 of track fit to make a ProtoP
 
  int m_idElectron;           ///< PDG id of e+/-
  int m_idMuon;               ///< PDG id of mu+/mu-
  int m_idPion;               ///< PDG id of pi+/pi-
  int m_idKaon;               ///< PDG id of K+/K-
  int m_idProton;             ///< PDG id of p/p~ 
  
  IAssociatorWeighted<TrStoredTrack,MCParticle,double>* m_track2MCParticleAsct;
  std::string m_trackAsctName;

};
#endif // MCPIDPROTOPALG_H
