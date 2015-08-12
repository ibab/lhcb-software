// Include files 

// from Gaudi
#include "Linker/LinkedTo.h"
#include "Kernel/LHCbID.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
#include "Event/MCParticle.h"
// local
#include "PatTStationDebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatTStationDebugTool
//
// 2010-10-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatTStationDebugTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTStationDebugTool::PatTStationDebugTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatDebugTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
PatTStationDebugTool::~PatTStationDebugTool() {} 
//=========================================================================
//  Check if a given LHCbID is associated to the MCParticle of the specified key
//=========================================================================
bool PatTStationDebugTool::matchKey( LHCb::LHCbID& id, int key ) {

  if ( id.isTT() ) {
    LinkedTo<LHCb::MCParticle> iLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );
    LHCb::STChannelID idS = id.stID();
    LHCb::MCParticle* part = iLink.first( idS );
    while ( 0 != part ) {
      if ( key == part->key() ) return true;
      part = iLink.next();
    }
  } else   if ( id.isIT() ) {
    LinkedTo<LHCb::MCParticle> iLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::ITClusters );
    LHCb::STChannelID idS = id.stID();
    LHCb::MCParticle* part = iLink.first( idS );
    while ( 0 != part ) {
      if ( key == part->key() ) return true;
      part = iLink.next();
    }
  } else if ( id.isOT() ) {
    LinkedTo<LHCb::MCParticle> oLink( evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
    LHCb::OTChannelID idO = id.otID();
  
    LHCb::MCParticle* part = oLink.first( idO );
    while ( 0 != part ) {
      if ( key == part->key() ) return true;
      part = oLink.next();
    }
  }
  
  return false;
}
//=========================================================================
//  Print the list of MCParticle keys associated to the specified LHCbID
//=========================================================================
void PatTStationDebugTool::printKey( MsgStream& msg, LHCb::LHCbID& id ) {

  if ( id.isIT() ) {
    LinkedTo<LHCb::MCParticle> iLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::ITClusters );
    LHCb::STChannelID idS = id.stID();
    LHCb::MCParticle* part = iLink.first( idS );
    while ( 0 != part ) {
      msg << " " << part->key();
      part = iLink.next();
    }
  } else if ( id.isTT() ) {
    LinkedTo<LHCb::MCParticle> iLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );
    LHCb::STChannelID idS = id.stID();
    LHCb::MCParticle* part = iLink.first( idS );
    while ( 0 != part ) {
      msg << " " << part->key();
      part = iLink.next();
    }
  } else if ( id.isOT() ) {
    LinkedTo<LHCb::MCParticle> oLink( evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
    LHCb::OTChannelID idO = id.otID();
  
    LHCb::MCParticle* part = oLink.first( idO );
    while ( 0 != part ) {
      msg << " " << part->key();
      part = oLink.next();
    }
  }
}

//=========================================================================
//  
//=========================================================================
double PatTStationDebugTool::xTrue( int key, double z  ) {
  LHCb::MCParticles* parts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  LHCb::MCParticle* part = parts->object( key );
  if ( 0 == part ) return -999.;
  double tx = part->momentum().px() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.x() + tx * ( z - origin.z() );
}

//=========================================================================
//  
//=========================================================================
double PatTStationDebugTool::yTrue( int key, double z  ) {
  LHCb::MCParticles* parts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  LHCb::MCParticle* part = parts->object( key );
  if ( 0 == part ) return -999.;
  double ty = part->momentum().py() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.y() + ty * ( z - origin.z() );
}
//=============================================================================
