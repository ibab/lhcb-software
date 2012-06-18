// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"

// includes from DigiEvent
#include "Event/VeloCluster.h"
#include "Event/VeloPixCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
#include "Event/FTCluster.h"
#include "Event/FTRawCluster.h"

// local
#include "PrLHCbID2MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrLHCbID2MCParticle
//
// 2010-03-22 Victor Coco
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PrLHCbID2MCParticle )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PrLHCbID2MCParticle::PrLHCbID2MCParticle( const std::string& name,
                                              ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator ),
      m_othitcreator("Tf::OTHitCreator")
{
  declareProperty( "TargetName",  m_targetName = "Pr/LHCbID" );
}
//=============================================================================
// Destructor
//=============================================================================
PrLHCbID2MCParticle::~PrLHCbID2MCParticle() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrLHCbID2MCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrLHCbID2MCParticle::execute() {

  debug() << "==> Execute" << endmsg;
  m_debug = msgLevel( MSG::DEBUG );

  LinkerWithKey<LHCb::MCParticle> lhcbLink( evtSvc(), msgSvc(), m_targetName );
  m_linker = &lhcbLink;

  //== Velo

  if ( exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default) ) {    
    LinkedTo<LHCb::MCParticle> veloLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
    m_detectorLink = &veloLink;    
    LHCb::VeloClusters* clusters = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    LHCb::VeloClusters::const_iterator iClus;
    for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {
      LHCb::LHCbID myId = LHCb::LHCbID( (*iClus)->channelID() );
      int id            = myId.veloID();
      int size          = (*iClus)->pseudoSize();
      linkAll( myId, id, size );
    }
  }

  //== TT

  if ( exist<LHCb::STCluster::Container>(LHCb::STClusterLocation::TTClusters) ) {
    LinkedTo<LHCb::MCParticle> ttLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters );
    m_detectorLink = &ttLink;
    const LHCb::STCluster::Container* cont = get<LHCb::STCluster::Container>(LHCb::STClusterLocation::TTClusters);
    for(  LHCb::STCluster::Container::const_iterator iclus = cont->begin();
          iclus != cont->end(); ++iclus) {
      LHCb::LHCbID myId = LHCb::LHCbID((*iclus)->channelID());
      int size          = (*iclus)->size();
      int id            = myId.stID();
      linkAll( myId, id, size );
    }
  }

  //== IT coordinates

  if ( exist<LHCb::STCluster::Container>(LHCb::STClusterLocation::ITClusters) ) {
    LinkedTo<LHCb::MCParticle> itLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters );
    m_detectorLink = &itLink;
    const LHCb::STCluster::Container* cont = get<LHCb::STCluster::Container>(LHCb::STClusterLocation::ITClusters);
    for(  LHCb::STCluster::Container::const_iterator iclus = cont->begin();
          iclus != cont->end(); ++iclus) {
      LHCb::LHCbID myId = LHCb::LHCbID((*iclus)->channelID());
      int size          = (*iclus)->size();
      int id            = myId.stID();
      linkAll( myId, id, size );
    }
  }

  //== OT coordinates  *** Problem if does not exist: error message!
  /*
  LinkedTo<LHCb::MCParticle> otLink( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
  if ( !otLink.notFound() ) {
    m_detectorLink = &otLink;
    Tf::OTHitRange othits = m_othitcreator->hits();
    for (Tf::OTHitRange::const_iterator otSTH = othits.begin();
         otSTH < othits.end();otSTH++){
      LHCb::LHCbID myId = (*otSTH)->lhcbID();
      int id            = myId.otID();
      linkAll( myId, id );
    }
  }
  */
  //== Velo Pixel

  if ( exist<LHCb::VeloPixClusters>(LHCb::VeloPixClusterLocation::VeloPixClusterLocation ) ) {
    LinkedTo<LHCb::MCParticle> veloPixLink( evtSvc(), msgSvc(), LHCb::VeloPixClusterLocation::VeloPixClusterLocation );
    m_detectorLink = &veloPixLink;
    LHCb::VeloPixClusters* clusters = get<LHCb::VeloPixClusters>(LHCb::VeloPixClusterLocation::VeloPixClusterLocation );
    LHCb::VeloPixClusters::const_iterator iClus;
    for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {
      LHCb::LHCbID myId = (*iClus)->lCluster().channelID();
      int id            = myId.velopixID();
      linkAll( myId, id );
    }
  }

  //== FT
  LinkedTo<LHCb::MCParticle> ftLink( evtSvc(), msgSvc(),LHCb::FTClusterLocation::Default );
  if ( !ftLink.notFound() ) {
    m_detectorLink = &ftLink;
    typedef FastClusterContainer<LHCb::FTRawCluster,int> FTRawClusters;
    FTRawClusters* clus = get<FTRawClusters>( LHCb::FTRawClusterLocation::Default );
    FTRawClusters::const_iterator iClus;
    for(iClus = clus->begin(); iClus != clus->end(); ++iClus) {
      LHCb::LHCbID myId = LHCb::LHCbID( (*iClus).channelID() );
      int id            = myId.ftID();
      linkAll( myId, id );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  link all particles to the specified id
//=========================================================================
void PrLHCbID2MCParticle::linkAll( LHCb::LHCbID myId, int firstId, int size ) {
  m_partList.clear();
  while( size > 0 ) {
    const LHCb::MCParticle* part = m_detectorLink->first( firstId );
    while ( 0 != part ) {       
      if ( std::find( m_partList.begin(), m_partList.end(), part ) == m_partList.end() ) {
        m_partList.push_back( part );
      }
      part = m_detectorLink->next();
    }
    ++firstId;
    --size;
  }
  if ( m_debug ) info() << format( "For ID %8x  MC: ", myId.lhcbID() );
  for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = m_partList.begin(); 
        m_partList.end() != itP; ++itP ) {
    if ( m_debug ) info() << (*itP)->key() << " ";
    m_linker->link( myId.lhcbID(), *itP );
  }
  if ( m_debug ) info() << endmsg;
}
//=============================================================================
