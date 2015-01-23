// $Id$
// ============================================================================
// Include files
// ============================================================================
// ST D& STL
// ============================================================================
#include <set>
// ============================================================================
// Local
// ============================================================================
#include "LoKi/GenParticles.h"
#include "LoKi/GenPIDOperators.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from the file LoKi/GenPIDOperators.h
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
  namespace GenParticles
  {
    // ========================================================================

    // ========================================================================
    // MCID == 
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const long                              pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const unsigned long                     pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::string&                      pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<long>&                 pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   )
    { return LoKi::GenParticles::IsID ( pid ) ; }
    // ========================================================================

    // ========================================================================
    // MCABSID ==
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const long                                 pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    // ========================================================================
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                   pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator==( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   )
    { return LoKi::GenParticles::IsAbsID ( pid ) ; }
    // ========================================================================

    // ========================================================================
    // MCID != 
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const long                              pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const unsigned long                     pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::string&                      pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<long>&                pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   )
    { return LoKi::GenParticles::IsNotID ( pid ) ; }
    // ========================================================================
    // ========================================================================
    // MCABSID !=
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const long                                 pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    // ========================================================================
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                    pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
    operator!=( const LoKi::GenParticles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   )
    { return LoKi::GenParticles::IsNotAbsID ( pid ) ; }
    // ========================================================================
 
    // ========================================================================
  } // namespace GenParticles
  // ==========================================================================
} // namespace LoKi
// ============================================================================
// The END
// ============================================================================
