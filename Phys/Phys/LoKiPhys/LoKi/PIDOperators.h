// $Id: PIDOperators.h,v 1.4 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 12:01:31  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/10/28 10:31:27  ibelyaev
//  fix for WIn32 platform
//
// Revision 1.1  2006/04/23 10:06:13  ibelyaev
//   add operators for ID and ABSID
//
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
  class Particle ;
  class ParticleID ;
} 
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    class    Identifier ;
    class AbsIdentifier ;    
  }
}
// ============================================================================


// ============================================================================
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
// ============================================================================


LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   ) ;

// ============================================================================

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const long                         pid  ,
            const LoKi::Particles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }

inline
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const long                         pid  ,
            const LoKi::Particles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::Identifier& id   , 
            const long                         pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::Identifier& id   , 
            const long                         pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }

// ============================================================================

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   ) ;

// ============================================================================

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) ;

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) ;

// ============================================================================

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const long                            pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }

inline
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const long                            pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const long                            pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }

inline 
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const long                            pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }

// ============================================================================

LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   ) ;
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   ) ;


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PIDOPERATORS_H
// ============================================================================
