// $Id: HltTrackUpgradeTool.cpp,v 1.9 2008-01-22 10:04:25 hernando Exp $
// Include files
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackUpgradeTool.h"
#include "Event/HltEnums.h"
#include "HltBase/HltConfigurationHelper.h"
#include "HltBase/ESequences.h"

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
  : GaudiTool ( type, name , parent )
{
  declareInterface<HltTrackUpgradeTool>(this);
  declareInterface<ITrackUpgrade>(this);

  declareProperty("RecoName", m_recoName = "empty");
  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  declareProperty("OrderByPt", m_orderByPt = false);

  m_tool = NULL;
};

void HltTrackUpgradeTool::recoConfiguration() {

  std::string info = "InfoID";

  m_recoConf.add("TConf/Tool",std::string("L0ConfirmWithT"));
  m_recoConf.add("TConf/RecoID", (int) HltEnums::MuonTKey); //TODO
  m_recoConf.add("TConf/Owner",true);
  m_recoConf.add("TConf/TransferIDs",true);
  m_recoConf.add("TConf/TransferAncestor",true);
  m_recoConf.add("TConf/TrackType", (int) LHCb::Track::Ttrack);
  m_recoConf.add("TConf/TESInput",std::string("none"));
  m_recoConf.add("TConf/TESOutput",LHCb::TrackLocation::HltTsa);

  m_recoConf.add("Velo/Tool",std::string("Tf::PatVeloSpaceTool"));
  m_recoConf.add("Velo/RecoID", (int) HltEnums::VeloKey);
  m_recoConf.add("Velo/Owner",true);
  m_recoConf.add("Velo/TransferIDs",false);
  m_recoConf.add("Velo/TransferAncestor",false);
  m_recoConf.add("Velo/TrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("Velo/TESInput",LHCb::TrackLocation::HltRZVelo);
  m_recoConf.add("Velo/TESOutput",LHCb::TrackLocation::HltVelo);
  
  m_recoConf.add("VeloTT/Tool",std::string("Tf::PatVeloTTTool"));
  m_recoConf.add("VeloTT/RecoID", (int) HltEnums::VeloTTKey);
  m_recoConf.add("VeloTT/Owner",false);
  m_recoConf.add("VeloTT/TransferIDs",false);
  m_recoConf.add("VeloTT/TransferAncestor",false);
  m_recoConf.add("VeloTT/TrackType", (int) LHCb::Track::Upstream);
  m_recoConf.add("VeloTT/TESInput",LHCb::TrackLocation::HltVelo);
  m_recoConf.add("VeloTT/TESOutput", LHCb::TrackLocation::HltVeloTT);

  m_recoConf.add("Forward/Tool",std::string("PatForwardTool"));
  m_recoConf.add("Forward/RecoID", (int) HltEnums::ForwardKey);
  m_recoConf.add("Forward/Owner",true);
  m_recoConf.add("Forward/TransferIDs",false);
  m_recoConf.add("Forward/TransferAncestor",false);
  m_recoConf.add("Forward/TrackType", (int) LHCb::Track::Long);
  m_recoConf.add("Forward/TESInput",LHCb::TrackLocation::HltVelo);
  m_recoConf.add("Forward/TESOutput", LHCb::TrackLocation::HltForward);

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgradeTool::~HltTrackUpgradeTool() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgradeTool::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  recoConfiguration();

  m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );

  // sc = setReco(m_recoName);
  // if ( sc.isFailure() ) return sc;
  return sc;
};

