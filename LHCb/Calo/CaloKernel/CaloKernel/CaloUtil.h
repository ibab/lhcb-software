// $Id: CaloUtil.h,v 1.1.1.1 2001-11-25 14:07:38 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/07/29 10:54:42  ibelyaev
// add new namespace with a number of useful utilities
// 
// ===========================================================================
#ifndef CALOKERNEL_CALOUTIL_H 
#define CALOKERNEL_CALOUTIL_H 1
// ===========================================================================

/** @namespace CaloUtil CaloUtil.h CaloKernel/CaloUtil.h
 *  
 *  A number of general useful methods 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   29/07/2001
 */

namespace CaloUtil 
{
  
  /** @class Delete 
   * 
   *  useful utility to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  class Delete
  { 
  public:
    /* delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    template <class TYPE>
    inline TYPE* operator() ( TYPE*& obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** @class Eraser
   * 
   *  useful utility to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class TYPE>
  class Eraser: public std::unary_function<TYPE*&,TYPE*>
  { 
  public:
    /* delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    inline TYPE* operator() ( TYPE*& obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** useful utility to delete the object through the pointer  
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   *
   *  @param obj pointer to object to be deleted 
   *  @return NULL pointer 
   */ 
  template <class TYPE>
  inline TYPE* Delete_Ptr( TYPE*& obj )
  { 
    if( 0 != obj ) { delete obj ; obj = 0 ; }
    return obj ;
  };
  
  /** @class Cast 
   * 
   *  useful utility to perform the cast 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class TYPE>
  class Cast
  { 
  public:
    /* delete the object 
     *  @param obj pointer to object to be cast
     *  @return pointer to type TYPE
     */
    template <class TYPE1>
    inline TYPE* operator() ( TYPE1* obj ) const 
    { return 0 == obj ? 0 : dynamic_cast<TYPE*> ( obj ) ; };
  };
  
}; ///< end of namespace CaloUtil 

// ===========================================================================
// The End 
// ===========================================================================
#endif ///< CALOKERNEL_CALOUTIL_H
// ===========================================================================
