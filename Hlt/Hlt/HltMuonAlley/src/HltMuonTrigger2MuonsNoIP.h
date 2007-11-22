// $Id: HltMuonTrigger2MuonsNoIP.h,v 1.3 2007-11-22 10:59:09 sandra Exp $
#ifndef HLTMUONTRIGGER2MUONSNOIP_H 
#define HLTMUONTRIGGER2MUONSNOIP_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltMuonAlleyPreTrigger HltMuonAlleyPreTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonTrigger2MuonsNoIP : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonTrigger2MuonsNoIP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonTrigger2MuonsNoIP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void makeDiMuonPair(Hlt::VertexContainer& vcon);

  StatusCode checkIfL0Dimuon(LHCb::RecVertex* vertexToTest);
  StatusCode checkIfSameAncestor(LHCb::RecVertex* vertexToTest);

  
protected:

  Estd::filter<LHCb::RecVertex>*  _massCut;
  Estd::filter<LHCb::RecVertex>*  _massAndDOCACut;
  Estd::filter<LHCb::RecVertex>*  _triggerMassCut;



  Hlt::SortTrackByPt _sortByPT;
  int m_massKey;
  int m_DOCAKey;
  int m_MuonKey;
  double m_minMassNoIP;  
  double m_maxDOCA;

  Hlt::VertexContainer m_overtices;  
  Hlt::VertexContainer m_selevertices;  
  Hlt::VertexContainer m_testedvertices;
  Hlt::VertexCreator _vertexCreator;
  HltHisto h_mass;
  HltHisto h_DOCA;

  std::string m_outputDimuonVerticesName;
};
#endif // HLTMUONTRIGGER2MUONSNOIP_H
