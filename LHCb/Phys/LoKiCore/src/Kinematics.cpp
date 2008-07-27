// $Id: Kinematics.cpp,v 1.9 2008-07-27 18:19:27 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <climits>
#include <limits.h>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
#include "LoKi/Tensors.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "Math/Boost.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::Kinematics
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
/*  simple function for evaluation of the euclidiam norm
 *  for LorentzVectors 
 *  (E**2+Px**2+Py**2+Pz**2)
 *  @param vct the vector
 *  @return euclidian norm squared   
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::euclidianNorm2 
( const LoKi::LorentzVector& vct ) 
{
  return 
    vct.e() * vct.e() + 
    vct.x() * vct.x() + 
    vct.y() * vct.y() + 
    vct.z() * vct.z() ;
} 
// ============================================================================
/*  simple function for evaluation of the euclidiam norm
 *  for LorentzVectors 
 *  sqrt(E**2+Px**2+Py**2+Pz**2)
 *  @param vct the vector
 *  @return euclidian norm    
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::euclidianNorm
( const LoKi::LorentzVector& vct ) 
{ return sqrt ( euclidianNorm2 ( vct )  ) ; } 
// ============================================================================
/*  simple function for evaluation of the square of 
 *  the euclidian distance inbetwee 2 LorentzVectors 
 *  (DeltaE**2+DeltaPx**2+DeltaPy**2+DeltaPz**2)
 *  @param vct1 the first vector
 *  @param vct2 the second vector
 *  @return the square of euclidian distance 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::delta2euclidian 
( const LoKi::LorentzVector& vct1 , 
  const LoKi::LorentzVector& vct2 ) 
{ return euclidianNorm2 ( vct1 - vct2 ) ; } 
// ============================================================================
/*  simple function which evaluates the transverse 
 *  momentum with respect a certain 3D-direction
 *  @param mom the momentum
 *  @param dir the direction
 *  @return the transverse moementum with respect to the direction
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::transverseMomentumDir 
( const LoKi::LorentzVector& mom , 
  const LoKi::ThreeVector&   dir ) 
{
  const double dmag2 = dir.Mag2() ;
  if ( 0 == dmag2 ) { return mom.P() ; }
  const LoKi::ThreeVector v( mom ) ;
  const LoKi::ThreeVector perp = v - dir * ( v.Dot( dir ) / dmag2 ) ;
  return perp.R() ;
}
// ============================================================================
/*  This routine returns the cosine angle theta 
 *  The decay angle calculated  is that between 
 *  the flight direction of the daughter neson, "D",
 *  in the rest frame of "M" (the parent of "D"), 
 *  with respect to the boost direction from 
 *  "M"'s rest frame 
 *  
 *  @param D 4-momentum of the daughter particle 
 *  @param M 4-momentum of mother particle 
 *  @return cosine of decay angle 
 *
 *  @see LoKi::LorentzVector.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-12-03
 */
// ============================================================================
double LoKi::Kinematics::decayAngle_
( const LoKi::LorentzVector& D , 
  const LoKi::LorentzVector& M ) 
{ 
  // boost the daugter particles to mother center of mass 
  ROOT::Math::Boost   boost ( M.BoostToCM() ) ;
  const LoKi::LorentzVector pD = boost( D ) ;
  // direction of 3-momentum of daughter in this rest frame 
  const LoKi::ThreeVector v3D =  LoKi::ThreeVector(pD).Unit() ;
  // mother direction
  const LoKi::ThreeVector v3M =  LoKi::ThreeVector( M).Unit()  ;
  // cosine 
  return v3D.Dot( v3M ) ;
}
// ============================================================================
/*  This routine returns the cosine angle theta 
 *  The decay angle calculated  is that between 
 *  the flight direction of the daughter neson, "D",
 *  in the rest frame of "Q" (the parent of "D"), 
 *  with respect to "Q"'s fligth direction in "P"'s
 *  (the parent of "Q") rest frame
 * 
 *  it is a EvtDecayAngle(P,Q,D) routine form EvtGen package
 *  
 *  @param D 4-momentum of the daughetr particle 
 *  @param Q 4-momentum of mother particle 
 *  @param P "rest frame system"
 *  @return cosine of decay angle 
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-12-03
 */
