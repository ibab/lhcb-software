// Gaudi
#include "GaudiKernel/IRegistry.h"

// LHCb
// Event/LinkerEvent
#include "Linker/LinkedTo.h"
// Event/MCEvent
#include "Event/MCParticle.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
// Kernel/LHCbKernel
#include "Kernel/LHCbID.h"

// Local
#include "PrPixelDebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrPixelDebugTool
//
// 2007-10-25 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(PrPixelDebugTool)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelDebugTool::PrPixelDebugTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {

  declareInterface<IPrDebugTool>(this);

}

//=============================================================================
// Destructor
//=============================================================================
PrPixelDebugTool::~PrPixelDebugTool() {}

//=============================================================================
// Check if a given LHCbID is associated to the MCParticle of the specified key
//=============================================================================
bool PrPixelDebugTool::matchKey(LHCb::LHCbID id, int key) {

  LinkedTo<LHCb::MCParticle> vLink(evtSvc(), msgSvc(),
                                   LHCb::VPClusterLocation::Default);
  LHCb::VPChannelID idV = id.vpID();
  LHCb::MCParticle* part = vLink.first(idV);
  while (part) {
    if (key == part->key()) return true;
    part = vLink.next();
  }
  return false;

}

//=========================================================================
// Print the list of MCParticle keys associated to the specified LHCbID
//=========================================================================
void PrPixelDebugTool::printKey(MsgStream& msg, LHCb::LHCbID id) {

  LinkedTo<LHCb::MCParticle> vLink(evtSvc(), msgSvc(),
                                   LHCb::VPClusterLocation::Default);
  LHCb::VPChannelID idV = id.vpID();
  LHCb::MCParticle* part = vLink.first(idV);
  while (part) {
    msg << " " << part->key();
    part = vLink.next();
  }

}

//=========================================================================
// Calculate the x position of a particle at a given z
//=========================================================================
double PrPixelDebugTool::xTrue(int key, double z) {

  LHCb::MCParticles* parts =
      get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LHCb::MCParticle* part = parts->object(key);
  if (NULL == part) return -999.;
  const double tx = part->momentum().px() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.x() + tx * (z - origin.z());

}

//=========================================================================
// Calculate the y position of a particle at a given z
//=========================================================================
double PrPixelDebugTool::yTrue(int key, double z) {

  LHCb::MCParticles* parts =
      get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LHCb::MCParticle* part = parts->object(key);
  if (NULL == part) return -999.;
  const double ty = part->momentum().py() / part->momentum().pz();
  Gaudi::XYZPoint origin = part->originVertex()->position();
  return origin.y() + ty * (z - origin.z());

}
