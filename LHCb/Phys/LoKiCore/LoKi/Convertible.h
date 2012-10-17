// ============================================================================
#ifndef LOKI_CONVERTIBLE_H 
#define LOKI_CONVERTIBLE_H 1
// ============================================================================
// Include files
// ============================================================================
// Boost 
// ============================================================================
//#ifndef WIN32 
//#include "boost/type_traits/conversion_traits.hpp" 
//#else
#include "boost/static_assert.hpp" 
//#endif
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
  // ==========================================================================
  //#ifdef WIN32
  // ==========================================================================
  namespace detail
  {
    // ========================================================================
    struct _Small {                 } ;
    struct _Big   { char dummy[2] ; } ;
    BOOST_STATIC_ASSERT ( sizeof( _Small ) != sizeof( _Big ) ) ;
    // ========================================================================    
    template <class T,class U>
    struct Conversion
    {
      // ======================================================================
      static _Small Test  (  U  ) ;
      static _Big   Test  ( ... ) ;
      static T      MakeT (     ) ;
      // ======================================================================
    public:
      // ======================================================================
      enum { value = sizeof( Test( MakeT() ) ) == sizeof( _Small ) } ;
      // ======================================================================
    } ;
    // ========================================================================
  }
  // ==========================================================================
  //#endif 
  // ==========================================================================
  /** @struct Convertible Convertible.h LoKi/Convertible.h
   *
   *  The trivial structure to determine at compile-time the 
   *  convertibility between classes
   *
   *  @todo Re-enable the native BOOST for LoKi::Convertible. 
   *        Currently it breaks Reflex for  Linux/GCC43 and WindowsVC7.1
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  template <class TYPE1,class TYPE2>
  struct Convertible 
  {
    //#ifndef WIN32
    //enum { same = false , 
    //       value = boost::is_convertible<TYPE1*,TYPE2*>::value } ;
    //#else
    enum { same = false , 
           value = LoKi::detail::Conversion<TYPE1*,TYPE2*>::value } ;
    //#endif  
  } ;
  // ==========================================================================
  template <class TYPE>
  struct Convertible<TYPE,TYPE> 
  { enum { same = true , value = true } ; } ;
  // ==========================================================================  
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONVERTIBLE_H
// ============================================================================
