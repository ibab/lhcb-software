// $Id: Helpers.h,v 1.1 2006-02-19 21:49:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_HELPERS_H 
#define LOKI_HELPERS_H 1
// ============================================================================
// Include files
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
 *  @date 2006-02-19 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace Helpers Helpers.h LoKi/Helpers.h
   *  Namespace with pure technical ("private") helper functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-19
   */
  namespace Helpers 
  {

    /** @rm _Min_vertex 
     *  Helper function to evaluate minimal value of 
     *  impact parameter, distance, chi2 etc... for 
     *  the funtions. which are based on VertexHolder
     * 
     *  @param first 'begin'-itrator for the sequence of vertices or 3D-points
     *  @param last  'end'-itrator for the sequence of vertices or 3D-points
     *  @param fun   'VertexHolder' based function to be evaluated 
     *  @param arg   the argument value for the function 
     *  @param result *OUTPUT* the minumum value of the function
     *  @return iterator to the vertex for which 
     *          the function has a minuma value 
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
      if ( first == last ) { return  last ; } 
      //
      VERTEX found = first ;      // INIT
      fun.setVertex( *found ) ;   // NB! CONFIGURE FUNCTION 
      result = fun( arg ) ;       // EVALUATE THE INITIAL RESULT 
      ++first ;                                     // ADVANCE 
      for ( ; first != last ; ++first ) 
      {
        fun.setVertex( *first ) ;     // THE MOST IMPORTAN LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res < result ) { result = _res  ; found = first ; }  
      }
      return found ;
    };

    /** @rm _Min_particle
     *  Helper function to evaluate minimal value of 
     *  impact parameter, distance, chi2 etc... for 
     *  the funtions.
     * 
     *  @param first 'begin'-itrator for the sequence of particles
     *  @param last  'end'-itrator for the sequence of particles 
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
      if ( first == last ) { return  last ; } 
      //
      PARTICLE found = first ;      // INIT
      fun.setParticle( *found ) ;   // NB! CONFIGURE FUNCTION 
      result = fun( arg ) ;       // EVALUATE THE INITIAL RESULT 
      ++first ;                                     // ADVANCE 
      for ( ; first != last ; ++first ) 
      {
        fun.setParticle( *first ) ;     // THE MOST IMPORTAN LINE HERE!
        typename FUNCTION::result_type _res = fun( arg ) ;
        if ( _res < result ) { result = _res  ; found = first ; }  
      }
      return found ;
    };

    /** @fn _First 
     *  trivial helper which returns either the first element 
     *  of the container, or its default" value (for empty 
     *  container)
     *  @param cnt the container itself 
     *  @return the front element (or its defualt value)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-19
     */
    template <class CONTAINER> 
    inline typename CONTAINER::value_type _First
    ( const CONTAINER& cnt ) 
    {
      if ( !cnt.empty() ) { return *(cnt.begin()) ; }
      return typename CONTAINER::value_type() ;
    } ;
    
    /** @fn _First 
     *  trivial helper which returns either the first element 
     *  of the container, or its default" value (for empty 
     *  or invalid container)
     *  @param cnt pointer to the the container
     *  @return the front element (or its defualt value)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-19
     */
    template <class CONTAINER> 
    inline typename CONTAINER::value_type _First
    ( const CONTAINER* cnt ) 
    { 
      if ( 0 != cnt ) { return _First(*cnt) ; }
      return typename CONTAINER::value_type() ; 
    }
    
  }; // end of namespace LoKi::Helpers 
} ; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HELPERS_H
// ============================================================================
