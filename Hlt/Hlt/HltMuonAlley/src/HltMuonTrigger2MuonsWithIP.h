// $Id: HltMuonTrigger2MuonsWithIP.h,v 1.1 2007-07-12 18:03:15 asatta Exp $
#ifndef HLTMUONTRIGGER2MUONSWITHIP_H 
#define HLTMUONTRIGGER2MUONSWITHIP_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltMuonAlleyPreTrigger HltMuonAlleyPreTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonTrigger2MuonsWithIP : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonTrigger2MuonsWithIP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonTrigger2MuonsWithIP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void makeDiMuonPair(const Hlt::TrackContainer& tcon1,
                      const Hlt::TrackContainer& tcon2,
                      // Estd::bifunction<Track,Track>& fun,
                      // Estd::filter<double>& cut,
                      Hlt::VertexContainer& vcon);
  StatusCode checkIfL0Dimuon(LHCb::RecVertex* vertexToTest);
  
protected:

  //  double m_ptMin;

  //  HltHisto m_histoPt;
  // HltHisto m_histoPt1;

protected:

  

  Estd::filter<LHCb::Track>* _negMuonFil;
  Estd::filter<LHCb::Track>* _posMuonFil;
  Estd::filter<LHCb::RecVertex>*  _massAndIPCut;
  //  Estd::filter<LHCb::RecVertex>*  _massCut;
  Estd::filter<LHCb::RecVertex>*  _massAndDOCAAndIPCut;
  //Estd::filter<LHCb::RecVertex>*  _massAndDOCACut;
  Estd::filter<LHCb::RecVertex>*  _massAndIPAndL0Cut;
  //Estd::filter<LHCb::RecVertex>*  _massAndL0Cut;
  Estd::filter<LHCb::RecVertex>*  _triggerMassAndIPCut;
  //Estd::filter<LHCb::RecVertex>*  _triggerMassCut;

  Estd::function<LHCb::Track>* _chargeFun;
  Hlt::TrackVertexBiFunction*   _ipFun;
  //Estd::filter<LHCb::Track>*   _ptCut;
 Hlt::TrackBiFunction*   _docaFun;


  Hlt::SortTrackByPt _sortByPT;
  int m_IPKey;
  int m_massKey;
  int m_DOCAKey;
  int m_MuonKey;
  double m_minMassWithIP;
  double m_minMassNoIP;  
  double m_minIP;
  double m_maxDOCA;
  int m_L0confirm;
  Hlt::TrackContainer m_otrack;
  Hlt::TrackContainer m_otrack2;

  Hlt::VertexContainer m_overtices;  
  Hlt::VertexContainer m_selevertices;  
  Hlt::VertexContainer m_testedvertices;
  Hlt::VertexCreator _vertexCreator;
  HltHisto h_mass;
  HltHisto h_DOCA;
  HltHisto h_IP;

  PatVertexContainer* m_patVertexBank;
  std::string m_selection2SummaryName;
  int m_selection2SummaryID;
  std::string m_outputDimuonVerticesName;
  bool m_pretrigger;  

};
#endif // HLTMUONTRIGGER2MUONSWITHIP_H
