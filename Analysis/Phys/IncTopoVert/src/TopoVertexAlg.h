// $Id: $
#ifndef TOPOVERTEXALG_H 
#define TOPOVERTEXALG_H 1

// Include files
// from Gaudi
//#include "Kernel/DaVinciTupleAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"  
#include "GaudiAlg/ISequencerTimerTool.h"
#include "IncTopoVert/ITopoVertexTool.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "IncTopoVert/IFilterVtxTopoTracksTool.h" 
#include "Kernel/IDistanceCalculator.h"
#include "IncTopoVert/IVertexFunctionTool.h"
#include "IncTopoVert/ITopoVertexAnalyserTupleTool.h"  
#include "Kernel/IParticleDescendants.h"  
#include "Kernel/IParticle2MCAssociator.h" 


//trigger stuff
#include "Event/L0DUReport.h" 
#include "Event/HltDecReports.h"

//stripping stuff     
#include "Kernel/IBackgroundCategory.h" 

/** @class TopoVertexAlg TopoVertexAlg.h
 *  This class is an example of usage of the TopoVertexTool
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-10-11
 */
class TopoVertexAlg : public GaudiTupleAlg {
public: 
  /// Standard constructor
  TopoVertexAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TopoVertexAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ITopoVertexTool * m_topoVertexTool ;
  IFilterVtxTopoTracksTool * m_filterTracksTool;
  IFilterVtxTopoTracksTool * m_filterTracksTypeTool;
  IFilterVtxTopoTracksTool * m_mcFilterTracksTool;
  ITopoVertexAnalyserTupleTool * m_vtx_analyser;

  IParticleDescendants* m_descend;
  IParticle2MCAssociator* m_p2mcAssoc;

  IDistanceCalculator*  m_Geom;
  ITrackVertexer * m_trackVertexer;
  ISequencerTimerTool* m_timerTool ; 
  int m_timer ; 

  std::string m_outputLocation;
//   std::string m_inputTrack4VFLocation;
//   std::string m_inputTrackLocation;
  bool m_mc;
  bool m_mc_only_ZVTOP;
  bool m_mc_only_Vf;

  bool m_opt;
  bool m_rdn_ips;
  
  std::string m_p2mcAssocType;

  std::string m_topoVertexToolType;
  std::string m_topoVertexToolName;

  std::string m_filterTracksToolType;
  std::string m_filterTracksToolName;

  std::string m_filterTracksTypeToolType;
  std::string m_filterTracksTypeToolName;

  std::string m_mcFilterTracksToolType;
  std::string m_mcFilterTracksToolName;


  std::string m_filterVfToolType;
  std::string m_filterVfToolName;


  std::string m_distanceCalculatorToolType; ///< Type of the distance calculator Tool

  std::string m_trackVertexerToolName; ///< Name of the track vertexer Tool
  std::string m_trackVertexerToolType; ///< Type of the track vertexer Tool

  bool isMatched(const  LHCb::Particle* P);
  double hasPartancestor(const LHCb::MCParticle*, const int );

  //trigger
  std::string m_HLT1Loc;

  //stripping
  IBackgroundCategory*  m_bkgCategory;
}; 


#endif // TOPOVERTEXALG_H
