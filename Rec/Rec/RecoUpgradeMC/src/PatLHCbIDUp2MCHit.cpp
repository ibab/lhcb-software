// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCHit.h"

// includes from DigiEvent
#include "Event/VeloCluster.h"
#include "Event/VeloPixCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// local
#include "PatLHCbIDUp2MCHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatLHCbIDUp2MCHit
//
// 2005-06-01 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatLHCbIDUp2MCHit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatLHCbIDUp2MCHit::PatLHCbIDUp2MCHit( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_level1(0)
{
  m_targetName = "Pat/LHCbIDMCHit";
  declareProperty( "TargetName", m_targetName );
  declareProperty( "LinkVELOPIX", m_linkVELOPIX=false);
}
//=============================================================================
// Destructor
//=============================================================================
PatLHCbIDUp2MCHit::~PatLHCbIDUp2MCHit() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatLHCbIDUp2MCHit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_linkTool = tool<ILHCbIDsToMCHits>("LHCbIDsToMCHits","ID2MCHit",this);
  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatLHCbIDUp2MCHit::execute() {

  debug() << "==> Execute" << endmsg;

  LinkerWithKey<LHCb::MCHit>
    lhcbLink( evtSvc(), msgSvc(), m_targetName );

  LHCb::MCHit* hit;


  // link veloPix, if requested
  if (m_linkVELOPIX) {
    // Link is made from VeloPixCluster relation to MCP but is linked to VeloPixLiteCluster ID 
    
    always()<<"# clusters: "<<LHCb::VeloPixClusterLocation::VeloPixClusterLocation<<endmsg;
    LinkedTo<LHCb::MCHit,LHCb::VeloPixCluster>
      veloPixLink( evtSvc(), msgSvc(), LHCb::VeloPixClusterLocation::VeloPixClusterLocation + "2MCHits" );
    LHCb::VeloPixClusters* clusters = get<LHCb::VeloPixClusters>(LHCb::VeloPixClusterLocation::VeloPixClusterLocation );
    always()<<"# clusters: "<<clusters->size()<<endmsg;
    if (clusters->size() == 0){
       error() << "Unable to retrieve VeloPixClusters, check the container name"
            << endmsg;
       return StatusCode::FAILURE;
    }
    if (clusters){
      LHCb::VeloPixClusters::const_iterator iClus;
      
      for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {
        m_hitList.clear();
        //int id   = LHCb::LHCbID( (*iClus)->channelID()).velopixID();
        //hit = veloPixLink.first((*iClus)->channelID() );
        bool noAssHit = false;
        ILHCbIDsToMCHits::LinkMap testMap;
        StatusCode sc = linkTool()->link(LHCb::LHCbID( (*iClus)->channelID()),testMap);
        for ( ILHCbIDsToMCHits::LinkMap::iterator it = testMap.begin(); testMap.end() != it ; ++it ){
          hit = it->first;
          if (hit == NULL ){
            noAssHit = true;
            always()<<"No associated MCHit"<<endmsg;
            continue;
          }
          addToList( hit );
        }
        if(noAssHit)continue;
        for ( std::vector<const LHCb::MCHit*>::const_iterator itP = 
                m_hitList.begin(); m_hitList.end() != itP; ++itP ) {
          LHCb::LHCbID temp = LHCb::LHCbID((*iClus)->lCluster().channelID()).velopixID(); 
          lhcbLink.link( temp.lhcbID(), *itP ); // same without cluster size
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatLHCbIDUp2MCHit::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
