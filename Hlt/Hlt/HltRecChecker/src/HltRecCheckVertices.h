// $Id: HltRecCheckVertices.h,v 1.3 2008-01-22 11:04:06 hernando Exp $
#ifndef HLTRECCHECKVERTICES_H 
#define HLTRECCHECKVERTICES_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "Event/MCParticle.h"
#include "HltBase/ERelations.h"
#include "HltBase/HltTypes.h"

/** @class HltRecCheckVertices HltRecCheckVertices.h
 *  
 *
 *  @author Jose A. Hernando
 *  @date   2006-05-24
 */
class HltRecCheckVertices : public HltAlgorithm {
public: 
  /// Standard constructor
  HltRecCheckVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRecCheckVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void relateVertices();
  
  void checkVertices();

protected:

  std::string m_linkName;
  std::string m_ipType;

  std::string m_TESInputVerticesName;
  LHCb::RecVertices* m_TESInputVertices;
  
  zen::bifunction<LHCb::Track,LHCb::RecVertex>* m_ipFun;

protected:

  Hlt::Histo* m_histoNRCV;
  Hlt::Histo* m_histoNMCV;
  Hlt::Histo* m_histoNDV;

  Hlt::Histo* m_histoX;
  Hlt::Histo* m_histoY;
  Hlt::Histo* m_histoZ;

  Hlt::Histo* m_histoDX;
  Hlt::Histo* m_histoDY;
  Hlt::Histo* m_histoDZ;

protected:

  zen::relation<LHCb::Track*,LHCb::MCVertex*> m_relTrackMCVertex;
  zen::relation<LHCb::MCVertex*,LHCb::Track*> m_relMCVertexTrack;

  zen::relation<LHCb::RecVertex*,LHCb::MCVertex*> m_relVertexMCVertex;
  zen::relation<LHCb::MCVertex*,LHCb::RecVertex*> m_relMCVertexVertex;



};
#endif // HLTMCMONITOR_H
