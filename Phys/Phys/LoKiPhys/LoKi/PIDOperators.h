// $Id$
// ============================================================================
#ifndef LOKI_PIDOPERATORS_H
#define LOKI_PIDOPERATORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// LHCnKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
// ============================================================================
// forwarde declarations
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  class Particle ;
  class ParticleID ;
  // ==========================================================================
}
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    class    Identifier ;
    class AbsIdentifier ;
    // ========================================================================
    /** @file
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
    // ========================================================================
    
    // ========================================================================
    // ID ==
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const long                              pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const unsigned long                     pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::string&                      pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<long>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   ) ;
    // ========================================================================
    // ABSID ==
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const long                                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   ) ;
    // ========================================================================


    // ========================================================================
    // ID !=
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const long                              pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const unsigned long                     pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::string&                      pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<long>&                pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   ) ;
    // ========================================================================
    // ABSID !=
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const long                                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                   pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LoKi::Particles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   ) ;
    // ========================================================================



    // ========================================================================
    // ID == 
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LHCb::ParticleID&                 pid    ,
                const LoKi::Particles::Identifier&    id     )
    { return id == pid ; }
    
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const long                              pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const unsigned long                     pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::string&                      pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<LHCb::ParticleID>&    pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<int>&                 pid   ,
                const LoKi::Particles::Identifier&   id     )
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned int>&        pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<long>&               pid   ,
                const LoKi::Particles::Identifier&   id     )
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned long>&       pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<std::string>&         pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id == pid ; }

    // ========================================================================
    // ABSID ==
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const LHCb::ParticleID&                    pid    ,
                const LoKi::Particles::AbsIdentifier&    id     )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const long                                 pid   ,
                const LoKi::Particles::AbsIdentifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const unsigned long                        pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::string&                         pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<LHCb::ParticleID>&       pid   , 
                const LoKi::Particles::AbsIdentifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<int>&                    pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned int>&           pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<long>&                   pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned long>&          pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator==( const std::vector<std::string>&            pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    
    
    // ========================================================================
    // ID != 
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LHCb::ParticleID&                 pid    ,
                const LoKi::Particles::Identifier&    id     )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const long                              pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const unsigned long                     pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::string&                      pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<LHCb::ParticleID>&    pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<int>&                 pid   ,
                const LoKi::Particles::Identifier&   id     )
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned int>&        pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<long>&               pid   ,
                const LoKi::Particles::Identifier&   id     )
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned long>&       pid   ,
                const LoKi::Particles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<std::string>&         pid   ,
                const LoKi::Particles::Identifier&    id    ) 
    { return id != pid ; }

    // ========================================================================
    // ABSID !=
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const LHCb::ParticleID&                    pid    ,
                const LoKi::Particles::AbsIdentifier&    id     )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const long                                 pid   ,
                const LoKi::Particles::AbsIdentifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const unsigned long                        pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::string&                         pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<LHCb::ParticleID>&       pid   , 
                const LoKi::Particles::AbsIdentifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<int>&                    pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned int>&           pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<long>&                   pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned long>&          pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
    operator!=( const std::vector<std::string>&            pid   ,
                const LoKi::Particles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================

    // ========================================================================
  } // namespace Particles
  // ==========================================================================
} // namespace LoKi
// ============================================================================


// ============================================================================
// The END
// ============================================================================
#endif // LOKI_PIDOPERATORS_H
// ============================================================================
