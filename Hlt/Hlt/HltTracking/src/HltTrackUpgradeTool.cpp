// $Id: HltTrackUpgradeTool.cpp,v 1.34 2009-06-30 20:34:58 aperezca Exp $
// Include files
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackUpgradeTool.h"
#include "EDictionary.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
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

//   //Generic tool not used, use especific instead
//   m_recoConf.add("TConf/Tool",std::string("L0ConfirmWithT"));
//   m_recoConf.add("TConf/Owner",true);
//   m_recoConf.add("TConf/View",true);
//   m_recoConf.add("TConf/TransferIDs",true);
//   m_recoConf.add("TConf/TransferAncestor",true);
//   m_recoConf.add("TConf/ITrackType", (int) LHCb::Track::TypeUnknown);  
//   m_recoConf.add("TConf/OTrackType", (int) LHCb::Track::Ttrack);
//   m_recoConf.add("TConf/TESOutput",std::string("Hlt1/Track/TConf"));

  m_recoConf.add("TMuonConf/Tool",std::string("L0ConfirmWithT/TMuonConf"));
  m_recoConf.add("TMuonConf/Owner",true);
  m_recoConf.add("TMuonConf/View",true);
  m_recoConf.add("TMuonConf/TransferIDs",true);
  m_recoConf.add("TMuonConf/TransferAncestor",true);
  m_recoConf.add("TMuonConf/ITrackType", (int) LHCb::Track::Muon);
  m_recoConf.add("TMuonConf/OTrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TMuonConf/TESOutput",std::string("Hlt1/Track/TMuonConf"));

  m_recoConf.add("THadronConf/Tool",std::string("L0ConfirmWithT/THadronConf"));
  m_recoConf.add("THadronConf/Owner",true);
  m_recoConf.add("THadronConf/View",true);
  m_recoConf.add("THadronConf/TransferIDs",true);
  m_recoConf.add("THadronConf/TransferAncestor",true);
  m_recoConf.add("THadronConf/ITrackType", (int) LHCb::Track::TypeUnknown);
  m_recoConf.add("THadronConf/OTrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("THadronConf/TESOutput",std::string("Hlt1/Track/THadronConf"));

  m_recoConf.add("TEleConf/Tool",std::string("L0ConfirmWithT/TEleConf"));
  m_recoConf.add("TEleConf/Owner",true);
  m_recoConf.add("TEleConf/View",true);
  m_recoConf.add("TEleConf/TransferIDs",true);
  m_recoConf.add("TEleConf/TransferAncestor",true);
  m_recoConf.add("TEleConf/ITrackType", (int) LHCb::Track::TypeUnknown);
  m_recoConf.add("TEleConf/OTrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TEleConf/TESOutput",std::string("Hlt1/Track/TEleConf"));  

  m_recoConf.add("Velo/Tool",std::string("Tf::PatVeloSpaceTool"));
  m_recoConf.add("Velo/Owner",true);
  m_recoConf.add("Velo/View",false);
  m_recoConf.add("Velo/TransferIDs",true);
  m_recoConf.add("Velo/TransferAncestor",true);
  m_recoConf.add("Velo/ITrackType", (int) LHCb::Track::VeloR);
  m_recoConf.add("Velo/OTrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("Velo/TESOutput",std::string("Hlt1/Track/Velo"));
  
  m_recoConf.add("VeloTT/Tool",std::string("Tf::PatVeloTTTool"));
  m_recoConf.add("VeloTT/Owner",false);
  m_recoConf.add("VeloTT/View",false);
  m_recoConf.add("VeloTT/TransferIDs",true);
  m_recoConf.add("VeloTT/TransferAncestor",true);
  m_recoConf.add("VeloTT/ITrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("VeloTT/OTrackType", (int) LHCb::Track::Upstream);
  m_recoConf.add("VeloTT/TESOutput", std::string("Hlt1/Track/VeloTT"));

  m_recoConf.add("Forward/Tool",std::string("PatForwardTool"));
  m_recoConf.add("Forward/Owner",true);
  m_recoConf.add("Forward/View",false);
  m_recoConf.add("Forward/TransferIDs",true);
  m_recoConf.add("Forward/TransferAncestor",true);
  m_recoConf.add("Forward/ITrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("Forward/OTrackType", (int) LHCb::Track::Long);
  m_recoConf.add("Forward/TESOutput", std::string("Hlt1/Track/Forward"));

  m_recoConf.add("GuidedForward/Tool",std::string("HltGuidedForward"));
  m_recoConf.add("GuidedForward/Owner",true);
  m_recoConf.add("GuidedForward/View",false);
  m_recoConf.add("GuidedForward/TransferIDs",true);
  m_recoConf.add("GuidedForward/TransferAncestor",true);
  m_recoConf.add("GuidedForward/ITrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("GuidedForward/OTrackType", (int) LHCb::Track::Long);
  m_recoConf.add("GuidedForward/TESOutput",std::string("Hlt1/Track/GuidedForward"));

  m_recoConf.add("FitTrack/Tool",std::string("HltTrackFit"));
  m_recoConf.add("FitTrack/Owner",true);
  m_recoConf.add("FitTrack/View",false);
  m_recoConf.add("FitTrack/TransferIDs",true);
  m_recoConf.add("FitTrack/TransferAncestor",true);
  m_recoConf.add("FitTrack/ITrackType", (int) LHCb::Track::Long);
  m_recoConf.add("FitTrack/OTrackType", (int) LHCb::Track::Long);
  m_recoConf.add("FitTrack/TESOutput",std::string("Hlt1/Track/FitTrack"));

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
    std::vector<std::string> cromos;
    boost::algorithm::split(cromos,key,boost::algorithm::is_any_of("/"));
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
  m_ItrackType = m_recoConf.retrieve<int>(m_recoName+"/ITrackType");
  m_OtrackType = m_recoConf.retrieve<int>(m_recoName+"/OTrackType");
  m_transferIDs = m_recoConf.retrieve<bool>(m_recoName+"/TransferIDs");
  m_transferAncestor = m_recoConf.retrieve<bool>(m_recoName+"/TransferAncestor");
 
  // by default, m_orderByPt is already false... so this only overrules
  // the case where someone overruled it to be true...
  // if (m_recoID <= hltInfoID( HltEnums::VeloKey) m_orderByPt = false;

  info() << " Reco: " << m_recoName 
         << " Tool: " << toolName
         << " Output: " << m_TESOutput << endreq;
  
  info() << " reco ID " << m_recoID
         << " owner " << m_owner << " transfers IDs " << m_transferIDs 
         << " transfer ancestor " << m_transferAncestor
         << " Input track type " << m_ItrackType 
         << " Output track type " << m_OtrackType << endreq;
  
  
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
  std::vector<std::string> cromos;
  boost::algorithm::split(cromos,toolname,boost::algorithm::is_any_of("/"));
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

StatusCode HltTrackUpgradeTool::upgrade
( const std::vector<Track*>& itracks ,
  std::vector<Track*>&       otracks ) 
{
  StatusCode sc = StatusCode::SUCCESS;
  beginExecute();
  
  std::vector<LHCb::Track*> tracks;
  for (std::vector<Track*>::const_iterator it = itracks.begin();
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

  if (isReco(seed)) { //Track has been involved in this reco?
    if (isOutput(seed)) { //has any ancestor involved in this reco?
      tracks.push_back(&seed);
      verbose() << " seed is its upgraded track of type " << m_OtrackType << endreq;
    } 
    else { //track must be a mother already upgraded for equal input and output types, find its descendants
      find(seed,tracks);
      verbose()<< " seed was already upgraded, found tracks " << tracks.size() << endreq;
    } 
  } 
  else if (seed.checkType( (LHCb::Track::Types) m_ItrackType)) { //(type == input) {
    m_timer->start(m_timerTool);
    sc = m_tool->tracksFromTrack(seed,tracks);
    m_timer->stop(m_timerTool);
    if (sc.isFailure()) {
      // TODO Warning(" reconstruction failure ",0);
      return sc;
    }
    recoDone(seed,tracks);
    verbose() << " seed upgraded, reco tracks " << tracks.size() << endreq;
  }
  else if (seed.checkType( (LHCb::Track::Types) m_OtrackType)) { //(type == output but from other tool {
    tracks.push_back(&seed);
    verbose() << " seed is of tool's output type " << m_OtrackType << "but from another tool. Accepted!" <<endreq;
  }
  
  return sc;
}

bool HltTrackUpgradeTool::isReco(const Track& track) {
  if (!m_doTrackReco) return false;
  int ncan = (int) track.info(m_recoID,-1);
  bool ok = (ncan != -1);
  verbose() << " has Reco ?" << ok << " info: " << ncan << endreq;
  return ok;
}

bool HltTrackUpgradeTool::isOutput(const Track& track) {
  if (!m_doTrackReco) return false;

  bool isOutType = track.checkType( (LHCb::Track::Types) m_OtrackType);
  if (!isOutType) {
    verbose() << " is Output type?" << isOutType << endreq;
    return false;
  }
  
  bool flaggedAncestor = false;  
  for (SmartRefVector<LHCb::Track>::const_iterator 
         it = track.ancestors().begin(); it != track.ancestors().end(); ++it) {
    if (((const LHCb::Track*)*it)->info(m_recoID,-1)!=-1){
      flaggedAncestor = true;
      verbose() << " found flagged ancestor!" << endreq;
      return true;
    }
  }
  verbose() << " no flagged ancestor!" << flaggedAncestor << endreq;

  return false;
}

void HltTrackUpgradeTool::recoDone(Track& seed, std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  // const GaudiUtils::VectorMap<int,double>& info = seed.extraInfo();
  for (std::vector<Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    Track& track = *(*it);
    track.setType( (LHCb::Track::Types) m_OtrackType );
    if (m_transferAncestor) track.addToAncestors( (Track*) &seed);
    if (m_transferIDs) addIDs(seed,track);
    track.addInfo(m_recoID,key);
    if (m_transferExtraInfo) {
      Hlt::MergeInfo(seed,track);
        // GaudiUtils::VectorMap<int,double>::const_iterator info_it = info.begin();
        //      for (; info_it != info.end(); ++info_it) track.addInfo(info_it->first,info_it->second);
      // track.setExtraInfo(seed.extraInfo()); Now merge!
    }
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

size_t HltTrackUpgradeTool::find(const Track& seed, std::vector<Track*>& tracks) {
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
  for (std::vector<Track*>::const_iterator it = con.begin(); 
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

