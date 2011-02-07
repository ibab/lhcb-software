// $Id: HltTrackUpgradeTool.cpp,v 1.46 2010-09-08 13:58:08 graven Exp $
// Include files
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackUpgradeTool.h"
#include "HltBase/HltUtils.h"
#include "HltBase/HltTrackMerge.h"

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
  declareInterface<ITrackUpgrade>(this);

  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  declareProperty("TransferAncestor",m_transferAncestor = true);
  declareProperty("TransferIDs",m_transferIDs = true);
  declareProperty("OrderByPt", m_orderByPt = false);
  declareProperty("DoTrackReco",m_doTrackReco = true);

  declareProperty( "TESOutput", m_TESOutput ); // force it to be set to nonzero value ... 
  declareProperty( "ITrackType", m_ItrackType = -1 ); // force it to be set ...
  declareProperty( "OTrackType", m_OtrackType = -1 ); // force it to be set ...
  declareProperty( "Tool" , m_toolName ); // force it to be set...
  declareProperty( "View" , m_hasView = false );
};


//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgradeTool::~HltTrackUpgradeTool() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgradeTool::initialize() {

  StatusCode sc = HltBaseTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_timer = tool< ISequencerTimerTool >( "SequencerTimerTool" );

  debug() << " InitReco " << endreq;

  m_recoID   = hltInfoID(m_toolName);

  info() << " Reco: " << name() 
         << " Tool: " << m_toolName
         << " Output: " << m_TESOutput 
         << " reco ID " << m_recoID << endreq;

  debug()<< " transfers IDs " << m_transferIDs 
         << " transfer ancestor " << m_transferAncestor
         << " Input track type " << m_ItrackType 
         << " Output track type " << m_OtrackType << endreq;

  Assert(!m_TESOutput.empty(), " No TES output location defined! " );
  Assert(!m_toolName.empty(), " No Tool name defined! " );
  
  m_tool = tool<ITracksFromTrack>( m_toolName );
  m_viewTool = m_hasView ? tool<ITrackView>( m_toolName ) : 0;
  
  Assert( m_tool != 0, " setReco() failed to create tool " + m_toolName );

  m_timer->increaseIndent();
  m_timerTool = m_timer->addTimer( m_toolName );
  m_timer->decreaseIndent();

  return sc;
};


//=============================================================================
// Main execution
//=============================================================================

void HltTrackUpgradeTool::beginExecute() {
  m_otracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_TESOutput);
}

StatusCode HltTrackUpgradeTool::upgrade
( const std::vector<const Track*>& itracks ,
  std::vector<Track*>&       otracks ) 
{
  beginExecute();
  
  std::vector<LHCb::Track*> tracks;
  for (std::vector<const Track*>::const_iterator it = itracks.begin();
       it != itracks.end(); ++it) {
    if ( iupgrade(**it,tracks).isFailure()) {
       Warning("Failed to upgrade track" ,StatusCode::SUCCESS, 10 ).ignore();
    }
    otracks.insert(otracks.end(),tracks.begin(),tracks.end());
  }
  if (m_orderByPt) std::sort(otracks.begin(),otracks.end(), Hlt::SortTrackByPt());
  
  debug() << " upgraded " << otracks.size() << " tracks " << endreq;
  return StatusCode::SUCCESS;
}

StatusCode HltTrackUpgradeTool::upgrade(const Track& itrack,
                                        std::vector<Track*>& otracks) {
  beginExecute();
  iupgrade(itrack,otracks);
  debug() << " upgraded " << otracks.size() << " tracks " << endreq;
  return StatusCode::SUCCESS;
}



StatusCode HltTrackUpgradeTool::iupgrade(const LHCb::Track& seed,
                                         std::vector<LHCb::Track*>& tracks) {
  StatusCode sc = StatusCode::SUCCESS;
  tracks.clear();

  if (isReco(seed)) { //Track has been involved in this reco?
    if (isOutput(seed)) { //has any ancestor involved in this reco?
      tracks.push_back(const_cast<LHCb::Track*>(&seed));
      verbose() << " seed is its upgraded track of type " << m_OtrackType << endreq;
    } else { //track must be a mother already upgraded for equal input and output types, find its descendants
      find(seed,tracks);
      verbose()<< " seed was already upgraded, found tracks " << tracks.size() << endreq;
    } 
  } else if (seed.checkType( (LHCb::Track::Types) m_ItrackType)) { //(type == input) 
    m_timer->start(m_timerTool);
    sc = m_tool->tracksFromTrack(seed,tracks);
    m_timer->stop(m_timerTool);
    if (sc.isFailure()) {
      // TODO Warning(" reconstruction failure ",0);
      return sc;
    }
    recoDone(seed,tracks);
    verbose() << " seed upgraded, reco tracks " << tracks.size() << endreq;
  } else if (seed.checkType( (LHCb::Track::Types) m_OtrackType)) { //(type == output but from other tool 
    tracks.push_back(const_cast<LHCb::Track*>(&seed));
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

void HltTrackUpgradeTool::recoDone(const Track& seed, std::vector<Track*>& tracks) {
  double key = (double) seed.key();
  // const GaudiUtils::VectorMap<int,double>& info = seed.extraInfo();
  for (std::vector<Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    Track& track = *(*it);
    track.setType( (LHCb::Track::Types) m_OtrackType );
    if (m_transferAncestor) track.addToAncestors( (Track*) &seed);
    if (m_transferIDs) track.addSortedToLhcbIDs( seed.lhcbIDs() ); // is the seed sorted??? 
    track.addInfo(m_recoID,key);
    if (m_transferExtraInfo) {
      Hlt::MergeInfo(seed,track);
        // GaudiUtils::VectorMap<int,double>::const_iterator info_it = info.begin();
        //      for (; info_it != info.end(); ++info_it) track.addInfo(info_it->first,info_it->second);
      // track.setExtraInfo(seed.extraInfo()); Now merge!
    }
  }
  const_cast<LHCb::Track&>(seed).addInfo(m_recoID, (double) tracks.size());
  debug() << " seed " << seed.key() << " n-descendants " 
          << seed.info(m_recoID,-1) << endreq;
  for (std::vector<Track*>::iterator it = tracks.begin(); it != tracks.end(); ++it)
      m_otracks->insert(*it); 
}


namespace {
struct cmp_ {
    cmp_(const LHCb::Track* track) : track_(track) {}
    bool operator()(const SmartRef<LHCb::Track>& ref) { return ref.target()==track_; }
    const LHCb::Track *track_;
};
}

size_t HltTrackUpgradeTool::find(const Track& seed, std::vector<Track*>& tracks) {
  /// @todo This comparison of doubles must be fixed
  if (!m_doTrackReco) return 0;
  int key = seed.key();
  double ncan = (double) seed.info(m_recoID,-1);
  debug() << " find seed key " << key << " n-descendants " << ncan << endreq;
  for (Tracks::iterator it = m_otracks->begin();it != m_otracks->end();++it) {
    const SmartRefVector<LHCb::Track>& ancestors = (*it)->ancestors();
    bool found = (std::find_if(ancestors.begin(),ancestors.end(),cmp_(&seed)) != ancestors.end());
    if (found) tracks.push_back(*it);    
  }
  if (ncan != (double) tracks.size()) {
    Error("Different number of tracks",StatusCode::SUCCESS,1);
    debug() << " different number of stored tracks! " << ncan 
            << " != " << tracks.size() << endreq;
    for (Tracks::iterator it = m_otracks->begin(); it != m_otracks->end();++it) {
      debug() << " key " << key << " mother " << (*it)->info(m_recoID,-1) << endreq;
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


