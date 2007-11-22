// $Id: HltMuonTrigger2MuonsWithIP.h,v 1.3 2007-11-22 10:59:09 sandra Exp $
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
  void makeDiMuonPair(Hlt::VertexContainer& vcon);

  StatusCode checkIfL0Dimuon(LHCb::RecVertex* vertexToTest);
  StatusCode checkIfSameAncestor(LHCb::RecVertex* vertexToTest); 


protected:


  Estd::filter<LHCb::RecVertex>*  _massAndDOCAAndIPCut;
  Estd::filter<LHCb::RecVertex>*  _triggerMassAndIPCut;

  Hlt::TrackVertexBiFunction*   _ipFun;


  Hlt::SortTrackByPt _sortByPT;
  int m_IPKey;
  int m_massKey;
  int m_DOCAKey;
  int m_MuonKey;
  double m_minMassWithIP;
  double m_minIP;
  double m_maxDOCA;

  Hlt::VertexContainer m_overtices;  
  Hlt::VertexContainer m_selevertices;  
  Hlt::VertexContainer m_testedvertices;
  Hlt::VertexCreator _vertexCreator;
  HltHisto h_mass;
  HltHisto h_DOCA;
  HltHisto h_IP;

  std::string m_outputDimuonVerticesName;
};
#endif // HLTMUONTRIGGER2MUONSWITHIP_H
