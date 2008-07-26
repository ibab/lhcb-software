// $Id: Tensors.cpp,v 1.1 2008-07-26 17:19:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/KinTypes.h"
#include "LoKi/Tensors.h"
// ============================================================================
/** @file 
 *  Implementation file for classes from namespace LoKi::Tensors
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
 *  @date 2008-07-26
 */
// ============================================================================
double LoKi::Tensors::Epsilon::epsilon 
( const size_t               mu ,
  const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 , 
  const LoKi::LorentzVector& v3 ) const 
{
  if ( &v1 == &v2 || &v2 == &v3 || &v1 == &v3 ) { return 0  ; } // RETURN 
  switch ( mu ) 
  {
  case X  : return e_1<X> ( v1 , v2 , v3 ) ;   // RETURN 
  case Y  : return e_1<Y> ( v1 , v2 , v3 ) ;   // RETURN 
  case Z  : return e_1<Z> ( v1 , v2 , v3 ) ;   // RETURN 
  case E  : return e_1<E> ( v1 , v2 , v3 ) ;   // RETURN  
  default : return                     0   ;
  }
  //
  return 0 ;
}
// ============================================================================
/*  evaluate the tensor e*v1*v2 product
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
 *  @param mu the first index 
 *  @param nu the second index 
 *  @param v1 the first  vector 
 *  @param v2 the second vector 
 *  @return the product (component) 
 */
// ============================================================================
double LoKi::Tensors::Epsilon::epsilon 
( const size_t               mu , 
  const size_t               nu , 
  const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 ) const 
{
  if ( &v1 == &v2 ) { return 0 ; } // RETURN ;
  switch ( mu ) 
  {
  case    X :
    switch ( nu ) 
    {
    case X : return                   0 ;
    case Y : return e_2<X,Y>( v1 , v2 ) ;
    case Z : return e_2<X,Z>( v1 , v2 ) ;
    case E : return e_2<X,E>( v1 , v2 ) ;
    default: return                   0 ;      
    } 
  case    Y :
    switch ( nu ) 
    {
    case X : return e_2<Y,X>( v1 , v2 ) ;
    case Y : return                   0 ;
    case Z : return e_2<Y,Z>( v1 , v2 ) ;
    case E : return e_2<Y,E>( v1 , v2 ) ;
    default: return                   0 ;
    } 
  case    Z :
    switch ( nu ) 
    {
    case X : return e_2<Z,X>( v1 , v2 ) ;
    case Y : return e_2<Z,Y>( v1 , v2 ) ;
    case Z : return                   0 ;
    case E : return e_2<Z,E>( v1 , v2 ) ;
    default: return                   0 ;
    } 
  case    E :
    switch ( nu ) 
    {
    case X : return e_2<E,X>( v1 , v2 ) ;
    case Y : return e_2<E,Y>( v1 , v2 ) ;
    case Z : return e_2<E,Z>( v1 , v2 ) ;
    case E : return                   0 ;
    default: return                   0 ;
      } 
  default:
    return 0 ;
  }
  //
  return 0 ;
}
// ============================================================================
/*  evaluate the e*v1*v2*v3 product
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
 *   const LorentzVector v = e ( v1 , v2 , v3 ) 
 *
 *  @endcode 
 *
 *  @param v1 the first  vector 
 *  @param v2 the second vector 
 *  @param v3 the third  vector 
 *  @return the product vector 
 */
