// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCParticle.h"

// includes from DigiEvent
#include "Event/VeloCluster.h"
#include "Event/VPCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// local
#include "PatLHCbIDUp2MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatLHCbIDUp2MCParticle
//
// 2010-03-22 Victor Coco
//-----------------------------------------------------------------------------

using namespace Tf;

DECLARE_ALGORITHM_FACTORY( PatLHCbIDUp2MCParticle )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatLHCbIDUp2MCParticle::PatLHCbIDUp2MCParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_level1(0),
    m_othitcreator("Tf::OTHitCreator")
{
  m_targetName = "Pat/LHCbID";
  declareProperty( "TargetName", m_targetName );
  declareProperty( "LinkOT", m_linkOT=true);
  declareProperty( "LinkIT", m_linkIT=true);
  declareProperty( "LinkTT", m_linkTT=true);
  declareProperty( "LinkVELO", m_linkVELO=true);
  declareProperty( "LinkVP", m_linkVP=false);
}
//=============================================================================
// Destructor
//=============================================================================
PatLHCbIDUp2MCParticle::~PatLHCbIDUp2MCParticle() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatLHCbIDUp2MCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if (m_linkOT) m_othitcreator.retrieve().ignore();
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatLHCbIDUp2MCParticle::execute() {

  debug() << "==> Execute" << endmsg;
  bool isVerbose = msgLevel( MSG::VERBOSE );

  LinkerWithKey<LHCb::MCParticle>
    lhcbLink( evtSvc(), msgSvc(), m_targetName );

  LHCb::MCParticle* part;

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
          if ( isVerbose ) verbose() << endreq;
          id++;
        }
        for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
            m_partList.begin(); m_partList.end() != itP; ++itP ) {
          LHCb::LHCbID temp = (*iClus)->channelID();
          lhcbLink.link( temp.lhcbID(), *itP ); // same without cluster size
        }
      }
    }
    else {
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
            if ( isVerbose ) verbose() << endreq;
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

  //== TT
  if (m_linkTT) {
    LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
      ttLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters );

    const LHCb::STCluster::Container* cont = 
      get<LHCb::STCluster::Container>(LHCb::STClusterLocation::TTClusters);
 

    for(  LHCb::STCluster::Container::const_iterator iclus = cont->begin();
	  iclus != cont->end(); ++iclus) {
      
      m_partList.clear();
      LHCb::LHCbID lId = LHCb::LHCbID((*iclus)->channelID());
      int size   = (*iclus)->size();
      int id = lId.stID();

      for ( int nn = 0; size > nn; ++nn ) {
	part = ttLink.first( id );
	while ( 0 != part ) {
	    addToList( part );
	    part = ttLink.next( );
	}
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
	part = itLink.first( id );
        while ( 0 != part ) {
	    addToList( part );
          part = itLink.next();
        }
	id++;
      }
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

    OTHitRange othits = m_othitcreator->hits();

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
      if ( isVerbose ) verbose() << endreq;
      for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
          m_partList.begin();m_partList.end() != itP; ++itP ) {
        lhcbLink.link( lId.lhcbID(), *itP ); 
      }
    }
  }


  // link vP, if requested
  if (m_linkVP) {
    // Link is made from VPCluster relation to MCP but is linked to VPLiteCluster ID (that might differ)

    
    // That is the correct one!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    LinkedTo<LHCb::MCParticle,LHCb::VPCluster> 
      vPLink( evtSvc(), msgSvc(), LHCb::VPClusterLocation::VPClusterLocation );
    //LinkedTo<LHCb::MCParticle,LHCb::VPCluster>
    //  vPLink( evtSvc(), msgSvc(), "VP/Clusters2MCParticles" );


    LHCb::VPClusters* clusters = get<LHCb::VPClusters>(LHCb::VPClusterLocation::VPClusterLocation );
    debug()<<"# clusters: "<<clusters->size()<<endmsg;
    
    if (clusters){
      
      std::sort( clusters->begin(), clusters->end(), increasingSensor);
      
      LHCb::VPClusters::const_iterator iClus;
      
      for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {
        m_partList.clear();
        part = vPLink.first((*iClus)->channelID() );
	
	if (part == NULL ){
          debug()<<"No associated MCP"<<endreq;
          continue;
        }
        while ( 0 != part ) {
          if ( isVerbose ) verbose() << " " << part->key();
          addToList( part );
          part = vPLink.next();
        }
        if ( isVerbose ) verbose() << endreq;
        for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
                m_partList.begin(); m_partList.end() != itP; ++itP ) {
          LHCb::LHCbID temp = LHCb::LHCbID((*iClus)->lCluster().channelID()).vpID();
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
StatusCode PatLHCbIDUp2MCParticle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
