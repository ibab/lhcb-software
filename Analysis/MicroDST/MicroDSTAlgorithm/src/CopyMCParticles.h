// $Id: CopyMCParticles.h,v 1.4 2008-09-01 17:28:14 jpalac Exp $
#ifndef COPYMCPARTICLES_H 
#define COPYMCPARTICLES_H 1

// Include files
// from MicroDST
#include <MicroDST/KeyedContainerClonerAlg.h>
#include <MicroDST/ICloneMCParticle.h>
#include "MicroDST/BindType2ClonerDef.h"
// from LHCb
#include <Event/MCParticle.h>
/** @class CopyMCParticles CopyMCParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::MCParticles and related objects 
 * from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::MCParticles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::MCParticles is performed by the 
 * ICloneMCParticle, the implementation of which is set by the property 
 * ICloneMCParticle (default MCParticleCloner)
 * @see ICloneMCParticle
 * @see MCParticleCloner
 *
 * <b>Example</b>: Clone MCParticles from "/Event/MC/Particles" to 
 * "/Event/MyLocation/MC/Particles" using an MCParticleCloner
 *  @code
 *
 *  // Add a CopyMCParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"MicroDST::KeyedContainerClonerAlg<LHCb::MCParticle>/CopyMCParticles"};
 *  CopyMCParticles.OutputPrefix = "MyLocation";
 *  CopyMCParticles.InputLocation = "MC/Particles";
 *  CopyMCParticles.ICloneMCParticle = "MCParticleCloner"
 *  @endcode
 * 
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-08
 */
template <> struct BindType2Cloner<LHCb::MCParticle> 
{
  typedef LHCb::MCParticle type;
  typedef ICloneMCParticle cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::MCParticle>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::MCParticle>::clonerType = "MCParticleCloner";
//=============================================================================
template<> struct Location<LHCb::MCParticle>
{
  const static std::string Default;
};
const std::string Location<LHCb::MCParticle>::Default = LHCb::MCParticleLocation::Default;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::MCParticle> CopyMCParticle;
DECLARE_ALGORITHM_FACTORY( CopyMCParticle );
#endif // COPYMCPARTICLES_H
