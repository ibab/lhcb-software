// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"

#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/VPCluster.h"
#include "Kernel/LHCbID.h"

// local
#include "PatPixelDebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelDebugTool
//
// 2007-10-25 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatPixelDebugTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelDebugTool::PatPixelDebugTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatDebugTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PatPixelDebugTool::~PatPixelDebugTool() {}
//=========================================================================
//  Check if a given LHCbID is associated to the MCParticle of the specified key
//=========================================================================
bool PatPixelDebugTool::matchKey( LHCb::LHCbID& id, int key ) {

  LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VPClusterLocation::VPClusterLocation );

  LHCb::VPChannelID idV = id.vpID();
  
  LHCb::MCParticle* part = vLink.first( idV );
  while ( 0 != part ) {
    if ( key == part->key() ) return true;
    part = vLink.next();
  }
  return false;
}
//=========================================================================
//  Print the list of MCParticle keys associated to the specified LHCbID
//=========================================================================
void PatPixelDebugTool::printKey( MsgStream& msg, LHCb::LHCbID& id ) {
  LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VPClusterLocation::VPClusterLocation );

  LHCb::VPChannelID idV = id.vpID();
  LHCb::MCParticle* part = vLink.first( idV );
  while ( 0 != part ) {
    msg << " " << part->key();
    part = vLink.next();
  }
}

//=========================================================================
//  
//=========================================================================
double PatPixelDebugTool::xTrue( int key, double z  ) {
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
double PatPixelDebugTool::yTrue( int key, double z  ) {
  LHCb::MCParticles* parts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  LHCb::MCParticle* part = parts->object( key );
  if ( 0 == part ) return -999.;
  double ty = part->momentum().py() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.y() + ty * ( z - origin.z() );
}
//=============================================================================
