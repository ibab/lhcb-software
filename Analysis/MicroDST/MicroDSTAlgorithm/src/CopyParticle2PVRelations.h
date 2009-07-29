// $Id: CopyParticle2PVRelations.h,v 1.3 2009-07-29 21:35:25 jpalac Exp $
#ifndef COPYPARTICLE2PVRELATIONS_H 
#define COPYPARTICLE2PVRELATIONS_H 1

// Include files
// from MicroDST
#include "MicroDST/RelationsClonerAlg.h"
#include <MicroDST/ICloneVertexBase.h>
#include "MicroDST/BindType2ClonerDef.h"
// from DaVinci
#include "Kernel/Particle2Vertex.h"
#include "Event/RecVertex.h"
/** @class CopyParticle2PVRelations CopyParticle2PVRelations.h
 *  
 *  MicroDSTAlgorithm to clone the associations between an LHCb::Particle
 *  and LHCb::RecVertices. 
 *  It inherits the std::string properties InputLocation and OutputPrefix from
 *  MicroDSTCommon. The associations are taken from the TES location 
 *  defined by InputLocation, and are cloned and put in TES location 
 *  "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 *  a leading "/Event" it is removed.
 *  The associations are typically in a Particle2Vertex::Table in a TES 
 *  location like "/Event/Phys/SomeParticleMakingAlg/Particle2VertexRelations".
 *  The algorithm picks up this relations table, which relates LHCb::Particles
 *  from "/Event/Phys/SomaParticleMakingAlg/Particles" to LHCb::RecVertices.
 *  For each LHCb::Particle to LHCb::MCParticle association, it picks up the 
 *  corresponding <b>previously cloned</b> LHCb::Particle from 
 *  "/Event/MyLocation/Phys/SomeParticleMakerAlg/Particles", and clones the 
 *  associated LHCb::RecVertices. If the LHCb::MCParticle has not been 
 *  previously cloned, no association is cloned or stored. The algorithm
 *  constructs a new Particle2Vertex::Table relating the clones and 
 *  stored it in a TES location as described above.
 *
 *  <b>Important conditions</b>: This algorithm relies on the LHCb::Particle
 *  set in the associations to have been previously cloned by CopyParticles,
 *  and the existence of the Particle2Vertex::Table associating the origin
 *  LHCb::Particles to LHCb::RecVertices
 *
 *  <b>Example</b>: Clone LHCb::Particle -> LHCb::MCParticle associations
 *  from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations"
 *  to "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations"
 *  @code
 *  mySeq = GaudiSequencer('SeqDC06selBd2Jpsi2MuMu_Kst2KPi')
 *  copyP2PV = CopyParticle2PVRelations()
 *  mySeq.Members += [copyP2PV]
 *  copyP2MCP.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations";
 *  copyP2MCP.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2009-04-15
 */
//=============================================================================
template <> struct BindType2Cloner<Particle2Vertex::Table> 
{
  typedef LHCb::RecVertex toType;
  typedef ICloneVertexBase toCloner;
};
//=============================================================================
template<> struct Defaults<Particle2Vertex::Table>
{
  const static std::string clonerType;
};
const std::string Defaults<Particle2Vertex::Table>::clonerType = "RecVertexCloner";
//=============================================================================
template<> struct Location<Particle2Vertex::Table>
{
  const static std::string Default;
};
const std::string Location<Particle2Vertex::Table>::Default = "NO DEFAULT LOCATION";
//=============================================================================
typedef MicroDST::RelationsClonerAlg<Particle2Vertex::Table> CopyParticle2PVRelations;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyParticle2PVRelations, CopyParticle2PVRelations )
#endif // COPYPARTICLE2PVRELATIONS_H
