// $Id: HltMuonPreTriggerDiMuons.h,v 1.2 2006-09-27 14:18:04 cattanem Exp $
#ifndef HLTMUONPRETRIGGERDIMUONS_H 
#define HLTMUONPRETRIGGERDIMUONS_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltMuonAlleyPreTrigger HltMuonAlleyPreTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonPreTriggerDiMuons : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonPreTriggerDiMuons( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonPreTriggerDiMuons( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void makeDiMuonPair(const Hlt::TrackContainer& tcon1,
                      const Hlt::TrackContainer& tcon2,
                      // Estd::bifunction<Track,Track>& fun,
                      // Estd::filter<double>& cut,
                      Hlt::VertexContainer& vcon);
  
protected:

  //  double m_ptMin;

  //  HltHisto m_histoPt;
  // HltHisto m_histoPt1;

protected:

  

  Estd::filter<LHCb::Track>* _negMuonFil;
  Estd::filter<LHCb::Track>* _posMuonFil;
  Estd::filter<LHCb::RecVertex>*  _massAndIPCut;
  Estd::filter<LHCb::RecVertex>*  _massCut;
  Estd::function<LHCb::Track>* _chargeFun;
  Hlt::trackvertex_bifunction* _ipFun;
  //Estd::filter<LHCb::Track>*   _ptCut;


  Hlt::SortTrackByPt _sortByPT;
  int m_IPKey;
  int m_massKey;
  int m_DOCAKey;
  double m_minMassWithIP;
  double m_minMassNoIP;  
  double m_minIP;
  Hlt::TrackContainer m_otrack;
  Hlt::TrackContainer m_otrack2;

  Hlt::VertexContainer m_overtices;  
  Hlt::VertexContainer m_selevertices;
  Hlt::VertexCreator _vertexCreator;
  HltHisto h_mass;
  HltHisto h_DOCA;
  HltHisto h_IP;


};
#endif // HLTMUONPRETRIGGERDIMUONS_H
