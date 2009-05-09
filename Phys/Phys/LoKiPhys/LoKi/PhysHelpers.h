// $Id: PhysHelpers.h,v 1.7 2009-05-09 19:24:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSHELPERS_H 
#define LOKI_PHYSHELPERS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Helpers.h"
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
 *  @date 2006-02-19 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Helpers Helpers.h LoKi/Helpers.h
   *  Namespace with pure technical ("private") helper functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-19
   */
  namespace Helpers 
  {
    // ========================================================================
    /** Helper function to evaluate minimal value of 
     *  something etc... for 
     *  the functions, which are based on VertexHolder
     * 
     *  @param first 'begin'-iterator for the sequence of vertices or 3D-points
     *  @param last  'end'-iterator for the sequence of vertices or 3D-points
     *  @param fun   'VertexHolder' based function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the minumum value of the function
     *  @return iterator to the vertex for which 
     *          the function has MINIMAL value 
     *
     *  @see LoKi::Vertices::VertexHolder
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-19
     */
    template <class VERTEX,class FUNCTION>
    inline 
    VERTEX _Min_vertex 
    ( VERTEX                          first  , 
      VERTEX                          last   ,
      const    FUNCTION&              fun    ,  
      typename FUNCTION::argument     arg    ,
      typename FUNCTION::result_type& result ) // *OUTPUT*
    {
      VERTEX found = last ;                    // INIT
      for ( ; first != last ; ++first ) 
      {
        fun.setVertex( *first ) ;              // THE MOST IMPORTANT LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res < result || last == found ) 
        { result = _res  ; found = first ; }  
      }
      return found ;
    };
    // ========================================================================
    /** Helper function to evaluate minimal value of 
     *  something etc... for 
     *  the functions, which are based on VertexHolder
     * 
     *  @param first 'begin'-iterator for the sequence of vertices or 3D-points
     *  @param last  'end'-iterator for the sequence of vertices or 3D-points
     *  @param cut    selection criteria for the vertices 
     *  @param fun   'VertexHolder' based function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the minumum value of the function
     *  @return iterator to the vertex for which 
     *          the function has MINIMAL value 
     *
     *  @see LoKi::Vertices::VertexHolder
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-19
     */
    template <class VERTEX,class FUNCTION, class PREDICATE>
    inline 
    VERTEX _Min_vertex 
    ( VERTEX                          first  , 
      VERTEX                          last   ,
      const    PREDICATE&             cut    ,
      const    FUNCTION&              fun    ,
      typename FUNCTION::argument     arg    ,
      typename FUNCTION::result_type& result )  // *OUTPUT*
    {
      VERTEX found = last ;                     // INIT
      for ( ; first != last ; ++first ) 
      {
        if ( !cut ( *first ) ) { continue ; }   // CONTINUE 
        fun.setVertex ( *first ) ;              // THE MOST IMPORTANT LINE 
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res < result || last == found ) 
        { result = _res  ; found = first ; }          
      }
      return found ;
    };
    // ========================================================================
    /** Helper function to evaluate maximal value of 
     *  something etc... for 
     *  the funtions, which are based on VertexHolder
     * 
     *  @param first 'begin'-iterator for the sequence of vertices or 3D-points
     *  @param last  'end'-iterator for the sequence of vertices or 3D-points
     *  @param fun   'VertexHolder' based function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the maximal value of the function
     *  @return iterator to the vertex for which 
     *          the function has MAXIMAL value 
     *
     *  @see LoKi::Vertices::VertexHolder
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-19
     */
    template <class VERTEX,class FUNCTION>
    inline 
    VERTEX _Max_vertex 
    ( VERTEX                          first  , 
      VERTEX                          last   ,
      const    FUNCTION&              fun    ,  
      typename FUNCTION::argument     arg    ,
      typename FUNCTION::result_type& result ) // *OUTPUT*
    {
      //
      VERTEX found = last ;      // INIT
      for ( ; first != last ; ++first ) 
      {
        fun.setVertex( *first ) ;     // THE MOST IMPORTANT LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res > result || last == found ) 
        { result = _res  ; found = first ; }  
      }
      return found ;
    };
    // ========================================================================
    /** Helper function to evaluate maximal value of 
     *  something etc... for 
     *  the functions, which are based on VertexHolder
     * 
     *  @param first 'begin'-iterator for the sequence of vertices or 3D-points
     *  @param last  'end'-iterator for the sequence of vertices or 3D-points
     *  @param fun   'VertexHolder' based function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the maximal value of the function
     *  @return iterator to the vertex for which 
     *          the function has MAXIMAL value 
     *
     *  @see LoKi::Vertices::VertexHolder
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-19
     */
    template <class VERTEX,class FUNCTION, class PREDICATE>
    inline 
    VERTEX _Max_vertex 
    ( VERTEX                          first  , 
      VERTEX                          last   ,
      const    PREDICATE&             cut    , 
      const    FUNCTION&              fun    ,  
      typename FUNCTION::argument     arg    ,
      typename FUNCTION::result_type& result ) // *OUTPUT*
    {
      VERTEX found = last  ;      // INIT
      for ( ; first != last ; ++first ) 
      {
        if ( !cut ( *first ) ) { continue ; } // CONTINUE
        fun.setVertex( *first ) ;     // THE MOST IMPORTANT LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res > result || last == found ) 
        { result = _res  ; found = first ; }  
      }
      return found ;
    };
    // ========================================================================
    /** Helper function to evaluate minimal value of 
     *  impact parameter, distance, chi2 etc... for 
     *  the funtions.
     * 
     *  @param first 'begin'-iterator for the sequence of particles
     *  @param last  'end'-iterator for the sequence of particles 
     *  @param fun   the function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the minumum value of the function
     *  @return iterator to the particle for which 
     *          the function has a minuma value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-19
     */
    template <class PARTICLE,class FUNCTION>
    inline 
    PARTICLE _Min_particle
    ( PARTICLE                        first  , 
      PARTICLE                        last   ,
      const    FUNCTION&              fun    ,  
      typename FUNCTION::argument     arg    ,
      typename FUNCTION::result_type& result ) // *OUTPUT*
    {
      PARTICLE found = last  ;      // INIT
      for ( ; first != last ; ++first ) 
      {
        fun.setParticle( *first ) ;     // THE MOST IMPORTAN LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res < result || last == found )
        { result = _res  ; found = first ; }  
      }
      return found ;
    };
    // ========================================================================
    template <class FUNCTOR>
    struct PMFA 
    {
      typedef typename FUNCTOR::argument     argument        ;
      typedef typename FUNCTOR::result_type  result_type     ;
      typedef result_type (FUNCTOR::*PMF)( argument ) const  ;
      // ======================================================================
      /// constructor 
      PMFA ( const FUNCTOR* fun , PMF _pmf ) 
        : m_fun ( fun ) , m_pmf ( _pmf ) {}
      /// the only one method 
      result_type operator() ( argument a ) const 
      { return (m_fun->*m_pmf) ( a ) ; }
      // ======================================================================
      template <class VERTEX> 
      void setVertex ( VERTEX vertex ) const { m_fun ->setVertex ( vertex ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PMFA () ;                   // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================
      /// the functor 
      const FUNCTOR* m_fun ;
      /// member function 
      PMF            m_pmf ;
      // ======================================================================
    } ;
    // ========================================================================    
  } // end of namespace LoKi::Helpers
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSHELPERS_H
// ============================================================================
