// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IRegistry.h"

#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/STCluster.h"
#include "Event/Track.h"

#include "STDet/DeSTDetector.h"

// local
#include "PatDebugTTTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDebugTTTruthTool
//
// 2007-10-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatDebugTTTruthTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDebugTTTruthTool::PatDebugTTTruthTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatDebugTTTool>(this);

}


StatusCode PatDebugTTTruthTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::TT);

  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
PatDebugTTTruthTool::~PatDebugTTTruthTool() {} 
//=========================================================================
//  Print the true TT clusters associated to the specified track
//=========================================================================
void PatDebugTTTruthTool::debugTTClusterOnTrack (  const LHCb::Track* track, 
                                                   const PatTTHits::const_iterator beginCoord,
                                                   const PatTTHits::const_iterator endCoord   ) {
  //== Get the truth on this track
  std::string containerName = track->parent()->registry()->identifier();
  std::string linkerName = "Link/"+containerName;
  if ( "/Event/" == containerName.substr(0,7) ) {
    linkerName = "Link/" + containerName.substr(7);
  }
  if ( exist<LHCb::LinksByKey>( linkerName ) ) {
    LinkedTo<LHCb::MCParticle, LHCb::Track> trLink( evtSvc(), msgSvc(), containerName );
    LinkedTo<LHCb::MCParticle> itLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );

    LHCb::MCParticle* part = trLink.first( track->key() );
    while ( 0 != part ) {
      double momentum = part->momentum().R();
      info() << format( " MC Key %4d PID %6d  P %8.3f GeV tx %9.5f ty %9.5f",
                        part->key(), part->particleID().pid(), momentum / Gaudi::Units::GeV,
                        part->momentum().x()/ part->momentum().z(),
                        part->momentum().y()/ part->momentum().z() ) << endreq;
      for ( PatTTHits::const_iterator itH = beginCoord; endCoord != itH; ++itH ){
        const PatTTHit* hit = *itH;
        LHCb::STChannelID id = hit->hit()->lhcbID().stID();
        bool found = false;
        for( unsigned int kk = 0; hit->hit()->sthit()->cluster().pseudoSize() > kk; ++kk ) {
	  LHCb::MCParticle* clusPart = 0;
          if ( id != LHCb::STChannelID(0)) clusPart = itLink.first( id );
          while ( 0 != clusPart ) {
            if ( clusPart->key() == part->key() ) found = true;
            clusPart = itLink.next();
          }
          id = m_tracker->nextRight(id);
        }
        if ( found ) {
          double xCoord = hit->hit()->x() ;
          info() << "      TT Clus " 
                 << format( "(S%1d,L%1d,R%2d,S%2d,S%3d) x%7.1f High %1d", 
                            id.station(), id.layer(), id.detRegion(), 
                            id.sector(), id.strip(), xCoord, 
                            (*itH)->hit()->sthit()->cluster().highThreshold() ) << endreq;
        }
      }
      part = trLink.next();
    }
  }
}

//=========================================================================
//  Print the MC keys associated to this cluster
//=========================================================================
void PatDebugTTTruthTool::debugTTCluster( MsgStream& msg, const PatTTHit* hit ) {

  LinkedTo<LHCb::MCParticle> itLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );
  std::string value = "";

  LHCb::STChannelID id = hit->hit()->lhcbID().stID();
  int lastKey = -1;
  for( unsigned int kk = 0; hit->hit()->sthit()->cluster().pseudoSize() > kk; ++kk ) {
    LHCb::MCParticle* part = 0;
    if (id != LHCb::STChannelID(0)) part = itLink.first( id );
    while ( 0 != part ) {
      if ( lastKey != part->key() ) {
        lastKey = part->key();
        msg << " " << lastKey;
      }
      part = itLink.next();
    }
    id = m_tracker->nextRight(id);
  }
}

//=============================================================================
