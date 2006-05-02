// $Id: Cast.h,v 1.2 2006-05-02 14:29:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_CAST_H 
#define LOKI_CAST_H 1
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
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi
{  
  /** @namespace  Cast Cast.h LoKi/Cast.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace  Cast 
  {
    /** @struct ConstAway 
     *  The trivial function which performs 
     *  const_cast 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-17
     */     
    template <class TYPE>
    struct ConstAway
      : public std::unary_function<const TYPE*,TYPE*>
    { 
      inline TYPE* operator() ( const TYPE* o ) const 
      { return const_cast<TYPE*>( o ) ; } 
    } ;
    
    template <class TYPE>
    struct ConstAway<const TYPE>
      : public std::unary_function<const TYPE*,TYPE*>
    { 
      inline TYPE* operator() ( const TYPE* o ) const 
      { return const_cast<TYPE*>( o ) ; } 
    } ;
    
    
    template <class TYPE>
    struct ConstAway<TYPE*>
      : public std::unary_function<TYPE*,TYPE*>
    { 
      inline TYPE* operator() ( const TYPE* o ) const 
      { return o ; } 
    } ;
    
  } ; // end of the namespace LoKi 
} ;

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CAST_H
// ============================================================================
