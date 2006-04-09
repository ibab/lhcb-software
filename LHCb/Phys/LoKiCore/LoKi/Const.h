// $Id: Const.h,v 1.1 2006-04-09 08:57:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, Version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_CONST_H 
#define LOKI_CONST_H 1
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
 *  @date 2006-03-29
 */
// ============================================================================


namespace LoKi 
{
  /** @struct Const Const.h LoKi/Const.h
   *  Auxillary structure to define "const-traits"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-29
   */
  template <class TYPE>
  struct  Const 
  { typedef           const TYPE                       Value ; } ;
  template <class TYPE>
  struct  Const<const TYPE>
  { typedef typename  Const<TYPE>::value const         Value ; } ;
  template <class TYPE>
  struct  Const<TYPE*>
  { typedef typename  Const<TYPE>::value const * const Value ; } ;
  template <class TYPE>
  struct  Const<TYPE&>
  { typedef typename  Const<TYPE>::value const &       Value ; } ;

} ; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONST_H
// ============================================================================
