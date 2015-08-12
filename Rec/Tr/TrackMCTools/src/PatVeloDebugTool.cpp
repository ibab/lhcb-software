// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"

#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/VeloCluster.h"
#include "Kernel/LHCbID.h"

// local
#include "PatVeloDebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloDebugTool
//
// 2007-10-25 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatVeloDebugTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloDebugTool::PatVeloDebugTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatDebugTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PatVeloDebugTool::~PatVeloDebugTool() {}
//=========================================================================
//  Check if a given LHCbID is associated to the MCParticle of the specified key
//=========================================================================
bool PatVeloDebugTool::matchKey( LHCb::LHCbID& id, int key ) {

  LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );

  LHCb::VeloChannelID idV = id.veloID();
  
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
void PatVeloDebugTool::printKey( MsgStream& msg, LHCb::LHCbID& id ) {
  LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );

  LHCb::VeloChannelID idV = id.veloID();
  LHCb::MCParticle* part = vLink.first( idV );
  while ( 0 != part ) {
    msg << " " << part->key();
    part = vLink.next();
  }
}

//=========================================================================
//  
//=========================================================================
double PatVeloDebugTool::xTrue( int key, double z  ) {
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
double PatVeloDebugTool::yTrue( int key, double z  ) {
  LHCb::MCParticles* parts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  LHCb::MCParticle* part = parts->object( key );
  if ( 0 == part ) return -999.;
  double ty = part->momentum().py() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.y() + ty * ( z - origin.z() );
}
//=============================================================================
