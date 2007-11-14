// $Id: HltTrackUpgradeTool.cpp,v 1.2 2007-11-14 14:00:10 hernando Exp $
// Include files
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackUpgradeTool.h"
#include "HltBase/HltConfigurationHelper.h"
#include "Event/HltNames.h"
#include "HltBase/HltSequences.h"

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

  m_recoConf.add("Velo/Tool",std::string("PatVeloSpaceTool"));
  m_recoConf.add("Velo/RecoID", (int) HltEnums::VeloKey);
  m_recoConf.add("Velo/Owner",true);
  m_recoConf.add("Velo/TrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("Velo/TESInput",LHCb::TrackLocation::HltRZVelo);
  m_recoConf.add("Velo/TESOutput",LHCb::TrackLocation::HltVelo);
  
  m_recoConf.add("VeloTT/Tool",std::string("PatVeloTTTool"));
  m_recoConf.add("VeloTT/RecoID", (int) HltEnums::VeloTTKey);
  m_recoConf.add("VeloTT/Owner",false);
  m_recoConf.add("VeloTT/TrackType", (int) LHCb::Track::Upstream);
  m_recoConf.add("VeloTT/TESInput",LHCb::TrackLocation::HltVelo);
  m_recoConf.add("VeloTT/TESOutput", LHCb::TrackLocation::HltVeloTT);

  m_recoConf.add("Forward/Tool",std::string("PatForwardTool"));
  m_recoConf.add("Forward/RecoID", (int) HltEnums::ForwardKey);
  m_recoConf.add("Forward/Owner",true);
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

  // sc = setReco(m_recoName);
  // if ( sc.isFailure() ) return sc;
  return sc;
};

StatusCode HltTrackUpgradeTool::setReco(const std::string& key) 
{
  m_recoName = key;
  info() << " InitReco " << endreq;
  if (!m_recoConf.has_key(m_recoName+"/Tool")) {
    fatal() << " unknown reconstruction " << m_recoName << endreq;
    return StatusCode::FAILURE;
  }

  std::string toolName = m_recoConf.retrieve<std::string>(m_recoName+"/Tool");
  m_recoID   = m_recoConf.retrieve<int>(m_recoName+"/RecoID");
  std::string TESInput  = m_recoConf.retrieve<std::string>(m_recoName+"/TESInput");
  m_TESOutput = m_recoConf.retrieve<std::string>(m_recoName+"/TESOutput");
  m_owner = m_recoConf.retrieve<bool>(m_recoName+"/Owner");
  m_trackType = m_recoConf.retrieve<int>(m_recoName+"/TrackType");
  if (m_recoID <= HltEnums::VeloKey) m_orderByPt = false;

  info() << " Reco " << m_recoName << " ID " << m_recoID 
         << " Tool " << toolName << " Input " << TESInput 
         << " Output " << m_TESOutput << " owner " << m_owner 
         << " track type " << m_trackType << endreq;

  
  if (m_tool) delete m_tool;
  m_tool = NULL;
  m_tool = tool<ITracksFromTrack>(toolName);
  if (!m_tool) fatal() << " not able to get tool " << toolName << endreq;

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
    Hlt::copy(m_tracks,otracks);
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
    sc = m_tool->tracksFromTrack(seed,tracks);
    if (sc.isFailure()) warning() << " reconstruction failure " 
                                  << m_recoName << endreq;
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
  double key = seed.info(m_recoID,-1.);
  bool ok = (key != -1.);
  verbose() << " has Reco ?" << ok << " nups " << key << endreq;
  return ok;
}


void HltTrackUpgradeTool::recoDone(Track& seed, 
                                   std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  for (std::vector<Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    Track& track = *(*it);
    if (m_transferExtraInfo) track.setExtraInfo(seed.extraInfo());
    track.addInfo(m_recoID,key);
    track.setType( (LHCb::Track::Types) m_trackType );
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
  size_t ncan = (size_t) seed.info(m_recoID,0);
  info() << " find seed key " << key << " nups " << ncan << endreq;
  for (Tracks::iterator it = m_otracks->begin();it != m_otracks->end();++it) {
    Track* ptrack = *it;
    if (ptrack->info(m_recoID,-1) == key) tracks.push_back(ptrack);
  }
  if (ncan != tracks.size())
    error() << " different number of stored tracks! " << endreq;

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
