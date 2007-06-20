// $Id: L0MuonConfWithT.cpp,v 1.2 2007-06-20 16:11:55 hernando Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "L0MuonConfWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonConfWithT
//
// 2007-02-14 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonConfWithT );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonConfWithT::L0MuonConfWithT( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
    declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName = "Hlt/Tracks/ConfirmedL0Muon");

  declareProperty("nSigma",m_nsigma=5);
  declareProperty("debugInfo", m_debugInfo = false);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonConfWithT::~L0MuonConfWithT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonConfWithT::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_prepareMuonSeed = tool<IPrepareMuonSeed>("PrepareMuonSeed");

  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool" );
  m_outputMuonTracks =
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
 
  
 

  m_myState = new State();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonConfWithT::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(true);
  m_inputL0Muons = get<L0MuonCandidates>(L0MuonCandidateLocation::Default);  
  

  if( m_debugInfo ) info() << " Nr of L0 Muon candidates: " 
                           << m_inputL0Muons->size() << endmsg; 
  if( 0 == m_inputL0Muons->size() ){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
//info()<<" sono qui "<<endreq;
   std::vector<LHCb::Track*>* foundTracksTmp = new std::vector<LHCb::Track*>;
  foundTracksTmp->reserve(50);
//info()<<" e ora qui"<<endreq;
  

  int muonCounter=0;  
  L0MuonCandidates::const_iterator itL0Mu;
 std::vector<LHCb::Track*>::iterator iterTrack;
  for (itL0Mu = m_inputL0Muons->begin();
       itL0Mu!=m_inputL0Muons->end();
       itL0Mu++){
    std::vector<MuonTileID> list_of_tile= (*itL0Mu)->muonTileIDs(1);
    MuonTileID tileM2=*(list_of_tile.begin());
    std::vector<MuonTileID> list_of_tileM3= (*itL0Mu)->muonTileIDs(2);
    MuonTileID tileM3=*(list_of_tileM3.begin());


    muonCounter++;

    if(m_debugInfo) info()<<"Processing muon number "<<muonCounter<<endmsg;
    
    LHCb::State myState;
    StatusCode sctmp = m_prepareMuonSeed->prepareSeed( *(*itL0Mu) , myState );
    if( sctmp.isFailure() ){
      err()<<"Failed to prepare the seed"<<endmsg;
    }
  //  info()<<" dopo prepare "<<endreq;
    //play with seeding tool
    m_TsaConfirmTool->tracks( myState , *foundTracksTmp );
   // info()<<" qopo tsa "<<endreq;
    for( iterTrack = foundTracksTmp->begin();
       iterTrack != foundTracksTmp->end() ;
       iterTrack++ ) {
       //info()<<" track "<<endreq;
      LHCb::Track* fitTrack =  (*iterTrack)->clone();
      fitTrack->addToLhcbIDs(tileM2);
      fitTrack->addToLhcbIDs(tileM3);
 
      Track* outTr = m_outputMuonTracks->newEntry();
     // info()<<" muon "<<endreq;   
      outTr->copy(*fitTrack);
      outTr->setFlag(Track::L0Candidate,true);

      m_outputTracks->push_back(outTr);
     // info()<<" hlt container"<<endreq;
      setFilterPassed(true);
      delete  (*iterTrack);
   
    }

    if(m_debugInfo) 
info()<<"Tool found : "<<foundTracksTmp->size()
                           <<" tracks for the seed number "<<muonCounter<<" !"<<endmsg;
     foundTracksTmp->clear();

    
  }//end loop muons


  delete foundTracksTmp;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonConfWithT::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
