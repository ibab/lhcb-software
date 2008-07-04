// $Id: HltL0MuonPrepare.cpp,v 1.8 2008-07-04 08:07:41 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include <algorithm>

// local
#include "HltL0MuonPrepare.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0MuonPrepare
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0MuonPrepare );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0MuonPrepare::HltL0MuonPrepare( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_maker(0)
{
  declareProperty("MinPt", m_PtMin = 0.1);
}
//=============================================================================
// Destructor
//=============================================================================
HltL0MuonPrepare::~HltL0MuonPrepare() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0MuonPrepare::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_outputTracks = &(registerTSelection<LHCb::Track>());
  
  m_maker = tool<IMuonSeedTool>("MuonSeedTool");

  saveConfiguration();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0MuonPrepare::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  L0MuonCandidates* inputL0Muon = 
    get<L0MuonCandidates>(L0MuonCandidateLocation::Default);

  Tracks* muons = new Tracks();
  put(muons, "Hlt/Track/"+m_outputTracks->id().str());

  for (std::vector<L0MuonCandidate*>::iterator it= inputL0Muon->begin();
       it !=  inputL0Muon->end(); ++it){
    const L0MuonCandidate& l0muon = *(*it);
    if (fabs(l0muon.pt()) < m_PtMin) continue;
    debug() << "l0pt " << l0muon.pt()<< endmsg;
    bool isClone=checkClone(*it);
    if (isClone) debug() << "is clone " << endmsg;
    if (isClone) continue;
    Track* track = new Track();
    sc = m_maker->makeTrack(l0muon,*track);
    //TODO: pushes both into IHltDataSvc (m_outputTracks) and into TES (muons)
    //      this should be possible to do through the IHltDataSvc...
    muons->insert(track);
    m_outputTracks->push_back(track);
  }

  if (m_debug) {
    debug() << " n L0MuonCandidates " << inputL0Muon->size() << endreq;
    debug() << " n L0Muon Tracks " << m_outputTracks->size() << endreq;
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltL0MuonPrepare::finalize() {
  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  
}

namespace {
    class isInMuonStation : public std::unary_function<bool,LHCb::LHCbID> {
    public:
       isInMuonStation(unsigned int station) : m_station (station) {}
       bool operator()(const LHCb::LHCbID& id) { return id.isMuon() && id.muonID().station()==m_station; }
    private:
       unsigned int m_station;
    };
}

//=============================================================================
bool HltL0MuonPrepare::checkClone(L0MuonCandidate* muon)
{
  const MuonTileID& tileM1 = muon->muonTileIDs(0).front();
  const MuonTileID& tileM2 = muon->muonTileIDs(1).front();

  for( std::vector<Track*>::const_iterator t = m_outputTracks->begin();
                                           t!= m_outputTracks->end(); ++t) {
    const std::vector< LHCb::LHCbID >& ids= (*t)->lhcbIDs();

    std::vector<LHCb::LHCbID>::const_iterator oldTileM1 = std::find_if( ids.begin(), ids.end(), isInMuonStation(0));
    if (oldTileM1 == ids.end() || !(*oldTileM1 == tileM1) ) continue; // not found, or no match...

    std::vector<LHCb::LHCbID>::const_iterator oldTileM2 = std::find_if( ids.begin(), ids.end(), isInMuonStation(1));
    if (oldTileM2 != ids.end() &&   *oldTileM2 == tileM2 ) return true; // found, and match...
  }
  return false;
}
