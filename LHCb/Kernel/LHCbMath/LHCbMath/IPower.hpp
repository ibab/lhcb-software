// $Id: IPower.hpp,v 1.1 2006-10-27 15:34:42 jpalac Exp $
#ifndef LHCBMATH_IPOWER_HPP 
#define LHCBMATH_IPOWER_HPP 1

namespace Gaudi 
{  
  namespace Math 
  {
    
    /** @fn Gaudi::Math::IPower 
     *  Template metafunction for compile-time calculation of even, positive
     *  powers of integer numbers.
     *
     *  @code 
     *   // calculate 2**35
     *   const int result = Gaudi::Math::IPower< int, 2,  35>::value;
     *   // calculate 16**3 in long
     *   const long result = Gaudi::Math::IPower< long, 16,  3>::value;
     *
     *  @code 
     *
     *  @author Juan PALACIOS juan.palacios@cern.ch
     *  @date 2006-10-26 
     */


    template <typename T>
    struct TypeWrapper 
    {
      typedef T value_type;
    };

    template<typename TYPE,  typename TypeWrapper<TYPE>::value_type X, int N>
    struct IPower
    {
      enum { value = X * IPower<TYPE, X, N-1>::value };
    };

    template<typename TYPE, typename TypeWrapper<TYPE>::value_type X>
    struct IPower<TYPE, X, 1>
    {
      enum { value = X };
    };

    template<typename TYPE,  typename TypeWrapper<TYPE>::value_type X>
    struct IPower<TYPE, X, 0>
    {
      enum { value = 1 };
    };
    
  } // Math namespace  
} // Gaudi namespace

#endif // LHCBMATH_IPOWER_HPP
