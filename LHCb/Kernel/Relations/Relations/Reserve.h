// $Id: Reserve.h,v 1.3 2006-06-11 15:23:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RESERVE_H 
#define RELATIONS_RESERVE_H 1
// ============================================================================
// include 
// ============================================================================
// STD & STL  
// ============================================================================
#include <vector>
// ============================================================================

namespace Relations
{
  /**
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
  {}
  
  template<class TYPE>
  inline void reserve( std::vector<TYPE>&   object    , 
                       const size_t         size      )   
  { object.reserve( size ); }
  
} ///< end of namespace Relations 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RESERVE_H
// ============================================================================
