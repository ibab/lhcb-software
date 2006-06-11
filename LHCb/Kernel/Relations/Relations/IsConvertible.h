// $Id: IsConvertible.h,v 1.1 2006-06-11 15:23:45 ibelyaev Exp $
// ==========================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ==========================================================================
#ifndef RELATIONS_ISCONVERTIBLE_H 
#define RELATIONS_ISCONVERTIBLE_H 1
// ==========================================================================
// Include files
// ==========================================================================
// Boost 
// ==========================================================================
#include "boost/static_assert.hpp"
// #include "boost/type_traits/is_convertible.hpp"
// ==========================================================================

namespace Relations 
{
  namespace detail
  {
    template <class T, class U>
    class Conversion 
    {
      typedef char _Small                   ;
      class        _Big { char dummy[2] ; } ;
      BOOST_STATIC_ASSERT( sizeof(_Small) != sizeof(_Big) )  ;
      static _Small Test  ( U   ) ;
      static _Big   Test  ( ... ) ;
      static T      MakeT (     ) ;
    public:
      enum { value = sizeof( Test ( MakeT() ) ) == sizeof( _Small ) } ;
    } ;
  } // end of namespace detail
  
  /** @class IsConvertible InConvertible.h Relations/InConvertible.h
   *  @todo need to use is_convertible from Boost!
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-06-11
   */
  template <class T, class U>
  struct IsConvertible 
  {
  public:
    enum 
      {
        value = Relations::detail::Conversion<T,U>::value , 
        same  = false 
      } ;
  } ;
  template <class T>
  struct IsConvertible<T,T> 
  {
  public:
    enum 
      {
        value = true ,
        same  = true 
      } ;
  } ;
  
} // end of namespace Relatiosn 

// ==========================================================================
// The END 
// ==========================================================================
#endif // RELATIONS_ISCONVERTIBLE_H
// ==========================================================================
