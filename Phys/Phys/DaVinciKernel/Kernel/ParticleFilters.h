// $Id: ParticleFilters.h,v 1.3 2010-05-27 15:29:10 jpalac Exp $
// ============================================================================
#ifndef KERNEL_PARTICLEFILTERS_H 
#define KERNEL_PARTICLEFILTERS_H 1
// ============================================================================
// Include files
// ============================================================================
// PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
namespace DaVinci 
{
  // ==========================================================================
  /** filter the particles according to some criteria
   *
   *  As criteria one can use e.g. any LoKi-functor:
   * 
   *  @code
   *
   *   #include "LoKi/ParticleCuts.h"
   *  
   *   ...
   *
   *   LHCb::Particle::Range       input = ... ;
   *   LHCb::Particle::ConstVector negative ;
   *  
   *   using namespace LoKi::Cuts ;
   *   DaVinci::filter ( input , Q < 0 , negative ) ;
   * 
   *  @endcode 
   * 
   *  Also many boost::lambda idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   LHCb::Particle::Range       input = ... ;
   *   LHCb::Particle::ConstVector negative ;
   *  
   *   using namespace boost::lambda ;
   *   DaVinci::filter ( input , bind(&LHCb::Particle::charge,_1) < 0 , negative ) ;
   * 
   *  @endcode 
   *
   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @reutrn number of added particles    
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2010-05-14
   */
  template <class PREDICATE> 
  std::size_t filter 
  ( const LHCb::Particle::Range& input     , 
    const PREDICATE&             criterion , 
    LHCb::Particle::ConstVector& output    ) 
  {
    std::size_t added = 0 ;
    for ( LHCb::Particle::ConstVector::const_iterator ip = input.begin() ; 
          input.end ()  != ip ; ++ip ) 
    {
      if ( criterion (*ip) ) 
      {
        output.push_back ( *ip ) ;
        ++added ;
      }
    }
    return added ;
  }
  // ==========================================================================
  /** filter the particles according to some criteria
   *
   *  As criteria one can use e.g. any LoKi-functor:
   * 
   *  @code
   *
   *   #include "LoKi/ParticleCuts.h"
   *  
   *   ...
   *
   *   LHCb::Particle::ConstVector input = ... ;
   *   LHCb::Particle::ConstVector negative ;
   *  
   *   using namespace LoKi::Cuts ;
   *   DaVinci::filter ( input , Q < 0 , negative ) ;
   * 
   *  @endcode 
   * 
   *  Also many boost::lambda idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   LHCb::Particle::ConstVector input = ... ;
   *   LHCb::Particle::ConstVector negative ;
   *  
   *   using namespace boost::lambda ;
   *   DaVinci::filter ( input , bind(&LHCb::Particle::charge,_1) < 0 , negative ) ;
   * 
   *  @endcode 
   *
   *   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @reutrn number of added particles    
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2010-05-14
   */
  template <class PREDICATE> 
  std::size_t filter 
  ( const LHCb::Particle::ConstVector& input     , 
    const PREDICATE&                   criterion , 
    LHCb::Particle::ConstVector&       output    ) 
  {
    return filter 
      ( LHCb::Particle::Range ( input ) , criterion , output ) ;
  }
  // ==========================================================================
  /** filter the particles according to some criteria
   * 
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @reutrn number of added particles    
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  std::size_t filter 
  ( const LHCb::Particle::Range& input     , 
    const IParticleFilter*      criterion , 
    LHCb::Particle::ConstVector& output    ) ;
  // ==========================================================================
  /** filter the particles according to some criteria
   * 
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @reutrn number of added particles    
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  std::size_t filter 
  ( const LHCb::Particle::ConstVector& input     , 
    const IParticleFilter*             criterion , 
    LHCb::Particle::ConstVector&       output    ) ;
  // ==========================================================================
} //                                                   end of namespace DaVinci 
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_PARTICLEFILTERS_H
// ============================================================================
