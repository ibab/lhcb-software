// $Id: PhysAlgs.h,v 1.6 2007-06-04 18:25:37 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSALGS_H 
#define LOKI_PHYSALGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
  /** @namespace LoKi::PhysAlgs PhysAlgs.h LoKi/PhysAlgs.h
   *  Collection of STL-like algorithms for the 
   *   manipulation with "Phsyics" obejcts 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-20
   */
  namespace  PhysAlgs 
  { 
    // ========================================================================
    /** The trivial algorithm  which counts the number 
     *  of particles in the decay tree, which satisfies 
     *  the certain critetia
     *
     *  @param p pointer to the particle 
     *  @param cut the criteria 
     *  @return number of particles in the tree 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class PREDICATE>
    inline size_t count_if 
    ( const LHCb::Particle* p   , 
      const PREDICATE&      cut ) 
    {
      size_t result = 0  ;
      if ( 0 != p ) 
      {
        // (1) count in daughters
        typedef SmartRefVector<LHCb::Particle> CHILDREN;
        const CHILDREN&  children = p->daughters() ;
        for ( CHILDREN::const_iterator child = children.begin()  ;
              children.end() != child ; ++child ) 
        { result += LoKi::PhysAlgs::count_if ( *child , cut ) ; } // RECURSION
      }
      // (1) check itself 
      if ( cut( p ) ) {        ++result ; }                      // INCREMENT  
      //
      return result ;                                            // RETURN 
    }     
    // ========================================================================
    /** The trivial algorithm which searches for the  
     *  particle in the decay tree, which satisfies 
     *  the certain critetia 
     *
     *  @param p pointer to the particle 
     *  @param cut the criteria 
     *  @return true if there is such particle 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class PREDICATE>
    inline bool  found  
    ( const LHCb::Particle* p   , 
      const PREDICATE&      cut ) 
    {
      // (1) check itself 
      if ( cut( p ) ) { return true  ; }                  // RETURN 
      // (2) check the validity
      if ( 0 == p   ) { return false ; }                  // RETURN 
      // (3) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = p->daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
            children.end() != child ; ++child ) 
      { 
        if ( LoKi::PhysAlgs::found ( *child , cut ) )     // RECURSION
        { return true ; } ;                               // EETURN 
      }
      //
      return false ;                                      // RETURN 
    }  
    // ========================================================================
    /** The trivial algorithm which scans the decay tree   
     *  and accumulates the value for some function for 
     *  the particles which satisfies the certain criteria 
     *
     *  @code 
     *  
     *  // find the product of all "weights" of the 
     *  // particles in the decay tree:
     *
     *  const LHCb::Particle* B = ... ;
     *  const double weight = LoKi::PhysAlgs::accumulate 
     *     ( B , WEIGHT , ALL , 1.0 , std::multiplies<double>() ) ;
     * 
     *  // find the sum of transverse momenta for all charged pions 
     *  // in the deacy tree: 
     *
     *  const double sumPionPT = LoKi::PhysAlgs:accumulate 
     *     ( B , PT , "pi+" == ABSID , 0.0 , std::plus<double>() ) ;
     *
     *  @code 
     *
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
    template <class FUNCTION,class PREDICATE,class OPERATION,class RESULT>
    inline RESULT accumulate 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      RESULT                         result ,
      OPERATION                      binop  ) 
    {
      // traverse the tree if possible 
      if ( 0 != p && !p->daughters().empty() ) 
      {
        // (1) accumulate from daughters 
        typedef SmartRefVector<LHCb::Particle> CHILDREN;
        const CHILDREN&  children = p->daughters() ;
        for ( CHILDREN::const_iterator child = children.begin() ; 
              children.end() != child ; ++child ) 
        { 
          result = LoKi::PhysAlgs::accumulate 
            ( *child , fun , cut , result , binop ) ;                // RECURSION
        }
      }
      // (2) check itself 
      if ( cut( p ) ) { result = binop ( result  , fun( p ) ) ; }    // INCREMENT
      // 
      return result ;                                                // RETURN 
    } 
    // ========================================================================
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param p        pointer to the particle 
     *  @param fun      functionto be accumulated 
     *  @param cut      the criteria
     *  @param minval   minimal value 
     *  @return updated minimal value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT min_value 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      RESULT                         minval ) 
    {
      // traverse the tree if possible 
      if ( 0 != p && !p->daughters().empty() ) 
      {
        // (1) check daughters 
        typedef SmartRefVector<LHCb::Particle> CHILDREN;
        const CHILDREN&  children = p->daughters() ;
        for ( CHILDREN::const_iterator child = children.begin() ; 
              children.end() != child ; ++child ) 
        {
          minval = LoKi::PhysAlgs::min_value 
            ( *child , fun , cut , minval ) ;                  // RECURSION
        }  
      }
      // (2) check itself 
      if ( cut( p ) ) { minval = std::min ( minval , fun(p) ) ; }
      //
      return minval ;                                        // RETURN 
    } 
    // ========================================================================    
    /** The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  maximal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  @param p        pointer to the particle 
     *  @param fun      functionto be accumulated 
     *  @param cut      the criteria
     *  @param maxval   maximal value 
     *  @return updated maximal  value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class FUNCTION,class PREDICATE,class RESULT>
    inline RESULT                    max_value 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      RESULT                         maxval ) 
    {
      // (1) traverse the tree if possible 
      if ( 0 != p && !p->daughters().empty()  )
      {
        // (2) check daughters 
        typedef SmartRefVector<LHCb::Particle> CHILDREN;
        const CHILDREN&  children = p->daughters() ;
        for ( CHILDREN::const_iterator child = children.begin() ; 
              children.end() != child ; ++child ) 
        {
          maxval = LoKi::PhysAlgs::max_value 
            ( *child , fun , cut , maxval ) ;                  // RECURSION
        }  
      }  
      // (3) check itself 
      if ( cut( p ) ) { maxval = std::max ( maxval , fun(p) ) ; }
      //
      return maxval ;                                        // RETURN 
    } 
    // ========================================================================    
    /** get the element from the tree which maximizes the fucntion 
     *  under the certain conditions 
     *  @param particle the decay tree head
     *  @param fun      function to be minimized 
     *  @param cut      selector for particles 
     *  @return the minimal element 
     */
    template <class FUNCTION,class PREDICATE>
    inline  LHCb::Particle* min_element 
    ( const LHCb::Particle* particle , 
      const FUNCTION&       fun      , 
      const PREDICATE&      cut      ) 
    {
      if ( 0 == particle    ) { return 0 ; }                         // RETURN
      const LHCb::Particle* result = 0 ;
      if ( cut ( particle ) ) { result = particle ; }   
      // check for daughters 
      // (2) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = particle -> daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
            children.end() != child ; ++child ) 
      {
        const LHCb::Particle* tmp = 
          LoKi::PhysAlgs::min_element ( *child , fun , cut ) ; // RECURSION
        if ( 0 == tmp     ) { continue ; }                     // CONTINUE 
        if ( 0 == result || 
             fun ( tmp ) < fun ( result  ) ) { result = tmp ; } // ATTENTION!
      }
      LHCb::Particle* final = 0 ;
      if ( 0 != result ) { final = const_cast<LHCb::Particle*> ( result ) ; }
      return final ;                                                // RETURN 
    }
    // ========================================================================    
    /** get the element from the tree which maximizes  the fucntion 
     *  under the certain conditions 
     *  @param particle the decay tree head
     *  @param fun      function to be minimized 
     *  @param cut      selector for particles 
     *  @return the minimal element 
     */
    template <class FUNCTION,class PREDICATE>
    inline  LHCb::Particle* max_element 
    ( const LHCb::Particle* particle , 
      const FUNCTION&       fun      , 
      const PREDICATE&      cut      ) 
    {
      if ( 0 == particle    ) { return 0 ; }                         // RETURN
      const LHCb::Particle* result = 0 ;
      if ( cut ( particle ) ) { result = particle ; }   
      // check for daughters 
      // (2) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = particle -> daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
            children.end() != child ; ++child ) 
      {
        const LHCb::Particle* tmp = 
          LoKi::PhysAlgs::max_element ( *child , fun , cut ) ; // RECURSION
        if ( 0 == tmp     ) { continue ; }                     // CONTINUE 
        if ( 0 == result || 
             fun ( result ) < fun ( tmp ) ) { result = tmp ; } // ATTENTION!
      }
      LHCb::Particle* final = 0 ;
      if ( 0 != result ) { final = const_cast<LHCb::Particle*> ( result ) ; }
      return final ;                                                // RETURN 
    }
    // ========================================================================
  } // end of namespace LoKi::PhysAlgs 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSALGS_H
// ============================================================================
