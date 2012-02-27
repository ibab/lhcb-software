// ===========================================================================
#ifndef CALOKERNEL_CALOUTIL_H 
#define CALOKERNEL_CALOUTIL_H 1
// ===========================================================================
// include files 
// STD & STL 
#include <functional>
/// forward declarations 
template<class TYPE>
class SmartRef;

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
    }
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
  }
  
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
    { return 0 == obj ? 0 : dynamic_cast<TYPE*> ( obj ) ; }
  };

  /** @class SafeCast 
   * 
   *  casting from one object TYPE1 to another TYPE2
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 23/07/2001
   */
  template <class TYPE1,class TYPE2> 
  class SafeCast: public std::unary_function<TYPE1,TYPE2>
  {
  public:
    /** casting from TYPE1 to TYPE2 
     *  @param   obj  object of TYPE1 
     *  @return  dynamic_cast<TYPE2> (obj) 
     */ 
    inline TYPE2 operator() ( TYPE1 obj ) const
    { return ( !obj ) ? (TYPE2) 0 : dynamic_cast<TYPE2>(obj) ; } 
    ///
  };

  /** @class CmpSmartRef
   *  
   *  compare smart references via resolving to raw  pointers 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 23/07/2001
   */
  template <class TYPE> 
  class CmpSmartRef: 
    public 
  std::binary_function<const SmartRef<TYPE>,const SmartRef<TYPE>,bool>
  {
  public:
    /**  compare smart references via resolving to raw pointers 
     *   @param o1 first object 
     *   @param o2 second object 
     *   @return result of comparison (equality test) between resolved pointers
     */
    inline bool operator() 
      ( const SmartRef<TYPE>& o1 , 
        const SmartRef<TYPE>& o2 ) const
    {
      const TYPE* p1 = o1 ; 
      const TYPE* p2 = o2 ;
      return p1 == p2 ; 
    }
  };

} ///< end of namespace CaloUtil 

// ===========================================================================
// The End 
// ===========================================================================
#endif ///< CALOKERNEL_CALOUTIL_H
// ===========================================================================