// ============================================================================
double LoKi::Kinematics::decayAngle
( const LoKi::LorentzVector& P , 
  const LoKi::LorentzVector& Q ,
  const LoKi::LorentzVector& D ) 
{
  const double pd  = P.Dot  ( D ) ;    // P * D 
  const double pq  = P.Dot  ( Q ) ;    // P * Q 
  const double qd  = Q.Dot  ( D ) ;    // D * Q 
  const double mq2 = Q.M2   () ;       // Q^2
  const double mp2 = P.M2   () ;       // P^2
  const double md2 = D.M2   () ;       // D^2
  
  const double value = 
    ( pq * pq - mq2 * mp2 ) * ( qd * qd - mq2 * md2 ) ;
  
  if ( 0 > value ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngle(P,Q,D):: invalid 4-momenta " );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return ( pd * mq2 - pq * qd ) / sqrt( value ) ;
}
// // ============================================================================
// /** This routine evaluates the cosine of "transversity angle", 
//  *  useful e.g. to disantangle the different partiasl vaves in 
//  *  0 -> 1 + 1 decay (e.g. Bs -> J/psi Phi 
//  *  
//  *  The code is kindly provided by Gerhard Raven 
//  * 
//  *  @param l1 4-vector of the first  particle, e.g. mu+
//  *  @param l2 4-vector of the second particle, e.g. mu- 
//  *  @param p1 4-vector of the third  particle, e.g. K+
//  *  @param p2 4-vector of the fourth particle, e.g. K- 
//  *  @return the cosine of transversity angle 
//  * 
//  *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//  *  @date 2004-12-03
//  */ 
// // ============================================================================
// double LoKi::Kinematics::transversityAngle 
// ( const LoKi::LorentzVector& l1 , 
//   const LoKi::LorentzVector& l2 , 
//   const LoKi::LorentzVector& p1 , 
//   const LoKi::LorentzVector& p2 ) 
// {
//   const LoKi::LorentzRotation 
//     bst ( - ( l1 + l2 ).boostVector() ) ;
//   return ( ( bst * p1 ).vect() ).cross( ( bst * p2 ).vect() ).cosTheta( ( bst * l1 ).vect() ) ;
// };
// // ============================================================================

// // ============================================================================
// /** This routine evaluated the angle theta_FB
//  *  used e.g. for evaluation of forward-backward 
//  *  asymmetry for decay B -> K* mu+ mu- 
//  *  The angle calculated is that 
//  *  between between the mu+ and K^*0 momenta 
//  *  in the di-muon rest frame
//  *  
//  *  The code is kindly provided by Helder Lopes 
//  *
//  *  @param  K  4-momenutm of   "K*"
//  *  @param  l1 4-momentum of the first  lepton
//  *  @param  l2 4-momentum of the second lepton
//  *  @return the cosine of theta_FB 
//  * 
//  *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//  *  @date 2004-12-03
//  */
// // ============================================================================
// double LoKi::Kinematics::forwardBackwardAngle
// ( const LoKi::LorentzVector& K  , 
//   const LoKi::LorentzVector& l1 , 
//   const LoKi::LorentzVector& l2 )
// {
//   // 
//   const LoKi::LorentzRotation bst( - ( l1 + l2 ).boostVector() ) ;
  
//   return ( ( bst * K ).vect() ).cosTheta( ( bst * l1 ).vect() ) ;
// };
// // ============================================================================

// ============================================================================
/** simple function which evaluates the magnitude of 3-momentum 
 *  of particle "v" in the rest system of particle "M" 
 *
 *  \f$ \left|\vec{p}\right| 
 *     \sqrt{  \frac{\left(v\cdotM\right)^2}{M^2} -v^2} \f$
 * 
 *  @attention particle M must be time-like particle!
 *  @param v the vector to be checked 
 *  @param M the defintion of "rest"-system
 *  @return the magnitude of 3D-momentum of v in rest-frame of M 
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-07-27
 */
// ============================================================================
double LoKi::Kinematics::restMomentum 
( const LoKi::LorentzVector& v , 
  const LoKi::LorentzVector& M ) 
{
  const double M2 = M.M2 () ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::restMomentum():: rest-system is not time-like" );
    return LoKi::Constants::InvalidMomentum ; 
  }
  const double vM = v.Dot(M) ;
  const double P2 = vM*vM / M2 - v.M2() ;
  if ( 0 >  P2 ) 
  { 
    LoKi::Report::Error 
      ( "LoKi::Kinematics::restMomentum():: P2 < 0 " );
    return LoKi::Constants::InvalidMomentum ; 
  }
  return ::sqrt ( P2 ) ;
}
// ============================================================================
/*  simple function which evaluates the energy
 *  of particle "v" in the rest system of particle "M" 
 * 
 *  \f$ e = \frac{v\cdotM}{\sqrt{M^2}} \f$
 *  
 *  @attention particle M must be time-like particle: M^2 > 0 !
 *  @param v the vector to be checked 
 *  @param M the defintion of "rest"-system
 *  @return the energy of v in rest-frame of M 
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-07-27
 */
