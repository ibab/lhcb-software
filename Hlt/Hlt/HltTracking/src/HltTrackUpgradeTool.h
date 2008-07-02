// $Id: HltTrackUpgradeTool.h,v 1.10 2008-07-02 19:32:02 graven Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADETOOL_H 
#define HLTTRACKING_HLTTRACKUPGRADETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/HltTypes.h"
#include "HltBase/HltUtils.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/ITrackView.h"
#include "HltBase/HltBaseTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"
#include "HltBase/EParser.h"

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

static const InterfaceID IID_HltTrackUpgradeTool ( "HltTrackUpgradeTool",1,0 );

class HltTrackUpgradeTool : public HltBaseTool, virtual public ITrackUpgrade {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_HltTrackUpgradeTool; }

  /// Standard constructor
  HltTrackUpgradeTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~HltTrackUpgradeTool( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  std::vector<std::string> recos();

  StatusCode setReco(const std::string& name);

  std::string reco() 
  {return m_recoName;}
  
  void beginExecute();

  // this method is save, it takes care of the memory
  StatusCode upgrade(std::vector<LHCb::Track*>& itracks,
                     std::vector<LHCb::Track*>& otracks);

  // this method is save, it takes care of the memory
  StatusCode upgrade(LHCb::Track& seed,
                     std::vector<LHCb::Track*>& track);

  // this method is unsave, it does not take care of the memory
  StatusCode iupgrade(LHCb::Track& seed,
                      std::vector<LHCb::Track*>& track);

  
  Tf::IStationSelector& view(const LHCb::Track& seed);
  
  std::vector<LHCb::LHCbID> lhcbIDsInView(const LHCb::Track& seed);

private:

  void recoConfiguration();

  void toolConfigure();

  bool isReco(const LHCb::Track& seed);
  
  size_t find(const LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
  void recoDone(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);

  void addIDs(const LHCb::Track& seed, LHCb::Track& track);

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
  bool m_owner;
  int m_trackType;

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
