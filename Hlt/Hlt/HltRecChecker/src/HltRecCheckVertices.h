// $Id: HltRecCheckVertices.h,v 1.8 2009-12-23 15:03:36 graven Exp $
#ifndef HLTRECCHECKVERTICES_H 
#define HLTRECCHECKVERTICES_H 1

// Include files
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/MCParticle.h"
#include "ERelations.h"

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

private:

  std::string m_linkName;

  std::string m_TESInputVerticesName;
  LHCb::RecVertices* m_TESInputVertices;
  
  Hlt::SelectionContainer<LHCb::Track,LHCb::Track> m_selections;


  AIDA::IHistogram1D* m_histoNRCV;
  AIDA::IHistogram1D* m_histoNMCV;
  AIDA::IHistogram1D* m_histoNDV;

  AIDA::IHistogram1D* m_histoX;
  AIDA::IHistogram1D* m_histoY;
  AIDA::IHistogram1D* m_histoZ;

  AIDA::IHistogram1D* m_histoDX;
  AIDA::IHistogram1D* m_histoDY;
  AIDA::IHistogram1D* m_histoDZ;

  zen::relation<LHCb::Track*,LHCb::MCVertex*> m_relTrackMCVertex;
  zen::relation<LHCb::MCVertex*,LHCb::Track*> m_relMCVertexTrack;

  zen::relation<LHCb::RecVertex*,LHCb::MCVertex*> m_relVertexMCVertex;
  zen::relation<LHCb::MCVertex*,LHCb::RecVertex*> m_relMCVertexVertex;



};
#endif // HLTMCMONITOR_H
