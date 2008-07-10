// $Id: Algs.h,v 1.15 2008-07-10 09:20:01 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ALGS_H 
#define LOKI_ALGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <utility>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
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
 *  @date 2006-02-10
 */
// ============================================================================
namespace LoKi
{  
  /** @namespace  LoKi::Algs Algs.h LoKi/Algs.h
   *
   *  Helper namespace with auxillary 
   *  functions/algorithms of the general interest 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  Algs 
  {
    // ========================================================================
    /** The trivial algorithm which accumulated the value 
     *  of the function over the sequence
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param functor   function to be evaluated 
     *  @param result    the initial value 
     *  @param binop     the accumulation predicate 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-10
     */
    template <class OBJECT,class RESULT,class FUNCTOR,class OPERATION>
    inline RESULT accumulate
    ( OBJECT         first     ,
      OBJECT         last      , 
      const FUNCTOR& functor   ,
      RESULT         result    , 
      OPERATION      binop     )
    {
      for ( ; first != last ; ++first ) 
      { result = binop ( result , functor( *first ) ) ; }
      return result ;
    } 
    // ========================================================================
    /** The trivial algorithm which accumulates the value 
     *  of the function over the sequence for elements which 
     *  satisfy certain criteria 
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param functor   function to be evaluated 
     *  @param predicate the seelection criteria
     *  @param result    the initial value 
     *  @param binop     the accumulation predicate 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-10
     */
    template <class OBJECT,class RESULT,class FUNCTOR,
              class PREDICATE,class OPERATION>
    inline RESULT accumulate
    ( OBJECT           first     ,
      OBJECT           last      , 
      const FUNCTOR&   functor   ,
      const PREDICATE& predicate ,
      RESULT           result    , 
      OPERATION        binop     )
    {
      for ( ; first != last ; ++first ) 
      { if ( predicate( *first ) ) 
      { result = binop ( result , functor( *first ) ) ; } }
      return result ;
    } 
    // ========================================================================
    /** The missing "copy_if" algorithm from STL 
     *
     *  @param first 'begin'-iterator for the input sequence of objects
     *  @param last  'end'-iterator for the input sequence of objects
     *  @param output the position fo output iterator 
     *  @param cut    the condition
     *  @return the updated position of output itrator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class OBJECT,class OUTPUT,class PREDICATE>
    inline OUTPUT copy_if 
    ( OBJECT    first  , 
      OBJECT    last   , 
      OUTPUT    output , 
      PREDICATE cut    ) 
    {
      for ( ; first != last ; ++first ) 
      { if ( cut( *first ) ) { *output = *first ; ++output ; } }
      return output ;
    }
    // ========================================================================
    /// a bit modified version of std::count_if 
    template <class OBJECT, class PREDICATE>
    inline std::size_t count_if 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      std::size_t result = 0 ;
      for ( ; first != last ; ++first ) 
      { if ( cuts ( *first ) ) { ++result ; } }
      return result ;
    }    
    // ========================================================================
    /// a bit modified version of std::count_if 
    template <class OBJECT, class PREDICATE>
    inline OBJECT find_if 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      for ( ; first != last ; ++first ) 
      { if ( cuts ( *first ) ) { return first ; } }
      return last  ;
    }
    // ========================================================================
    /// useful shortcut
    template <class OBJECT,class PREDICATE>
    inline bool found 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      return last != LoKi::Algs::find_if ( first , last , cuts ) ;
    }
    // ========================================================================
    /** helper function to find "extremum" (minimum or maximum)
     *  of the certain function over the sequence of values 
     *  @see LoKi::select_min
     *  @see LoKi::select_max 
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template<class OBJECT,class FUNCTION,class BINOP>
    inline std::pair<OBJECT,typename FUNCTION::result_type> 
    extremum
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      BINOP            binop )
    {
      typedef std::pair<OBJECT,typename FUNCTION::result_type> PAIR ;
      // empty sequence?
      if ( first == last ) { return PAIR ( last , 0 ) ; }
      // store the first result: 
      OBJECT result = first        ;
      typename FUNCTION::result_type value  = fun(*result) ;
      for ( ; first != last ; ++first ) 
      {
        const double tmp =  fun(*first) ;
        if ( binop ( tmp , value )  ) 
        {
          result = first ;
          value  = tmp   ;
        } 
      }
      return PAIR ( result , value ) ;     // RETURN 
    }
    // ========================================================================
    /** helper function to find "extremum" (minimum or maximum)
     *  of the certain function over the sequence of values 
     *  @see LoKi::select_min
     *  @see LoKi::select_max 
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template<class OBJECT,class FUNCTION,class PREDICATE,class BINOP>
    inline std::pair<OBJECT,typename FUNCTION::result_type> 
    extremum
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      const PREDICATE& cut   ,
      BINOP            binop )
    {
      typedef std::pair<OBJECT,typename FUNCTION::result_type> PAIR ;
      // empty sequence?
      if ( first == last ) { return PAIR ( last , 0 )  ; }
      // store 
      OBJECT result = last ;
      typename FUNCTION::result_type value  = 0    ;
      for ( ; first != last ; ++first ) 
      {
        // good value?
        if ( !cut ( *first ) ) { continue ; }                  // CONTINUE 
        // the first good value? 
        if ( last == result ) 
        {
          result = first ;
          value  = fun( *result ) ;
          continue ;                                          // CONTINUE 
        }
        // evaluate the  function  
        typename FUNCTION::result_type tmp = fun ( *first ) ;
        // compare it!
        if ( binop ( tmp , value ) ) 
        {
          result = first ;
          value  = tmp   ;
          continue  ;                                         // CONTINUE   
        }
      }
      // return the pair of result and  value 
      return PAIR ( result , value ) ;    // RETURN 
    }
    // ========================================================================
    /** select element from the sequence with maximal value of 
     *  given function 
     *
     *  It is just an extension of the STL <tt>std::max_element</tt> 
     *  standard algorithm with additional conditions 
     *
     *  @code 
     *  SEQUENCE prts = ... ; 
     *  SEQUENCE::iterator particle = 
     *              select_max( prts.begin() , 
     *                          prts.end()   , 
     *                          PT           , 
     *                          P > 10 * GeV ); 
     *  @endcode 
     *
     *  Where <tt>SEQUENCE</tt> could be any container or container-like 
     *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
     *  This example illustrates the selection of particle with maximal
     *  transverse momentum  from "container" <tt>prts</tt>. Only 
     *  particles with momentum in excess of 10 GeV are considered.
     *
     *  @see PT 
     *  @see LoKi::Function
     *  @param first  'begin' iterator of the sequence 
     *  @param last   'end'   iterator of the sequence    
     *  @param fun    function 
     *  @param cut    predicate which need to be satisfied 
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION,class PREDICATE>
    inline OBJECT select_min
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      const PREDICATE& cut   )
    {
      return LoKi::Algs::extremum 
        ( first , last , fun , cut , std::less<double> () ).first  ;
    }
    // ========================================================================
    /** select element from the sequence with maximal value of 
     *  given function 
     *
     *  It is just an extension of the STL <tt>std::max_element</tt> 
     *  standard algorithm with additional conditions 
     *
     *  @code 
     *  SEQUENCE prts = ... ; 
     *  SEQUENCE::iterator particle = 
     *              select_max( prts.begin() , 
     *                          prts.end()   , 
     *                          PT           , 
     *                          P > 10 * GeV ); 
     *  @endcode 
     *
     *  Where <tt>SEQUENCE</tt> could be any container or container-like 
     *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
     *  This example illustrates the selection of particle with maximal
     *  transverse momentum  from "container" <tt>prts</tt>. Only 
     *  particles with momentum in excess of 10 GeV are considered.
     *
     *  @see PT 
     *  @see LoKi::Function
     *  @param first  'begin' iterator of the sequence 
     *  @param last   'end'   iterator of the sequence    
     *  @param fun    function 
     *  @param cut    predicate which need to be satisfied 
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION,class PREDICATE>
    inline OBJECT select_max
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      const PREDICATE& cut   )
    {
      return LoKi::Algs::extremum 
        ( first , last , fun , cut , std::greater<double>() ).first ;
    }
    // ========================================================================
    /** select element from the sequence with maximal value of 
     *  given function 
     *
     *  It is just an extension of the STL <tt>std::max_element</tt> 
     *  standard algorithm with additional conditions 
     *
     *  @code 
     *  SEQUENCE prts = ... ; 
     *  SEQUENCE::iterator particle = 
     *              select_max( prts.begin() , 
     *                          prts.end()   , 
     *                          PT           ) ; 
     *  @endcode 
     *
     *  Where <tt>SEQUENCE</tt> could be any container or container-like 
     *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
     *  This example illustrates the selection of particle with maximal
     *  transverse momentum  from "container" <tt>prts</tt>. Only 
     *  particles with momentum in excess of 10 GeV are considered.
     *
     *  @see PT 
     *  @see LoKi::Function
     *  @param first  'begin' iterator of the sequence 
     *  @param last   'end'   iterator of the sequence    
     *  @param fun    function 
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION>
    inline OBJECT select_min
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   )
    {
      return LoKi::Algs::extremum 
        ( first , last , fun , std::less<double>() ).first ;
    }
    // ========================================================================
    /** select element form the sequence with maximal value of 
     *  given function 
     *
     *  It is just an extension of the STL <tt>std::max_element</tt> 
     *  standard algorithm with additional conditions 
     *
     *  @code 
     *  SEQUENCE prts = ... ; 
     *  SEQUENCE::iterator particle = 
     *              select_max( prts.begin() , 
     *                          prts.end()   , 
     *                          PT           ); 
     *  @endcode 
     *
     *  Where <tt>SEQUENCE</tt> could be any container or container-like 
     *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
     *  This example illustrates the selection of particle with maximal
     *  transverse momentum  from "container" <tt>prts</tt>. Only 
     *  particles with momentum in excess of 10 GeV are considered.
     *
     *  @see PT 
     *  @see LoKi::Function
     *  @param first  'begin' iterator of the sequence 
     *  @param last   'end'   iterator of the sequence    
     *  @param fun    function 
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION>
    inline OBJECT select_max
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   )
    {
      return LoKi::Algs::extremum 
        ( first , last , fun , std::greater<double>() ).first ;
    }
    // ========================================================================    
    /** very simple algorithm for minimum value of the function over the 
     *  sequence 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-20
     */
    template <class OBJECT, class FUNCTION,class RESULT>
    inline RESULT min_value 
    ( OBJECT           first  , 
      OBJECT           last   , 
      const FUNCTION&  fun    , 
      RESULT           result )
    {
      for ( ; first != last ; ++first ) 
      { result = std::min ( result , fun (*first) ) ; }
      return result ;
    }
    // ========================================================================    
    /** very simple algorithm for maximum value of the function over the 
     *  sequence 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-20
     */
    template <class OBJECT, class FUNCTION, class PREDICATE, class RESULT>
    inline RESULT min_value 
    ( OBJECT           first  , 
      OBJECT           last   , 
      const FUNCTION&  fun    , 
      const PREDICATE& cut    , 
      RESULT           result )
    {
      for ( ; first != last ; ++first ) 
      {
        if ( cut ( *first ) ) 
        {  result = std::min ( result , fun ( *first ) ) ; } 
      }
      return result ;
    }
    // ========================================================================    
    /** very simple algorithm for maximum value of the function over the 
     *  sequence 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-20
     */
    template <class OBJECT, class FUNCTION,class RESULT>
    inline RESULT max_value 
    ( OBJECT           first  , 
      OBJECT           last   , 
      const FUNCTION&  fun    , 
      RESULT           result )
    {
      for ( ; first != last ; ++first ) 
      { result = std::max ( result , fun ( *first ) ) ; }
      return result ;
    }
    // ========================================================================    
    /** very simple algorithm for maximum value of the function over the 
     *  sequence 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-20
     */
    template <class OBJECT, class FUNCTION, class PREDICATE, class RESULT>
    inline RESULT max_value 
    ( OBJECT           first  , 
      OBJECT           last   , 
      const FUNCTION&  fun    , 
      const PREDICATE& cut    , 
      RESULT           result )
    {
      for ( ; first != last ; ++first ) 
      {
        if ( cut ( *first ) ) 
        {  result = std::max ( result , fun (*first) ) ; } 
      }
      return result ;
    }
    // ========================================================================
    template <class OBJECT,class PREDICATE1,class PREDICATE2>
    inline bool _found_2 
    ( OBJECT            first  , 
      OBJECT            last   , 
      const PREDICATE1& cut1   , 
      const PREDICATE2& cut2   ,
      OBJECT            except )
    {
      bool ok1 = false ;
      bool ok2 = false ;      
      for ( ; first != last ; ++first )
      {
        if ( first == except ) { continue ; }                       // CONTINUE 
        //
        const bool _ok1 = cut1 ( *first ) ;
        if ( ok2 && _ok1 ) { return true ; }                        // RETURN 
        //
        const bool _ok2 = cut2 ( *first ) ;
        if ( ok1 && _ok2 ) { return true ; }                        // RETURN 
        //
        if ( _ok1 ) { ok1 = true ; }
        if ( _ok2 ) { ok2 = true ; }        
      }
      // 
      return false ;                                                // RETURN
    }
    // ========================================================================
    /** simple algorithm to check the presence of at least 
     *  two *different* objects in the sequence which satisfy 
     *  two predicates 
     *  @param first 'begin'-iterator for the sequence 
     *  @param last  'end'-iterator for the sequence 
     *  @param cut1 the first predicate 
     *  @param cut2 the second predicate 
     *  @return true if there are two elements  
     */  
    template <class OBJECT, class PREDICATE1, class PREDICATE2>
    inline bool found_2 
    ( OBJECT            first , 
      OBJECT            last  , 
      const PREDICATE1& cut1  ,
      const PREDICATE2& cut2  )
    {
      return _found_2 ( first , last , cut1 , cut2 , last ) ;
    }
    // ========================================================================
    template <class OBJECT, class PREDICATE1, class PREDICATE2, class PREDICATE3>
    inline bool _found_3
    ( OBJECT            first  , 
      OBJECT            last   , 
      const PREDICATE1& cut1   ,
      const PREDICATE2& cut2   ,
      const PREDICATE3& cut3   , 
      OBJECT            except )
    {
      for ( OBJECT f1 = first ; f1 != last ; ++f1 )
      {
        //
        if ( f1 == except ) { continue ; }
        //
        if       ( cut1 ( *f1 ) && 
                   _found_2 ( first , last , cut2 , cut3 , f1 ) ) { return true ; }
        else if  ( cut2 ( *f1 ) && 
                   _found_2 ( first , last , cut1 , cut3 , f1 ) ) { return true ; }
        else if  ( cut3 ( *f1 ) && 
                   _found_2 ( first , last , cut1 , cut2 , f1 ) ) { return true ; }  
      }
      return false ;  
    }
    // ========================================================================
    /** simple algorithm to check the presence of at least 
     *  three *different* objects in the sequence which satisfy 
     *  three  predicates 
     *  @param first 'begin'-iterator for the sequence 
     *  @param last  'end'-iterator for the sequence 
     *  @param cut1 the first predicate 
     *  @param cut2 the second predicate 
     *  @param cut3 the third predicate 
     *  @return true if there are two elements  
     */  
    template <class OBJECT, class PREDICATE1, class PREDICATE2, class PREDICATE3>
    inline bool found_3
    ( OBJECT            first , 
      OBJECT            last  , 
      const PREDICATE1& cut1  ,
      const PREDICATE2& cut2  ,
      const PREDICATE3& cut3  )
    {
      return _found_3 ( first , last , cut1 , cut2 , cut3 , last ) ;
    }
    // ========================================================================
    template <class OBJECT, 
              class PREDICATE1, class PREDICATE2, 
              class PREDICATE3, class PREDICATE4>
    inline bool _found_4
    ( OBJECT            first  , 
      OBJECT            last   , 
      const PREDICATE1& cut1   ,
      const PREDICATE2& cut2   ,
      const PREDICATE3& cut3   ,
      const PREDICATE4& cut4   ,
      OBJECT            except )
    {
      for ( OBJECT f1 = first ; f1 != last ; ++f1 ) 
      {
        if ( f1 == except ) { continue ; }
        //
        if       ( cut1 ( *f1 ) && 
                   _found_3 ( first , last , cut2 , cut3 , cut4 , f1 ) ) { return true ; }
        else if  ( cut2 ( *f1 ) && 
                   _found_3 ( first , last , cut1 , cut3 , cut4 , f1 ) ) { return true ; }
        else if  ( cut3 ( *f1 ) && 
                   _found_3 ( first , last , cut1 , cut2 , cut4 , f1 ) ) { return true ; }
        else if  ( cut4 ( *f1 ) && 
                   _found_3 ( first , last , cut1 , cut2 , cut3 , f1 ) ) { return true ; }        
      }
      return false ;
    }
    // ========================================================================
    /** simple algorithm to check the presence of at least 
     *  four *different* objects in the sequence which satisfy 
     *  four  predicates 
     *  @param first 'begin'-iterator for the sequence 
     *  @param last  'end'-iterator for the sequence 
     *  @param cut1 the first predicate 
     *  @param cut2 the second predicate 
     *  @param cut3 the third  predicate 
     *  @param cut4 the fourth predicate 
     *  @return true if there are two elements  
     */  
    template <class OBJECT, 
              class PREDICATE1, class PREDICATE2, 
              class PREDICATE3, class PREDICATE4>
    inline bool found_4
    ( OBJECT            first , 
      OBJECT            last  , 
      const PREDICATE1& cut1  ,
      const PREDICATE2& cut2  ,
      const PREDICATE3& cut3  ,
      const PREDICATE4& cut4  )
    {
      return _found_4 ( first , last , cut1 , cut2 , cut3 , cut4 , last ) ;
    }
    // ========================================================================
    template <class OBJECT, 
              class PREDICATE1, class PREDICATE2, 
              class PREDICATE3, class PREDICATE4, class PREDICATE5>
    inline bool _found_5
    ( OBJECT            first  , 
      OBJECT            last   , 
      const PREDICATE1& cut1   ,
      const PREDICATE2& cut2   ,
      const PREDICATE3& cut3   ,
      const PREDICATE4& cut4   ,
      const PREDICATE5& cut5   ,
      OBJECT            except )
    {
      for ( OBJECT f1 = first ; f1 != last ; ++f1 ) 
      {
        if ( f1 == except ) { continue ; }
        //
        if       ( cut1 ( *f1 ) && 
                   _found_4 ( first , last , cut2 , cut3 , cut4 , cut5 , f1 ) ) { return true ; }
        else if  ( cut2 ( *f1 ) && 
                   _found_4 ( first , last , cut1 , cut3 , cut4 , cut5 , f1 ) ) { return true ; }
        else if  ( cut3 ( *f1 ) && 
                   _found_4 ( first , last , cut1 , cut2 , cut4 , cut5 , f1 ) ) { return true ; }
        else if  ( cut4 ( *f1 ) && 
                   _found_4 ( first , last , cut1 , cut2 , cut3 , cut5 , f1 ) ) { return true ; }        
        else if  ( cut5 ( *f1 ) && 
                   _found_4 ( first , last , cut1 , cut2 , cut3 , cut4 , f1 ) ) { return true ; }        
      }
      return false ;
    }  
    // ========================================================================
    /** simple algorithm to check the presence of at least 
     *  five *different* objects in the sequence which satisfy 
     *  five  predicates 
     *  @param first 'begin'-iterator for the sequence 
     *  @param last  'end'-iterator for the sequence 
     *  @param cut1 the first predicate 
     *  @param cut2 the second predicate 
     *  @param cut3 the third  predicate 
     *  @param cut4 the fourth predicate 
     *  @param cut5 the fith predicate 
     *  @return true if there are two elements  
     */  
    template <class OBJECT, 
              class PREDICATE1, class PREDICATE2, 
              class PREDICATE3, class PREDICATE4, class PREDICATE5>
    inline bool found_5
    ( OBJECT            first  , 
      OBJECT            last   , 
      const PREDICATE1& cut1   ,
      const PREDICATE2& cut2   ,
      const PREDICATE3& cut3   ,
      const PREDICATE4& cut4   ,
      const PREDICATE5& cut5   ) 
    {
      return _found_5 
        ( first , last , cut1 , cut2 , cut3 , cut4 , cut5 , last ) ;
    }
    // ========================================================================
    template <class OBJECT, class PREDICATE>
    inline bool _found_N 
    ( OBJECT first                          , 
      OBJECT last                           ,
      const std::vector<PREDICATE>& cuts    , 
      const std::set<size_t>&       indices ) 
    {
      for ( OBJECT f = first ; last != f ; ++f ) 
      {
        const size_t index = f - first ;
        if ( indices.end() != indices.find ( index ) ) { continue ; } 
        for ( typename std::vector<PREDICATE>::const_iterator icut = cuts.begin() ;
              cuts.end() != icut ; ++icut ) 
        {
          const PREDICATE& cut = *icut ;
          if ( !cut ( *f ) )       { continue    ; }              // CONTINUE 
          if ( 1 == cuts.size()  ) { return true ; }              // TRUE 
          std::set<size_t>    _indices    ( indices ) ;
          _indices.insert ( index ) ;
          std::vector<PREDICATE> _cuts    ( cuts    ) ;
          _cuts.erase ( _cuts.begin() + ( icut - cuts.begin() ) ) ;
          return _found_N ( first , last , _cuts , _indices ) ;
        }
      }  
      return false ;
    }
    // ========================================================================
    /** find "N" different elements in the sequence whcih satisfy to N
     *  various predicates 
     *  @param first 'begin'-iterator of the sequence 
     *  @param last  'end'-iterator for the sequence
     *  @param cust  the list of N-predicates 
     *  @return true if there are N different good elements in the sequence
     */
    template <class OBJECT, class PREDICATE>
    inline bool found_N 
    ( OBJECT first                          , 
      OBJECT last                           ,
      const std::vector<PREDICATE>& cuts    ) 
    {
      if ( cuts.empty()       )                          { return false ; }
      const size_t dist = std::distance ( first , last ) ;
      if ( dist < cuts.size() )                          { return false ; }
      //
      if      ( 1 == cuts.size() ) 
      { return last != std::find_if ( first , last , cuts.front() ) ; }
      else if ( 2 == cuts.size() )
      { return found_2 ( first , last , cuts.front() , cuts.back() ) ; }
      else if ( 3 == cuts.size() )
      { return found_3 ( first , last , 
                         *( cuts.begin ()     ) ,
                         *( cuts.begin () + 1 ) ,
                         *( cuts.begin () + 2 ) ) ; }
      else if ( 4 == cuts.size() )
      { return found_4 ( first , last , 
                         *( cuts.begin ()     ) ,
                         *( cuts.begin () + 1 ) ,
                         *( cuts.begin () + 2 ) ,
                         *( cuts.begin () + 3 ) ) ; }
      else if ( 5 == cuts.size() )
      { return found_5 ( first , last , 
                         *( cuts.begin ()     ) ,
                         *( cuts.begin () + 1 ) ,
                         *( cuts.begin () + 2 ) ,
                         *( cuts.begin () + 3 ) ,
                         *( cuts.begin () + 4 ) ) ; }
      
      std::set<size_t> indices ;
      return _found_N ( first , last , cuts , indices ) ;
    }
    // ========================================================================
  } // end of namespace LoKi::Algs  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGS_H
// ============================================================================
