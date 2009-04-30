// $Id: CopyParticles.h,v 1.16 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYPARTICLES_H 
#define COPYPARTICLES_H 1

// Include files
// from MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include <MicroDST/ICloneParticle.h>
#include "MicroDST/BindType2ClonerDef.h"
// from LHCb
#include <Event/Particle.h>
/** @class CopyParticles CopyParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::Particles and related objects 
 * from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::Particles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::Particles is performed by the 
 * ICloneParticle, the implementation of which is set by the property 
 * ClonerType (default ParticleCloner)
 * @see ICloneParticle
 * @see ParticleCloner
 *
 * <b>Example</b>: Clone particles from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" to 
 * "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" using a ParticleCloner
 *  @code
 *  MySelection = GaudiSequencer("SomeSelectionSequence")
 *  copyParticles = CopyParticles()
 *  copyParticles.OutputPrefix = "MyLocation"
 *  copyParticles.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"
 *  #copyParticles.addTool(ParticleCloner, name='ClonerType')
 *  #copyParticles.ParticleCloner.addTool(VertexCloner, name='IClonerVertex')
 *  #copyParticles.ParticleCloner.addTool(ProtoParticleCloner, name='ICloneProtoParticle')
 *  MySelection.Members += [copyParticles]
 *  @endcode
 * 
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-16
 */
//=============================================================================
template <> struct BindType2Cloner<LHCb::Particle> 
{
  typedef LHCb::Particle type;
  typedef ICloneParticle cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::Particle>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::Particle>::clonerType = "ParticleCloner";
//=============================================================================
template<> struct Location<LHCb::Particle>
{
  const static std::string Default;
};
const std::string Location<LHCb::Particle>::Default = LHCb::ParticleLocation::Production;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::Particle> CopyParticles;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyParticles, CopyParticles )
#endif // COPYPRIMARYVERTICES_H
