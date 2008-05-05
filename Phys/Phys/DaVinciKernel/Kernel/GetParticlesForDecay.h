// $Id: GetParticlesForDecay.h,v 1.1 2008-05-05 11:46:04 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_GETPARTICLESFORDECAY_H 
#define DAVINCIKERNEL_GETPARTICLESFORDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
namespace DaVinci  
{
  namespace Utils
  {
    // ========================================================================
    /** simple function for filtering (by PID) the particles, which 
     *  matched the PID
     *  
     *  @code
     *  
     *   const LHCb::Particle::ConstVector input = ... ;
     *   const LHCb::Decay& decay = ... ;
     *
     *   LHCb::Particle::ConstVector result ;
     * 
     *   // use the function:
     *
     *   DaVinci::Utils::getParticles 
     *            ( input . begin () , 
     *              input . end   () , 
     *              decay ( 1 )      , // get the first component 
     *              std::back_inserter ( result ) ) ;
     *
     *  @endcode 
     *
     *  It is also applicable to Monte-Carlo data:
     *
     *  @code
     *  
     *   const LHCb::MCParticle::ConstVector input = ... ;
     *   const LHCb::Decay& decay = ... ;
     *
     *   LHCb::MCParticle::ConstVector result ;
     * 
     *   // use the function:
     *
     *   DaVinci::Utils::getParticles 
     *            ( input . begin () , 
     *              input . end   () , 
     *              decay ( 1 )      , // get the first component 
     *              std::back_inserter ( result ) ) ;
     *
     *  @endcode 
     *
     *  @param begin the begin iterator for the input sequence  
     *  @param end   the end   iterator for the input sequence  
     *  @param item  the decay component 
     *  @param result the output iterator 
     *  @param return the updated output iterator 
     *
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-03-31
     */
    template <class INPUT , class OUTPUT>
    inline 
    OUTPUT 
    getParticles 
    ( INPUT                       begin  , 
      INPUT                       end    ,
      const LHCb::Decay::Item&    item   , 
      OUTPUT                      result ) 
    {
      for ( ; begin != end ; ++begin ) 
      {
        if ( ! (*begin)        ) { continue ; }   // CONTINUE 
        const LHCb::ParticleID& pid = (*begin)->particleID() ;
        if ( pid != item.pid() ) { continue ; }   // CONTINUE 
        *result = *begin ;
        ++result ;
      }
      return result ;
    }
    // ========================================================================
  } // end of namespace DaVinci::Utils
} // end of namespace DaVinci
// ============================================================================
// The END
// ============================================================================
#endif // DAVINCIKERNEL_GETPARTICLESDECAY_H
// ============================================================================
