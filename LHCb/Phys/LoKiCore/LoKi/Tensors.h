// $Id: Tensors.h,v 1.6 2009-05-09 19:15:53 ibelyaev Exp $
// ===============================================================
#ifndef LOKI_TENSORS_H 
#define LOKI_TENSORS_H 1
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/KinTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::Tensors 
   *  Collection of general purpose tensors and operations with them
   *  @todo try tp move tensors into LHCbMath/LHCbKernel/DaVinciKernel/Gaudi
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-0725
   */
  namespace Tensors 
  {
    // ========================================================================
    /** @enum Indices 
     *  The list of Lorentz indices 
     *  The numbers are in accordance to 
     *  ROOT::Math::LorentzVector 
     *  @see ROOT::Math::Lorentz::Vector 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */  
    enum {
      //
      X    = 0 , 
      Y    = 1 , 
      Z    = 2 , 
      T    = 3 , 
      //
      PX   = X , 
      PY   = Y , 
      PZ   = Z , 
      E    = T ,
      //
      LAST = 4  
    } ;
    // ========================================================================
    /** @struct Delta_
     *
     *  (Compile-time) Kronecker delta: \f$ \delta^{\mu}_{\nu} \f$ 
     *
     *  @code
     *
     *    int d12 = Delta_<1,2>::value ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    template <unsigned int I, unsigned int J> 
    struct Delta_      { enum { value =  0 } ; } ;
    /// the proper template specialization for diagonal elements 
    template <unsigned int I> 
    struct Delta_<I,I> { enum { value =  1 } ; } ;
    // ========================================================================
    /** @struct  Delta 
     *  Kronecker delta: \f$ \delta^i_j \f$ 
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
    struct Delta
    {
      // ======================================================================
      /// Kroneker delta
      inline int delta 
      (  const  size_t i , 
         const  size_t j ) const { return i == j ; }
      /// Kroneker delta (functional form)
      inline int operator() 
        ( const size_t i , 
          const size_t j ) const { return i == j ; }
      // ======================================================================
    } ;  
    // ========================================================================
    /** @struct G_ 
     *
     *  (Compile-time) Minkowski metric: 
     * \f$  g_{\mu\nu} = 
     *   \begin{pmatrix}
     *      -1 &  0  &  0  & 0 \\
     *       0 & -1  &  0  & 0 \\
     *       0 &  0  & -1  & 0 \\
     *       0 &  0  &  0  & 1  
     * \end{pmatrix} 
     * \f$ 
     *
     *  The metric has been choosen in the way (-,-,-,+) in 
     *   accordance with ROOT::Math::LorentzVector 
     *
     *  @code
     *
     *    const int gXX = G_<X,X>::value ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    template <unsigned int I, unsigned int J>
    struct G_          { enum { value =  0 } ; } ;
    /// the proper template specialzation for diagonal elements 
    template <unsigned int I>
    struct G_<I,I>     { enum { value = -1 } ; } ;
    /// the proper template specialzation for time component 
    template <>
    struct G_<T,T>     { enum { value =  1 } ; } ;
    // ========================================================================
    /** struct G 
     *
     *  Minkowski metric: 
     * \f$  g_{\mu\nu} = 
     *   \begin{pmatrix}
     *      -1 &  0  &  0  & 0 \\
     *       0 & -1  &  0  & 0 \\
     *       0 &  0  & -1  & 0 \\
     *       0 &  0  &  0  & 1  
     * \end{pmatrix} 
     * \f$ 
     *
     *  The metric has been choosen in the way (-,-,-,+) in 
     *  accordance with ROOT::Math::LorentzVector 
     *
     *  @code
     *
     *    G g ;
     *
     *    int g00 = g(0,0) ;
     *
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    struct G 
    {
      // ======================================================================
      /// the only one important function: get the metric 
      inline int  operator () 
        ( const size_t i , 
          const size_t j ) const { return g( i, j ) ; }      
      /// the only one important function: get the metric 
      inline int  g
      ( const size_t i , 
        const size_t j ) const 
      { 
        return    
          ( i    != j )  ?  0 : 
          ( LAST <= i )  ?  0 :
          ( T    == i )  ?  1 : -1 ;
      }
      // ======================================================================
    };
    // ========================================================================
    /** @struct Epsilon_
     *  
     *  (Compile-time) 4D Antisymmetric Levy-Civita tensor
     *   \f$ \epsilon_{\mu\nu\lambda\delta} \f$ 
     *
     *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
     *
     *  @code
     *
     *   int result = Epsilon_<X,Y,Z,T>::value ;
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
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<I,I,J,K> { enum { value = 0 } ; }   ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<I,J,I,K> : public Epsilon_<I,I,J,K> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<I,J,K,I> : public Epsilon_<I,I,J,K> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<J,I,K,I> : public Epsilon_<I,I,J,K> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<J,I,I,K> : public Epsilon_<I,I,J,K> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J, unsigned int K> 
    struct Epsilon_<J,K,I,I> : public Epsilon_<I,I,J,K> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,I,J,J> { enum { value = 0 } ; } ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,J,I,J> : public Epsilon_<I,I,J,J> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,J,J,I> : public Epsilon_<I,I,J,J> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,I,I,J> { enum { value = 0 } ; } ;
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,I,J,I> : public Epsilon_<I,I,I,J> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<I,J,I,I> : public Epsilon_<I,I,I,J> {} ;
    /// stopping criteria for compile-time recursion
    template <unsigned int I, unsigned int J> 
    struct Epsilon_<J,I,I,I> : public Epsilon_<I,I,I,J> {} ;
    // ========================================================================
    /// stopping criteria for compile-time recursion
    template <unsigned int I> 
    struct Epsilon_<I,I,I,I> { enum { value = 0 } ; } ;
    // ========================================================================
    /// stopping criteria for compile-time recursion    
    template <>
    struct Epsilon_<X,Y,Z,T> { enum { value = 1 } ; } ;
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
    //  ========================================================================
    /// the generic evaluation of Levy-Chivita elements
    template <unsigned int I , unsigned int J ,
              unsigned int K , unsigned int L >
    struct Epsilon_ 
    {
    private:      
      // ======================================================================
      // helper types for permutations 
      typedef Epsilon_<J,I,K,L>  _12 ; // permute the 1st and the 2nd index 
      typedef Epsilon_<I,K,J,L>  _23 ; // permute the 2nd and the 3rd index 
      typedef Epsilon_<I,J,L,K>  _34 ; // permute the 3rd and the 4th index 
      // ======================================================================
    private:
      // ======================================================================
      /// valid indices? 
      enum { valid = ( I != J ) && ( J != K ) && ( K != L )  && ( L != I ) } ;
      // ======================================================================      
    public:
      // ======================================================================
      /** compile-time recursion here!
       *  The most important lines: the actual compile-evaluation
       *  of Levy-Civita symbols 
       */
      enum { value = !valid ? 0 : 
             detail::_Value<(I<J),
             detail::_Value<(J<K),
             detail::_Value<(K<L),(L<LAST)?1:0,_34>::value,_23>::value,_12>::value } ;
      // ======================================================================
    };
    // ========================================================================
    /** @struct Epsilon1_ 
     *  (Compile-time) evaluation of the tensor product of 
     *  two Levy-Civita symbols:
     *
     *  The following identity has been used:
     * \f[ 
     *  \alpha^{IJK}_{LMN} = 
     *  \epsilon^{IJK\kappa}
     *  \epsilon_{LMN\kappa} = - 
     *  \begin{Vmatrix}
     *    \delta^{I}_{L} & \delta^{I}_{M} & \delta^{I}_{N} \\ 
     *    \delta^{J}_{L} & \delta^{J}_{M} & \delta^{J}_{N}  \\ 
     *    \delta^{K}_{L} & \delta^{K}_{M} & \delta^{K}_{N} 
     *  \end{Vmatrix} 
     * = \delta^{I}_{N}\delta^{J}_{M}\delta^{K}_{L}
     *  + \delta^{I}_{M}\delta^{J}_{L}\delta^{K}_{N}
     *  + \delta^{I}_{L}\delta^{J}_{N}\delta^{K}_{M}
     *  - \delta^{I}_{L}\delta^{J}_{M}\delta^{K}_{N}
     *  - \delta^{I}_{M}\delta^{J}_{N}\delta^{K}_{L}
     *  - \delta^{I}_{N}\delta^{J}_{L}\delta^{K}_{M}
     * \f] 
     *  
     *  @code
     *
     *  const int alpha_XYZXYE = Epsilon1_<X,Y,Z,X,Y,E>::value ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-28
     */
    template <unsigned int I, 
              unsigned int J, 
              unsigned int K, 
              unsigned int L,
              unsigned int M,
              unsigned int N>
    struct Epsilon1_ 
    {
      enum 
        {
          value = Delta_<I,N>::value * Delta_<J,M>::value * Delta_<K,L>::value 
          +       Delta_<I,M>::value * Delta_<J,L>::value * Delta_<K,N>::value 
          +       Delta_<I,L>::value * Delta_<J,N>::value * Delta_<K,M>::value 
          //
          -       Delta_<I,L>::value * Delta_<J,M>::value * Delta_<K,N>::value 
          -       Delta_<I,M>::value * Delta_<J,N>::value * Delta_<K,L>::value 
          -       Delta_<I,N>::value * Delta_<J,L>::value * Delta_<K,M>::value 
        } ;
    } ;
    // ========================================================================
    /** @struct Epsilon2_ 
     *  (Compile-time) evaluation of the tensor product of 
     *  two Levy-Civita symbols:
     *
     * \f[
     *  \alpha^{IJ}_{KL} = 
     *  \epsilon^{IJ\gamma\kappa}
     *  \epsilon_{KL\rho\kappa} = - 
     *  \begin{Vmatrix}
     *    \delta^{I}_{K} & \delta^{I}_{L} \\ 
     *    \delta^{J}_{K} & \delta^{J}_{L}  \\ 
     *  \end{Vmatrix} 
     * \f]
     *  
     *  @code
     *
     *  const int alpha_XYYE = Epsilon2_<X,Y,Y,E>::value ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-28
     */
    template <unsigned int I, 
              unsigned int J, 
              unsigned int K, 
              unsigned int L>
    struct Epsilon2_ 
    {
      enum 
        { value = -2 * ( Delta_<I,K>::value * Delta_<J,L>::value - 
                         Delta_<J,K>::value * Delta_<I,L>::value ) } ;
    } ;
    // ========================================================================
    /** @struct Epsilon
     *  Simple implementation of 4D Antisymmetric Levy-Civita symbols
     *   \f$ \epsilon_{\mu\nu\lambda\delta} \f$ and some related operations 
     *
     *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-0725
     */
    struct Epsilon 
    {
    public:
      // ======================================================================
      /** the major method for evaluation of elements of Levy-Civita tensor
       *   \f$ \epsilon_{\mu\nu\lambda\delta} \f$ 
       *  
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       *
       *  @code 
       *
       *   Epsilon e ;
       *
       *   int symbol = e(0,1,2,3) ;
       *
       *  @endcode
       * 
       *  @attention the evaluation could be rather CPU expensive, 
       *             please use the templated form if indices 
       *             are known at compile-time 
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
          const size_t l ) const 
      { return epsilon ( i , j , k , l ) ; }  
      // ======================================================================
      /** evaluate the tensor product: e*v
       * 
       *  \f$  t_{\mu\nu\lambda} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v = ... ;
       *
       *   Epsilon e ;
       *
       *   const double t_XYE = e ( X , Y , E , v ) ;
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{xyzt} = 1 \f$ 
       *
       *  @attention the evaluation could be rather CPU expensive, 
       *             please use the templated form if indices 
       *             are known at compile-time 
       *  
       *  @see Epsilon::e_3 
       *
       *  @param mu     the first  index 
       *  @param nu     the second index 
       *  @param lambda the third  index 
       *  @param v the input vector 
       *  @return the product (component) 
       */
      inline double operator () 
        ( const size_t               mu     , 
          const size_t               nu     , 
          const size_t               lambda ,
          const LoKi::LorentzVector& v      ) const 
      { return epsilon ( mu , nu , lambda , v ) ; }
      // ======================================================================
      /** evaluate the tensor product: e*v1*v2
       * 
       *  \f$  v_{\mu\nu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\lambda}v_2^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_XY = e ( X , Y , v1 , v2 ) ;
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       *
       *  @attention the evaluation could be rather CPU expensive, 
       *             please use the templated form if indices 
       *             are known at compile-time 
       *
       *  @see Epsilon::e_2 
       *
       *  @param mu the first index 
       *  @param nu the second index 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @return the product (component) 
       */
      inline double operator() 
        ( const size_t               mu , 
          const size_t               nu , 
          const LoKi::LorentzVector& v1 , 
          const LoKi::LorentzVector& v2 ) const 
      { return epsilon ( mu , nu , v1 , v2 ) ; }
      // ======================================================================
      /** evaluate the e*v1*v2*v3 product
       * 
       *  \f$  v_{\mu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\nu}v_2^{\lambda}v_3^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_X = e ( X , v1 , v2 , v3 ) ; 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       *
       *  @param mu the first index 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @return the product (component) 
       */
      inline double operator() 
        ( const size_t               mu ,
          const LoKi::LorentzVector& v1 ,
          const LoKi::LorentzVector& v2 , 
          const LoKi::LorentzVector& v3 ) const 
      { return epsilon ( mu , v1 , v2 , v3 ) ; }
      // ======================================================================
      /** evaluate the e*v1*v2*v3 product
       * 
       *  \f$  v_{\mu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\nu}v_2^{\lambda}v_3^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const LorentzVector = e ( v1 , v2 , v3 ) ; 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{xyzt} = 1 \f$ 
       *
       *  The following identity holds numerically:
       *  @code
       *  
       *   const LorentzVector& v1  = ... ;
       *   const LorentzVector& v2  = ... ;
       *   const LorentzVector& v3  = ... ;
       * 
       *   Epsilon e ;
       *
       *   // NUMERICAL INDENTITY: 
       *
       *   Assert( e ( v1 , v2 , v3 , v4 ) == v1.Dot( e ( v2 , v3 , v4 ) ) , ... ) ;
       *
       *  @endcode 
       *
       *  @see Epsilon::e_1 
       *
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @return the product (vector)
       */
      inline LorentzVector operator() 
        ( const LoKi::LorentzVector& v1 ,
          const LoKi::LorentzVector& v2 , 
          const LoKi::LorentzVector& v3 ) const 
      { return epsilon ( v1 , v2 , v3 ) ; }
      // ======================================================================
      /** evaluate the e*v1*v2*v3*v4 product
       * 
       *  \f$  r = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *       v_1^{\mu}
       *       v_2^{\nu}
       *       v_3^{\lambda}
       *       v_4^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *   const LorentzVector& v4 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_X = e ( v1 , v2 , v3 , v4 ) ; 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  The following identity holds numerically:
       *  @code
       *  
       *   const LorentzVector& v1  = ... ;
       *   const LorentzVector& v2  = ... ;
       *   const LorentzVector& v3  = ... ;
       *   const LorentzVector& v4  = ... ;
       * 
       *   Epsilon e ;
       *
       *   // NUMERICAL INDENTITY: 
       *
       *   Assert( e ( v1 , v2 , v3 , v4 ) == v1.Dot( e ( v2 , v3 , v4 ) ) , ... ) ;
       *
       *  @endcode 
       *
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @param v3 the fourth vector 
       *  @return the product (vector)
       */
      inline double operator() 
        ( const LoKi::LorentzVector& v1 ,
          const LoKi::LorentzVector& v2 , 
          const LoKi::LorentzVector& v3 ,
          const LoKi::LorentzVector& v4 ) const 
      { return epsilon ( v1 , v2 , v3 , v4 ) ; }
      // ======================================================================
      /* evaluate the tensor product: (e*v1*v2*v3)*(e*v4*v5*v6)
       * 
       *  \f$  r = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *  \epsilon_{\mu\rho\delta\tau}
       *      v_1^{\nu}v_2^{\lambda}v_3^{\kappa} 
       *      v_4^{\rho}v_5^{\delta}v_6^{\tau}   \f$
       *
       *  This expression typically appears in evaution of 
       *  various "plane-angles".
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *   const LorentzVector& v4 = ... ;
       *   const LorentzVector& v5 = ... ;
       *   const LorentzVector& v6 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double r = e ( v1 , v2 , v3 , v4 , v5 , v6 ) ; 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @param v4 the fourth vector 
       *  @param v5 the fith   vector 
       *  @param v6 the sixth  vector 
       *  @return the tensor product
       */
      double operator() 
        ( const LoKi::LorentzVector& v1 , 
          const LoKi::LorentzVector& v2 , 
          const LoKi::LorentzVector& v3 , 
          const LoKi::LorentzVector& v4 , 
          const LoKi::LorentzVector& v5 , 
          const LoKi::LorentzVector& v6 ) const 
      { return epsilon ( v1 , v2 , v3 , v4 , v5 , v6 ) ; }
      // =====================================================================
    public:
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
       *  @attention The evaluation could  be rather time consuming, 
       *             If the indices are known at compile time, try to use 
       *             the templated methods - they are much efficient 
       *  
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
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
      /** evaluate the tensor product e*v1
       * 
       *  \f$  t_{\mu\nu\lambda} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v = ... ;
       *
       *   Epsilon e ;
       *
       *   const double t_XYE = e.e_1<X,Y,E>( v ) 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @see Epsilon::e_3
       *
       *  @param v the input vector 
       *  @return the product (component) 
       */      
      inline double epsilon 
      ( const size_t               mu     , 
        const size_t               nu     , 
        const size_t               lambda , 
        const LoKi::LorentzVector& v      ) const 
      {
        //
        if ( mu == nu || nu == lambda || lambda == mu ) { return 0 ; }
        if ( mu >  nu     ) { return -epsilon ( nu , mu     , lambda , v ) ; }
        if ( nu >  lambda ) { return -epsilon ( mu , lambda , nu     , v ) ; }
        //
        // take into account Minkowski metric:
        const double x = -v.Px () ;
        const double y = -v.Py () ;
        const double z = -v.Pz () ;
        const double e =  v.E  () ;
        //
        return -x * epsilon ( 0  , mu , nu     , lambda )   //  3 permutations 
          +     y * epsilon ( mu ,  1 , nu     , lambda )   //  2 permutations 
          -     z * epsilon ( mu , nu ,  2     , lambda )   //  1 permutation
          +     e * epsilon ( mu , nu , lambda ,      3 ) ; // no permitations
      }
      // ======================================================================
      /** evaluate the tensor e*v1*v2 product
       * 
       *  \f$  v_{\mu\nu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *       v_1^{\lambda}v_2^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_XY = e.e_2<X,Y>( v1 , v2 ) 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @see Epsilon::e_2
       *
       *  @param mu the first index 
       *  @param nu the second index 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @return the product (component) 
       */
      double epsilon 
      ( const size_t               mu , 
        const size_t               nu , 
        const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 ) const ;
      // ======================================================================
      /** evaluate the e*v1*v2*v3 product
       * 
       *  \f$  v_{\mu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\nu}v_2^{\lambda}v_3^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_X = e ( Epsilon::X , v1 , v2 , v3 ) 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @see Epsilon::e_1
       *
       *  @param mu the index of the result 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @return the product (component) 
       */
      double epsilon 
      ( const size_t                i ,  
        const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 , 
        const LoKi::LorentzVector& v3 ) const ;
      // ======================================================================
      /** evaluate the e*v1*v2*v3 product
       * 
       *  \f$  v_{\mu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *   v_1^{\nu}v_2^{\lambda}v_3^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const LorentzVector v = e ( v1 , v2 , v3 ) 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @see Epsilon::e_1
       *
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @return the product vector 
       */
      LorentzVector epsilon 
      ( const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 , 
        const LoKi::LorentzVector& v3 ) const ;
      // ======================================================================
      /** evaluate the tensor product: e*v1*v2*v3*v4
       * 
       *  \f$  r = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *    v_1^{\mu}
       *    v_2^{\nu}
       *    v_3^{\lambda}
       *    v_4^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *   const LorentzVector& v4 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double r = e.epsilon ( v1 , v2 , v3 , v4 ) ; 
       *
       *  @endcode 
       *
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @param v4 the fourth vector 
       *  @return the tensor product
       */
      double epsilon 
      ( const LoKi::LorentzVector& v1 ,
        const LoKi::LorentzVector& v2 , 
        const LoKi::LorentzVector& v3 ,
        const LoKi::LorentzVector& v4 ) const ;
      // ======================================================================
      /** evaluate the tensor product: (e*a1*a2*a3)*(e*b1*b2*b3)
       * 
       *  \f$  r = 
       *  \epsilon_{\mu\nu\lambda\kappa}
       *  \epsilon_{\mu\rho\delta\tau}
       *          a_1^{\nu}a_2^{\lambda}a_3^{\kappa} 
       *          b_1^{\rho}b_2^{\delta}b_3^{\tau}   \f$
       *
       *  This expression typically appears in evalution of 
       *  various "plane-angles" (for this case a3=b3) 
       *
       *  @code 
       *  
       *   const LorentzVector& a1 = ... ;
       *   const LorentzVector& a2 = ... ;
       *   const LorentzVector& a3 = ... ;
       *   const LorentzVector& b1 = ... ;
       *   const LorentzVector& b2 = ... ;
       *   const LorentzVector& b3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double r = e.epsilon ( a1 , a2 , a3 , b1 , b2 , b3 ) ; 
       *
       *  @endcode 
       *
       *  The following identity has been used:
       * \f[ 
       *     \epsilon^{\alpha\beta\gamma\kappa}
       *     \epsilon_{\mu\mu\rho\kappa} = - 
       *  \begin{Vmatrix}
       *   \delta^{\alpha}_{\mu} & \delta^{\alpha}_{\nu} & \delta^{\alpha}_{\rho} \\ 
       *   \delta^{\beta}_{\mu}  & \delta^{\beta}_{\nu}  & \delta^{\beta}_{\rho}  \\ 
       *   \delta^{\gamma}_{\mu} & \delta^{\gamma}_{\nu} & \delta^{\gamma}_{\rho} 
       *  \end{Vmatrix} 
       * \f]
       *  
       *  Convention here: \f$ \epsilon_{0123} = \epsilon_{XYZT} = 1 \f$ 
       * 
       *  @param a1 the first  vector 
       *  @param a2 the second vector 
       *  @param a3 the third  vector 
       *  @param b1 the fourth vector 
       *  @param b2 the fith   vector 
       *  @param b3 the sixth  vector 
       *  @return the tensor product
       */
      double epsilon 
      ( const LoKi::LorentzVector& a1 , 
        const LoKi::LorentzVector& a2 , 
        const LoKi::LorentzVector& a3 , 
        const LoKi::LorentzVector& b1 , 
        const LoKi::LorentzVector& b2 , 
        const LoKi::LorentzVector& b3 ) const ;  
      // ======================================================================
    public:
      // ======================================================================     
      /** evaluate the magnitude of the "4-normal" 
       * 
       *  \f$ l^2 = L_{\mu}L^{\mu} 
       *      = \left( \epsilon_{\mu\nu\lambda\delta}
       *     a^{\nu}b^{\lambda}c^{\delta} \right)^2 
       *    = 
       *     \left(ab\right)c^2 +      
       *     \left(ac\right)b^2 +     
       *     \left(bc\right)a^2 -     
       *      a^2b^2c^2 - 2\left(ab\right)\left(bc\right)\left(ac\right)
       *   \f$ 
       *
       *  @attention For time-like input vectors, 
       *             the 4-normal is a space-like vector,  
       *             and therefore the result must be non-positive! 
       *
       *  @param a the first  vector 
       *  @param b the second vector 
       *  @param c the third  vector 
       *  @return the magnitude of 4-normal
       */
      double mag2 
      ( const LoKi::LorentzVector& a , 
        const LoKi::LorentzVector& b , 
        const LoKi::LorentzVector& c  ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate the tensor product e*v1
       * 
       *  \f$  t_{\mu\nu\lambda} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v = ... ;
       *
       *   Epsilon e ;
       *
       *   const double t_XYE = e.e_1<Epsilon::X,Epsilon::Y,Epsilon::E>( v ) 
       *
       *  @endcode 
       *
       *  @param v the input vector 
       *  @return the product (component) 
       */
      template <unsigned int I, unsigned int J, unsigned int K>
      inline double e_3 
      ( const LoKi::LorentzVector& v ) const 
      {
        // take Minkowski metric into account 
        const double x = -v.Px () ;
        const double y = -v.Py () ;
        const double z = -v.Pz () ;
        const double e =  v.E  () ;
        //
        return x * Epsilon_<I,J,K,X>::value 
          +    y * Epsilon_<I,J,K,Y>::value 
          +    z * Epsilon_<I,J,K,Z>::value 
          +    e * Epsilon_<I,J,K,E>::value ;
      }
      // ======================================================================
      /** evaluate the tensor e*v1*v2 product
       * 
       *  \f$  v_{\mu\nu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\lambda}v_2^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_XY = e.e_2<Epsilon::X,Epsilon::Y>( v1 , v2 ) 
       *
       *  @endcode 
       *
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @return the product (component) 
       */
      template <unsigned int I, unsigned int J> 
      inline double e_2
      ( const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 ) const 
      {
        
        const double x1 = -v1.Px () ;
        const double y1 = -v1.Py () ;
        const double z1 = -v1.Pz () ;
        const double e1 =  v1.E  () ;
        
        const double x2 = -v2.Px () ;
        const double y2 = -v2.Py () ;
        const double z2 = -v2.Pz () ;
        const double e2 =  v2.E  () ;
        
        return x1 * y2 * Epsilon_<I,J,X,Y>::value 
          +    x1 * z2 * Epsilon_<I,J,X,Z>::value
          +    x1 * e2 * Epsilon_<I,J,X,E>::value 
          //
          +    y1 * x2 * Epsilon_<I,J,Y,X>::value 
          +    y1 * z2 * Epsilon_<I,J,Y,Z>::value 
          +    y1 * e2 * Epsilon_<I,J,Y,E>::value 
          //
          +    z1 * x2 * Epsilon_<I,J,Z,X>::value 
          +    z1 * y2 * Epsilon_<I,J,Z,Y>::value 
          +    z1 * e2 * Epsilon_<I,J,Z,E>::value 
          //
          +    e1 * x2 * Epsilon_<I,J,E,X>::value 
          +    e1 * y2 * Epsilon_<I,J,E,Y>::value 
          +    e1 * z2 * Epsilon_<I,J,E,Z>::value ;
      }
      // ======================================================================
      /** evaluate the e*v1*v2*v3 product
       * 
       *  \f$  v_{\mu} = 
       *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\nu}v_2^{\lambda}v_3^{\kappa} \f$
       *
       *  @code 
       *  
       *   const LorentzVector& v1 = ... ;
       *   const LorentzVector& v2 = ... ;
       *   const LorentzVector& v3 = ... ;
       *
       *   Epsilon e ;
       *
       *   const double v_X = e.e_1<Epsilon::X>( v1 , v2 , v3 ) 
       *
       *  @endcode 
       *
       *  @param v1 the first  vector 
       *  @param v2 the second vector 
       *  @param v3 the third  vector 
       *  @return the product (component) 
       */
      template <unsigned int I> 
      inline double e_1 
      ( const LoKi::LorentzVector& v1 , 
        const LoKi::LorentzVector& v2 , 
        const LoKi::LorentzVector& v3 ) const 
      {
        const double x1 = -v1.Px () ;
        const double y1 = -v1.Py () ;
        const double z1 = -v1.Pz () ;
        const double e1 =  v1.E  () ;
        
        const double x2 = -v2.Px () ;
        const double y2 = -v2.Py () ;
        const double z2 = -v2.Pz () ;
        const double e2 =  v2.E  () ;
        
        const double x3 = -v3.Px () ;
        const double y3 = -v3.Py () ;
        const double z3 = -v3.Pz () ;
        const double e3 =  v3.E  () ;
        
        return x1 * y2 * z3 * Epsilon_<I,X,Y,Z>::value 
          +    x1 * y2 * e3 * Epsilon_<I,X,Y,E>::value  
          +    x1 * z2 * y3 * Epsilon_<I,X,Z,Y>::value  
          +    x1 * z2 * e3 * Epsilon_<I,X,Z,E>::value 
          +    x1 * e2 * y3 * Epsilon_<I,X,E,Y>::value 
          +    x1 * e2 * z3 * Epsilon_<I,X,E,Z>::value  
          //
          +    y1 * x2 * z3 * Epsilon_<I,Y,X,Z>::value  
          +    y1 * x2 * e3 * Epsilon_<I,Y,X,E>::value 
          +    y1 * z2 * x3 * Epsilon_<I,Y,Z,X>::value 
          +    y1 * z2 * e3 * Epsilon_<I,Y,Z,E>::value 
          +    y1 * e2 * x3 * Epsilon_<I,Y,E,X>::value 
          +    y1 * e2 * z3 * Epsilon_<I,Y,E,Z>::value 
          // 
          +    z1 * x2 * y3 * Epsilon_<I,Z,X,Y>::value 
          +    z1 * x2 * e3 * Epsilon_<I,Z,X,E>::value
          +    z1 * y2 * x3 * Epsilon_<I,Z,Y,X>::value
          +    z1 * y2 * e3 * Epsilon_<I,Z,Y,E>::value
          +    z1 * e2 * x3 * Epsilon_<I,Z,E,X>::value
          +    z1 * e2 * y3 * Epsilon_<I,Z,E,Y>::value 
          // 
          +    e1 * x2 * y3 * Epsilon_<I,E,X,Y>::value  
          +    e1 * x2 * z3 * Epsilon_<I,E,X,Z>::value 
          +    e1 * y2 * x3 * Epsilon_<I,E,Y,X>::value 
          +    e1 * y2 * z3 * Epsilon_<I,E,Y,Z>::value 
          +    e1 * z2 * x3 * Epsilon_<I,E,Z,X>::value 
          +    e1 * z2 * y3 * Epsilon_<I,E,Z,Y>::value  ;
      }
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Tensors 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TENSORS_H
// ============================================================================
