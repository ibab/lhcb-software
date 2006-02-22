// $Id: PhysAlgs.h,v 1.1 2006-02-22 20:53:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
 *  By usage of this code one clearly states the disagreement
 *  with the campain of Dr.O.Callot et al.:
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace PhysAlgs PhysAlgs.h LoKi/PhysAlgs.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-20
   */
  namespace  PhysAlgs 
  {
    
    /** @fn count_if 
     *
     *  The trivial algorithm  which counts the number 
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
      // (1) check itself 
      if ( cut( p ) ) {        ++result ; }                      // INCREMENT  
      // (2) check the validity
      if ( 0 == p   ) { return   result ; }                      // RETURN 
      // (3) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = p->daughters() ;
      for ( CHILDREN::const_iterator child = children.begin()  ;
              children.end() != child ; ++child ) 
      { result += LoKi::PhysAlgs::count_if ( *child , cut ) ; } // RECURSION
      //
      return result ;                                           // RETURN 
    } ;
    
    /** @fn found 
     *
     *  The trivial algorithm which searches for the  
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
    inline size_t found  
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
    } ;
    
    /** @fn accumulate 
     *
     *  The trivial algorithm which scans the decay tree   
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
    template <class FUNCTION,class PREDICATE,class OPERATION>
    inline typename FUNCTION::result_type  accumulate 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      typename FUNCTION::result_type result ,
      OPERATION                      binop  ) 
    {
      // (1) check itself 
      if ( cut( p ) ) { result = binop( result  , fun( p ) ) ; }
      // (2) check the validity
      if ( 0 == p   ) { return result ; }                           // RETURN 
      // (3) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = p->daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
            children.end() != child ; ++child ) 
      { 
        result += LoKi::PhysAlgs::accumulate 
          ( *child , fun , cut , result , binop ) ;                // RECURSION
      }
      //
      return result ;                                              // RETURN 
    } ;

    /** @fn min_value
     *
     *  The trivial algorithm which scans the decay 
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
    template <class FUNCTION,class PREDICATE>
    inline typename FUNCTION::result_type min_value 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      typename FUNCTION::result_type minval ) 
    {
      // (1) check itself 
      if ( cut( p ) ) { minval = std::min ( minval , fun(p) ) ; }
      // (2) check the validity
      if ( 0 == p   ) { return minval ; }                     // RETURN 
      // (3) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = p->daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
              children.end() != child ; ++child ) 
      {
        minval = LoKi::PhysAlgs::min_value 
          ( *child , fun , cut , minval ) ;                  // RECURSION
      }  
      //
      return minval ;                                        // RETURN 
    } ;
    
    /** @fn max_value
     *  The trivial algorithm which scans the decay 
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
    template <class FUNCTION,class PREDICATE>
    inline typename FUNCTION::result_type max_value 
    ( const LHCb::Particle*          p      , 
      const FUNCTION&                fun    ,
      const PREDICATE&               cut    , 
      typename FUNCTION::result_type maxval ) 
    {
      // (1) check itself 
      if ( cut( p ) ) { maxval = std::max ( maxval , fun(p) ) ; }
      // (2) check the validity
      if ( 0 == p   ) { return maxval ; }                     // RETURN 
      // (3) check daughters 
      typedef SmartRefVector<LHCb::Particle> CHILDREN;
      const CHILDREN&  children = p->daughters() ;
      for ( CHILDREN::const_iterator child = children.begin() ; 
              children.end() != child ; ++child ) 
      {
        maxval = LoKi::PhysAlgs::max_value 
          ( *child , fun , cut , maxval ) ;                  // RECURSION
      }  
      //
      return maxval ;                                        // RETURN 
    } ;


  }; // end of namespace LoKi::PhysAlgs 
} ; // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSALGS_H
// ============================================================================
