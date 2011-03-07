// $Id: VertexCompare.h,v 1.1 2010/03/01 15:58:14 pmorawsk Exp $
#ifndef VERTEXCOMPARE_H
#define VERTEXCOMPARE_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "MCInterfaces/IForcedBDecayTool.h"
#include "TrackInterfaces/IPVOfflineTool.h"
// #include "TrackInterfaces/IPVSeeding.h"

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
  int nBackTracks;                // number of backward tracks in a vertex
  Gaudi::XYZPoint position;       // position
  Gaudi::XYZPoint positionSigma;  // position sigmas
  int indexMCPVInfo;              // index to MCPVInfo
  LHCb::RecVertex* pRECPV;        // pointer to REC PV
  bool rec1, rec2;
} RecPVInfo;

class VertexCompare : public GaudiTupleAlg {
public:
  /// Standard constructor
  VertexCompare( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VertexCompare( );                ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  bool debugLevel() const { return msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ; }

private:

  IForcedBDecayTool* m_forcedBtool;
//   IPVOfflineTool*    m_pvsfit;

//   int m_nTracksToBeRecble;
  bool m_produceHistogram;
  bool m_produceNtuple;
//   double m_dzIsolated;
//   std::string m_inputTracksName;
  std::string m_inputVerticesName1;
  std::string m_inputVerticesName2;
//   std::string m_pvSeedingName;
  int m_nevt,          m_nVtx;
  int m_nPartVtx             ;

 void printRat(std::string mes, int a, int b);
//  bool getInputTracks( std::vector<LHCb::Track*>& vecOfTracks,  std::string& trackLoc);
 bool getInputVertices( std::vector<LHCb::RecVertex*>& vecOfVertices1 , std::vector<LHCb::RecVertex*>& vecOfVertices2 );
 void matchByDistance(std::vector<LHCb::RecVertex>& verthalf, std::vector<LHCb::RecVertex>& vertfull, std::vector<int>& link);

};
#endif // VERTEXCOMPARE_H
