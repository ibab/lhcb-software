// $Id: BuildMCTrees.h,v 1.5 2007-04-16 16:16:08 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $,  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/11/29 14:27:48  ibelyaev
//  add BuildMCTrees
//
// Revision 1.3  2006/11/27 11:58:37  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 16:13:39  ibelyaev
// New Packaage: MC-dependent part of LoKi project 
//
// ============================================================================
#ifndef LOKI_BUILDMCTREES_H 
#define LOKI_BUILDMCTREES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCParticles.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
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

namespace LoKi 
{
  /** @namespace LoKi::MCTrees 
   *  useful namespace with utility to build MC-trees from flat lists 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2005-01-20
   */
  namespace MCTrees 
  {
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 
     *
     *  @code
     *
     *  typedef ....  INSEQ  ;
     *  typedef ....  OUTSEQ ;
     *  
     *  // input container of (MC)particles (flat list) 
     *  INSEQ  input = ... ;
     *  // output container of (MC) particles (only independent trees)
     *  OUTSEQ trees ; 
     *
     *  LoKi::MCTrees::buldTrees 
     *    ( input.begin() , input.end() , std::back_inserter ( trees ) ) ;
     *
     *
     *  @endcode 
     *
     *  @param first begin of flat sequence of MCParticles 
     *  @param last   end    of flat sequence  of MCParticles 
     *  @param output output iterator 
     *  @return iterator for  sequence of trees 
     */
    template <class MCPARTICLE, class OUTPUT>
    inline OUTPUT buildTrees 
    ( MCPARTICLE first   , 
      MCPARTICLE last    , 
      OUTPUT     output  ) 
    {
      if ( first == last ) { return output  ; } 
      // helper function 
      LoKi::MCParticles::FromMCDecayTree fromDecay( first , last ) ;
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
    };
    
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  typedef ....  SEQUENCE  ;
     *  typedef ....  OUTSEQ    ;
     *  
     *  // Container of (MC)particles (flat list) 
     *  SEQUENCE particles  = ... ;
     *
     *  SEQUENCE::iterator it  = 
     *    LoKi::MCTrees::buldTrees ( input.begin() , input.end() ) ;
     *
     *  // from here the sequence [particles.begin(),it) contains 
     *  // only independent tress 
     *
     *  @endcode 
     *
     *  @param first begin of flat sequence of MCParticles 
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return iterator for  sequence of trees 
     */
    template <class MCPARTICLE>
    inline MCPARTICLE buildTrees
    ( MCPARTICLE first   , 
      MCPARTICLE last    ) 
    {
      MCPARTICLE output = first ;
      if ( first == last ) { return output  ; } 
      // helper function 
      LoKi::MCParticles::FromMCDecayTree fromDecay( first , last ) ;
      for ( ; first != last ; ++first  )
      { 
        fromDecay.remove              ( *first ) ;  // remove 'self'
        // use the helper function 
        const bool result = fromDecay ( *first ) ;  // evaluate the cut 
        fromDecay.add                 ( *first ) ;  // add    'self' 
        if ( result )                 { continue ; }   // CONTINUE
        // it is a head of decay tree! 
        std::iter_swap ( output , first ) ;
        ++output ;
      }
      return output ;
    };
    
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LHCb::MCParticle::Vector input = ... ;
     *
     *  LHCb::MCParticle::Vector output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LHCb::MCParticle::Vector
    buildTrees 
    ( const LHCb::MCParticle::Vector& input ) ;

    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LHCb::MCParticle::ConstVector input = ... ;
     *
     *  LHCb::MCParticle::ConstVector output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LHCb::MCParticle::ConstVector
    buildTrees 
    ( const LHCb::MCParticle::ConstVector& input ) ;
    
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LHCb::MCParticle::Container* input = ... ;
     *
     *  LHCb::MCParticle::ConstVector output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LHCb::MCParticle::ConstVector
    buildTrees 
    ( const LHCb::MCParticle::Container* input ) ;
    
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LHCb::MCParticle::Container* input = ... ;
     *
     *  LHCb::MCParticle::ConstVector output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LHCb::MCParticle::ConstVector
    buildTrees 
    ( const LoKi::Types::MCRange& input ) ;
    
    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LoKi:Keeper<LHCb::MCParticle>  input = ... ;
     *
     *  LoKi::Keeper<LHCb::MCParticle> output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LoKi::Keeper<LHCb::MCParticle>
    buildTrees 
    ( const LoKi::Keeper<LHCb::MCParticle>& input ) ;


    /** @fn buildTrees
     *  simple function which 'builds' the trees from flat list 'in place' 
     *
     *  @code
     *
     *  LoKi::UniqueKeeper<LHCb::MCParticle>  input = ... ;
     *
     *  LoKi::UniqueKeeper<LHCb::MCParticle> output = 
     *    LoKi::MCTrees::buldTrees ( input ) ;
     *
     *  @endcode 
     *
     *  @param input input conmtainer of MCParticles  
     *  @param end   end   of flat sequence  of MCParticles 
     *  @return container of MCtrees  
     */
    LoKi::UniqueKeeper<LHCb::MCParticle>
    buildTrees 
    ( const LoKi::UniqueKeeper<LHCb::MCParticle>& input ) ;


  }  // end of namespace LoKi::MCTrees 

} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BUILDMCTREES_H
// ============================================================================
