// $Id: MCExtract.h,v 1.5 2007-04-16 16:16:08 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/11/27 11:58:37  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.3  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.2  2006/02/09 15:42:06  ibelyaev
//  add LoKi::MCAlgs namespace with algorithms
//
// ============================================================================
#ifndef LOKI_EXTRACT_H 
#define LOKI_EXTRACT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi 
// ============================================================================
#include  "LoKi/Objects.h"
#include  "LoKi/MCTypes.h"
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
  /** @namespace LoKi::Extract 
   *  namespace with various "extarction" utilities 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25 
   */
  namespace Extract 
  {    
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin  , 
      MCPARTICLE        end    , 
      OUTPUT            output , 
      const PREDICATE&  cut    )
    {
      for ( ; begin != end ; ++begin ) 
      { output = getMCParticles ( *begin , output , cut ) ; }
      return output ;
    };
    
    /** @fn getMCParticles 
     *  Simple function to extract (recursively) all Monte Carlo 
     *  particles form the given 
     *  Monte Carlo particle
     *  Indeed it is just a flat representation of the whole 
     *  Monte Carlo decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const LHCb::MCParticle* particle  = ... ;
     *
     *  // all daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::getMCParticles
     *   ( particle , std::back_inserter( partciles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::getMCParticles
     *        ( particle                        , 
     *          std::back_inserter( pi0s )      , 
     *          111 == MCID                     ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   , 
      const PREDICATE&        cut      )
    {
      if ( 0 == particle ) { return output ; } // RETURN 
      // fill and advance the output iterator 
      if ( cut ( particle ) ) 
      { 
        // ATTENTION: const_cast is in use! 
        *output  = const_cast<LHCb::MCParticle*>( particle ) ; 
        ++output ;                                              // ADVANCE 
      }
      const SmartRefVector<LHCb::MCVertex>& vs = particle->endVertices();
      return getMCParticles ( vs.begin() , vs.end() , output , cut ) ;
    };
    
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCVertex*   vertex , 
      OUTPUT                  output , 
      const PREDICATE&        cut    )
    {
      if ( 0 == vertex ) { return output ; } // RETURN 
      const SmartRefVector<LHCb::MCParticle>& ps = vertex->products() ;
      return getMCParticles ( ps.begin() , ps.end() , output , cut ) ;
    };
    
    template <class OUTPUT> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   )
    { return getMCParticles ( particle , output , LoKi::Objects::_ALL_ ) ; }
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin  , 
      MCPARTICLE        end    , 
      OUTPUT            output )
    { return getMCParticles ( begin , end , output , LoKi::Objects::_ALL_ ) ; }
    
    /** @fn mcParticles 
     *  Simple function to extract all Monte Carlo particles that 
     *  make contribution to the given particle. 
     *  Indeed it is just a flat representation of the Monet Carlo decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const MCParticle* particle  = ... ;
     *
     *  // all daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::mcParticles
     *   ( particle , std::back_inserter( particles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::mcParticles
     *   ( particle    , 
     *     std::back_inserter( pi0s )      , 
     *     111 == MCID ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT mcParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT            output   , 
      const PREDICATE&  cut      )
    { return getMCParticles ( particle , output , cut ) ; } ;
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  , 
      const PREDICATE&  cut     )
    { return getMCParticles ( begin , end , output , cut ) ; } ;
    
    template <class OUTPUT> 
    inline OUTPUT mcParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT            output   )
    { return getMCParticles ( particle , output ) ; } ;
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  )
    { return getMCparticles ( begin , end , output ) ; } ;

    
    /** @fn getMCChildren 
     *  extratc all children particle form the given MC-particle 
     *  in a form of the flat list 
     *
     *  @code 
     *
     *  const LHCb::MCParticle* B = ... ;
     *  // get all first level daughters:
     *  std::vector<LHCB::MCParticle*> daughters ;
     *
     *  LoKi::Extract::getMCChildren 
     *      ( B , std::back_inserter( daughters ) ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to the particle 
     *  @param output   output iterator 
     *  @return the updated position of the output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT getMCChildren 
    ( const LHCb::MCParticle* particle ,
      OUTPUT                  output   )
    {
      if ( 0 == particle ) { return output ; }             // RETURN 
      typedef SmartRefVector<LHCb::MCVertex>   MCVS ;
      typedef SmartRefVector<LHCb::MCParticle> MCPS ;
      const MCVS& vs = particle->endVertices() ;
      for ( MCVS::const_iterator iv = vs.begin() ; vs.end() != iv ; ++iv )
      {
        const LHCb::MCVertex* v = *iv ;
        if ( 0 == v ) { continue ; }                       // CONTINUE 
        const MCPS& ps = v->products() ;
        for ( MCPS::const_iterator ip = ps.begin() ; ps.end() != ip ; ++ip ) 
        {
          const LHCb::MCParticle* p = *ip ;
          if ( 0 == p ) { continue ; }                     // CONTINUE 
          // ATTENTION: const_cats is in action! 
          *output = const_cast<LHCb::MCParticle*> ( p ) ;
          ++output ;                                       // ADVANCE 
        } 
      }
      return output ;
    } ;

    
    /** @fn mcChildren 
     *  extratc all children particle form the given MC-particle 
     *  in a form of the flat list 
     *
     *  @code 
     *
     *  const LHCb::MCParticle* B = ... ;
     *  // get all first level daughters:
     *  std::vector<LHCB::MCParticle*> daughters ;
     *
     *  LoKi::Extract::mcChildren 
     *      ( B , std::back_inserter( daughters ) ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to the particle 
     *  @param output   output iterator 
     *  @return the updated position of the output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT mcChildren 
    ( const LHCb::MCParticle* particle ,  
      OUTPUT                  output   )
    { return getMCChildren( particle , output ) ; }

  } // end of namespace Extract 
  //
}  // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTRACT_H
// ============================================================================