// ============================================================================
double LoKi::Kinematics::restEnergy 
( const LoKi::LorentzVector& v , 
  const LoKi::LorentzVector& M ) 
{
  const double M2 = M.M2 () ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::restEnergy():: rest-system is not time-like" );
    return LoKi::Constants::InvalidEnergy ; 
  }
  // evaluate the energy 
  return v.Dot( M ) / ::sqrt ( M2 ) ;
}
// ============================================================================
/** simple function to evaluate the cosine angle between 
 *  two directions (v1 and v2) in the rest system of M 
 *
 *  \f$ \cos\theta = 
 *   \frac{\vec{p}_1\vec{p}_2}{\left|\vec{p}_1\right|
 *                             \left|\vec{p}_2\right|} = 
 *     \frac{1}{\left|\vec{p}_1\right|\left|\vec{p}_2\right|} 
 *     \left( E_1E_2 -\frac{1}{2}
 *   \left(\left(v_1+v_2\right)^2-v_1^2-v_2^2 \righ) \right) \f$ 
 *  
 *  \f$ E_1 E_2 = \frac{ \left(v_1\cdotM\righ)\keft(v_2\cdotM\right)}
 *    {M^2} \f$ and 
 *  \f$ {\left|\vec{p}_1\right|\left|\vec{p}_2\right| = 
 *   \sqrt{ 
 *   \left( \frac{\left(v_1\cdotM\right)^2}{M^2}-v_1^2 \right)   
 *   \left( \frac{\left(v_2\cdotM\right)^2}{M^2}-v_2^2 \right) } \f$ 
 * 
 *  @attention the particle M must be time-like particle: M^2 > 0 ! 
 *  @param v1 the first vector 
 *  @param v2 the last vector 
 *  @param M  the defintion of rest-system
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-07-27
 */
