// $Id: Convertible.h,v 1.2 2006-05-02 14:29:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_CONVERTIBLE_H 
#define LOKI_CONVERTIBLE_H 1
// ============================================================================
// Include files
// ============================================================================
// Boost 
// ============================================================================
#include "boost/type_traits/conversion_traits.hpp" 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23 
 */
// ============================================================================

namespace LoKi 
{
  /** @struct Convertible Convertible.h LoKi/Convertible.h
   *
   *  The trivial structure to determina at compile-time the 
   *  convertibility between classes
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  template <class TYPE1,class TYPE2>
  struct Convertible 
  {
    enum { value = boost::is_convertible<TYPE1*,TYPE2*>::value } ; 
  } ;
  
}; //end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONVERTIBLE_H
// ============================================================================
