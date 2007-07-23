// $Id: Const.h,v 1.4 2007-07-23 17:07:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CONST_H 
#define LOKI_CONST_H 1
// ============================================================================
// Include files
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
 *  @date 2006-03-29
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @struct Const Const.h LoKi/Const.h
   *  Auxillary structure to define "const-traits"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-29
   */
  template <class TYPE>
  struct  Const 
  { typedef           const TYPE                       Value ; } ;
  // ==========================================================================
  template <class TYPE>
  struct  Const<const TYPE>
  { typedef typename  Const<TYPE>::value const         Value ; } ;
  // ==========================================================================
  template <class TYPE>
  struct  Const<TYPE*>
  { typedef typename  Const<TYPE>::value const * const Value ; } ;
  // ==========================================================================
  template <class TYPE>
  struct  Const<TYPE&>
  { typedef typename  Const<TYPE>::value const &       Value ; } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONST_H
// ============================================================================
