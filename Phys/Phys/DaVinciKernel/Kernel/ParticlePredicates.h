// $Id: $
#ifndef KERNEL_PARTICLEPREDICATES_H
#define KERNEL_PARTICLEPREDICATES_H 1

// Include files
#include "Event/Particle.h"

/** @namespace DaVinci Kernel/ParticlePredicates.h
 *
 *
 *  @author Juan Palacios
 *  @date   2011-01-10
 */
namespace DaVinci
{

  /** @namespace Utils Kernel/ParticlePredicates.h
   *
   *
   *  @author Juan Palacios
   *  @date   2011-01-10
   */
  namespace Utils
  {

    class IParticlePredicate
    {
    public:
      virtual ~IParticlePredicate() { }
    public:
      virtual bool operator() ( const LHCb::Particle* obj ) const = 0;

    };

    ///  Functor to check if a Particle is in the TES.
    class ParticleInTES : virtual public IParticlePredicate,
                          public std::unary_function<const LHCb::Particle*, bool>
    {
    public:
      virtual ~ParticleInTES() { }
    public:
      inline bool operator() ( const LHCb::Particle* obj ) const
      {
        return ( obj && obj->parent() );
      }
    };

    class ParticleTrue : virtual public IParticlePredicate,
                         public std::unary_function<const LHCb::Particle*, bool>
    {
    public:
      virtual ~ParticleTrue() { }
    public:
      inline bool operator() ( const LHCb::Particle* ) const
      {
        return true;
      }
    };

    class ParticleFalse : virtual public IParticlePredicate,
                          public std::unary_function<const LHCb::Particle*, bool>
    {
    public:
      virtual ~ParticleFalse() { }
    public:
      inline bool operator() ( const LHCb::Particle* ) const
      {
        return false;
      }
    };

  }

}

#endif // KERNEL_PARTICLEPREDICATES_H
