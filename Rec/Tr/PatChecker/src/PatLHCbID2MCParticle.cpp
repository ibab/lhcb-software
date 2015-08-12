// Include files 

#include "Event/MCParticle.h"

// includes from DigiEvent
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// local
#include "PatLHCbID2MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatLHCbID2MCParticle
//
// 2005-06-01 : Olivier Callot
//-----------------------------------------------------------------------------

using namespace Tf;

DECLARE_ALGORITHM_FACTORY( PatLHCbID2MCParticle )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatLHCbID2MCParticle::PatLHCbID2MCParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_otHitCreator("Tf::OTHitCreator/OTHitCreator")
{
  m_targetName = "Pat/LHCbID";
  declareProperty( "TargetName", m_targetName );
  declareProperty( "LinkOT", m_linkOT=true);
  declareProperty( "LinkIT", m_linkIT=true);
  declareProperty( "LinkTT", m_linkTT=true);
  declareProperty( "LinkVELO", m_linkVELO=true);
  declareProperty( "UseUT",   m_useUT=false);
}
//=============================================================================
// Destructor
//=============================================================================
PatLHCbID2MCParticle::~PatLHCbID2MCParticle() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatLHCbID2MCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if (m_linkOT) m_otHitCreator.retrieve().ignore();
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatLHCbID2MCParticle::execute() {

  if( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
  bool isVerbose = msgLevel( MSG::VERBOSE );

  LinkerWithKey<LHCb::MCParticle> lhcbLink( evtSvc(), msgSvc(), m_targetName );
  
  LHCb::MCParticle* part = nullptr;
  
  // link velo, if requested
  if (m_linkVELO) {

    LinkedTo<LHCb::MCParticle,LHCb::VeloCluster> 
      veloLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
    
    LHCb::VeloClusters* clusters = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    
    if (clusters){
      LHCb::VeloClusters::const_iterator iClus;
      
      for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {
        m_partList.clear();
        int id   = LHCb::LHCbID( (*iClus)->channelID()).veloID();
        int size = (*iClus)->pseudoSize();
        for ( int nn = 0; size > nn; ++nn ) {
          if ( isVerbose )  verbose() << format( "   VeloChannelID %8x ", id );
          part = veloLink.first( id );
          while ( 0 != part ) {
            if ( isVerbose ) verbose() << " " << part->key();
            addToList( part );
            part = veloLink.next();
          }
          if ( isVerbose ) verbose() << endmsg;
          id++;
        }
        for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
                m_partList.begin(); m_partList.end() != itP; ++itP ) {
          LHCb::LHCbID temp = (*iClus)->channelID();
          lhcbLink.link( temp.lhcbID(), *itP ); // same without cluster size
        }
      }
    } else {
      LHCb::VeloLiteCluster::FastContainer * liteClusters =
        get<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default);
      
      if (liteClusters){
        LHCb::VeloLiteCluster::FastContainer::const_iterator iClus;
        
        for(iClus = liteClusters->begin(); iClus != liteClusters->end(); ++iClus) {
          m_partList.clear();
          int id   = LHCb::LHCbID( (*iClus).channelID()).veloID();
          int size = (*iClus).pseudoSize();
          for ( int nn = 0; size > nn; ++nn ) {
            if ( isVerbose )  verbose() << format( "   VeloChannelID %8x ", id );
            part = veloLink.first( id );
            while ( 0 != part ) {
              if ( isVerbose ) verbose() << " " << part->key();
              addToList( part );
              part = veloLink.next();
            }
            if ( isVerbose ) verbose() << endmsg;
            id++;
          }
          for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
                  m_partList.begin(); m_partList.end() != itP; ++itP ) {
            LHCb::LHCbID temp = (*iClus).channelID();
            lhcbLink.link( temp.lhcbID(), *itP ); // same without cluster size
          }
        }
      }
    }
  }
  
  //== TT (UT)
  if (m_linkTT) {
    LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
      ttLink( evtSvc(), msgSvc(), (m_useUT ? LHCb::STClusterLocation::UTClusters : LHCb::STClusterLocation::TTClusters) );
    
    const LHCb::STCluster::Container* cont = 
      get<LHCb::STCluster::Container>(m_useUT ? LHCb::STClusterLocation::UTClusters : LHCb::STClusterLocation::TTClusters);
    
    
    for(  LHCb::STCluster::Container::const_iterator iclus = cont->begin();
          iclus != cont->end(); ++iclus) {
      
      m_partList.clear();
      LHCb::LHCbID lId = LHCb::LHCbID((*iclus)->channelID());
      int size   = (*iclus)->size();
      int id = lId.stID();
      
      
      for ( int nn = 0; size > nn; ++nn ) {
        if ( isVerbose )  verbose() << format( "   TTChannelID %8x ",  id );
        part = ttLink.first( id );
        while ( 0 != part ) {
          if ( isVerbose ) verbose() << " " << part->key();
          addToList( part );
          part = ttLink.next( );
        }
        if ( isVerbose ) verbose() << endmsg;
        id++;
      }
      
      for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = m_partList.begin();
            m_partList.end() != itP; ++itP ) {
        lhcbLink.link( lId.lhcbID(), *itP );
      }
    }
  }
  
  //== IT coordinates
  if (m_linkIT) {
    LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
      itLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters );
    
    const LHCb::STCluster::Container* cont = 
      get<LHCb::STCluster::Container>(LHCb::STClusterLocation::ITClusters);
    
    for(  LHCb::STCluster::Container::const_iterator iclus = cont->begin();
          iclus != cont->end(); ++iclus) {

      m_partList.clear();
      LHCb::LHCbID lId = LHCb::LHCbID((*iclus)->channelID());
      int size   = (*iclus)->size();
      
      int id = lId.stID();
     
      
      for ( int nn = 0; size > nn; ++nn ) {
        if ( isVerbose )  verbose() << format( "   ITChannelID %8x ",  id );
        part = itLink.first( id );
        while ( 0 != part ) {
          if ( isVerbose ) verbose() << " " << part->key();
          addToList( part );
          part = itLink.next();
        }
        if ( isVerbose ) verbose() << endmsg;
        id++;
      }
      if ( isVerbose ) verbose() << endmsg;
      for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
              m_partList.begin();m_partList.end() != itP; ++itP ) {
        lhcbLink.link( lId.lhcbID(), *itP );
      }
    }
  }
  
  //== OT coordinates
  if (m_linkOT) {
    LinkedTo<LHCb::MCParticle> 
      otLink( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
    
    OTHitRange othits = m_otHitCreator->hits();
    
    for (OTHitRange::const_iterator otSTH = othits.begin();
         otSTH < othits.end();otSTH++){ 
      m_partList.clear();
      LHCb::LHCbID lId = (*otSTH)->lhcbID();
      int id = lId.otID();
      if ( isVerbose )  verbose() << format( "   OTChannelID %8x ",  id );
      part = otLink.first( id );
      while ( 0 != part ) {
        if ( isVerbose ) verbose() << " " << part->key();
        addToList( part );
        part = otLink.next();
      }
      if ( isVerbose ) verbose() << endmsg;
      for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
              m_partList.begin();m_partList.end() != itP; ++itP ) {
        lhcbLink.link( lId.lhcbID(), *itP ); 
      }
    }
  }
  
  if( msgLevel( MSG::DEBUG ) ) debug() << "==> End of execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatLHCbID2MCParticle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
