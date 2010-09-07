// $Id: HltTrackUpgradeTool.h,v 1.22 2010-09-07 12:45:04 graven Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADETOOL_H 
#define HLTTRACKING_HLTTRACKUPGRADETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/ITrackView.h"
#include "HltBase/HltBaseTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"
#include "EDictionary.h"

/** @class HltTrackUpgradeTool HltTrackUpgradeTool.h
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

class HltTrackUpgradeTool : public HltBaseTool, virtual public ITrackUpgrade {
public: 

  /// Standard constructor
  HltTrackUpgradeTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~HltTrackUpgradeTool( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  StatusCode setReco(const std::string& name);

  std::string reco() 
  {return m_recoName;}
  
  void beginExecute();

  // this method is save, it takes care of the memory
  StatusCode upgrade
  ( const std::vector<const LHCb::Track*>& itracks ,
    std::vector<LHCb::Track*>&       otracks );
  
  // this method is save, it takes care of the memory
  StatusCode upgrade(const LHCb::Track& seed,
                     std::vector<LHCb::Track*>& track);

  // this method is unsave, it does not take care of the memory
  StatusCode iupgrade(const LHCb::Track& seed,
                      std::vector<LHCb::Track*>& track);
  
  std::vector<Tf::IStationSelector*> view(const LHCb::Track& seed)
  {
    Assert(m_viewTool!=0,"view() method not implemented for this reco!");
    return m_viewTool->view(seed);
  }
  
  std::vector<LHCb::LHCbID> lhcbIDsInView(const LHCb::Track& seed)
  {
    Assert(m_viewTool!=0,"view() method not implemented for this reco!");
    return m_viewTool->lhcbIDsInView(seed);
  }

private:

  void recoConfiguration();

  void toolConfigure();

  bool isReco(const LHCb::Track& track);

  bool isOutput(const LHCb::Track& track);
  
  size_t find(const LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
  void recoDone(const LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);


  void printInfo(const std::string& title, 
                 const std::vector<LHCb::Track*>& con);

  void printInfo(const std::string& title, const LHCb::Track& track);

  template <class T>
  void recoregister(const std::string& name, const std::string& prop,
                    const T& t) {
    std::string key = name+"/"+prop;m_recoConf.add(key,t);
    info() << " reco ["<<key<<"] = " << t <<endreq;
  }

  std::string m_TESOutput;
  std::string m_recoName;
  
  int m_recoID;
  int m_ItrackType;
  int m_OtrackType;

  bool m_transferExtraInfo;
  bool m_transferIDs;
  bool m_transferAncestor;
  bool m_orderByPt;
  bool m_doTrackReco;

  ISequencerTimerTool* m_timer;
  int m_timerTool;
  zen::dictionary m_recoConf;

  LHCb::Tracks* m_otracks;

  ITracksFromTrack* m_tool;  
  std::map<std::string,ITracksFromTrack*> m_toolsmap;

  ITrackView* m_viewTool;
  std::map<std::string,ITrackView*> m_viewsmap;
  
};
#endif // HLTTRACKING_H
