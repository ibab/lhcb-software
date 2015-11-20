// $Id$
// ============================================================================
#ifndef LOKI_MCALGS_H 
#define LOKI_MCALGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCDecayVertex.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the functions from LoKi::Extract namespace 
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
 *  @date 2006-02-08 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace MCAlgs 
  { 
    // ========================================================================
    /*  the trivial algorithm to count the number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @param   decayOnly flag to indicate the search only for decay products 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline size_t  count_if
    ( OBJECT           first      ,
      OBJECT           last       , 
      const PREDICATE& predicate  , 
      const bool       decayOnly  ) ;
    // ========================================================================
    /*  the trivial algorithm to find the MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @param   decayOnly flag to indicate the search only for decay products 
     *  @return  true of tehre exist at least one particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline bool found
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate , 
      const bool       decayOnly ) ;
    // ========================================================================
    /*  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param first begin of the sequence 
     *  @param last  end    of the sequence 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param minval   minimal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE, class RESULT>
    inline RESULT                    min_value 
    ( OBJECT                         first     ,
      OBJECT                         last      , 
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         minval    , 
      const bool                     decayOnly ) ;
    // ========================================================================
    /*  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  maximal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param first begin of the sequence 
     *  @param last  end    of the sequence 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param maxval   maximal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT max_value 
    ( OBJECT                         first     ,
      OBJECT                         last      , 
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         maxval    , 
      const bool                     decayOnly ) ;
    // ========================================================================
    /*  The trivial algorithm which scans the decay tree   
     *  and accumulates the value for some function for 
     *  the MC particles which satisfies the certain criteria 
     *  @param first  begin-iterator for sequence of particles
     *  @param last   end-iterator for sequence of particles
     *  @param fun    functionto be accumulated 
     *  @param cut    the criteria
     *  @param result accumulation result 
     *  @param binop  binary operations used for accumulation 
     *  @return updated accumulation result  
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE,
              class OPERATION,class RESULT>
    inline RESULT accumulate 
    ( OBJECT                         first     , 
      OBJECT                         last      ,
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         result    ,
      OPERATION                      binop     , 
      const bool                     decayOnly ) ;
    // ========================================================================
    /** the trivial algorithm to count number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  Count number of muons in the decay tree of the B:
     *
     *  @code 
     *
     *  const LHCb::MCVertex* vx = .. ;
     *
     *  const size_n muons = 
     *     LoKi::MCAlgs::count_if ( vx , "mu+" == MCABSID ) ;
     * 
     *  @endcode 
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Cuts::MCABSID 
     *
     *  @param   vertex pointer to MC vertex 
     *  @param   predicate criteria 
     *  @param   decayOnly search only for particles from decays
     *  @return  number of particles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline size_t  count_if 
    ( const LHCb::MCVertex* vertex            , 
      const PREDICATE&      predicate         , 
      const bool            decayOnly = false )
    {
      // 
      if ( 0 == vertex ) { return 0 ; }                       // RETURN 
      if ( decayOnly && !LoKi::MCVertices::IsDecay( vertex ) ) { return 0 ; }
      //
      typedef SmartRefVector<LHCb::MCParticle> Products ;
      const Products& products = vertex->products() ;
      //
      return LoKi::MCAlgs::count_if 
        ( products.begin() , products.end() , predicate , decayOnly ) ;
    }
    // ========================================================================
    /** the trivial algorithm to count number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @code 
     *
     *  const LHCb::MCParticle* B = .. ;
     *
     *  const size_n muons = 
     *     LoKi::MCAlgs::count_if( B , "mu+" == MCABSID ) ;
     * 
     *  @endcode 
     *
     *  @see LHCb::MCParticle
     *  @see LoKi::Cuts::MCABSID 
     *
     *  @param   particle pointer to MC particle  
     *  @param   predicate criteria 
     *  @param   decayOnly search only for decay products 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline size_t  count_if 
    ( const LHCb::MCParticle* particle          , 
      const PREDICATE&        predicate         ,
      const bool              decayOnly = false )
    {
      if ( 0 == particle ) { return 0 ; }
      // evaluate the predicate! 
      size_t result = predicate( particle ) ? 1 : 0 ;
      //
      typedef SmartRefVector<LHCb::MCVertex> EndVertices;
      const EndVertices& vertices = particle->endVertices() ;
      //
      return result + LoKi::MCAlgs::count_if 
        ( vertices.begin() , vertices.end() , predicate , decayOnly ) ;
    }     
    // ========================================================================
    /** trivial algorithm which returns true if 
     *  the exist at least one particle in the tree which satisfy 
     *  the certain criteria
     *
     *  Check the presence of high-PT muon in the 
     *   decay tree of the particle:
     *
     *  @code 
     *  
     *  const LHCb::MCParticle* B = ... ;
     *  
     *  MCCut lepton = "mu+" == MCABSID && PT > 2 * GeV ;
     * 
     *  const bool good = found ( B , lepton ) ;
     * 
     *  @endcode 
     *
     *  @see LoKi::Types::MCCut
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCPT 
     *
     *  @param   particle pointer to MC particle  
     *  @param   predicate criteria 
     *  @param   decayOnly search only within decay products 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline bool found  
    ( const LHCb::MCParticle* particle          , 
      const PREDICATE&        predicate         , 
      const bool              decayOnly = false )
    {
      if ( 0 == particle         ) { return false ; }   // RETURN 
      // evaluate the predicate! 
      if ( predicate( particle ) ) { return true  ; }   // RETURN 
      //
      typedef SmartRefVector<LHCb::MCVertex> EndVertices;
      const EndVertices& vertices = particle->endVertices() ;
      //
      return LoKi::MCAlgs::found 
        ( vertices.begin() , vertices.end() , predicate , decayOnly ) ;
    }
    // ========================================================================
    /** trivial algorithm which returns true if 
     *  the exist at least one particle in the tree which satisfy 
     *  the certain criteria
     *
     *  @code 
     *  
     *  const LHCb::MCVertex* vx = ... ;
     *  
     *  MCCut lepton = "mu+" == MCABSID && PT > 2 * GeV ;
     * 
     *  const bool good = found ( vx , lepton ) ;
     * 
     *  @endcode 
     *
     *  @see LoKi::Types::MCCut
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCPT 
     *  
     *  @param   vertex  pointer to MC vertex 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline bool found  
    ( const LHCb::MCVertex*   vertex            , 
      const PREDICATE&        predicate         ,
      const bool              decayOnly = false )
    {
      if ( decayOnly && !LoKi::MCVertices::IsDecay( vertex ) ) { return false ; }
      if ( 0 == vertex           ) { return false ; }   // RETURN 
      //
      typedef SmartRefVector<LHCb::MCParticle> Products ;
      const Products& products = vertex->products() ;
      //
      return LoKi::MCAlgs::found 
        ( products.begin() , products.end() , predicate , decayOnly ) ;
    }
    // ========================================================================
    /** the trivial algorithm to count the number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline size_t  count_if
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate , 
      const bool       decayOnly )
    {
      size_t result = 0 ;
      for ( ; first != last ; ++first ) 
      { result += LoKi::MCAlgs::count_if ( *first , predicate , decayOnly ) ; }
      //
      return result ;
    }
    // ========================================================================
    /** the trivial algorithm to find the MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @param   decayOnly search only within decay products 
     *  @return  true of tehre exist at least one particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline bool found
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate , 
      const bool       decayOnly ) 
    {
      for ( ; first != last ; ++first ) 
      { 
        if ( LoKi::MCAlgs::found ( *first , predicate , decayOnly ) ) 
        { return true ; } // RETURN 
      }
      return false ;
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param particle the particle  
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param minval   minimal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT min_value 
    ( const LHCb::MCParticle*        particle          ,
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         minval            , 
      const bool                     decayOnly = false ) 
    { 
      // (1) traverse the tree if possible 
      if ( 0 != particle ) 
      {        
        // check all end-vertices :
        minval = LoKi::MCAlgs::min_value 
          ( particle -> endVertices() . begin () ,
            particle -> endVertices() . end   () , 
            fun , cut , minval , decayOnly ) ;            // RECURSION!
      }
      // (2) check itself 
      if ( cut ( particle ) ) 
      { minval = std::min ( minval , fun ( particle ) ) ; }
      //
      return minval ;                                        // RETURN 
    } 
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param vertex   the vertex 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param minval   minimal value 
     *  @param  decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT min_value 
    ( const LHCb::MCVertex*          vertex            ,
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         minval            ,
      const bool                     decayOnly = false ) 
    { 
      /// check for "only" decay condition:
      if ( decayOnly && !LoKi::MCVertices::IsDecay( vertex ) ) { return minval ; }
      // nothing to do...
      if ( 0 == vertex ) { return minval ; }               // RETURN ;
      // check for all products:
      minval = LoKi::MCAlgs::min_value 
        ( vertex -> products().begin () , 
          vertex -> products().end   () ,
          fun , cut , minval , decayOnly ) ;                 // RECURSION!
      //
      return minval ;                                        // RETURN 
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param first begin of the sequence 
     *  @param last  end    of the sequence 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param minval   minimal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT min_value 
    ( OBJECT                         first     ,
      OBJECT                         last      , 
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         minval    , 
      const bool                     decayOnly ) 
    {
      for ( ; first != last ; ++first ) 
      {
        minval = LoKi::MCAlgs::min_value 
          ( *first , fun , cut , minval , decayOnly ) ;
      }
      return minval ;
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  maximal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param particle the particle  
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param maxval   maximal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated maximal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT max_value 
    ( const LHCb::MCParticle*        particle          ,
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         maxval            , 
      const bool                     decayOnly = false ) 
    { 
      // (1) traverse the tree if possible 
      if ( 0 != particle ) 
      {        
        // check all end-vertices :
        maxval = LoKi::MCAlgs::max_value 
          ( particle -> endVertices() . begin () ,
            particle -> endVertices() . end   () ,
            fun , cut , maxval , decayOnly ) ;            // RECURSION!
      }
      // (2) check itself 
      if ( cut ( particle ) ) 
      { maxval = std::max ( maxval , fun ( particle ) ) ; }
      //
      return maxval ;                                        // RETURN 
    } 
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  maximal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param vertex   the vertex 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param minval   maximal value 
     *  @param  decayOnly flag to indicate seach through the decays products only
     *  @return updated maximal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT max_value 
    ( const LHCb::MCVertex*          vertex            ,
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         maxval            ,
      const bool                     decayOnly = false ) 
    { 
      /// check for "only" decay condition:
      if ( decayOnly && !LoKi::MCVertices::IsDecay( vertex ) ) { return maxval ; }
      // nothing to do...
      if ( 0 == vertex ) { return maxval ; }               // RETURN ;
      // check for all products:
      maxval = LoKi::MCAlgs::max_value 
        ( vertex -> products().begin () , 
          vertex -> products().end   () ,
          fun , cut , maxval , decayOnly ) ;                 // RECURSION!
      //
      return maxval ;                                        // RETURN 
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  maximal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param first begin of the sequence 
     *  @param last  end    of the sequence 
     *  @param fun      function to be evaluated 
     *  @param cut      the criteria
     *  @param maxval   maximal value 
     *  @param decayOnly flag to indicate seach through the decays products only
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT max_value 
    ( OBJECT                         first     ,
      OBJECT                         last      , 
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         maxval    , 
      const bool                     decayOnly ) 
    {
      for ( ; first != last ; ++first ) 
      {
        maxval = LoKi::MCAlgs::max_value 
          ( *first , fun , cut , maxval , decayOnly ) ;
      }
      return maxval ;
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay tree   
     *  and accumulates the value for some function for 
     *  the MC particles which satisfies the certain criteria 
     *  @param particle pointer  to the particle 
     *  @param fun      function to be accumulated 
     *  @param cut      the criteria
     *  @param result   accumulation result 
     *  @param binop  binary operations used for accumulation 
     *  @param decayOnly the flag to indicat search only for decays 
     *  @return updated accumulation result  
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class FUNCTION,class PREDICATE,class OPERATION,class RESULT>
    inline RESULT accumulate 
    ( const LHCb::MCParticle*        particle          , 
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         result            ,
      OPERATION                      binop             , 
      const bool                     decayOnly = false ) 
    {
      if ( 0 != particle ) 
      {
        result = LoKi::MCAlgs::accumulate 
          ( particle -> endVertices().begin() , 
            particle -> endVertices().end() , 
            fun , cut , result , binop , decayOnly ) ;              // RECURSION
      }
      // (2) check itself 
      if ( cut ( particle ) ) 
      { result = binop ( result  , fun ( particle ) ) ; }            // INCREMENT
      // 
      return result ;                                                // RETURN 
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay tree   
     *  and accumulates the value for some function for 
     *  the MC particles which satisfies the certain criteria 
     *  @param vertex pointer to the vertex 
     *  @param fun    functionto be accumulated 
     *  @param cut    the criteria
     *  @param result accumulation result 
     *  @param binop  binary operations used for accumulation 
     *  @param decayOnly the flag to indicate search only for decays 
     *  @return updated accumulation result  
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class FUNCTION,class PREDICATE,class OPERATION,class RESULT>
    inline RESULT accumulate 
    ( const LHCb::MCVertex*          vertex            , 
      const FUNCTION&                fun               ,
      const PREDICATE&               cut               , 
      RESULT                         result            ,
      OPERATION                      binop             , 
      const bool                     decayOnly = false ) 
    {
      /// check for "only" decay condition:
      if ( decayOnly && !LoKi::MCVertices::IsDecay( vertex ) ) { return result ; }
      // nothing to do...
      if ( 0 == vertex ) { return result ; }               // RETURN ;
      // check for all products:
      result = LoKi::MCAlgs::accumulate 
        ( vertex -> products() . begin () ,
          vertex -> products() . end   () ,
          fun , cut , result , binop , decayOnly ) ;       // RECURSION 
      //
      return result ;                                      // RETURN 
    }
    // ========================================================================
    /** The trivial algorithm which scans the decay tree   
     *  and accumulates the value for some function for 
     *  the MC particles which satisfies the certain criteria 
     *  @param p      pointer to the particle 
     *  @param fun    functionto be accumulated 
     *  @param cut    the criteria
     *  @param result accumulation result 
     *  @param binop  binary operations used for accumulation 
     *  @return updated accumulation result  
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class OBJECT, class FUNCTION,class PREDICATE,
              class OPERATION,class RESULT>
    inline RESULT accumulate 
    ( OBJECT                         first     , 
      OBJECT                         last      ,
      const FUNCTION&                fun       ,
      const PREDICATE&               cut       , 
      RESULT                         result    ,
      OPERATION                      binop     , 
      const bool                     decayOnly ) 
    {
      for ( ; first != last ; ++first ) 
      {
        result = LoKi::MCAlgs::accumulate 
          ( *first , fun , cut , result , binop , decayOnly ) ;
      }
      return result ;    
    }
    // ========================================================================
    /// search for minimal element in the tree 
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* min_element 
    ( const LHCb::MCParticle* particle          , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) ;
    // ========================================================================
    /// search for minimal element in the tree 
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* min_element 
    ( const LHCb::MCVertex*   vertex            , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) ;
    // ========================================================================
    /** simple algorithm to find a minimal element in the tree of MC-particle
     *  @param particle MC-particle 
     *  @param fun     function to be minimized 
     *  @param cut     selector for the particles 
     *  @param decayOnly flag to scal only teh decay vertcies 
     *  @return the minimum element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-04
     */
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* min_element 
    ( const LHCb::MCParticle* particle          , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) 
    {
      const LHCb::MCParticle* result = 0  ;
      if ( 0 == particle    ) { return result ; }                  // RETURN
      if ( cut ( particle ) ) { result = particle ; }
      // loop over vertices 
      typedef SmartRefVector<LHCb::MCVertex> EVs ;
      const EVs& evs = particle->endVertices () ;
      for ( EVs::const_iterator iev = evs.begin() ; evs.end() != iev ; ++iev )
      {
        const LHCb::MCVertex* ev = *iev ;
        if ( decayOnly && !ev->isDecay()  ) { continue ; }      // CONTINUE 
        /// get the minimum per vertex 
        const LHCb::MCParticle* tmp = LoKi::MCAlgs::min_element 
          ( ev , fun , cut , decayOnly ) ;
        if ( 0 == tmp                     ) { continue ; }      // CONITNUE 
        if ( 0 == result ||
             fun ( tmp ) < fun ( result ) ) { result = tmp ; }  // ATTENTION!
      }
      return result ;                                           // RETURN  
    }    
    // ========================================================================
    /** simple algorithm to find a minimal element in the tree of MC-particle
     *  @param vertex   MC-vertex 
     *  @param fun     function to be minimized 
     *  @param cut     selector for the particles 
     *  @param decayOnly flag to scal only teh decay vertcies 
     *  @return the minimum element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-04
     */
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* min_element 
    ( const LHCb::MCVertex*   vertex            , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) 
    {
      const LHCb::MCParticle* result = 0  ;
      if ( 0 == vertex                     ) { return result ; } // RETURN
      if ( decayOnly && !vertex->isDecay() ) { return result ; } // RETURN
      // loop over products 
      typedef SmartRefVector<LHCb::MCParticle> Ps ;
      const Ps& ps = vertex->products() ;
      for ( Ps::const_iterator ip = ps.begin() ; ps.end() != ip ; ++ip )
      {
        const LHCb::MCParticle* tmp = LoKi::MCAlgs::min_element 
          ( *ip , fun , cut , decayOnly ) ;  
        if ( 0 == tmp    ) { continue ; }                       // CONTINUE 
        if ( 0 == result ||
             fun ( tmp ) < fun ( result ) ) { result = tmp ; }  // ATTENTION!
      }
      return result ;
    }
    // ========================================================================
    /// search for minimal element in the tree 
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* max_element 
    ( const LHCb::MCParticle* particle          , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) ;
    // ========================================================================
    /// search for minimal element in the tree 
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* max_element 
    ( const LHCb::MCVertex*   vertex            , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) ;
    // ========================================================================
    /** simple algorithm to find a minimal element in the tree of MC-particle
     *  @param particle MC-particle 
     *  @param fun     function to be maximized
     *  @param cut     selector for the particles 
     *  @param decayOnly flag to scal only teh decay vertcies 
     *  @return the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-04
     */
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* max_element 
    ( const LHCb::MCParticle* particle          , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) 
    {
      const LHCb::MCParticle* result = 0  ;
      if ( 0 == particle    ) { return result ; }                  // RETURN
      if ( cut ( particle ) ) { result = particle ; }
      // loop over vertices 
      typedef SmartRefVector<LHCb::MCVertex> EVs ;
      const EVs& evs = particle->endVertices () ;
      for ( EVs::const_iterator iev = evs.begin() ; evs.end() != iev ; ++iev )
      {
        const LHCb::MCVertex* ev = *iev ;
        if ( decayOnly && !ev->isDecay()  ) { continue ; }      // CONTINUE 
        /// get the minimum per vertex 
        const LHCb::MCParticle* tmp = LoKi::MCAlgs::max_element 
          ( ev , fun , cut , decayOnly ) ;
        if ( 0 == tmp                     ) { continue ; }      // CONITNUE 
        if ( 0 == result ||
             fun ( result ) < fun ( tmp ) ) { result = tmp ; }  // ATTENTION!
      }
      return result ;                                           // RETURN  
    }    
    // ========================================================================
    /** simple algorithm to find a maximal element in the tree of MC-particle
     *  @param vertex   MC-vertex 
     *  @param fun     function to be maximized
     *  @param cut     selector for the particles 
     *  @param decayOnly flag to scal only teh decay vertcies 
     *  @return the maximum element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-04
     */
    template <class FUNCTION,class PREDICATE>
    inline const LHCb::MCParticle* max_element 
    ( const LHCb::MCVertex*   vertex            , 
      const FUNCTION&         fun               , 
      const PREDICATE&        cut               , 
      const bool              decayOnly         ) 
    {
      const LHCb::MCParticle* result = 0  ;
      if ( 0 == vertex                     ) { return result ; } // RETURN
      if ( decayOnly && !vertex->isDecay() ) { return result ; } // RETURN
      // loop over products 
      typedef SmartRefVector<LHCb::MCParticle> Ps ;
      const Ps& ps = vertex->products() ;
      for ( Ps::const_iterator ip = ps.begin() ; ps.end() != ip ; ++ip )
      {
        const LHCb::MCParticle* tmp = LoKi::MCAlgs::max_element 
          ( *ip , fun , cut , decayOnly ) ;  
        if ( 0 == tmp    ) { continue ; }                       // CONTINUE 
        if ( 0 == result ||
             fun ( result ) < fun ( tmp ) ) { result = tmp ; }  // ATTENTION!
      }
      return result ;
    }
    // ======================================================================== 
    /** trivial algorithm seekd fot the 
     *  first particle in the tree which satisfy 
     *  the certain criteria
     *
     *  @param   particle pointer to MC particle  
     *  @param   predicate criteria 
     *  @param   decayOnly search only within decay products 
     *  @return  the first paticle that satisfy criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline 
    const LHCb::MCParticle* 
    foundFirst  
    ( const LHCb::MCParticle* particle          , 
      const PREDICATE&        predicate         , 
      const bool              decayOnly = false )
    {
      if ( 0 == particle          ) { return        0 ; }            // RETURN 
      // evaluate the predicate! 
      if ( predicate ( particle ) ) { return particle ; }            // RETURN 
      //
      typedef SmartRefVector<LHCb::MCVertex>   EndVertices ;
      //      typedef SmartRefVector<LHCb::MCParticle> Ps          ;
      const EndVertices& vertices = particle->endVertices() ;
      //
      for ( EndVertices::const_iterator ivx = vertices.begin() ;
            vertices.end() != ivx ; ++ivx ) 
      {
        const LHCb::MCVertex* vx = *ivx ;
        if ( 0 == vx                     ) { continue ; }      // CONTINUE 
        if ( decayOnly && !vx->isDecay() ) { continue ; }      // CONTINUE
        //
        typedef SmartRefVector<LHCb::MCParticle> Ps          ;
        const Ps& ps = vx->products() ;
        for ( Ps::const_iterator  ip = ps.begin() ; ps.end() != ip ; ++ip ) 
        { 
          const LHCb::MCParticle* p  = *ip ;
          if ( 0 == p ) { continue ; }                         // CONTINUE 
          // start recursion 
          const LHCb::MCParticle* ff = 
            foundFirst ( p , predicate , decayOnly ) ;            // RECURSION 
          // 
          if ( 0 != ff ) { return ff ; }                             // RETURN
        }
      }
      //
      return 0 ;                                                     // RETURN 
    }
    // ========================================================================
  } //                                            end of namespace LoKi::MCAlgs
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCALGS_H
// ============================================================================
