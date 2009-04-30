// $Id: CopyProtoParticles.h,v 1.12 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYPROTOPARTICLES_H 
#define COPYPROTOPARTICLES_H 1

// Include files
// from MicroDST
// from MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include <MicroDST/ICloneProtoParticle.h>
#include "MicroDST/BindType2ClonerDef.h"
// from LHCb
#include <Event/ProtoParticle.h>
/** @class CopyProtoParticles CopyProtoParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::ProtoParticles from one TES location 
 * to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::ProtoParticles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::ProtoParticles is performed by the 
 * ICloneProtoParticle, the implementation of which is set by the property 
 * ClonerType (default ProtoParticleCloner). It is the 
 * ICloneProtoParticle that controls the depth of the cloning, ie 
 * what is done with related elements like LHCb::Tracks etc.
 *
 * @see ICloneProtoParticle
 * @see ProtoParticleCloner
 *
 * <b>Example</b>: Clone ProtoParticles from "/Event/Rec/ProtoP/Charged" to 
 * "/Event/MyLocation/Rec/ProtoP/Charged" using a ProtoParticleCloner
 *  @code
 *
 *  // Add a CopyProtoParticles instance to a selection sequence
 *  mySeq = GaudiSequencer("SeqDC06selBd2Jpsi2MuMu_Kst2KPi")
 *  copyPP = CopyProtoParticles()
 *  mySeq.Members += [copyPP]
 *  copyPP.OutputPrefix = "MyLocation"
 *  copyPP.InputLocation = "Rec/ProtoP/Charged"
 *  copyPP.addTool(ProtoParticleCloner)
 *
 *  @endcode
 * 
 *
 *  @todo Test.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-01
 */
template <> struct BindType2Cloner<LHCb::ProtoParticle> 
{
  typedef LHCb::ProtoParticle type;
  typedef ICloneProtoParticle cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::ProtoParticle>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::ProtoParticle>::clonerType = "ProtoParticleCloner";
//=============================================================================
template<> struct Location<LHCb::ProtoParticle>
{
  const static std::string Default;
};
const std::string Location<LHCb::ProtoParticle>::Default = "";
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::ProtoParticle>
CopyProtoParticles;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyProtoParticles, CopyProtoParticles)
#endif // COPYPROTOPARTICLES_H
