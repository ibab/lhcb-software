#ifndef PRIMARYVERTEXCHECKER_H 
#define PRIMARYVERTEXCHECKER_H 1

// Include files
#include "GaudiAlg/GaudiTupleAlg.h"
#include "MCInterfaces/IForcedBDecayTool.h"

typedef struct {  
  LHCb::MCVertex* pMCPV;     // pointer to MC PV 
  int nRecTracks;            // number of reconstructed tracks from this MCPV  
  int nRecBackTracks;        // number of reconstructed backward tracks
  int indexRecPVInfo;        // index to reconstructed PVInfo (-1 if not reco)
  int nCorrectTracks;        // correct tracks belonging to reconstructed PV
  int multClosestMCPV;       // multiplicity of closest reconstructable MCPV
  double distToClosestMCPV;  // distance to closest reconstructible MCPV
  std::vector<LHCb::MCParticle*> m_mcPartInMCPV;
  std::vector<LHCb::Track*> m_recTracksInMCPV;
} MCPVInfo;

typedef struct {
 public:
  int nTracks;                    // number of tracks in a vertex
  int nVeloTracks;                // number of velo tracks in a vertex
  Gaudi::XYZPoint position;       // position
  Gaudi::XYZPoint positionSigma;  // position sigmas
  int indexMCPVInfo;              // index to MCPVInfo
  LHCb::RecVertex* pRECPV;        // pointer to REC PV
} RecPVInfo;

class PrimaryVertexChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  PrimaryVertexChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrimaryVertexChecker( );                ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  bool debugLevel() const { return msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ; }

private:

  IForcedBDecayTool* m_forcedBtool;

  int    m_nTracksToBeRecble;
  bool   m_produceHistogram;
  bool   m_produceNtuple;
  bool   m_requireVelo;
  double m_dzIsolated;
  bool   m_matchByTracks;
  std::string m_inputTracksName;
  std::string m_inputVerticesName;

  int m_nevt;
  int m_nMCPV,             m_nRecMCPV;
  int m_nMCPV_isol,        m_nRecMCPV_isol;
  int m_nMCPV_close,       m_nRecMCPV_close;
  int m_nFalsePV,          m_nFalsePV_real;
  int m_nMCPV_1mult,       m_nRecMCPV_1mult;
  int m_nMCPV_isol_1mult,  m_nRecMCPV_isol_1mult;
  int m_nMCPV_close_1mult, m_nRecMCPV_close_1mult;
  int m_nMCPV_2mult,       m_nRecMCPV_2mult;
  int m_nMCPV_isol_2mult,  m_nRecMCPV_isol_2mult;
  int m_nMCPV_close_2mult, m_nRecMCPV_close_2mult;
  int m_nMCPV_3mult,       m_nRecMCPV_3mult;
  int m_nMCPV_isol_3mult,  m_nRecMCPV_isol_3mult;
  int m_nMCPV_close_3mult, m_nRecMCPV_close_3mult;
  int m_nL0PvOfB,          m_nRecL0PvOfB;
  int m_nBFalse,           m_nRecBFalse;

 std::vector<MCPVInfo>::iterator closestMCPV(std::vector<MCPVInfo>& rblemcpv, 
                                             std::vector<MCPVInfo>::iterator& itmc);

 void collectProductss(LHCb::MCVertex* mcpv,  LHCb::MCVertex* mcvtx, std::vector<LHCb::MCParticle*>& allprods);
 void printRat(std::string mes, int a, int b);
 void match_mc_vertex_by_tracks(int ipv, std::vector<RecPVInfo>&  rinfo, std::vector<MCPVInfo>& mcpvvec);
 void match_mc_vertex_by_distance(int ipv, std::vector<RecPVInfo>&  rinfo, std::vector<MCPVInfo>& mcpvvec);
 void count_reconstructed_tracks(std::vector<MCPVInfo>& mcpvvec, std::vector<LHCb::Track*>& vecOfTracks, std::string trackLoc);
 int  count_velo_tracks(LHCb::RecVertex* RecVtx);
 void count_reconstructible_mc_particles(std::vector<MCPVInfo>& mcpvvec);
 bool getInputTracks( std::vector<LHCb::Track*>& vecOfTracks,  std::string& trackLoc);
 bool getInputVertices( std::vector<LHCb::RecVertex*>& vecOfVertices );

};
#endif // PRIMARYVERTEXCHECKER_H
