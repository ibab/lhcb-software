#ifndef PATPV_PATPV3D_H 
#define PATPV_PATPV3D_H 1

// Include files
// -------------
// From Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// From Event
#include "Event/Track.h"
#include "Event/RecVertex.h"

// DeVelo
#include "VeloDet/DeVelo.h"

// Tools
#include "PVSeedTool.h"

// Local
#include "PatVertexState.h"

/** @class PatPV3D PatPV3D.h
 *  Algorithm to find the primary vertices at the HLT.
 *  Based on the HltPrimaryVertex package from Nicolas Arnaud.
 *
 *  @author Eduardo Rodrigues
 *  @date   2004-02-17
 */

//-----------------------------------------------------------------------------


class PatPV3D : public GaudiAlgorithm {
public:
  /// Standard constructor
  PatPV3D( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatPV3D( );    ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  StatusCode recalculateTrackError(LHCb::Track* track, double &ex2, double &ey2);
  StatusCode flagTracks();
  StatusCode searchVertexSeeds(std::vector<double> &zseeds);  
  StatusCode searchVertices(std::vector<double> &zseeds, PatVertexStates& myTrgVertices);
  StatusCode cleanVertices(PatVertexStates&);
  StatusCode improveVertices(double chi2PeTrack, PatVertexStates&);
  StatusCode storeVertices(PatVertexStates&);

  void computeErrorParameters( LHCb::Track* track, double z,
                               double & minZDistToVELOCluster,
                               int & nbStaBeforeLastVELOCluster,
                               double & distFirstLastVELOCluster );

  void storeDummyVertices(std::vector<double> &zseeds);



  class sortTrgVertexByPosition {
  public: 
    bool operator() ( PatVertexState* first, PatVertexState* second ) const {
      return first->position().z() < second->position().z();
    }
  };
  
  class sortTrgVertexByTrackNum {
  public:
    bool operator() ( PatVertexState* first, PatVertexState* second ) const {
      return first->nbTracksInTrgVertex() > second->nbTracksInTrgVertex();
    }
  };

protected:
 
private: 
    
  double m_zMin;                    ///< Low bound in z for Vtx search (mm)
  double m_zMax;                    ///< High bound in Z for Vtx search (mm)
  double m_vertexHalfWidth;         ///< initial z half-width of the vertex
  double m_chi2Max;                 ///< Cut for track chi2 when computing a vertex
  int    m_minNbTracksVertex;       ///< Absolute min. number of tracks / vtx
  int    m_minNbTracksCloseVertex;  ///< Absolute min. number of tracks for close vtx
  double m_dzCloseVertex;           ///< dz cut of close vtx
  int    m_minNbBackTracksVertex;   ///< Absolute min. number of backward tracks / vtx
  double m_maxTransDistSquared;     ///< track too far for vertexing
  double m_minDistBetweenVertices;  ///< Max dist. to merge vertices
  double m_zErrorMax;               ///< For monitoring
  bool   m_improveVertexLocation;   ///< Improve Vtx Location flag 
  double m_chi2PerTrackCut;         ///< Cut for track chi2 when improving vtx
  bool   m_modifyTrackErrors;       ///< Flag to change or not VELO track errors
  double m_detectorResolutionCoeff; ///< detector resolution
  double m_multipleScatteringCoeff; ///< multiple scattering resolution
  bool   m_saveSeedsAsPV;           ///< Save seeds as PVs (for monitoring)
  
  double  m_x0MS;         // X0 (tunable) of MS to add for extrapolation of 
                          // track parameters to PV
  double  m_scatCons;     // calculated from m_x0MS

  int m_vertexSearchNotPossible;

  LHCb::Tracks*  m_inputTracks;    ///< Pointer to the SpaceVelo tracks
  LHCb::RecVertices* m_outputVertices; ///< Pointer to the 3D vertices

  std::string m_inputTracksName;
  std::string m_outputVerticesName;  

  // Tools
  PVSeedTool* m_pvSeedTool;

  std::vector<vtxCluster> m_vclusters;
  std::vector<localTrack> m_lotravec;  
  DeVelo* m_velo;
};
#endif // PATPV_PATPV3D_H
