// $Id$
// ============================================================================
// Include files
// ============================================================================
// Kernel
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/PIDOperators.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    // MCID == 
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const long                              pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const unsigned long                     pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::string&                      pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<long>&                 pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   )
    { return LoKi::Particles::IsID ( pid ) ; }
    // ========================================================================
    // MCABSID ==
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const long                                 pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    // ========================================================================
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                   pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   )
    { return LoKi::Particles::IsAbsID ( pid ) ; }
    // ========================================================================

    // ========================================================================
    // MCID != 
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const long                              pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const unsigned long                     pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::string&                      pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<long>&                pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   )
    { return LoKi::Particles::IsNotID ( pid ) ; }
    // ========================================================================
    // MCABSID !=
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const long                                 pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    // ========================================================================
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                    pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   )
    { return LoKi::Particles::IsNotAbsID ( pid ) ; }
    // ========================================================================
  } // namespace Particles
  // ==========================================================================
} // namespace LoKi
// ============================================================================
// The END
// ============================================================================
