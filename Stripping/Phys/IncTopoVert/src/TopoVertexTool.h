// $Id: $
#ifndef TOPOVERTEXTOOL_H 
#define TOPOVERTEXTOOL_H 1

// Include files

// from Gaudi
#include "IncTopoVert/ITopoVertexTool.h"            // Interface

#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDistanceCalculator.h"
#include "TrackInterfaces/ITrackVertexer.h"


/** @class TopoVertexTool TopoVertexTool.h
 *  This class implementw ITopoVertexTool. See this interface
 *  class for a description of the purpose of the class.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-09-21
 */
class TopoVertexTool : public GaudiTool, virtual public ITopoVertexTool {

public: 

  typedef std::vector< RecVertexVector *>               VerticesList;
  typedef std::map<const LHCb::Track*,RecVertexVector > TrackVerticesMap;
  typedef std::map<const LHCb::Track*,LHCb::Vertex * >  TrackVertexMap;
  typedef std::vector<const LHCb::Track*>               TrackVector;
  
  /// Standard constructor
  TopoVertexTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~TopoVertexTool( ); ///< Destructor


  StatusCode initialize();
  
  StatusCode findVertices(std::vector<const LHCb::Track*> & tracks,
                          std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step= NULL);

  StatusCode findVertices(std::vector<const LHCb::Particle*> & particles, int* n_sig_track_step= NULL);
  StatusCode findVertices(std::vector<const LHCb::Particle*> & particles,
                                  std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step= NULL);
  StatusCode findVertices(TrackVector & tracks, int* n_sig_track_step = NULL); 

  StatusCode writeOnTES(std::string location);

  int getNbSignalTrack(RecVertexVector VecVtx);
  int getNbSignalTrack(VerticesList VtxList);



protected:

  RecVertexVector  m_2tracks_vertices;
  TrackVerticesMap m_2tracks_vertices_per_track;
  TrackVector      m_tracks;
  VerticesList     m_list_of_clusters;
  
  StatusCode step1();
  StatusCode step2();
  StatusCode step3();
  StatusCode step4();
  StatusCode step5();
  StatusCode step6();

private:

  IDistanceCalculator * m_distanceCalculator; 
  ITrackVertexer * m_trackVertexer;

  std::string m_vertexFunctionToolType; ///< Type of the vertex function Tool
  std::string m_vertexFunctionToolName; ///< Name of the vertex function Tool

  std::string m_distanceCalculatorToolType; ///< Type of the distance calculator Tool
  std::string m_distanceCalculatorToolName; ///< Name of the distance calculator Tool

  std::string m_trackVertexerToolType; ///< Type of the track vertexer Tool
  std::string m_trackVertexerToolName; ///< Name of the track vertexer Tool

  double m_twoTracksVtxChi2Max;    ///< Maximum chi2 of two tracks vertices
  double m_twoTracksVtxVfMin;      ///< Minimum value of vertex function of two tracks vertices
  double m_twoTracksVtxVfRatioMin; ///< Minimum ratio (vertex function of two tracks vertices over highest vertex function)
  double m_TrackVtxChi2Max;        ///< Maximum contribution of a track to a vertex

  void addUnResolvedToCluster(RecVertexVector & cluster, RecVertexVector & vertices, unsigned int index=0);
  LHCb::RecVertex * removeHighChisqTracksFromVertex(LHCb::RecVertex * vertex);
  
  void cleanUpTemporary();
  void cleanUpResult();
  void cleanUpFull();

  /// Temporary : to be passed to the vertex function tool
  void setVertexFunctionParam();

  bool m_writtenOnTES;

  bool m_MC;
  
  double m_step;
 
  double m_max_finder_min_gradient_mag;
  int    m_max_finder_max_iteration;
  double m_max_finder_min_step;
  double m_max_finder_max_jump;

  double m_resolver_cut; 
  double m_resolver_min_step;
  double m_resolver_max_iteration;
  
  double m_dx_for_gradient;
};
#endif // TOPOVERTEXTOOL_H
