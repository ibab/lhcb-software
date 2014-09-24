// $Id: ParticleFilters.h,v 1.4 2010-06-04 12:35:23 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_PARTICLEFILTERS_H 
#define KERNEL_PARTICLEFILTERS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
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
   *   const LHCb::Particle::Range input = ... ;
   *  
   *   using namespace LoKi::Cuts ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   DaVinci::filter ( input , Q < 0        , negative ) ;
   * 
   *   // filter pions
   *   LHCb::Particle::ConstVector pions ;
   *   DaVinci::filter ( input , "pi+" == ID  , pions    ) ;
   * 
   *  @endcode 
   * 
   *  Also many <code>boost::lambda</code> idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   const LHCb::Particle::Range    input = ... ;
   *
   *   using namespace boost::lambda ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::charge,_1) < 0 , negative ) ;
   *
   *   // filter J/psi 
   *   LHCb::Particle::ConstVector jpsis     ;
   *   const LHCb::ParticleID s_pid ( 443 ) ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::particleID,_1) == s_pid , psis ) ;
   *
   *  @endcode 
   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @return number of added particles    
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
   *   const LHCb::Particle::ConstVector input = ... ;
   *  
   *   using namespace LoKi::Cuts ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   DaVinci::filter ( input , Q < 0        , negative ) ;
   * 
   *   // filter pions
   *   LHCb::Particle::ConstVector pions ;
   *   DaVinci::filter ( input , "pi+" == ID  , pions    ) ;
   * 
   *  @endcode 
   * 
   *  Also many <code>boost::lambda</code> idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   const LHCb::Particle::ConstVector input = ... ;
   *
   *   using namespace boost::lambda ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::charge,_1) < 0 , negative ) ;
   *
   *   // filter J/psi 
   *   LHCb::Particle::ConstVector jpsis     ;
   *   const LHCb::ParticleID s_pid ( 443 ) ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::particleID,_1) == s_pid , psis ) ;
   *
   *  @endcode 
   *
   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @return number of added particles    
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
  /** split the particles into two containers according some criteria
   *
   *  As criteria one can use e.g. any LoKi-functor:
   * 
   *  @code
   *
   *   #include "LoKi/ParticleCuts.h"
   *  
   *   ...
   *
   *   const LHCb::Particle::Range input = ... ;
   *  
   *   using namespace LoKi::Cuts ;
   *
   *   // split into negative & positive 
   *   LHCb::Particle::ConstVector negative ;
   *   LHCb::Particle::ConstVector positive ;
   *   DaVinci::filter ( input , Q < 0        , negative , positive ) ;
   * 
   *   // split into positive pions & others 
   *   LHCb::Particle::ConstVector pions ;
   *   LHCb::Particle::ConstVector other ;
   *   DaVinci::filter ( input , "pi+" == ID  , pions    , other    ) ;
   * 
   *  @endcode 
   * 
   *  Also many <code>boost::lambda</code> idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   const LHCb::Particle::Range    input = ... ;
   *
   *   using namespace boost::lambda ;
   *
   *   // split into negative & positive 
   *   LHCb::Particle::ConstVector negative ;
   *   LHCb::Particle::ConstVector positive ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::charge,_1) < 0 , 
   *                     negative  , positive ) ;
   *
   *   // split into  J/psi and others 
   *   const LHCb::ParticleID s_pid ( 443 ) ;
   *   LHCb::Particle::ConstVector jpsis    ;
   *   LHCb::Particle::ConstVector other    ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::particleID,_1) == s_pid , 
   *                     psis      , other    ) ;
   *
   *  @endcode 
   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of     selected particles 
   *  @param output2   (OUPUT) the container of non-selected particles 
   *  @return number of added particles    
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2010-05-14
   */
  template <class PREDICATE> 
  std::size_t filter 
  ( const LHCb::Particle::Range& input     , 
    const PREDICATE&             criterion , 
    LHCb::Particle::ConstVector& output    ,
    LHCb::Particle::ConstVector& output2   ) 
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
      else { output2.push_back ( *ip ) ; }
      
    }
    return added ;
  }
  // ==========================================================================
  /** split the particles into two containers according some criteria
   *
   *  As criteria one can use e.g. any LoKi-functor:
   * 
   *  @code
   *
   *   #include "LoKi/ParticleCuts.h"
   *  
   *   ...
   *
   *   const LHCb::Particle::ConstVector input = ... ;
   *  
   *   using namespace LoKi::Cuts ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   LHCb::Particle::ConstVector positive ;
   *   DaVinci::filter ( input , Q < 0        , negative , positive ) ;
   * 
   *   // filter pions
   *   LHCb::Particle::ConstVector pions ;
   *   LHCb::Particle::ConstVector other ;
   *   DaVinci::filter ( input , "pi+" == ID  , pions    , other    ) ;
   * 
   *  @endcode 
   * 
   *  Also many <code>boost::lambda</code> idioms can be used:
   *  
   *  @code
   *
   *   #include <boost/lambda/lambda.hpp>
   *   #include <boost/lambda/bind.hpp>
   *  
   *   ...
   *
   *   const LHCb::Particle::ConstVector input = ... ;
   *
   *   using namespace boost::lambda ;
   *
   *   // filter negative 
   *   LHCb::Particle::ConstVector negative ;
   *   LHCb::Particle::ConstVector positive ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::charge,_1) < 0 , 
   *                     negative  , positive ) ;
   *
   *   // filter J/psi 
   *   const LHCb::ParticleID s_pid ( 443 ) ;
   *   LHCb::Particle::ConstVector jpsis    ;
   *   LHCb::Particle::ConstVector other    ;
   *   DaVinci::filter ( input , 
   *                     bind(&LHCb::Particle::particleID,_1) == s_pid , 
   *                     psis      , other    ) ;
   *
   *  @endcode 
   *
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of     selected particles 
   *  @param output2   (OUPUT) the container of non-selected particles 
   *  @return number of added particles    
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2010-05-14
   */
  template <class PREDICATE> 
  std::size_t filter 
  ( const LHCb::Particle::ConstVector& input     , 
    const PREDICATE&                   criterion , 
    LHCb::Particle::ConstVector&       output    ,
    LHCb::Particle::ConstVector&       output2   ) 
  {
    return filter 
      ( LHCb::Particle::Range ( input ) , criterion , output , output2 ) ;
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
  GAUDI_API 
  std::size_t filter 
  ( const LHCb::Particle::Range& input     , 
    const IParticleFilter*       criterion , 
    LHCb::Particle::ConstVector& output    ) ;
  // ==========================================================================
  /** filter the particles according to some criteria
   * 
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @return number of added particles    
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  GAUDI_API 
  std::size_t filter 
  ( const LHCb::Particle::ConstVector& input     , 
    const IParticleFilter*             criterion , 
    LHCb::Particle::ConstVector&       output    ) ;
  // ==========================================================================
  /** split the particles according to some criteria
   * 
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of     selected particles 
   *  @param output2   (OUPUT) the container of non-selected particles 
   *  @return number of added particles    
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  GAUDI_API 
  std::size_t filter 
  ( const LHCb::Particle::Range& input     , 
    const IParticleFilter*       criterion , 
    LHCb::Particle::ConstVector& output    ,
    LHCb::Particle::ConstVector& output2   ) ;
  // ==========================================================================
  /** split the particles according to some criteria
   * 
   *  @param input     (INPUT) the container of input particles 
   *  @param criterion (INPUT) the selection criteria 
   *  @param output    (OUPUT) the container of selected particles 
   *  @param output2   (OUPUT) the container of non-selected particles 
   *  @return number of added particles    
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  GAUDI_API 
  std::size_t filter 
  ( const LHCb::Particle::ConstVector& input     , 
    const IParticleFilter*             criterion , 
    LHCb::Particle::ConstVector&       output    ,
    LHCb::Particle::ConstVector&       output2   ) ;
  // ==========================================================================
} //                                                   end of namespace DaVinci 
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_PARTICLEFILTERS_H
// ============================================================================
