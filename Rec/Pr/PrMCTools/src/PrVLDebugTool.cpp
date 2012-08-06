// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
// Event/LinkerEvent
#include "Linker/LinkedTo.h"
// Event/MCEvent
#include "Event/MCParticle.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Kernel/LHCbKernel
#include "Kernel/LHCbID.h"
// Local
#include "PrVLDebugTool.h"

/** @file PrVLDebugTool.cpp
 *
 *  Implementation of class : PrVLDebugTool 
 *
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(PrVLDebugTool)

//=============================================================================
/// Standard constructor
//=============================================================================
PrVLDebugTool::PrVLDebugTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
    GaudiTool(type, name, parent) {

  declareInterface<IPrDebugTool>(this);

}

//=============================================================================
/// Check if a LHCbID is associated to the MCParticle of the specified key
//=============================================================================
bool PrVLDebugTool::matchKey(LHCb::LHCbID id, int key) {

  LinkedTo<LHCb::MCParticle> link(evtSvc(), msgSvc(), 
                                  LHCb::VLClusterLocation::Default);
  LHCb::VLChannelID vlid = id.vlID();
  LHCb::MCParticle* particle = link.first(vlid);
  while (particle) {
    if (key == particle->key()) return true;
    particle = link.next();
  }
  return false;

}

//=============================================================================
// Print the MCParticle keys associated to the specified LHCbID
//=============================================================================
void PrVLDebugTool::printKey(MsgStream& msg, LHCb::LHCbID id) {

  LinkedTo<LHCb::MCParticle> link(evtSvc(), msgSvc(), 
                                  LHCb::VLClusterLocation::Default);

  LHCb::VLChannelID vlid = id.vlID();
  LHCb::MCParticle* particle = link.first(vlid);
  while (particle) {
    msg << " " << particle->key();
    particle = link.next();
  }

}

//=============================================================================
/// Get the x-coordinate of a particle at a given z position 
//=============================================================================
double PrVLDebugTool::xTrue(int key, double z) {

  LHCb::MCParticles* particles = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LHCb::MCParticle* particle = particles->object(key);
  if (!particle) return -999.;
  const double tx = particle->momentum().px() / particle->momentum().pz();
  const Gaudi::XYZPoint origin = particle->originVertex()->position();
  return origin.x() + tx * (z - origin.z());

}

//=============================================================================
/// Get the y-coordinate of a particle at a given z position 
//=============================================================================
double PrVLDebugTool::yTrue(int key, double z) {

  LHCb::MCParticles* particles = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LHCb::MCParticle* particle = particles->object(key);
  if (!particle) return -999.;
  const double ty = particle->momentum().py() / particle->momentum().pz();
  const Gaudi::XYZPoint origin = particle->originVertex()->position();
  return origin.y() + ty * (z - origin.z());

}
