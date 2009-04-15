// $Id: CopyParticle2MCRelations.h,v 1.1 2009-04-15 16:44:03 jpalac Exp $
#ifndef COPYPARTICLE2MCRELATIONS_H 
#define COPYPARTICLE2MCRELATIONS_H 1

// Include files
// from MicroDST
#include "MicroDST/RelationsClonerAlg.h"
#include <MicroDST/ICloneMCParticle.h>
#include "MicroDST/BindType2ClonerDef.h"
// from DaVinci
#include "Kernel/Particle2MCParticle.h"

/** @class CopyParticle2MCRelations CopyParticle2MCRelations.h
 *  
 *  MicroDSTAlgorithm to clone the associations between an LHCb::Particle
 *  and and LHCb::RecVertex. Typically this association should be between
 *  a candidate LHCb::Particle and any primary vertices used to select
 *  it. 
 *  It inherits the std::string properties InputLocation and OutputPrefix from
 *  MicroDSTCommon. The associations are taken from the TES location 
 *  defined by InputLocation, and are cloned and put in TES location 
 *  "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 *  a leading "/Event" it is removed.
 *  The associations are typically in a Particle2Vertex::Table in a TES 
 *  location like "/Event/Phys/SomeParticleMakingAlg/Particle2VertexRelations".
 *  The algorithm picks up this relations table, which relates LHCb::Particles
 *  from "/Event/Phys/SomaParticleMakingAlg/Particles" to LHCb::RecVertex.
 *  For each LHCb::Particle to LHCb::RecVertex association, it picks up the 
 *  corresponding <b>previously cloned</b> LHCb::Particle and LHCb::RecVertex 
 *  from "/Event/MyLocation/Phys/SomeParticleMakerAlg/Particles" and
 *  "/Event/MyLocation/Phys/<location of associated vertices>" respectively.
 *  If these have not been previously cloned, no association is cloned or 
 *  stored. This algorithm <b>is not resopnsible for cloning the associated
 *  TES objects</b>
 *
 *  <b>Important conditions</b>: This algorithm relies on the LHCb::Particle
 *  and LHCb::MCParticle sets in the associations to have been previously
 *  cloned by CopyParticles and CopyRelatedMCParticles.
 *
 *  <b>Example</b>: Clone LHCb::Particle -> LHCb::MCParticle associations
 *  from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2MCPRelations"
 *  to "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2MCPRelations"
 *  @code
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyParticle2MCRelations"};
 *  CopyParticle2MCRelations.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations";
 *  CopyParticle2MCRelations.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2009-04-15
 */
//=============================================================================
template <> struct BindType2Cloner<Particle2MCParticle::WTable> 
{
  typedef LHCb::MCParticle type;
  typedef ICloneMCParticle toCloner;
};
//=============================================================================
template<> struct Defaults<Particle2MCParticle::WTable>
{
  const static std::string clonerType;
};
const std::string Defaults<Particle2MCParticle::WTable>::clonerType = "MCParticleCloner";
//=============================================================================
template<> struct Location<Particle2MCParticle::WTable>
{
  const static std::string Default;
};
const std::string Location<Particle2MCParticle::WTable>::Default = "NO DEFAULT LOCATION";
//=============================================================================
typedef MicroDST::RelationsClonerAlg<Particle2MCParticle::WTable> CopyParticle2MCRelations;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyParticle2MCRelations, CopyParticle2MCRelations )
#endif // COPYPARTICLE2MCRELATIONS_H
