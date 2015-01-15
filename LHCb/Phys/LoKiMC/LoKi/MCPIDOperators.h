// $Id$
// ============================================================================
#ifndef LOKI_MCPIDOPERATORS_H
#define LOKI_MCPIDOPERATORS_H 1
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
  class MCParticle ;
  class ParticleID ;
  // ==========================================================================
}
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace MCParticles
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
    // MCID ==
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const long                              pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const unsigned long                     pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::string&                      pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<long>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   ) ;
    // ========================================================================
    // MCABSID ==
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const long                                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   ) ;
    // ========================================================================


    // ========================================================================
    // MCID !=
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id  */ ,
                const LHCb::ParticleID&                 pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const long                              pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const unsigned long                     pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::string&                      pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<int>&                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<unsigned int>&        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<long>&                pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<unsigned long>&       pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::Identifier& /* id */ ,
                const std::vector<std::string>&         pid   ) ;
    // ========================================================================
    // MCABSID !=
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id  */ ,
                const LHCb::ParticleID&                    pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const long                                 pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const unsigned long                        pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::string&                         pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id  */ ,
                const std::vector<LHCb::ParticleID>&       pid    ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<int>&                    pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned int>&           pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<long>&                   pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<unsigned long>&          pid   ) ;
    // ========================================================================
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LoKi::MCParticles::AbsIdentifier& /* id */ ,
                const std::vector<std::string>&            pid   ) ;
    // ========================================================================



    // ========================================================================
    // MCID == 
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LHCb::ParticleID&                 pid    ,
                const LoKi::MCParticles::Identifier&    id     )
    { return id == pid ; }
    
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const long                              pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const unsigned long                     pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::string&                      pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<LHCb::ParticleID>&    pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<int>&                 pid   ,
                const LoKi::MCParticles::Identifier&   id     )
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned int>&        pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<long>&               pid   ,
                const LoKi::MCParticles::Identifier&   id     )
    { return id == pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned long>&       pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<std::string>&         pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id == pid ; }

    // ========================================================================
    // MCABSID ==
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const LHCb::ParticleID&                    pid    ,
                const LoKi::MCParticles::AbsIdentifier&    id     )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const long                                 pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const unsigned long                        pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::string&                         pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<LHCb::ParticleID>&       pid   , 
                const LoKi::MCParticles::AbsIdentifier&    id    )
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<int>&                    pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned int>&           pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<long>&                   pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<unsigned long>&          pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator==( const std::vector<std::string>&            pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id == pid ; }
    // ========================================================================




    // ========================================================================
    // MCID != 
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LHCb::ParticleID&                 pid    ,
                const LoKi::MCParticles::Identifier&    id     )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const long                              pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const unsigned long                     pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::string&                      pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<LHCb::ParticleID>&    pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<int>&                 pid   ,
                const LoKi::MCParticles::Identifier&   id     )
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned int>&        pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<long>&               pid   ,
                const LoKi::MCParticles::Identifier&   id     )
    { return id != pid ; }      
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned long>&       pid   ,
                const LoKi::MCParticles::Identifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<std::string>&         pid   ,
                const LoKi::MCParticles::Identifier&    id    ) 
    { return id != pid ; }

    // ========================================================================
    // MCABSID !=
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const LHCb::ParticleID&                    pid    ,
                const LoKi::MCParticles::AbsIdentifier&    id     )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const long                                 pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const unsigned long                        pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::string&                         pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<LHCb::ParticleID>&       pid   , 
                const LoKi::MCParticles::AbsIdentifier&    id    )
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<int>&                    pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned int>&           pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<long>&                   pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<unsigned long>&          pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================
    inline 
    LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
    operator!=( const std::vector<std::string>&            pid   ,
                const LoKi::MCParticles::AbsIdentifier&    id    ) 
    { return id != pid ; }
    // ========================================================================



    // ========================================================================    
  } // namespace MCParticles
  // ==========================================================================
} // namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_MCPIDOPERATORS_H
// ============================================================================
