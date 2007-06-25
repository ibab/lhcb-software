// $Id: HltVertexUpgrade.h,v 1.1 2007-06-25 20:56:48 hernando Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADE_H 
#define HLTTRACKING_HLTTRACKUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/EDictionary.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/HltFunctions.h"

/** @class HltVertexUpgrade HltVertexUpgrade.h
 *  
 *  functionality:
 *  Options:
 *        RecoName: name of the reconstruction to be done,
 *           valid names: "Velo","VeloTT","Forward","MuonForward"
 *        OrderByPt: true/false order by Pt output tracks in HltDataStore
 *
 *  Note:

 *  @author Jose Angel Hernando Morata
 *  @date   2006-08-28
 */
class HltVertexUpgrade : public HltAlgorithm {
public: 
  /// Standard constructor
  HltVertexUpgrade( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexUpgrade( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void recoConfiguration();

  StatusCode initReco();
  
  void saveConfiguration();

  size_t upgrade(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
protected:

  bool isReco(const LHCb::Track& seed);
  
  size_t find(const LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
  void recoDone(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);

protected:

  std::vector<LHCb::Track*> m_tracks1;

  std::vector<LHCb::Track*> m_tracks2;

  std::string m_recoName;
  int m_recoID;

  bool m_transferExtraInfo;

protected:

  Estd::dictionary m_recoConf;

  std::string m_patOutputVerticesName;

  PatVertexContainer* m_patOutputVertices;

  PatTrackContainer* m_patOutputTracks;

  ITrackUpgrade* m_tool;

  HltHisto m_histoPatOutputTracks;

  Hlt::SortTrackByPt _sortByPt;

  Hlt::VertexCreator _makeVertex;

};
#endif // HLTTRACKING_H
