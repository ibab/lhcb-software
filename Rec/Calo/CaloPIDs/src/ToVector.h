// ============================================================================
#ifndef CALOPIDS_TOVECTOR_H 
#define CALOPIDS_TOVECTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Local 
// ============================================================================
#include "ToString.h"
// ============================================================================

namespace Gaudi
{
  namespace Utils 
  {
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ()
    { return std::vector<TYPE>() ; }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ) 
    { return std::vector<TYPE>( 1 , o0 ) ; }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 ) 
    { 
      std::vector<TYPE> _vct (2) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ) 
    { 
      std::vector<TYPE> _vct (3) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ,
      const TYPE& o3 ) 
    { 
      std::vector<TYPE> _vct (4) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      _vct[3] = o3 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ,
      const TYPE& o3 ,
      const TYPE& o4 ) 
    { 
      std::vector<TYPE> _vct (5) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      _vct[3] = o3 ;
      _vct[4] = o4 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ,
      const TYPE& o3 , 
      const TYPE& o4 , 
      const TYPE& o5 ) 
    {
      std::vector<TYPE> _vct (6) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      _vct[3] = o3 ;
      _vct[4] = o4 ;
      _vct[5] = o5 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ,
      const TYPE& o3 , 
      const TYPE& o4 , 
      const TYPE& o5 , 
      const TYPE& o6 ) 
    { 
      std::vector<TYPE> _vct (7) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      _vct[3] = o3 ;
      _vct[4] = o4 ;
      _vct[5] = o5 ;
      _vct[6] = o6 ;
      return _vct ;
    }
    // ========================================================================
    template <class TYPE>
    inline  std::vector<TYPE> 
    toVector 
    ( const TYPE& o0 ,
      const TYPE& o1 , 
      const TYPE& o2 ,
      const TYPE& o3 , 
      const TYPE& o4 , 
      const TYPE& o5 , 
      const TYPE& o6 ,
      const TYPE& o7 ) 
    { 
      std::vector<TYPE> _vct (8) ;
      _vct[0] = o0 ;
      _vct[1] = o1 ;
      _vct[2] = o2 ;
      _vct[3] = o3 ;
      _vct[4] = o4 ;
      _vct[5] = o5 ;
      _vct[6] = o6 ;
      _vct[7] = o7 ;
      return _vct ;
    }
    // ========================================================================
    
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ) 
    { return toString ( toVector<TYPE> ( o0 , o1 ) ) ; }
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 ) 
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 ) ) ; }
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 , 
      const TYPE& o3 ) 
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 , o3 ) ) ; }
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 , 
      const TYPE& o3 , 
      const TYPE& o4 )
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 , o3 , o4 ) ) ; }
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 , 
      const TYPE& o3 , 
      const TYPE& o4 , 
      const TYPE& o5 ) 
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 , o3 , o4 , o5 ) ) ; }
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 , 
      const TYPE& o3 ,
      const TYPE& o4 , 
      const TYPE& o5 ,
      const TYPE& o6 ) 
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 , o3 , o4 , o5 , o6 ) ) ;}
    // ========================================================================
    template <class TYPE> 
    std::string toString 
    ( const TYPE& o0 , 
      const TYPE& o1 ,
      const TYPE& o2 , 
      const TYPE& o3 ,
      const TYPE& o4 , 
      const TYPE& o5 ,
      const TYPE& o6 , 
      const TYPE& o7 )
    { return toString ( toVector<TYPE> ( o0 , o1 , o2 , o3 , o4 , o5 , o6 , o7 ) ) ;}
    // ========================================================================
    
  } // end of namespace Utils 
} // end of namespace Gaudi 

// ============================================================================
// The END 
// ============================================================================
#endif // TOVECTOR_H
// ============================================================================
