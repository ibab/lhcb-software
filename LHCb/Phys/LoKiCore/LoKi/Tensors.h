// $Id: Tensors.h,v 1.1 2008-07-26 12:04:58 ibelyaev Exp $
// ===============================================================
#ifndef LOKI_TENSORS_H 
#define LOKI_TENSORS_H 1

namespace LoKi
{
  namespace Tensors 
  {
    // ========================================================================
    /** @struct Delta_
     *  Kronecker delta
     *
     *  @code
     *
     *    int ij = Delta_<I,J>::value ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    template <unsigned int I, unsigned int J> 
    struct Delta_ { enum { value = (I == J) } } ;
    // ========================================================================
    /** @struct  Delta 
     *  Kronecker delta:
     *
     *  @code
     *
     *   Delta delta ;
     *
     *    const size_t i = ... ;
     *    const size_t j = ... ;
     *
     *    int ij = delta( i , j ) ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    struct Delta : public std::binary_function<size_t,size_t,int>
    {
      // ======================================================================
      /// Kroneker delta
      inline int delta 
      ( const size_t i , 
        const size_t j ) const { return i == j ; }
      /// Kroneker delta (functional form)
      inline int operator() 
      ( const size_t i , 
        const size_t j ) const { return delta ( i , j )  ; }
      // ======================================================================
    } ;  
    // ========================================================================
    /** @struct Epsilon_
     *  4D Antisymmetric Levy-Civita tensor 
     *
     *  Convention here: 
     *    - epsilon(x,y,z,t) = 1
     *
     *  @code
     *
     *   int result = Epsilon_<0,1,2,3>::value ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */     
    template <unsigned int I , 
              unsigned int J ,
              unsigned int K ,
              unsigned int L > struct Epsilon_ ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,I,J,K> { enum { value = 0 } ; } ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,J,I,K> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,J,K,I> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,J,J,K> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,J,K,J> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion 
    template <unsigned int I , unsigned int J , unsigned int K>
    struct Epsilon_<I,J,K,K> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion    
    template <>
    struct Epsilon_<0,1,2,3> { enum { value = 1 } ; } ;
    // ========================================================================    
    namespace detail 
    {
      // ======================================================================
      /// helper structure for conditional selection 
      template <bool C,int VALUE,class TYPE>
      struct _Value                   { enum { value = VALUE        } ; } ;
      // ======================================================================
      /// helper structure for conditional selection 
      template <int VALUE,class TYPE>
      struct _Value<false,VALUE,TYPE> { enum { value = -TYPE::value } ; } ;
      // ======================================================================
    }
    // ========================================================================
    /// the generic evaluation of Levy-Chivita elements
    template <unsigned int I , unsigned int J ,
              unsigned int K , unsigned int L >
    struct Epsilon_ 
    {
    private:      
      typedef Epsilon_<J,I,K,L>  _12 ;
      typedef Epsilon_<I,K,J,L>  _23 ;
      typedef Epsilon_<I,J,L,K>  _34 ;
    public:
      // ======================================================================
      /// compile-time recursion here!
      enum { value = detail::_Value<(I<J),detail::_Value<(J<K),detail::_Value<(K<L),(L<4)?1:0,_34>::value,_23>::value,_12>::value } ;
      // ======================================================================
    };
    // ========================================================================
    /** @struct Epsilon
     *  Simple implementation of 4D Levy-Civita symbol
     *  @todo probably it is worth to move LoKi::Kinematics::Epsilon into LHCbKernel,
     *                 LHCbMath or DaVinciKernel packages. 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    struct Epsilon 
    {
    public:
      // ============================================================================
      /** the major method for evaluation of elements of Levy-Civita tensor
       *
       *  @code 
       *
       *   Epsilon e ;
       *
       *   int symbol = e(0,1,2,3) ;
       *
       *  @endcode
       *  
       *  @param i the first  index 
       *  @param j the second index 
       *  @param k the third  index 
       *  @param l the last   index 
       *  @return the value of Levy-Civita symbol 
       */
      inline int operator() 
      ( const size_t i , 
        const size_t j , 
        const size_t k , 
        const size_t l ) const { return epsilon ( i , j , k , l ) ; }  
      // ======================================================================
      inline double operator () 
      ( const LoKi::LorentzVector& v , 
        const size_t               i , 
        const size_t               j , 
        const size_t               k  ) const { return epsilon ( v , i , j , k ) ; }
      // ======================================================================
      /** the major method for evaluation of elements of Levy-Civita tensor
       *  @code 
       *
       *   Epsilon lcs ;
       *
       *   int symbol = lcs.epsilon (0,1,2,3) ;
       *
       *  @endcode
       *  
       *  @param i the first  index 
       *  @param j the second index 
       *  @param k the third  index 
       *  @param l the last   index 
       *  @return the value of Levy-Civita symbol 
       */
      inline int epsilon
      ( const size_t i , 
        const size_t j , 
        const size_t k , 
        const size_t l ) const 
      {
        /// the regular cases 
        if      (  i <  j && j <  k && k <  l && l <  4 ) { return 1 ; } // RETURN 
        else if (  i == j || j == k || k == l || l == i ) { return 0 ; } // RETURN
        else if (  i >  3 || j >  3 || k >  3 || l >  3 ) { return 0 ; } // RETURN
        /// permutations are required:
        if      ( i > j  ) { return -epsilon ( j , i , k , l ) ; } // RETURN 
        else if ( j > k  ) { return -epsilon ( i , k , j , l ) ; } // RETURN 
        else if ( k > l  ) { return -epsilon ( i , j , l , k ) ; } // RETURN
        /// here we can go only if some of number >=4, return 0..
        return 0 ;  
      }
      // ======================================================================      
      inline double epsilon 
      ( const LoKi::LorentzVector& v , 
        const size_t               i , 
        const size_t               j , 
        const size_t               k  ) const 
      {
        //
        if ( i == j || j == k || k == i ) { return 0 ; }
        if ( i >  j ) { return -epsilon ( v , j , i , k ) ; }
        if ( j >  k ) { return -epsilon ( v , i , k , j ) ; }
        if ( i >  k ) { return  epsilon ( v , k , k , i ) ; }
        //
        return  
          - v.E  () * epsilon ( i , j , k , 3 )    //   +   &   - 
          - v.Px () * epsilon ( 0 , i , j , k )    //   -   &   + 
          + v.Py () * epsilon ( i , 1 , j , k )    //   -   &   - 
          - v.Pz () * epsilon ( i , j , 2 , k ) ;  //   -   &   + 
      }
      // ======================================================================
      inline double epsilon 
      ( const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 , 
        const size_t               i  , 
        const size_t               j  ) const 
      {
        if      ( i == j ) { return                           0  ; } // RETURN 
        else if ( i >  j ) { return -epsilon ( v1 , v2 , j , i ) ; } // RETURN 
        
        
        const int e01 =  epsilon ( 0 , 1 , i , j ) ;
        const int e02 = -epsilon ( 0 , i , 2 , j ) ;
        const int e03 =  epsilon ( 0 , i , j , 3 ) ;
        
        const int e12 =  epsilon ( i , 1 , 2 , j ) ;
        const int e13 = -epsilon ( i , 1 , j , 3 ) ;
        const int e23 =  epsilon ( i , j , 2 , 3 ) ;
        
        return 
          + v1.Px () * v2.Py () * e01      // -  &  -  &  + 
          + v1.Px () * v2.Pz () * e02      // -  &  -  &  +
          - v1.Px () * v2.E  () * e03      // -  &  +  &  + 
          - v1.Py () * v2.Px () * e01      // -  &  -  &  - 
          + v1.Py () * v2.Pz () * e12      // -  &  -  &  + 
          - v1.Py () * v2.E  () * e13      // -  &  +  &  + 
          - v1.Pz () * v2.Px () * e02      // -  &  -  &  - 
          - v1.Pz () * v2.Py () * e12      // -  &  -  &  -  
          - v1.Pz () * v2.E  () * e23      // -  &  +  &  + 
          + v1.E  () * v2.Px () * e03      // +  &  -  &  - 
          + v1.E  () * v2.Py () * e13      // +  &  -  &  -
          + v1.E  () * v2.Pz () * e23 ;    // +  &  -  &  -    
      }
    };

}

}

#endif // LOKI_TENSORS_H