// ============================================================================
double LoKi::Kinematics::cosThetaRest 
( const LoKi::LorentzVector& v1 ,
  const LoKi::LorentzVector& v2 , 
  const LoKi::LorentzVector& M  ) 
{
  const double M2 = M.M2 () ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosThetaRest():: rest-system is not time-like" );
    return LoKi::Constants::InvalidAngle ; 
  }
  /// 
  const double v1M  = v1.Dot ( M ) ;
  const double v2M  = v2.Dot ( M ) ;
  const double m1_2 = v1.M2  ( )   ;
  const double m2_2 = v2.M2  ( )   ;
  
  // calculate e1*e2 
  const double e1e2  = v1M * v2M / M2 ;   // calculate e1*e2 
  
  // calculate (|p1|*|p2|)^2
  const double p1p2_ = 
    ( ( v1M * v1M ) / M2 - m1_2 ) *
    ( ( v2M * v2M ) / M2 - m2_2 ) ; // calculate (|p1|*|p2|)^2
  //
  if ( 0 > p1p2_ ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosThetaRest():: (|p1|*|p2|)^2 < 0 " );
    return LoKi::Constants::InvalidAngle ; 
  }
  //
  const double p1p2 = ::sqrt ( p1p2_ ) ;   // |p1|*|p2| 
  //
  const double var = (v1+v2).M2() - m1_2 - m2_2 ;
  //
  // finally evaluate the cosine 
  return ( e1e2 - 0.5 * var ) / p1p2 ;  
}
// ============================================================================
// evaluate cos(chi)
// ============================================================================
double LoKi::Kinematics::cosDecayAngleChi 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{ 
  // ========================================================================
  // Mother Particle 
  LoKi::LorentzVector M  (d1 ) ; M += d2 ; M += h1 ; M+= h2 ;
  // ========================================================================
  const double M2 = M.M2() ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosDecayAngleChi():: rest-system is not time-like" ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  // ========================================================================
  LoKi::Tensors::Epsilon e ;
  // ========================================================================
  // get the normales 
  const LoKi::LorentzVector L1 ( e ( d1 , d2 , M ) ) ;
  const LoKi::LorentzVector L2 ( e ( h1 , h2 , M ) ) ;
  // ========================================================================
  
  const double l1 = L1.M2() ;
  const double l2 = L2.M2() ;
  
  if ( 0 <= l1 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosDecayAngleChi ():: L1^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosDecayAngleChi ():: L2^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  
  // use simple expression for |L| and E :
  const double cosChi = 
    -0.5 * ( ( L1 + L2 ).M2() - l1 - l2 ) / ::sqrt ( l1 * l2 ) ;  
  //
  return cosChi ;
}
// ============================================================================
/*  evaluate the angle \f$\chi\f$ 
 *  beween two decay planes, 
 *  formed by particles v1&v2 and h1&h2 correspondingly. 
 *  The angle is evaluated in the rest frame 
 *  of "mother" particles (defined as v1+v2+h1+h2) 
 *      
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-27
 */
// ============================================================================
double LoKi::Kinematics::decayAngleChi 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  // ========================================================================
  // Mother Particle 
  LoKi::LorentzVector M  (d1 ) ; M += d2 ; M += h1 ; M+= h2 ;
  // ========================================================================
  const double M2 = M.M2() ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi():: rest-system is not time-like" ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  LoKi::Tensors::Epsilon e ;
  // ========================================================================
  // get the normales 
  const LoKi::LorentzVector L1 ( e ( d1 , d2 , M ) ) ;
  const LoKi::LorentzVector L2 ( e ( h1 , h2 , M ) ) ;
  // ========================================================================
  
  const double l1 = L1.M2() ;
  const double l2 = L2.M2() ;
  
  if ( 0 <= l1 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi ():: L1^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi ():: L2^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }

  // use simple expression for |L| and E :
  const double cosChi = 
    -0.5 * ( ( L1 + L2 ).M2() - l1 - l2 ) / ::sqrt ( l1 * l2 ) ;  
  
  // try to get the sine    
  
  const LoKi::LorentzVector h12 = h1 + h2 ;
  const double _sin  = - e ( L1 , L2 , h12 , M ) ;
  const double h12M  = h12.Dot ( M ) ;
  const double p_h12 = h12M*h12M / M2 - h12.M2() ;
  if ( 0 >= p_h12 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi ():: |h1+h2| < 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  const double sinChi =  _sin / sqrt( l1 * l2 * p_h12 * M2 ) ;
  //
  return ::atan2 ( sinChi , cosChi ) ;
}
// ============================================================================
double LoKi::Kinematics::sinDecayAngleChi 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  // ========================================================================
  // Mother Particle 
  LoKi::LorentzVector M  (d1 ) ; M += d2 ; M += h1 ; M+= h2 ;
  // ========================================================================
  const double M2 = M.M2() ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi():: rest-system is not time-like" ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  // ========================================================================
  LoKi::Tensors::Epsilon e ;
  // ========================================================================
  // get the normales 
  const LoKi::LorentzVector L1 ( e ( d1 , d2 , M ) ) ;
  const LoKi::LorentzVector L2 ( e ( h1 , h2 , M ) ) ;
  // ========================================================================
  
  const double l1 = L1.M2() ;
  const double l2 = L2.M2() ;
  
  if ( 0 <= l1 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi ():: L1^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi ():: L2^2 >= 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  
  // try to get the sine    
  
  const LoKi::LorentzVector h12 = h1 + h2 ;
  const double _sin  = - e ( L1 , L2 , h12 , M ) ;
  const double h12M  = h12.Dot ( M ) ;
  const double p_h12 = h12M*h12M / M2 - h12.M2() ;
  if ( 0 >= p_h12 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi ():: |h1+h2| < 0 " );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  const double sinChi =  _sin / sqrt( l1 * l2 * p_h12 * M2 ) ;
  //
  return sinChi ;  
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================

