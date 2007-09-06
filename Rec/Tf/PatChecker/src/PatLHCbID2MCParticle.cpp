// $Id: PatLHCbID2MCParticle.cpp,v 1.2 2007-09-06 16:38:52 smenzeme Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

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

DECLARE_ALGORITHM_FACTORY( PatLHCbID2MCParticle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatLHCbID2MCParticle::PatLHCbID2MCParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_othitcreator("Tf::OTHitCreator"),
    m_ithitcreator("Tf::STHitCreator/ITHitCreator"),
    m_tthitcreator("Tf::STHitCreator/TTHitCreator")
{
  m_targetName = "Pat/LHCbID";
  declareProperty( "TargetName", m_targetName );
}
//=============================================================================
// Destructor
//=============================================================================
PatLHCbID2MCParticle::~PatLHCbID2MCParticle() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatLHCbID2MCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_othitcreator.retrieve().ignore();
  m_ithitcreator.retrieve().ignore();
  m_tthitcreator.retrieve().ignore();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatLHCbID2MCParticle::execute() {

  debug() << "==> Execute" << endmsg;
  bool isVerbose = msgLevel( MSG::VERBOSE );

  LinkerWithKey<LHCb::MCParticle>
    lhcbLink( evtSvc(), msgSvc(), m_targetName );

  LHCb::MCParticle* part;

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

  LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
    itLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters );
  LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
    ttLink( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters );
  LinkedTo<LHCb::MCParticle,LHCb::OTTime> 
    otLink( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );

  //== TT
  STHitRange tthits = m_tthitcreator->ttHits();
   
  for (STHitRange::const_iterator itTTH = tthits.begin();
       itTTH < tthits.end();itTTH++){

    m_partList.clear();
    LHCb::LHCbID lId = (*itTTH)->lhcbID();
    if ( lId.isST() ) {
      int id = lId.stID();
      if ( isVerbose )  verbose() << format( "   ITChannelID %8x ", id );
      part = ttLink.first( id );
      while ( 0 != part ) {
        if ( isVerbose ) verbose() << " " << part->key();
        addToList( part );
        part = ttLink.next();
      }
      if ( isVerbose ) verbose() << endreq;
      id++;
    }
    for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = m_partList.begin();
          m_partList.end() != itP; ++itP ) {
      lhcbLink.link( lId.lhcbID(), *itP );
    }
  }

  //== T coordinates
  STHitRange ithits = m_ithitcreator->itHits();
  for (STHitRange::const_iterator itSTH = ithits.begin();
	     itSTH < ithits.end();itSTH++){
    m_partList.clear();
    LHCb::LHCbID lId = (*itSTH)->lhcbID();
    int size   = (*itSTH)->size();
    int id = lId.stID();
      for ( int nn = 0; size > nn; ++nn ) {
        if ( isVerbose )  verbose() << format( "   ITChannelID %8x ", id );
        part = itLink.first( id );
        while ( 0 != part ) {
          if ( isVerbose ) verbose() << " " << part->key();
          addToList( part );
          part = itLink.next();
        }
        if ( isVerbose ) verbose() << endreq;
        id++;
      }
      for ( std::vector<const LHCb::MCParticle*>::const_iterator itP = 
	    m_partList.begin();m_partList.end() != itP; ++itP ) {
      lhcbLink.link( lId.lhcbID(), *itP );
    }
  }

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

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatLHCbID2MCParticle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
