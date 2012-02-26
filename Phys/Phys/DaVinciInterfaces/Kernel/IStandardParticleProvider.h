
#ifndef DAVINCIKERNEL_IStandardParticleProvider_H
#define DAVINCIKERNEL_IStandardParticleProvider_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event Model
namespace LHCb
{
  class Particle;
  class ParticleID;
  class ProtoParticle;
}

/** @class IStandardParticleProvider Kernel/IStandardParticleProvider.h
 *
 *  Interface to tool that provides pointer to 'standard' Particles
 *  for a given ProtoParticle and PID hypothesis
 *
 *  @author Chris Jones
 *  @date   26/02/2012
 */

class GAUDI_API IStandardParticleProvider : virtual public IAlgTool
{

public:

  /// Declare the interface
  DeclareInterfaceID(IStandardParticleProvider, 1, 0);

public:

  /** Get a Particle for a given ProtoParticle and PID
   *  @param proto Pointer to the ProtoParticle
   *  @param pid   The PID hypothesis
   *  @return Pointer to the associated standard Particle
   */
  virtual const LHCb::Particle * particle( const LHCb::ProtoParticle * proto,
                                           const LHCb::ParticleID& pid ) const = 0;

};

#endif // DAVINCIKERNEL_IStandardParticleProvider_H
