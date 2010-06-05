// $Id: Operations.h,v 1.1 2010-06-05 20:13:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_OPERATIONS_H 
#define LOKI_OPERATIONS_H 1
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// Include files
// ============================================================================
/** @file LoKi/Operations.h
 *  Collection of basic operations with containters 
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Operations 
  {
    // ========================================================================
    /** @struct Union 
     *  Helper structure to represent the union of two containters 
     *  @see std::set_union      
     *  @see LoKi::Operations::Union            
     *  @see LoKi::BasicFunctors::Pipe
     *  @see LoKi::BasicFunctors::Source
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct Union : public std::binary_function < std::vector<TYPE> , 
                                                 std::vector<TYPE> , 
                                                 std::vector<TYPE> >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      _Type operator() ( const _Type& a , const _Type& b )  const
      {
        // 
        if ( &a == &b ) { return a ; }                               // RETURN 
        //
        _Type _r ;
        _Type _a ( a ) ;
        _Type _b ( b ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        std::stable_sort ( _b.begin() , _b.end () ) ;
        typename _Type::iterator _ib = std::unique ( _b.begin () , _b.end() ) ;
        //
        _r.reserve  ( ( _ia - _a.begin() ) + ( _ib - _b.begin() ) ) ;
        //
        std::set_union ( _a.begin () , _ia , 
                         _b.begin () , _ib , std::back_inserter( _r ) ) ;
        //
        return _r ;
      }  
      // ======================================================================
    } ;
    // ========================================================================
    /** @struct Difference 
     *  Helper structure to define "difference" for two containers
     *  @see LoKi::BasicFunctors::Map
     *  @see LoKi::BasicFunctors::Pipe
     *  @see LoKi::BasicFunctors::Source
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct Difference : public std::binary_function < std::vector<TYPE> , 
                                                      std::vector<TYPE> , 
                                                      std::vector<TYPE> >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      _Type operator() ( const _Type& a , const _Type& b )  const
      {
        // 
        if ( &a == &b ) { return _Type() ; }                         // RETURN 
        //
        _Type _r ;
        _Type _a ( a ) ;
        _Type _b ( b ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        std::stable_sort ( _b.begin() , _b.end () ) ;
        typename _Type::iterator _ib = std::unique ( _b.begin () , _b.end() ) ;
        //
        _r.reserve  ( _ia - _a.begin() );
        //
        std::set_difference ( _a.begin () , _ia , 
                              _b.begin () , _ib , std::back_inserter( _r ) ) ;
        //
        return _r ;
      } 
      // ======================================================================
    } ;
    // ========================================================================
    /** @struct SymDifference 
     *  Helper structure to define "symmetric-difference" for two containers
     *  @see LoKi::BasicFunctors::Map
     *  @see LoKi::BasicFunctors::Pipe
     *  @see LoKi::BasicFunctors::Source
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct SymDifference : public std::binary_function < std::vector<TYPE> , 
                                                         std::vector<TYPE> , 
                                                         std::vector<TYPE> >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      _Type operator() ( const _Type& a , const _Type& b )  const
      {
        // 
        if ( &a == &b ) { return _Type() ; }                         // RETURN 
        //
        _Type _r ;
        _Type _a ( a ) ;
        _Type _b ( b ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        std::stable_sort ( _b.begin() , _b.end () ) ;
        typename _Type::iterator _ib = std::unique ( _b.begin () , _b.end() ) ;
        //
        _r.reserve  ( ( _ia - _a.begin() ) + ( _ib - _b.begin() ) ) ;
        //
        std::set_symmetric_difference 
          ( _a.begin () , _ia , 
            _b.begin () , _ib , std::back_inserter( _r ) ) ;
        //
        return _r ;
      } 
      // ======================================================================
    } ;
    // ========================================================================
    /** @struct Intersection
     *  Helper structure to define "intersection" for two containers
     *  @see LoKi::BasicFunctors::Map
     *  @see LoKi::BasicFunctors::Pipe
     *  @see LoKi::BasicFunctors::Source
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct Intersection : public std::binary_function < std::vector<TYPE> , 
                                                        std::vector<TYPE> , 
                                                        std::vector<TYPE> >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      _Type operator() ( const _Type& a , const _Type& b )  const
      {
        // 
        if ( &a == &b ) { return a ; }                                // RETURN 
        //
        _Type _r ;
        _Type _a ( a ) ;
        _Type _b ( b ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        std::stable_sort ( _b.begin() , _b.end () ) ;
        typename _Type::iterator _ib = std::unique ( _b.begin () , _b.end() ) ;
        //
        _r.reserve  ( std::min ( _ia - _a.begin() , _ib - _b.begin() ) ) ;
        //
        std::set_intersection
          ( _a.begin () , _ia , 
            _b.begin () , _ib , std::back_inserter( _r ) ) ;
        //
        return _r ;
      } 
      // ======================================================================
    } ;
    // ========================================================================
    /** @struct Includes
     *  Helper structure to define "includes" for two containers
     *  @see LoKi::BasicFunctors::Map
     *  @see LoKi::BasicFunctors::Pipe
     *  @see LoKi::BasicFunctors::Source
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct Includes : public std::binary_function < std::vector<TYPE> , 
                                                    std::vector<TYPE> , 
                                                    bool              >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      bool operator() ( const _Type& a , const _Type& b )  const
      {
        // 
        if ( &a == &b ) { return true ; }                         // RETURN 
        //
        _Type _a ( a ) ;
        _Type _b ( b ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        std::stable_sort ( _b.begin() , _b.end () ) ;
        typename _Type::iterator _ib = std::unique ( _b.begin () , _b.end() ) ;
        //
        return std::includes 
          ( _a.begin () , _ia , 
            _b.begin () , _ib ) ;
        //
      } 
      // ======================================================================
    } ;
    // ========================================================================
    /** @struct Unique
     *  Helper structure to define "unique" for container 
     *  @see LoKi::BasicFunctors::Pipe
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    struct Unique : public std::unary_function < std::vector<TYPE> , 
                                                 std::vector<TYPE> >
    {
      // ======================================================================
      typedef std::vector<TYPE> _Type ;
      // ======================================================================
      /// the main method 
      bool operator() ( const _Type& a )  const
      {
        //
        _Type _a ( a ) ;
        //
        std::stable_sort ( _a.begin() , _a.end () ) ;
        typename _Type::iterator _ia = std::unique ( _a.begin () , _a.end() ) ;
        //
        return _Type ( _a.begin() , _ia ) ;
      } 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                    the end of namespace LoKi::Operations 
  // ==========================================================================
} //                                                  the end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_OPERATIONS_H
// ============================================================================
