// $Id: MCExtract.h,v 1.9 2008-06-12 08:16:49 ibelyaev Exp $
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
// LoKi 
// ============================================================================
#include  "LoKi/Objects.h"
#include  "LoKi/MCTypes.h"
#include  "LoKi/MCDecayVertex.h"
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
   *  namespace with various "extraction" utilities 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25 
   */
  namespace Extract 
  {    
    // ========================================================================
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin             , 
      MCPARTICLE        end               , 
      OUTPUT            output            , 
      const PREDICATE&  cut               ,
      const bool        decayOnly = false )
    {
      for ( ; begin != end ; ++begin ) 
      { output = getMCParticles ( *begin , output , cut , decayOnly ) ; }
      return output ;
    }
    // ========================================================================
    /** Simple function to extract (recursively) all Monte Carlo 
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
     *  @param cut the predicate 
     *  @param decayOnly flag to indicate the search only within decay products 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle          , 
      OUTPUT                  output            , 
      const PREDICATE&        cut               , 
      const bool              decayOnly = false )
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
      return getMCParticles ( vs.begin() , vs.end() , output , cut , decayOnly ) ;
    }
    // ========================================================================
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCVertex*   vertex            , 
      OUTPUT                  output            , 
      const PREDICATE&        cut               , 
      const bool              decayOnly = false )
    {      
      if ( 0 == vertex ) { return output ; } // RETURN 
      if ( decayOnly && !LoKi::MCVertices::IsDecay ( vertex ) ) { return output ; }
      const SmartRefVector<LHCb::MCParticle>& ps = vertex->products() ;
      return getMCParticles ( ps.begin() , ps.end() , output , cut , decayOnly ) ;
    }
    // ========================================================================
    template <class OUTPUT> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle          , 
      OUTPUT                  output            , 
      const bool              decayOnly = false )
    { return getMCParticles ( particle , output , LoKi::Objects::_ALL_ , decayOnly ) ; }
    // ========================================================================   
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin             , 
      MCPARTICLE        end               , 
      OUTPUT            output            ,
      const bool        decayOnly = false )
    { return getMCParticles ( begin , end , output , LoKi::Objects::_ALL_ , decayOnly ) ; }
    // ======================================================================== 
    /** Simple function to extract all Monte Carlo particles that 
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
    ( const LHCb::MCParticle* particle          , 
      OUTPUT                  output            , 
      const PREDICATE&        cut               ,
      const bool              decayOnly = false )
    { return getMCParticles ( particle , output , cut , decayOnly ) ; } 
    // ========================================================================    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin             , 
      MCPARTICLE        end               , 
      OUTPUT            output            , 
      const PREDICATE&  cut               ,     
      const bool        decayOnly = false )
    { return getMCParticles ( begin , end , output , cut , decayOnly ) ; } 
    // ========================================================================    
    template <class OUTPUT> 
    inline OUTPUT mcParticles 
    ( const LHCb::MCParticle* particle          , 
      OUTPUT                  output            ,
      const bool              decayOnly = false )
    { return getMCParticles ( particle , output , decayOnly ) ; } ;
    // ========================================================================    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin             , 
      MCPARTICLE        end               , 
      OUTPUT            output            ,
      const bool        decayOnly = false )
    { return getMCparticles ( begin , end , output , decayOnly ) ; } ;
    // ========================================================================    
    /** Extract all children particle form the given MC-particle 
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
     *  @param decayOnly search only for "decays"
     *  @return the updated position of the output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT getMCChildren 
    ( const LHCb::MCParticle* particle          ,
      OUTPUT                  output            , 
      const bool              decayOnly = false )
    {
      if ( 0 == particle ) { return output ; }             // RETURN 
      typedef SmartRefVector<LHCb::MCVertex>   MCVS ;
      typedef SmartRefVector<LHCb::MCParticle> MCPS ;
      const MCVS& vs = particle->endVertices() ;
      for ( MCVS::const_iterator iv = vs.begin() ; vs.end() != iv ; ++iv )
      {
        const LHCb::MCVertex* v = *iv ;
        if ( 0 == v ) { continue ; }                       // CONTINUE 
        if ( decayOnly && !LoKi::MCVertices::IsDecay ( v ) ) { continue ; }
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
    } 
    // ========================================================================
    /** Extract all children particle form the given MC-particle 
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
     *  @param decayOnly search only for particles in "decays"
     *  @return the updated position of the output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT mcChildren 
    ( const LHCb::MCParticle* particle          ,  
      OUTPUT                  output            ,
      const bool              decayOnly = false )
    { return getMCChildren ( particle , output , decayOnly ) ; }
    // ========================================================================
  } // end of namespace Extract
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTRACT_H
// ============================================================================
