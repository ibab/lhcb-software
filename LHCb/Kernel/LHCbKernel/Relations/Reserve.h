// $Id: Reserve.h,v 1.2 2002-04-03 15:35:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RESERVE_H 
#define RELATIONS_RESERVE_H 1
// STD & STL  
#include <vector>
namespace Relations
{
  /** @fn reserve Reserve.h Relations/Reserve.h
   *
   *  Helper function to reserve the size of container 
   *
   *  @warning the implementation  is platform dependent! 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   07/02/2002
   */
  template<class TYPE> 
  inline void reserve( TYPE &            /* object */ ,
                       const size_t      /* size   */ ) 
  {};
  
#ifndef WIN32 
  template<class TYPE>
  inline void reserve( std::vector<TYPE>&   object    , 
                       const size_t         size      )   
  { object.reserve( size ); };
#endif

}; ///< end of namespace Relations 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RESERVE_H
// ============================================================================
