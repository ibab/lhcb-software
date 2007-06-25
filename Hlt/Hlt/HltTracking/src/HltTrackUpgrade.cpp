// $Id: HltTrackUpgrade.cpp,v 1.1 2007-06-25 20:56:47 hernando Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltTrackUpgrade.h"
#include "HltBase/HltConfigurationHelper.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackUpgrade );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackUpgrade::HltTrackUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("RecoName", m_recoName = "empty");

  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  declareProperty("OrderByPt", m_orderByPt = false);
  
};

void HltTrackUpgrade::recoConfiguration() {

  std::string info = "ExtraInfoID";

  m_recoConf.add("Velo/Tool",std::string("VeloTrackUpgrade"));
  m_recoConf.add("Velo/RecoID", 
                 HltConfigurationHelper::getID(*m_conf,"VeloKey",info));
  m_recoConf.add("Velo/PatInput",LHCb::TrackLocation::HltRZVelo);
  m_recoConf.add("Velo/PatOutput",LHCb::TrackLocation::HltVelo);
  
  m_recoConf.add("VeloTT/Tool",std::string("VeloTTTrackUpgrade"));
  m_recoConf.add("VeloTT/RecoID", 
                 HltConfigurationHelper::getID(*m_conf,"VeloTTKey",info));
  m_recoConf.add("VeloTT/PatInput",LHCb::TrackLocation::HltVelo);
  m_recoConf.add("VeloTT/PatOutput",LHCb::TrackLocation::HltVeloTT);

  m_recoConf.add("Forward/Tool",std::string("ForwardTrackUpgrade"));
  m_recoConf.add("VeloTT/RecoID", 
                 HltConfigurationHelper::getID(*m_conf,"ForwardKey",info));
  m_recoConf.add("VeloTT/PatInput",LHCb::TrackLocation::HltVelo);
  m_recoConf.add("VeloTT/PatOutput",LHCb::TrackLocation::HltForward);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgrade::~HltTrackUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  recoConfiguration();

  sc = initReco();
  if ( sc.isFailure() ) return sc;

  checkInput(m_inputTracks," input tracks");
  checkInput(m_outputTracks," output tracks");

  saveConfiguration();

  return sc;

};

StatusCode HltTrackUpgrade::initReco() 
{ 

  std::string key = m_recoName;
  if (!m_recoConf.has_key(key+"/Tool")) {
    fatal() << " unknown reconstruction " << m_recoName << endreq;
    return StatusCode::FAILURE;
  }

  std::string toolName = m_recoConf.retrieve<std::string>(key+"/Tool");
  m_recoID   = m_recoConf.retrieve<int>(key+"/RecoID");
  std::string patInput  = m_recoConf.retrieve<std::string>(key+"/PatInput");  
  std::string patOutput = m_recoConf.retrieve<std::string>(key+"/PatOutput");

  int veloTTID = m_recoConf.retrieve<int>("VeloTT/RecoID");
  if (m_recoID< veloTTID) m_orderByPt = false;
  
  m_tool = NULL;
  m_tool = tool<ITrackUpgrade>(toolName);
  if (!m_tool) fatal() << " not able to get tool " << toolName << endreq;

  m_patInputTracks = NULL; m_patOutputTracks = NULL;
  m_patInputTracks  = m_patDataStore->tracks(patInput);
  m_patOutputTracks = m_patDataStore->tracks(patOutput);

  infoContainer(m_patInputTracks,  " pat  input tracks", patInput);
  infoContainer(m_patOutputTracks, " pat output tracks", patOutput);

  initializeHisto(m_histoPatInputTracks,"PatInputTracks");
  initializeHisto(m_histoPatOutputTracks,"PatOutputTracks");

  debug() << " reconstruction " << m_recoName << " = " << m_recoID << endreq;
  debug() << " Tool " << toolName << " PatInput " << patInput 
          << " PatOutput " << patOutput << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackUpgrade::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  for (Hlt::TrackContainer::iterator it = m_inputTracks->begin();
       it != m_inputTracks->end(); ++it) {
    Track& seed = *(*it);
    m_tracks.clear();
    if (!isReco(seed)) {
      sc = m_tool->upgrade(seed,m_tracks);
      if (sc.isFailure()) warning() << " reconstruction failure " 
                                    << m_recoName << endreq;
      recoDone(seed,m_tracks);
    } else find(seed,m_tracks);
    if (m_debug) {
      printInfo(" seed    ", seed);
      printInfo(" upgrade ", m_tracks);
    }
    Hlt::copy(m_tracks,*m_outputTracks);
  }
  if (m_orderByPt)
    std::sort(m_outputTracks->begin(),m_outputTracks->end(),_sortByPt);

  return sc;
}

bool HltTrackUpgrade::isReco(const Track& seed) {
  double key = seed.info(m_recoID,-1.);
  bool ok = (key != -1.);
  debug() << " has Reco " << ok << " key " << key << endreq;
  return ok;
}


void HltTrackUpgrade::recoDone(Track& seed, 
                               std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  for (std::vector<Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    Track& track = *(*it);
    if (m_transferExtraInfo) track.setExtraInfo(seed.extraInfo());
    track.addInfo(m_recoID,key);
  }
  seed.addInfo(m_recoID, (double) tracks.size());
}


size_t HltTrackUpgrade::find(const Track& seed,
                             std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  size_t ncan = (size_t) seed.info(m_recoID,0);
  for (PatTrackContainer::iterator it = m_patOutputTracks->begin();
       it != m_patOutputTracks->end(); ++it) {
    Track* ptrack = *it;
    if (ptrack->info(m_recoID,-1) == key) tracks.push_back(ptrack);
  }
  if (ncan != tracks.size())
    error() << " different number of stored tracks! " << endreq;
  return tracks.size();
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackUpgrade::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

void HltTrackUpgrade::saveConfiguration() {

  HltAlgorithm::saveConfiguration();

  std::string type = "HltTrackUpgrade";
  confregister("Type",type);
  confregister("RecoName",m_recoName);
}
