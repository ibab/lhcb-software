// $Id: HltL0MuonPrepare.cpp,v 1.2 2008-05-03 15:19:53 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltL0MuonPrepare.h"
#include "Event/HltEnums.h"
//#include "Event/HltSummaryFunctor.h"

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
{
  // declareCondition("MinPt",m_ptMin = 0.);
  
  declareProperty("MinPt", m_PtMin = 0.1);

  m_maker = 0;

  m_doInitSelections = false;
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

  m_outputTracks = &(registerTSelection<LHCb::Track>(m_outputSelectionName));
  
  m_maker = tool<IMuonSeedTool>("MuonSeedTool");
  if (!m_maker)
    error() << " not able to create maker muon tracks tool " << endreq;

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
  //  put(muons,"Hlt/Track/L0Muon");
  put(muons, "Hlt/Track/"+m_outputSelectionName);

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

//=============================================================================
bool HltL0MuonPrepare::checkClone(L0MuonCandidate* muon)
{
  std::vector<MuonTileID> list_of_tileM1= muon->muonTileIDs(0);
  MuonTileID tileM1=*(list_of_tileM1.begin());

  std::vector<MuonTileID> list_of_tileM2= muon->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tileM2.begin());
  for( std::vector<Track*>::const_iterator pItr=m_outputTracks->begin();
            pItr<m_outputTracks->end();pItr++){
    std::vector< LHCb::LHCbID > list_lhcb=      (*pItr)->lhcbIDs();
    MuonTileID oldTileM1;
    MuonTileID oldTileM2;

    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==0){
          oldTileM1=iM1->muonID();
          break;
        }
      }
    }
    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==1){
          oldTileM2=iM1->muonID();
          break;
        }
      }
    }
    if(oldTileM1.isValid()&&oldTileM1==tileM1&&oldTileM2.isValid()&&oldTileM2==tileM2)return true;



  }
  return false;

}
