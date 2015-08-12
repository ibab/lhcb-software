// $Id: $
// Include files 

// from Gaudi
#include "Linker/LinkedTo.h"
#include "Kernel/LHCbID.h"
#include "Event/OTTime.h"
#include "Event/FTCluster.h"
#include "Event/MCParticle.h"

// local
#include "PrTStationDebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrTStationDebugTool
//
// 2012-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrTStationDebugTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrTStationDebugTool::PrTStationDebugTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPrDebugTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PrTStationDebugTool::~PrTStationDebugTool() {} 

//=========================================================================
//  Check if a given LHCbID is associated to the MCParticle of the specified key
//=========================================================================
bool PrTStationDebugTool::matchKey ( LHCb::LHCbID id, int key ) {
  if ( id.isOT() ) {
    LinkedTo<LHCb::MCParticle> oLink( evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
    LHCb::OTChannelID idO = id.otID();
    
    LHCb::MCParticle* part = oLink.first( idO );
    while ( 0 != part ) {
      if ( key == part->key() ) return true;
      part = oLink.next();
    }
  } else if ( id.isFT() ) {    
    LinkedTo<LHCb::MCParticle> fLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
    LHCb::FTChannelID idO = id.ftID();
    
    LHCb::MCParticle* part = fLink.first( idO );
    while ( 0 != part ) {
      if ( key == part->key() ) return true;
      part = fLink.next();
    }
  }
  return false;
}
//=========================================================================
//  Print the list of MCParticle keys associated to the specified LHCbID
//=========================================================================
void PrTStationDebugTool::printKey( MsgStream& msg, LHCb::LHCbID id ) {
  if ( id.isOT() ) {
    LinkedTo<LHCb::MCParticle> oLink( evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
    LHCb::OTChannelID idO = id.otID();
  
    LHCb::MCParticle* part = oLink.first( idO );
    if ( 0 != part ) msg << " MC:";
    while ( 0 != part ) {
      msg << " " << part->key();
      part = oLink.next();
    }
  } else if ( id.isFT() ) {
    LinkedTo<LHCb::MCParticle> fLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
    LHCb::FTChannelID idO = id.ftID();
  
    LHCb::MCParticle* part = fLink.first( idO );
    if ( 0 != part ) msg << " MC:";
    while ( 0 != part ) {
      msg << " " << part->key();
      part = fLink.next();
    }
  }
}
//=============================================================================