// ============================================================================
LoKi::LorentzVector LoKi::Tensors::Epsilon::epsilon 
( const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 , 
  const LoKi::LorentzVector& v3 ) const 
{
  if ( &v1 == &v2 || &v2 == &v3 || &v3 == &v1 ) { return LoKi::LorentzVector() ; }
  return LoKi::LorentzVector 
    ( e_1<X> ( v1 , v2 , v3 ) , 
      e_1<Y> ( v1 , v2 , v3 ) , 
      e_1<Z> ( v1 , v2 , v3 ) , 
      e_1<E> ( v1 , v2 , v3 ) ) ;                  
}
// ============================================================================
/** evaluate the e*v1*v2*v3 product
 * 
 *  \f$  r = 
 *  \epsilon_{\mu\nu\lambda\kappa}v_1^{\mu}
 *          v_2^{\nu}v_3^{\lambda}v_4^{\kappa} \f$
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
 *   const double v = e.epsilon ( v1 , v2 , v3 , v4 ) ; 
 *
 *  @endcode 
 *
 *  @param v1 the first  vector 
 *  @param v2 the second vector 
 *  @param v3 the third  vector 
 *  @param v3 the fourth vector 
 *  @return the product (vector)
 */
// ============================================================================
double LoKi::Tensors::Epsilon::epsilon 
( const LoKi::LorentzVector& v1 ,
  const LoKi::LorentzVector& v2 , 
  const LoKi::LorentzVector& v3 ,
  const LoKi::LorentzVector& v4 ) const 
{
  if ( &v1 == &v2 || &v1 == &v3 || &v1 == &v4 || 
       &v2 == &v3 || &v2 == &v4 || &v3 == &v4 ) { return 0 ; }
  
  // take into account Minkowski metric:
  
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
  
  const double x4 = -v4.Px () ;
  const double y4 = -v4.Py () ;
  const double z4 = -v4.Pz () ;
  const double e4 =  v4.E  () ;
  
  return x1 * y2 * z3 * e4 * Epsilon_<X,Y,Z,E>::value 
    +    x1 * y2 * e3 * z4 * Epsilon_<X,Y,E,Z>::value  
    +    x1 * z2 * y3 * e4 * Epsilon_<X,Z,Y,E>::value  
    +    x1 * z2 * e3 * y4 * Epsilon_<X,Z,E,Y>::value 
    +    x1 * e2 * y3 * z4 * Epsilon_<X,E,Y,Z>::value 
    +    x1 * e2 * z3 * y4 * Epsilon_<X,E,Z,Y>::value  
    //
    +    y1 * x2 * z3 * e4 * Epsilon_<Y,X,Z,E>::value  
    +    y1 * x2 * e3 * z4 * Epsilon_<Y,X,E,Z>::value 
    +    y1 * z2 * x3 * e4 * Epsilon_<Y,Z,X,E>::value 
    +    y1 * z2 * e3 * x4 * Epsilon_<Y,Z,E,X>::value 
    +    y1 * e2 * x3 * z4 * Epsilon_<Y,E,X,Z>::value 
    +    y1 * e2 * z3 * x4 * Epsilon_<Y,E,Z,X>::value 
    // 
    +    z1 * x2 * y3 * e4 * Epsilon_<Z,X,Y,E>::value 
    +    z1 * x2 * e3 * y4 * Epsilon_<Z,X,E,Y>::value
    +    z1 * y2 * x3 * e4 * Epsilon_<Z,Y,X,E>::value
    +    z1 * y2 * e3 * x4 * Epsilon_<Z,Y,E,X>::value
    +    z1 * e2 * x3 * y4 * Epsilon_<Z,E,X,Y>::value
    +    z1 * e2 * y3 * x4 * Epsilon_<Z,E,Y,X>::value 
    // 
    +    e1 * x2 * y3 * z4 * Epsilon_<E,X,Y,Z>::value  
    +    e1 * x2 * z3 * y4 * Epsilon_<E,X,Z,Y>::value 
    +    e1 * y2 * x3 * z4 * Epsilon_<E,Y,X,Z>::value 
    +    e1 * y2 * z3 * x4 * Epsilon_<E,Y,Z,X>::value 
    +    e1 * z2 * x3 * y4 * Epsilon_<E,Z,X,Y>::value 
    +    e1 * z2 * y3 * x4 * Epsilon_<E,Z,Y,X>::value  ; 
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
