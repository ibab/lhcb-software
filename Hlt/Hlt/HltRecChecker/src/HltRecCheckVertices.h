// $Id: HltRecCheckVertices.h,v 1.1 2006-10-24 09:50:49 hernando Exp $
#ifndef HLTRECCHECKVERTICES_H 
#define HLTRECCHECKVERTICES_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "Event/MCParticle.h"
#include "HltBase/ERelations.h"
#include "HltBase/HltFunctions.h"

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
  
  Estd::bifunction<LHCb::Track,LHCb::RecVertex>* m_ipFun;

protected:

  HltHisto m_histoNRCV;
  HltHisto m_histoNMCV;
  HltHisto m_histoNDV;

  HltHisto m_histoX;
  HltHisto m_histoY;
  HltHisto m_histoZ;

  HltHisto m_histoDX;
  HltHisto m_histoDY;
  HltHisto m_histoDZ;

protected:

  Estd::relation<LHCb::Track,LHCb::MCVertex> m_relTrackMCVertex;
  Estd::relation<LHCb::MCVertex,LHCb::Track> m_relMCVertexTrack;

  Estd::relation<LHCb::RecVertex,LHCb::MCVertex> m_relVertexMCVertex;
  Estd::relation<LHCb::MCVertex,LHCb::RecVertex> m_relMCVertexVertex;



};
#endif // HLTMCMONITOR_H
