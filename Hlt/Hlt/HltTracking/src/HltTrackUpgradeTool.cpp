// $Id: HltTrackUpgradeTool.cpp,v 1.25 2008-07-09 14:15:38 hernando Exp $
// Include files
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackUpgradeTool.h"
#include "Event/HltEnums.h"
#include "HltBase/ESequences.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackUpgradeTool
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( HltTrackUpgradeTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackUpgradeTool::HltTrackUpgradeTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : HltBaseTool ( type, name , parent )
  , m_timer(0)
  , m_otracks(0)
  , m_tool(0)
{
  declareInterface<HltTrackUpgradeTool>(this);
  declareInterface<ITrackUpgrade>(this);

  declareProperty("RecoName", m_recoName = "empty");
  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  declareProperty("OrderByPt", m_orderByPt = false);
  declareProperty("DoTrackReco",m_doTrackReco = true);

};

void HltTrackUpgradeTool::recoConfiguration() {

  std::string info = "InfoID";

  m_recoConf.add("TConf/Tool",std::string("L0ConfirmWithT"));
  m_recoConf.add("TConf/Owner",true);
  m_recoConf.add("TConf/View",true);
  m_recoConf.add("TConf/TransferIDs",true);
  m_recoConf.add("TConf/TransferAncestor",true);
  m_recoConf.add("TConf/TrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TConf/TESOutput",std::string("Hlt1/Track/TConf"));

  m_recoConf.add("TMuonConf/Tool",std::string("L0ConfirmWithT/TMuonConf"));
  m_recoConf.add("TMuonConf/Owner",true);
  m_recoConf.add("TMuonConf/View",true);
  m_recoConf.add("TMuonConf/TransferIDs",true);
  m_recoConf.add("TMuonConf/TransferAncestor",true);
  m_recoConf.add("TMuonConf/TrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TMuonConf/TESOutput",std::string("Hlt1/Track/TMuonConf"));

  m_recoConf.add("THadronConf/Tool",std::string("L0ConfirmWithT/THadronConf"));
  m_recoConf.add("THadronConf/Owner",true);
  m_recoConf.add("THadronConf/View",true);
  m_recoConf.add("THadronConf/TransferIDs",true);
  m_recoConf.add("THadronConf/TransferAncestor",true);
  m_recoConf.add("THadronConf/TrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("THadronConf/TESOutput",std::string("Hlt1/Track/THadronConf"));

  m_recoConf.add("TEleConf/Tool",std::string("L0ConfirmWithT/TEleConf"));
  m_recoConf.add("TEleConf/Owner",true);
  m_recoConf.add("TEleConf/View",true);
  m_recoConf.add("TEleConf/TransferIDs",true);
  m_recoConf.add("TEleConf/TransferAncestor",true);
  m_recoConf.add("TEleConf/TrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TEleConf/TESOutput",std::string("Hlt1/Track/TEleConf"));

  m_recoConf.add("Velo/Tool",std::string("Tf::PatVeloSpaceTool"));
  m_recoConf.add("Velo/Owner",true);
  m_recoConf.add("Velo/View",false);
  m_recoConf.add("Velo/TransferIDs",false);
  m_recoConf.add("Velo/TransferAncestor",false);
  m_recoConf.add("Velo/TrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("Velo/TESOutput",std::string("Hlt1/Track/Velo"));
  
  m_recoConf.add("VeloTT/Tool",std::string("Tf::PatVeloTTTool"));
  m_recoConf.add("VeloTT/Owner",false);
  m_recoConf.add("VeloTT/View",false);
  m_recoConf.add("VeloTT/TransferIDs",false);
  m_recoConf.add("VeloTT/TransferAncestor",false);
  m_recoConf.add("VeloTT/TrackType", (int) LHCb::Track::Upstream);
  m_recoConf.add("VeloTT/TESOutput", std::string("Hlt1/Track/VeloTT"));

  m_recoConf.add("Forward/Tool",std::string("PatForwardTool"));
  m_recoConf.add("Forward/Owner",true);
  m_recoConf.add("Forward/View",false);
  m_recoConf.add("Forward/TransferIDs",false);
  m_recoConf.add("Forward/TransferAncestor",false);
  m_recoConf.add("Forward/TrackType", (int) LHCb::Track::Long);
  m_recoConf.add("Forward/TESOutput", std::string("Hlt1/Track/Forward"));

  m_recoConf.add("GuidedForward/Tool",std::string("HltGuidedForward"));
  m_recoConf.add("GuidedForward/Owner",true);
  m_recoConf.add("GuidedForward/View",false);
  m_recoConf.add("GuidedForward/TransferIDs",false);
  m_recoConf.add("GuidedForward/TransferAncestor",false);
  m_recoConf.add("GuidedForward/TrackType", (int) LHCb::Track::Long);
  m_recoConf.add("GuidedForward/TESOutput",
                 std::string("Hlt1/Track/GuidedForward"));

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgradeTool::~HltTrackUpgradeTool() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgradeTool::initialize() {

  StatusCode sc = HltBaseTool::initialize();
  if ( sc.isFailure() ) return sc;

  recoConfiguration();

  m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );

  // sc = setReco(m_recoName);
  // if ( sc.isFailure() ) return sc;
  return sc;
};

std::vector<std::string> HltTrackUpgradeTool::recos() {
  std::vector<std::string> mrecos;
  std::vector<std::string> keys = m_recoConf.keys();
  for (std::vector<std::string>::iterator it = keys.begin();
       it != keys.end(); it++) {
    std::string key = *it;
    std::vector<std::string> cromos = EParser::parse(key,"/");
    if (cromos.size()==2 && cromos[1] == "Tool")
      mrecos.push_back(cromos[0]);
  }
  return mrecos;
}


StatusCode HltTrackUpgradeTool::setReco(const std::string& key) 
{
  m_recoName = key;
  debug() << " InitReco " << endreq;
  bool ok = m_recoConf.has_key(m_recoName+"/Tool");
  Assert(ok," no reconstruction with name "+m_recoName);

  std::string toolName = m_recoConf.retrieve<std::string>(m_recoName+"/Tool");
  m_recoID   = hltInfoID(toolName);

  m_TESOutput = m_recoConf.retrieve<std::string>(m_recoName+"/TESOutput");
  m_owner = m_recoConf.retrieve<bool>(m_recoName+"/Owner");
  m_trackType = m_recoConf.retrieve<int>(m_recoName+"/TrackType");
  m_transferIDs = m_recoConf.retrieve<bool>(m_recoName+"/TransferIDs");
  m_transferAncestor = 
    m_recoConf.retrieve<bool>(m_recoName+"/TransferAncestor");
  
  if (m_recoID <= HltEnums::VeloKey) m_orderByPt = false;

  info() << " Reco: " << m_recoName 
         << " Tool: " << toolName
         << " Output: " << m_TESOutput << endreq;
  
  debug() << " reco ID " << m_recoID
          << " owner " << m_owner << " transfers IDs " << m_transferIDs 
          << " transfer ancestor " << m_transferAncestor
          << " track type " << m_trackType << endreq;
  
  m_tool = 0;
  bool hasView = m_recoConf.retrieve<bool>(m_recoName+"/View");
  m_viewTool = 0;
  if (m_toolsmap.find(key) != m_toolsmap.end()) {
    m_tool = m_toolsmap[key];
    if (hasView) m_viewTool = m_viewsmap[key];
  } else {
    toolConfigure();
    m_tool = tool<ITracksFromTrack>(toolName,this);
    m_toolsmap[key] = m_tool;
    if (hasView) {
      m_viewTool = tool<ITrackView>(toolName,this);
      m_viewsmap[key] = m_viewTool;
    }
  }
  
  Assert(m_tool," setReco() not able to create tool "+toolName);

  // m_timer = tool<ISequencerTimerTool>("SequencerTimerTool");
  m_timer->increaseIndent();
  m_timerTool = m_timer->addTimer(toolName);
  m_timer->decreaseIndent();

  return StatusCode::SUCCESS;
}

void HltTrackUpgradeTool::toolConfigure() 
{
  IJobOptionsSvc* optSvc = svc<IJobOptionsSvc>( "JobOptionsSvc" );
  std::string toolname = 
    m_recoConf.retrieve<std::string>(m_recoName+"/Tool");
  std::vector<std::string> cromos = EParser::parse(toolname,"/");
  if (cromos.size() != 2) return;
  std::string root = name()+"."+cromos[1];
  if (m_recoName == "TMuonConf") {
    SimpleProperty<int> pp = SimpleProperty<int>("particleType",0);
    optSvc->addPropertyToCatalogue(root,pp);
    debug() << " setting property " << root << pp << endreq;
  } else if (m_recoName == "THadronConf") {
    SimpleProperty<std::string> pp0 = 
      SimpleProperty<std::string>("trackingTool","PatConfirmTool");
    optSvc->addPropertyToCatalogue(root,pp0);
    SimpleProperty<int> pp1 = SimpleProperty<int>("particleType",1);
    optSvc->addPropertyToCatalogue(root,pp1);
    debug() << " setting property " << root << pp0 << endreq;
    debug() << " setting property " << root << pp1 << endreq;
  } else if (m_recoName == "TEleConf") {
    SimpleProperty<int> pp = SimpleProperty<int>("particleType",2);
    optSvc->addPropertyToCatalogue(root,pp);
    debug() << " setting property " << root << pp << endreq;
  }
  release(optSvc);
}


//=============================================================================
// Main execution
//=============================================================================

void HltTrackUpgradeTool::beginExecute() {
  if (m_owner)
    m_otracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_TESOutput);
}

StatusCode HltTrackUpgradeTool::upgrade(std::vector<Track*>& itracks,
                                        std::vector<Track*>& otracks) {
  StatusCode sc = StatusCode::SUCCESS;
  beginExecute();
  
  std::vector<LHCb::Track*> tracks;
  for (std::vector<Track*>::iterator it = itracks.begin();
       it != itracks.end(); ++it) {
    sc = iupgrade(**it,tracks);
    if (sc.isFailure()) {
        warning() << " failed to upgrade... " << endmsg;
    }
    otracks.insert(otracks.end(),tracks.begin(),tracks.end());
  }
  if (m_orderByPt)
    std::sort(otracks.begin(),otracks.end(), Hlt::SortTrackByPt());
  
  debug() << " upgraded " << otracks.size() << " tracks " << endreq;
  return sc;
}

StatusCode HltTrackUpgradeTool::upgrade(Track& itrack,
                                        std::vector<Track*>& otracks) {
  StatusCode sc = StatusCode::SUCCESS;
  beginExecute();
  
  iupgrade(itrack,otracks);
  
  debug() << " upgraded " << otracks.size() << " tracks " << endreq;
  return sc;
}



StatusCode HltTrackUpgradeTool::iupgrade(LHCb::Track& seed,
                                        std::vector<LHCb::Track*>& tracks) {
  StatusCode sc = StatusCode::SUCCESS;
  tracks.clear();
  if (seed.checkType( (LHCb::Track::Types) m_trackType)) {
    tracks.push_back(&seed);
    verbose() << " seed is its upgraded track  " << m_trackType << endreq;
  } else if (!isReco(seed)) {
    m_timer->start(m_timerTool);
    sc = m_tool->tracksFromTrack(seed,tracks);
    m_timer->stop(m_timerTool);
    if (sc.isFailure()) {
      // TODO Warning(" reconstruction failure ",0);
      return sc;
    }
    recoDone(seed,tracks);
    verbose() << " seed upgraded, reco tracks " << tracks.size() << endreq;
  } else {
    find(seed,tracks);
    verbose()<< " seed was upgraded, found tracks " << tracks.size() << endreq;
  }
  if (!tracks.empty()) {
    printInfo(" upgrade seed    ", seed);
    printInfo("         tracks  ", tracks);
  }
  return sc;
}

bool HltTrackUpgradeTool::isReco(const Track& seed) {
  if (!m_doTrackReco) return false;
  int ncan = (int) seed.info(m_recoID,-1);
  bool ok = (ncan != -1);
  verbose() << " has Reco ?" << ok << " n-descendants " << ncan << endreq;
  return ok;
}


void HltTrackUpgradeTool::recoDone(Track& seed, 
                                   std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  for (std::vector<Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    Track& track = *(*it);
    track.setType( (LHCb::Track::Types) m_trackType );
    if (m_transferAncestor) track.addToAncestors( (Track*) &seed);
    if (m_transferIDs) addIDs(seed,track);
    if (m_transferExtraInfo) track.setExtraInfo(seed.extraInfo());
    track.addInfo(m_recoID,key);
  }
  seed.addInfo(m_recoID, (double) tracks.size());
  debug() << " seed " << seed.key() << " n-descendants " 
          << seed.info(m_recoID,-1) << endreq;
  if (m_owner) {
    for (std::vector<Track*>::iterator it = tracks.begin();
         it != tracks.end(); ++it)
      m_otracks->insert(*it); 
  }  
}

size_t HltTrackUpgradeTool::find(const Track& seed,
                             std::vector<Track*>& tracks) {
  /// @todo This comparison of doubles must be fixed
  if (!m_doTrackReco) return 0;
  double key = (double) seed.key();
  double ncan = (double) seed.info(m_recoID,-1);
  debug() << " find seed key " << key << " n-descendants " << ncan << endreq;
  for (Tracks::iterator it = m_otracks->begin();it != m_otracks->end();++it) {
    Track* ptrack = *it;
    const SmartRefVector<LHCb::Track>& ancestors = ptrack->ancestors();
    bool ok = false;
    for (SmartRefVector<LHCb::Track>::const_iterator 
           it2 = ancestors.begin(); it2 != ancestors.end(); it2++) 
      if ((const LHCb::Track*) *it2 == (const LHCb::Track*) &seed) ok = true;
    if (ok) tracks.push_back(ptrack);    
    // if (ptrack->info(m_recoID,-1) == key)
  }
  if (ncan != (double) tracks.size()) {
    Error("Different number of tracks",StatusCode::SUCCESS,1);
    debug() << " different number of stored tracks! " << ncan 
            << " != " << tracks.size() << endreq;
    for (Tracks::iterator it = m_otracks->begin();
         it != m_otracks->end();++it) {
      Track& track = *(*it);
      debug() << " key " << key << " mother " 
              << track.info(m_recoID,-1) << endreq;
    }
  }  

  verbose() << " found tracks " << tracks.size() << endreq;
  return tracks.size();
}

void HltTrackUpgradeTool::printInfo(const std::string& title,
                                    const std::vector<Track*>& con) {
  for (Hlt::TrackContainer::const_iterator it = con.begin(); 
       it != con.end(); it++) 
    printInfo(title,*(*it));
}

void HltTrackUpgradeTool::printInfo(const std::string& title,
                                    const Track& track) {
  debug() << title << " track  " << track.key() << " slopes " 
          << track.slopes()  << " pt " << track.pt() << endreq;
}

void HltTrackUpgradeTool::addIDs(const Track& seed, Track& track) {
  const std::vector<LHCbID>& ids = seed.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); ++it) 
    track.addToLhcbIDs(*it);
}

