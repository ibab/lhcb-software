// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "Linker/LinkedTo.h"
#include "Event/MCParticle.h"

// local
#include "PatDebugTrackTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDebugTrackTool
//
// 2009-03-31 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatDebugTrackTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDebugTrackTool::PatDebugTrackTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatDebugTrackTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PatDebugTrackTool::~PatDebugTrackTool() {} 

//=========================================================================
// Return the keys of linked MC particles 
//=========================================================================
std::vector<int> PatDebugTrackTool::keys( const LHCb::Track* track) {
  std::vector<int> keys;
  std::string containerName = track->parent()->registry()->identifier();
  LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), containerName );
  LHCb::MCParticle* part = vLink.first( track->key() );
  while ( 0 != part ) {
    keys.push_back( part->key() );
    part = vLink.next();
  }
  return keys;
}
//=============================================================================
