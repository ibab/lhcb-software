// $Id: $
#ifndef KERNEL_PARTICLEPREDICATES_H 
#define KERNEL_PARTICLEPREDICATES_H 1

// Include files
#include "Event/Particle.h"

/** @namespace DaVinci::Utils Kernel/ParticlePredicates.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2011-01-10
 */
namespace DaVinci {
  
  namespace Utils {
  
    class IParticlePredicate
    {
    public:
      virtual bool operator()(const LHCb::Particle*) const = 0;
    };

    ///  Functor to check if a Particle is in the TES.
    //  template <class T> 
    struct ParticleInTES : virtual public IParticlePredicate,
                           public std::unary_function<const LHCb::Particle*, bool> 
    {

      inline bool operator() (const LHCb::Particle* obj) const    
      {
        //        std::cout << "DaVinci::Utils::InTES!!!!" << std::endl;
        return ( 0!=obj && 0!=obj->parent()) ;
      }
    
    };

    struct ParticleTrue : virtual public IParticlePredicate,
                          public std::unary_function<const LHCb::Particle*, bool> 
    {
      inline bool operator() (const LHCb::Particle* ) const
      {
        return true;
      }
 
    };

    struct ParticleFalse : virtual public IParticlePredicate,
                           public std::unary_function<const LHCb::Particle*, bool> 
    {
      inline bool operator() (const LHCb::Particle* ) const
      {
        return false;
      }
 
    };

  
  }
  
}

#endif // KERNEL_PARTICLEPREDICATES_H
