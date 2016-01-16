// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// Local
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenExtract.h"
// ============================================================================
/** @file 
 *
 *  implementation of some MC-streamers 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-05-24 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// get particles 
  template <class PARTICLE>
  inline 
  LoKi::GenTypes::GenContainer
  _get_ ( PARTICLE                   first , 
          PARTICLE                   last  , 
          const LoKi::Types::GCuts&  cuts  ) 
  {
    //
    LoKi::GenTypes::GenContainer r ; r.reserve ( 100 ) ;
    //
    LoKi::Extract::genParticles ( first , 
                                  last  , 
                                  std::back_inserter ( r ) , cuts ) ;
    //
    // eliminate duplicates 
    std::set<const HepMC::GenParticle*>  s ( r.begin () , r.end () ) ;
    //
    return LoKi::GenTypes::GenContainer ( s.begin () , s.end () ) ;
  }
  // ==========================================================================
  template <class PARTICLES>
  inline 
  LoKi::GenTypes::GenContainer
  _get_ ( const PARTICLES*           parts , 
          const LoKi::Types::GCuts&  cuts  ) 
  {
    //
    LoKi::GenTypes::GenContainer r ; r.reserve ( 100 ) ;
    //
    LoKi::Extract::genParticles ( parts , std::back_inserter ( r ) , cuts ) ;
    //
    // eliminate duplicates 
    std::set<const HepMC::GenParticle*>  s ( r.begin () , r.end () ) ;
    //
    return LoKi::GenTypes::GenContainer ( s.begin () , s.end () ) ;
  }
  // ==========================================================================
} //                                                  end of anonymos namespace 
// ============================================================================
LoKi::GenTypes::GenContainer 
operator >> 
( const LoKi::GenTypes::GenContainer& input ,  
  const LoKi::Types::GCuts&           cuts  )
{ return _get_ ( input.begin() , input.end () , cuts ) ; }
// =============================================================================
LoKi::GenTypes::GenContainer 
operator >> 
( const LoKi::GenTypes::GRange&       input ,  
  const LoKi::Types::GCuts&           cuts  ) 
{ return _get_ ( input.begin() , input.end () , cuts ) ; }
// ============================================================================
LoKi::GenTypes::GenContainer 
operator >> 
( const LHCb::HepMCEvent*             event ,  
  const LoKi::Types::GCuts&           cuts  ) 
{ return _get_ ( event , cuts ) ; }
// ============================================================================
LoKi::GenTypes::GenContainer 
operator >> 
( const LHCb::HepMCEvent::Container*  event ,  
  const LoKi::Types::GCuts&           cuts  ) 
{ return _get_ ( event , cuts ) ; }
// ============================================================================
LoKi::GenTypes::GenContainer 
operator >> 
( const HepMC::GenEvent*              event ,  
  const LoKi::Types::GCuts&           cuts  ) 
{ return _get_ ( event , cuts ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
