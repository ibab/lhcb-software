// $Id: Reserve.h,v 1.1 2002-03-18 19:32:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RESERVE_H 
#define RELATIONS_RESERVE_H 1

namespace Relations
{
  /** @function reserve Reserve.h Relations/Reserve.h
   *
   *  Helper function to reserve the size of container 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   07/02/2002
   */
  template<class TYPE, class SIZE> 
  inline void reserve(             TYPE & object , const SIZE size ) { ;};
  
  template<class TYPE>
  inline void reserve( std::vector<TYPE>& object , 
                       const typename std::vector<TYPE>::size_type  size )   
  { 
    std::cerr << "special reservation!" << std::endl;
    
    object.reserve( size ); 
  };
  
}; ///< end of namespace Relations 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RESERVE_H
// ============================================================================
