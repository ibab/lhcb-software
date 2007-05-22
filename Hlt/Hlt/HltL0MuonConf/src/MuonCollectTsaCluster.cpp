// Include files// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"
//#include "GaudiAlg/Tuples.h" 
//#include "GaudiAlg/Tuple.h"
#include "GaudiKernel/ChronoEntity.h"

// from Event
#include "Event/L0DUBase.h"
#include "Event/State.h"
#include "Event/Track.h"

// from Tsa
#include "TsaKernel/ITsaCollector.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/OTCluster.h"

// local
#include "MuonCollectTsaCluster.h"
#include "HltBase/IPrepareMuonSeed.h"

#include "HltBase/ITrackConfirmTool.h" 

using namespace LHCb;
using namespace Gaudi;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonCollectTsaCluster );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonCollectTsaCluster::MuonCollectTsaCluster( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) {
  
  declareProperty("nSigma",m_nsigma=5);
  declareProperty("debugInfo", m_debugInfo = false);
}

//=============================================================================
// Destructor
//=============================================================================
MuonCollectTsaCluster::~MuonCollectTsaCluster(){} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonCollectTsaCluster::initialize(){
  
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  always() << "==> Initialize MuonCollectTsaCluster v1r2" << endmsg;
  
  m_prepareMuonSeed = tool<IPrepareMuonSeed>("PrepareMuonSeed");

  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================          
StatusCode MuonCollectTsaCluster::execute() {

  if( m_debugInfo) debug() << "==> Execute" << endmsg;


  LHCb::Tracks* foundTracks = new LHCb::Tracks();
  foundTracks->reserve(50);
  std::vector<LHCb::Track*>* foundTracksTmp = new std::vector<LHCb::Track*>;
  foundTracksTmp->reserve(50);

  // Get L0Muons:
  m_inputL0Muons = get<L0MuonCandidates>(L0MuonCandidateLocation::Default);  
  
  if( m_debugInfo ) info() << " Nr of L0 Muon candidates: " << m_inputL0Muons->size() << endmsg;

  if( 0 == m_inputL0Muons->size() ){
    put( foundTracks , "Hlt/Track/L0TsaConfTool" );     
    delete foundTracksTmp;
    return StatusCode::SUCCESS;
  }

  /**
   *                Start loop over Muons          
   */
  int muonCounter=0;  
  L0MuonCandidates::const_iterator itL0Mu;
  for (itL0Mu = m_inputL0Muons->begin();
       itL0Mu!=m_inputL0Muons->end();
       itL0Mu++){

    muonCounter++;

    if(m_debugInfo) debug()<<"Processing muon number "<<muonCounter<<endmsg;
    
    LHCb::State myState;
    StatusCode sctmp = m_prepareMuonSeed->prepareSeed( *(*itL0Mu) , myState );

    if( sctmp.isFailure() ){
      err()<<"Fialed to prepare the seed"<<endmsg;
    }
    
    //play with seeding tool
    m_TsaConfirmTool->tracks( myState , *foundTracksTmp );

    if(m_debugInfo) debug()<<"Tool found : "<<foundTracksTmp->size()
                           <<" tracks for the seed number "<<muonCounter<<" !"<<endmsg;
     
    
  }//end loop muons
  
  std::vector<LHCb::Track*>::iterator iterTrack;
  for( iterTrack = foundTracksTmp->begin(); 
       iterTrack != foundTracksTmp->end() ; 
       iterTrack++ ) {
    
    LHCb::Track* fitTrack =  (*iterTrack)->clone();
    foundTracks->insert( fitTrack );
  }

  if(m_debugInfo) info()<<"Tool found number of tracks: "<<foundTracks->size() <<endmsg;

  if( 50 < foundTracks->size() ) always()<<"found more tracks than thought: "
                                         <<foundTracks->size()
                                         <<", change reserve !"<<endmsg;
  
  
  // put the containers on the store
  put( foundTracks , "Hlt/Track/L0TsaConfTool" );     

  delete foundTracksTmp;
  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonCollectTsaCluster::finalize(){

  debug() << "==> Finalize" << endmsg;
  
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

