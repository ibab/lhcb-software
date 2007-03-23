// $Id: IsConvertible.h,v 1.3 2007-03-23 16:40:17 cattanem Exp $
// ==========================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ==========================================================================
#ifndef RELATIONS_ISCONVERTIBLE_H 
#define RELATIONS_ISCONVERTIBLE_H 1
// ==========================================================================
// Include files
// ==========================================================================
// Boost 
// ==========================================================================
#include "boost/static_assert.hpp"
// ==========================================================================
#ifndef WIN32 
// use Boost 
#include "boost/type_traits/is_convertible.hpp"
#else 
// use the trick from A.Alexandrescu's book 
namespace Relations 
{
  namespace detail
  {
    struct _Small {                 } ;
    struct _Big   { char dummy[2] ; } ;
    BOOST_STATIC_ASSERT( sizeof(_Small) != sizeof(_Big) )  ;
    
    template <class T, class U>
    struct Conversion 
    {
      static _Small Test  ( U   ) ;
      static _Big   Test  ( ... ) ;
      static T      MakeT (     ) ;
    public:
      enum { value = sizeof( Test ( MakeT() ) ) == sizeof( _Small ) } ;
    } ;    
  } // end of namespace detail
} // end of namespace Relations 
#endif 
// ==========================================================================

namespace Relations 
{
  /** @class IsConvertible IsConvertible.h Relations/IsConvertible.h
   *
   *  @todo need to use is_convertible from Boost!
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-06-11
   */
  template <class T, class U>
  struct IsConvertible 
  {
    enum 
      {
        same  = false ,
#ifdef WIN32 
        value = Relations::detail::Conversion<T,U>::value       
#else 
        value = boost::is_convertible<T,U>::value  
#endif 
      } ;
  } ;
  template <class T>
  struct IsConvertible<T,T> 
  { enum { same  = true , value = true } ; } ;
} // end of namespace Relations
// ==========================================================================
// The END 
// ==========================================================================
#endif // RELATIONS_ISCONVERTIBLE_H
// ==========================================================================
