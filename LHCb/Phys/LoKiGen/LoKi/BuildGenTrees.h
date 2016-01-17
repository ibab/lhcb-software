// $Id$
// ============================================================================
#ifndef LOKI_BUILDGENTREES_H 
#define LOKI_BUILDGENTREES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenParticles.h"
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
 *  @date 2006-01-23 
 */
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::GenTrees 
   *  useful namespace with utility to build HepMC-trees from flat lists 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2005-01-20
   */
  namespace GenTrees 
  {
    // ========================================================================
    /** Simple function which 'builds' the trees from flat list 
     *
     *  @code
     *
     *  typedef ....  INSEQ  ;
     *  typedef ....  OUTSEQ ;
     *  
     *  // input container of (HepMC)particles (flat list) 
     *  INSEQ  input = ... ;
     *  // output container of (HepMC) particles (only independent trees)
     *  OUTSEQ trees ; 
     *
     *  LoKi::GenTrees::buldTrees 
     *    ( input.begin() , input.end() , std::back_inserter ( trees ) ) ;
     *
     *
     *  @endcode 
     *
     *  @param first begin of flat sequence of HepMC::GenParticles 
     *  @param last   end    of flat sequence  of HepMC::GenParticles 
     *  @param output output iterator 
     *  @return iterator for  sequence of trees 
     *  @attention  it could be a bit slow 
     */
    template <class PARTICLE, class OUTPUT>
    inline OUTPUT buildTrees 
    ( PARTICLE first   , 
      PARTICLE last    , 
      OUTPUT   output  ) 
    {
      if ( first == last ) { return output  ; } 
      // helper function 
      LoKi::GenParticles::FromHepMCTree fromDecay( first , last ) ;
      for ( ; first != last ; ++first  )
      { 
        fromDecay.remove              ( *first ) ;  // remove 'self'
        // use the helper function 
        const bool result = fromDecay ( *first ) ;
        fromDecay.add                 ( *first ) ;  // add    'self' 
        if ( result )                 { continue ; }   // CONTINUE
        // it is a head of decay tree! 
        *output = *first  ;
        ++output ;
      }
      return output ;
    } 
    // ========================================================================
  } //                                          end of namespace LoKi::GenTrees
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BUILDGENTREES_H
// ============================================================================
