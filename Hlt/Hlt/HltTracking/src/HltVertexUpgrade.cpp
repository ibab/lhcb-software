// $Id: HltVertexUpgrade.cpp,v 1.1 2007-06-25 20:56:48 hernando Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltVertexUpgrade.h"
#include "HltBase/HltConfigurationHelper.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexUpgrade );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexUpgrade::HltVertexUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("RecoName", m_recoName = "empty");

  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);

  declareProperty( "PatOutputVerticesName",  
                   m_patOutputVerticesName = "Hlt/Vertex/VertexUpgradeBank");
  
};

void HltVertexUpgrade::recoConfiguration() {

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
HltVertexUpgrade::~HltVertexUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltVertexUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  recoConfiguration();

  sc = initReco();
  if ( sc.isFailure() ) return sc;

  checkInput(m_inputVertices," input vertices ");
  checkInput(m_outputVertices," output vertices ");

  // Prepare output vertex container
  m_patOutputVertices = 
    m_patDataStore->createVertexContainer(m_patOutputVerticesName,200);

  saveConfiguration();

  return sc;

};

StatusCode HltVertexUpgrade::initReco() 
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

StatusCode HltVertexUpgrade::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  for (Hlt::VertexContainer::iterator it = m_inputVertices->begin();
       it != m_inputVertices->end(); ++it) {
    RecVertex& vseed = *(*it);
    const SmartRefVector<Track>& otracks = vseed.tracks();
    Track& seed1 = (Track&) *(*otracks.begin());
    Track& seed2 = (Track&) *(*(otracks.begin()+1));
    m_tracks1.clear();
    m_tracks2.clear();
    // TODO what happens if the track is already update!!
    size_t n1 = upgrade(seed1,m_tracks1);
    size_t n2 = upgrade(seed2,m_tracks2);
    if (n1<=0 || n2 <=0) continue;
    for (std::vector<Track*>::iterator t1 = m_tracks1.begin();
         t1 != m_tracks1.end(); ++t1) {
      Track& track1 = *(*t1);
      for (std::vector<Track*>::iterator t2 = m_tracks2.begin();
           t2 != m_tracks2.end(); ++t2) {
        Track& track2 = *(*t2);
        LHCb::RecVertex* sv = m_patOutputVertices->newEntry();
        _makeVertex(track1,track2,*sv);
        m_outputVertices->push_back(sv);
        // TODO - wait till RecVertex is done
        // if (m_transferExtraInfo)
        //   sv->setExtraInfo( vseed.extraInfo() );
      }
    }
  }

  return sc;
}


size_t HltVertexUpgrade::upgrade(Track& seed, 
                                 std::vector<LHCb::Track*>& tracks)
{
  StatusCode sc = StatusCode::SUCCESS;
  tracks.clear();
  if (!isReco(seed)) {
    sc = m_tool->upgrade(seed,tracks);
    if (sc.isFailure()) warning() << " reconstruction failure " 
                                  << m_recoName << endreq;
    recoDone(seed,tracks);
  } else find(seed,tracks);
  if (m_debug) {
    printInfo(" seed    ", seed);
    printInfo(" upgrade ", tracks);
  }
  return tracks.size();
}

bool HltVertexUpgrade::isReco(const Track& seed) {
  double key = seed.info(m_recoID,-1.);
  bool ok = (key != -1.);
  debug() << " has Reco " << ok << " key " << key << endreq;
  return ok;
}


void HltVertexUpgrade::recoDone(Track& seed, 
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


size_t HltVertexUpgrade::find(const Track& seed,
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
StatusCode HltVertexUpgrade::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

void HltVertexUpgrade::saveConfiguration() {

  HltAlgorithm::saveConfiguration();

  std::string type = "HltVertexUpgrade";
  confregister("Type",type);
  confregister("RecoName",m_recoName);
}