StatusCode HltTrackUpgradeTool::setReco(const std::string& key) 
{
  m_recoName = key;
  debug() << " InitReco " << endreq;
  bool ok = m_recoConf.has_key(m_recoName+"/Tool");
  Assert(ok," no reconstruction with name "+m_recoName);
  
  std::string toolName = m_recoConf.retrieve<std::string>(m_recoName+"/Tool");
  m_recoID   = m_recoConf.retrieve<int>(m_recoName+"/RecoID");
  std::string TESInput  = m_recoConf.retrieve<std::string>(m_recoName+"/TESInput");
  m_TESOutput = m_recoConf.retrieve<std::string>(m_recoName+"/TESOutput");
  m_owner = m_recoConf.retrieve<bool>(m_recoName+"/Owner");
  m_trackType = m_recoConf.retrieve<int>(m_recoName+"/TrackType");
  m_transferIDs = m_recoConf.retrieve<bool>(m_recoName+"/TransferIDs");
  m_transferAncestor = 
    m_recoConf.retrieve<bool>(m_recoName+"/TransferAncestor");
  
  if (m_recoID <= HltEnums::VeloKey) m_orderByPt = false;

  info() << " Reco: " << m_recoName 
          << " Tool: " << toolName 
          << " Input: " << TESInput 
          << " Output: " << m_TESOutput << endreq;

  debug() << " reco ID " << m_recoID
          << " owner " << m_owner << " transfers IDs " << m_transferIDs 
          << " transfer ancestor " << m_transferAncestor
          << " track type " << m_trackType << endreq;
  
  if (m_tool) delete m_tool;
  m_tool = NULL;
  m_tool = tool<ITracksFromTrack>(toolName,this);
  Assert(m_tool," setReco() not able to create tool "+toolName);

  m_timer = tool<ISequencerTimerTool>("SequencerTimerTool");
  m_timer->increaseIndent();
  m_timerTool = m_timer->addTimer(toolName);
  m_timer->decreaseIndent();


  return StatusCode::SUCCESS;
}

void HltTrackUpgradeTool::beginExecute() {
  if (m_owner)
    m_otracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_TESOutput);
}


//=============================================================================
// Main execution
//=============================================================================

StatusCode HltTrackUpgradeTool::upgrade(std::vector<Track*>& itracks,
                                        std::vector<Track*>& otracks) {
  StatusCode sc = StatusCode::SUCCESS;
  beginExecute();
  
  for (std::vector<Track*>::iterator it = itracks.begin();
       it != itracks.end(); ++it) {
    Track& seed = *(*it);
    upgrade(seed,m_tracks);
    zen::copy(m_tracks,otracks);
  }
  if (m_orderByPt)
    std::sort(otracks.begin(),otracks.end(),_sortByPt);
  
  debug() << " upgraded " << otracks.size() << " tracks " << endreq;
  return sc;
}


StatusCode HltTrackUpgradeTool::upgrade(LHCb::Track& seed,
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
      Warning(" reconstruction failure ",1);
      return sc;
    }
    recoDone(seed,tracks);
    verbose() << " seed upgraded, reco tracks " << tracks.size() << endreq;
  } else {
    find(seed,tracks);
    verbose()<< " seed was upgraded, found tracks " << tracks.size() << endreq;
  }
  if (tracks.size()>0) {
    printInfo(" upgrade seed    ", seed);
    printInfo("         tracks  ", tracks);
  }
  return sc;
}

bool HltTrackUpgradeTool::isReco(const Track& seed) {
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
  if (m_owner) {
    for (std::vector<Track*>::iterator it = tracks.begin();
         it != tracks.end(); ++it)
      m_otracks->insert(*it); 
  }  
}


size_t HltTrackUpgradeTool::find(const Track& seed,
                             std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  double ncan = (double) seed.info(m_recoID,-1);
  debug() << " find seed key " << key << " n-descendants " << ncan << endreq;
  for (Tracks::iterator it = m_otracks->begin();it != m_otracks->end();++it) {
    Track* ptrack = *it;
    if (ptrack->info(m_recoID,-1) == key) tracks.push_back(ptrack);
  }
  if (ncan != (double) tracks.size()) {
    error() << " different number of stored tracks! " << ncan 
            << " != " << tracks.size() << endreq;
    for (Tracks::iterator it = m_otracks->begin();
         it != m_otracks->end();++it) {
      Track& track = *(*it);
      info() << " key " << key << " mother " 
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
