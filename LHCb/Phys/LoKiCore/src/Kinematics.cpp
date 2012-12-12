// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <climits>
#include <limits.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
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
#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT GenVector
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable 
  #pragma warning(push)
#endif
#include "Math/Boost.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC remark 1572
  #pragma warning(pop)
#endif
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
( const LoKi::ThreeVector& mom , 
  const LoKi::ThreeVector& dir ) 
{
  const double dmag2 = dir.Mag2() ;
  if ( 0 == dmag2 ) { return mom.R() ; }
  const LoKi::ThreeVector perp = mom - dir * ( mom.Dot( dir ) / dmag2 ) ;
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
  const double pd  = P.Dot  ( D ) ;       // P * D 
  const double pq  = P.Dot  ( Q ) ;       // P * Q 
  const double qd  = Q.Dot  ( D ) ;       // D * Q 
  const double mq2 = Q.M2   (   ) ;       // Q^2
  const double mp2 = P.M2   (   ) ;       // P^2
  const double md2 = D.M2   (   ) ;       // D^2
  
  const double value = 
    ( pq * pq - mq2 * mp2 ) * ( qd * qd - mq2 * md2 ) ;
  if ( 0 > value ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngle(P,Q,D):: invalid 4-momenta, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return ( pd * mq2 - pq * qd ) / ::sqrt ( value ) ;
}
// ============================================================================
/** simple function which evaluates the magnitude of 3-momentum 
 *  of particle "v" in the rest system of particle "M" 
 *
 *  \f$ \left|\vec{p}\right| 
 *     \sqrt{  \frac{\left(v\cdot M\right)^2}{M^2} -v^2} \f$
 * 
 *  @attention particle M must be time-like particle!
 *  @param v the vector to be checked 
 *  @param M the defintion of "rest"-system
 *  @return the magnitude of 3D-momentum of v in rest-frame of M 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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
      ( "LoKi::Kinematics::restMomentum():: rest-system is not time-like, reuturn InvalidMomentum" );
    return LoKi::Constants::InvalidMomentum ; 
  }
  const double vM = v.Dot(M) ;
  const double P2 = vM*vM / M2 - v.M2() ;
  if ( 0 >  P2 ) 
  { 
    LoKi::Report::Error 
      ( "LoKi::Kinematics::restMomentum():: P2 < 0, return InvalidMomentum" );
    return LoKi::Constants::InvalidMomentum ; 
  }
  return ::sqrt ( P2 ) ;
}
// ============================================================================
/*  simple function which evaluates the energy
 *  of particle "v" in the rest system of particle "M" 
 * 
 *  \f$ e = \frac{v\cdot M}{\sqrt{M^2}} \f$
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
      ( "LoKi::Kinematics::restEnergy():: rest-system is not time-like, return InvaildEnergy" );
    return LoKi::Constants::InvalidEnergy ; 
  }
  // evaluate the energy 
  return v.Dot( M ) / ::sqrt ( M2 ) ;
}
// ============================================================================
/** simple function to evaluate the cosine angle between 
 *  two directions (v1 and v2) in the rest system of M 
 *
 *  @attention the particle M must be time-like particle: M^2 > 0 ! 
 *  @param v1 the first vector 
 *  @param v2 the last vector 
 *  @param M  the defintion of rest-system
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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
      ( "LoKi::Kinematics::cosThetaRest():: rest-system is not time-like, return InvalidAngle" );
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
  if ( 0 >= p1p2_ ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosThetaRest():: (|p1|*|p2|)^2 < 0, return InvalidAngle" );
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
  // get the intermediate particles D & H 
  const LoKi::LorentzVector D ( d1 + d2 ) ;
  const LoKi::LorentzVector H ( h1 + h2 ) ;
  
  // Evaluator of various tensor expressions:
  LoKi::Tensors::Epsilon e ;
  
  // evaluate the length of normales :
  const double l1 = e.mag2 ( d1 , d2 , H ) ; // == | [d1,d2,M] |
  const double l2 = e.mag2 ( h1 , h2 , D ) ; // == | [h1,h2,M] | 
  
  if ( 0 <= l1 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosDecayAngleChi () : L1^2 >= 0, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosDecayAngleChi () : L2^2 >= 0, return InvaildAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  
  const double cosChi = 
    - e.epsilon ( d1 , d2 , H , h1 , h2 , D ) / ::sqrt ( l1 * l2 ) ;
  
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
  //
  const double cosChi = cosDecayAngleChi ( d1 , d2 , h1 , h2 ) ;
  if ( LoKi::Constants::InvalidAngle == cosChi )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi () : invalid cosine, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  const double sinChi = sinDecayAngleChi ( d1 , d2 , h1 , h2 ) ;  
  if ( LoKi::Constants::InvalidAngle == sinChi )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngleChi () : invalid   sine, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
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
  //  reconstrcut the intermediate particles 
  const LoKi::LorentzVector D ( d1 + d2 ) ;
  const LoKi::LorentzVector H ( h1 + h2 ) ;
  /// Mother Particle 
  const LoKi::LorentzVector M ( D  + H  ) ;
  // ========================================================================
  const double M2 = M.M2() ;
  if ( 0 >= M2 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi():: rest-system is not time-like, return InvalidAngle" ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  // ========================================================================
  // avaluator of various tensor expressions 
  LoKi::Tensors::Epsilon e ;
  // ========================================================================
  // get the length of 4-normales 
  const double l1 = e.mag2 ( d1 , d2 , H ) ; // == | [d1,d2,M] |
  const double l2 = e.mag2 ( h1 , h2 , D ) ; // == | [h1,h2,M] | 
  
  if ( 0 <= l1 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi () : L1^2 >= 0, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi () : L2^2 >= 0, return InvaildAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  
  // try to get the sine
  
  const double DH = D.Dot ( H ) ;
  
  const double var = e ( d1 , d2 , h1 , h2 ) * ( DH * DH - D.M2()*H.M2() ) ;
  
  const double HM    = H.Dot ( M ) ;
  const double p_H   = HM*HM - H.M2()*M2  ;
  if ( 0 >= p_H ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinDecayAngleChi () : |H| < 0, return InvaildAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  const double sinChi =  var / ::sqrt ( l1 * l2 * p_H ) ;
  //
  return sinChi ;  
}
// ============================================================================
/*  evaluate the Armenteros-Podolanski variable \f$\mathbf{\alpha}\f$,     
 *  @param d1  three momentum of the first  daughter particle 
 *  @param d2  three momentum of the second daughter particle 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-21 
 */
// ============================================================================
double LoKi::Kinematics::armenterosPodolanskiX 
( const LoKi::ThreeVector& d1 , 
  const LoKi::ThreeVector& d2 )
{
  return  ( d1.Mag2() - d2.Mag2() ) / ( d1 + d2 ).Mag2() ;
}
// ============================================================================
/*  evaluate the chi2 of the mass  \f$\chi^2_{M}\f$.
 *  @param mass  the nominal mass 
 *  @param mom   four momentum 
 *  @param cov   four momentum covariance matrix 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-23 
 */
// ============================================================================
double LoKi::Kinematics::chi2mass 
( const double               mass , 
  const LoKi::LorentzVector& mom  , 
  const Gaudi::SymMatrix4x4& cov  ) 
{ 
  const double dmass2 = mom.M2() - mass * mass ;
  
  // reduced part of the projection matrix 
  static Gaudi::Vector4 s_D ;
  
  // fill the reduced projection matrix 
  s_D [ 0 ] = -2 * mom.Px () ;
  s_D [ 1 ] = -2 * mom.Py () ;
  s_D [ 2 ] = -2 * mom.Pz () ;
  s_D [ 3 ] =  2 * mom.E  () ;
  
  // evaluate V_D     = (D*V*D^T)-1
  const double v_D    = 1.0 / ROOT::Math::Similarity ( s_D , cov ) ;
  
  // evaluate chi2 
  return dmass2*dmass2*v_D ;
}
// ============================================================================
/*  @evaluate the (cosine) of tansversity angle, e.g. for decay  
 *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
 *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
 *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
 * 
 *  
 *  The evaluation is performed using the explicit Lorentz-invariant 
 *  expression:
 *  \f[
 *   \cos \theta_{\mathrm{tr}} = 
 *    \frac{ \epsilon_{\mu\nu\lambda\kappa}
 *          d_1^{\mu}h_1^{\nu}h_2^{\lambda}L_H^{\kappa} }
 *    {
 *     \sqrt{  \left( d_1 \cdot D  \right) / D^2 - d_1^2 } 
 *     \sqrt{  - L_H^2 }      
 *    },     
 *  \f]
 * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
 *  \f$ 
 *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
 *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
 *  \f$, and \f$ D = d_1 + d_2 \f$. 
 *
 *  @param d1 (INPUT) the 4-momentum of positive lepton 
 *  @param d2 (INPUT) the 4-momentum of negative lepton 
 *  @param h1 (INPUT) the 4-momentum of positive kaon 
 *  @param h2 (INPUT) the 4-momentum of negative kaon  
 *  @return the cosine od the transversity angle    
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-02
 */
// ============================================================================
double LoKi::Kinematics::cosThetaTr 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  
  // get the intermediate compound particles D 
  const LoKi::LorentzVector D ( d1 + d2 ) ;
  
  // Evaluator of various tensor expressions:
  LoKi::Tensors::Epsilon e ;
  
  // evaluate the length of normales :
  const double l2 = e.mag2 ( h1 , h2 , D ) ; // == | [h1,h2,D] | 
  if ( 0 <= l2 )
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosThetaTh () : L^2 >= 0, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return  e.epsilon ( d1 , h1 , h2 , D ) /
    restMomentum ( d1 , D ) / ::sqrt ( -l2 ) ;
}
// ============================================================================
/** evaluate the sine of transverse angle phi, e.g. for decay  
 *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
 *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
 *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
 *    
 *  The evaluation is performed using the explicit Lorentz-invariant 
 *  expression as angle between the ``in-plane'' vector \f$q\f$, and 
 *  vector \f$H\f$ in rest frame of \f$D\f$, where 
 *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$, 
 *  the ``4-normal'' is defiend as 
 *  \f$  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
 *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
 *  \f$, \f$ D = d_1 + d_2 \f$, \f$ H = h_1 + h_2 \f$.
 *
 *  @see LoKi::Kinematics::cosThetaRest 
 *  @param d1 (INPUT) the 4-momentum of positive lepton 
 *  @param d2 (INPUT) the 4-momentum of negative lepton 
 *  @param h1 (INPUT) the 4-momentum of positive kaon 
 *  @param h2 (INPUT) the 4-momentum of negative kaon  
 *  @return the cosine of the transversity angle theta 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-02
 */
// ===========================================================================
double LoKi::Kinematics::cosPhiTr 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  
  // get the intermediate compound particles D & H
  const LoKi::LorentzVector D ( d1 + d2 ) ;
  const LoKi::LorentzVector H ( h1 + h2 ) ;
  
  // Evaluator of various tensor expressions:
  LoKi::Tensors::Epsilon e ;
  
  // get "4-normal", that is z-axis here 
  LoKi::LorentzVector l  = e ( h1 , h2 , D ) ;
  const double        l2 = l.M2() ;
  if ( l2 >= 0 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::cosPhiTr () : L^2 >= 0, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return cosThetaRest ( d1 - d1.Dot(l)/l2*l  , H  , D ) ;
}
// ============================================================================
/** evaluate the sine of transverse angle phi, e.g. for decay  
 *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
 *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
 *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
 *    
 *  The evaluation is performed using the explicit Lorentz-invariant 
 *  expression:
 *  \f[
 *   \sin \phi_{\mathrm{tr}} = 
 *    - frac { 
 *      \epsilon_{\mu\nu\lambda\kappa}
 *       d_1^{\mu}L_H^{\mu}D^{\lambda}H^{\kappa}
 *     }{
 *     \sqrt{ -L^2 }
 *     \sqrt{  D^2 }
 *     \sqrt{ \left( q \cdot D  \right) / D^2 - q^2 } 
 *     \sqrt{ \left( M \cdot D  \right) / D^2 - M^2 } 
 *     }
 *  \f] 
 * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
 *  \f$ 
 *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
 *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
 *  \f$,
 * \f$ D = d_1 + d_2 \f$,  
 * \f$ H = h_1 + h_2 \f$,  
 * \f$ M = D+ H \f$ and ``in-plane'' 4-vector \f$q\f$ is defined as 
 *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$.
 *
 *  @param d1 (INPUT) the 4-momentum of positive lepton 
 *  @param d2 (INPUT) the 4-momentum of negative lepton 
 *  @param h1 (INPUT) the 4-momentum of positive kaon 
 *  @param h2 (INPUT) the 4-momentum of negative kaon  
 *  @return the cosine of the transversity angle theta 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-02
 */
// ============================================================================
double LoKi::Kinematics::sinPhiTr 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  // get the intermediate compound particles D & H
  const LoKi::LorentzVector D ( d1 + d2 ) ;
  const LoKi::LorentzVector H ( h1 + h2 ) ;
  
  // Evaluator of various tensor expressions:
  LoKi::Tensors::Epsilon e ;
  
  // get "4-normal", that is z-axis here 
  LoKi::LorentzVector l  = e ( h1 , h2 , D ) ;
  const double        l2 = l.M2() ;
  if ( l2 >= 0 ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::sinPhiTr () : L^2 >= 0, return InvalidAngle" );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return - e ( d1 , l , D , H  ) 
    / ::sqrt         ( -l2     )
    / ::sqrt         (  D.M2() )
    / restMomentum   ( d1 - d1.Dot(l)/l2*l , D ) 
    / restMomentum   ( H + D  , D ) ; 
}
// ============================================================================
/** evaluate the transverse angle phi, \f$ \phi_{\mathrm{tr}}\f$, 
 *  e.g. for decay  
 *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
 *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
 *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
 *    
 *  The evaluation is performed using the explicit Lorentz-invariant 
 *  expression for \f$ \sin \phi_{\mathrm{tr}} \f$ and 
 *   \f$ \cos \phi_{\mathrm{tr}} \f$ and 
 * 
 *  @see LoKi::Kinematics::cosPhiTr 
 *  @see LoKi::Kinematics::sinPhiTr 
 *
 *  @param d1 (INPUT) the 4-momentum of positive lepton 
 *  @param d2 (INPUT) the 4-momentum of negative lepton 
 *  @param h1 (INPUT) the 4-momentum of positive kaon 
 *  @param h2 (INPUT) the 4-momentum of negative kaon  
 *  @return the cosine of the transversity angle theta 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-02
 */
// ============================================================================
double LoKi::Kinematics::anglePhiTr 
( const LoKi::LorentzVector& d1 , 
  const LoKi::LorentzVector& d2 , 
  const LoKi::LorentzVector& h1 , 
  const LoKi::LorentzVector& h2 ) 
{
  const double cosChi = cosPhiTr ( d1 , d2 , h1 , h2 ) ;
  const double sinChi = sinPhiTr ( d1 , d2 , h1 , h2 ) ;  
  //
  return ::atan2 ( sinChi , cosChi ) ;
}
// ============================================================================
namespace
{
  // ==========================================================================
  inline double adjust_phi ( double angle )
  {
    static const double s_180 = 180 * Gaudi::Units::degree ;
    static const double s_360 = 360 * Gaudi::Units::degree ;
    //
    while ( angle >      s_180 ) { angle -= s_360 ; }
    while ( angle < -1 * s_180 ) { angle += s_360 ; }
    //
    return angle ; 
  }
  // ==========================================================================  
}
// ============================================================================
/*  trivial function to calculate \f$ \Delta \eta \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return dr2 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::deltaEta 
( const LoKi::LorentzVector& p1 , 
  const LoKi::LorentzVector& p2 ) { return p1.Eta() - p2.Eta() ; }
// =============================================================================
/*  trivial function to calculate \f$ \Delta \phi \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return delta-phi
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// =============================================================================
GAUDI_API
double LoKi::Kinematics::deltaPhi
( const LoKi::LorentzVector& p1 , 
  const LoKi::LorentzVector& p2 )
{
  const double phi1 = std::atan2 ( p1.Py () , p1.Px () ) ;
  const double phi2 = std::atan2 ( p2.Py () , p2.Px () ) ;
  //
  return adjust_phi ( phi1 - phi2 ) ;
}
// ============================================================================
/*  trivial function to calculate \f$ \Delta R^2 \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return dr2 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::deltaR2 
( const LoKi::LorentzVector& p1 , 
  const LoKi::LorentzVector& p2 )
{
  //
  const double dphi = deltaPhi ( p1 , p2 ) ;
  const double deta = deltaEta ( p1 , p2 ) ;
  //
  return dphi * dphi + deta * deta ;
}
// ============================================================================
/*  trivial function to calculate Kullback-Liebler divergency 
 *  @param p1 the first  particle 
 *  @param c1 the first  covariance matrix 
 *  @param p2 the second particle 
 *  @param c2 the second covariance matrix 
 *  @return Kullback-Liebler divergency
 *  @thanks The code is provided by Matt Needham
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::kullback
( const LoKi::LorentzVector& p1 , 
  const Gaudi::SymMatrix4x4& c1 ,
  const LoKi::LorentzVector& p2 , 
  const Gaudi::SymMatrix4x4& c2 ) 
{
  return kullback 
    ( p1.Vect()                             , 
      c1.Sub<Gaudi::SymMatrix3x3> ( 0 , 0 ) , 
      p2.Vect()                             , 
      c2.Sub<Gaudi::SymMatrix3x3> ( 0 , 0 ) ) ;    
}
// ============================================================================
/*  trivial function to calculate Kullback-Liebler divergency 
 *  @param p1 the first  particle 
 *  @param c1 the first  covariance matrix 
 *  @param p2 the second particle 
 *  @param c2 the second covariance matrix 
 *  @return Kullback-Liebler divergency
 *  @thanks The code is provided by Matt Needham
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::kullback
( const LoKi::Vector3D&      p1 , 
  const Gaudi::SymMatrix3x3& c1 ,
  const LoKi::Vector3D&      p2 ,
  const Gaudi::SymMatrix3x3& c2 ) 
{
  //
  if ( ( &p1 == &p2 || p1 == p2 ) && 
       ( &c1 == &c2 || c1 == c2 ) ) { return 0 ; } // the same ?  
  //
  Gaudi::SymMatrix3x3 c_1 ( c1 ) ;
  Gaudi::SymMatrix3x3 c_2 ( c2 ) ;
  //
  c_1.Invert() ;
  c_2.Invert() ;
  //
  const Gaudi::SymMatrix3x3    diff ( c1  - c2  ) ;
  const Gaudi::SymMatrix3x3 invDiff ( c_2 - c_1 ) ;
  const Gaudi::SymMatrix3x3 invSum  ( c_2 + c_1 ) ;
  //
  return 
    Gaudi::Math::trace      (   diff * invDiff ) + 
    Gaudi::Math::Similarity ( invSum , p1 - p2 ) ;
}
// ============================================================================
/*  trivial function to calculate the "mass-distance"
 *  \f$ ( m^2_{12} - m^2_1 - m^2_2 ) / m^2_{12}\f$
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return mass-differened 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::deltaM2 
( const LoKi::LorentzVector& p1 , 
  const LoKi::LorentzVector& p2 ) 
{ 
  //
  const double m2 =  ( p1 + p2 ).M2() ;
  //
  return ( m2 - 2 * p1.M2() - 2 * p2.M2() ) / m2 ; 
}
// ============================================================================
/*  trivial function to calculate the delta-angle 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return the angle between vectors 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::deltaAlpha
( const LoKi::Vector3D& p1 , 
  const LoKi::Vector3D& p2 ) 
{
  const double val = p1.Unit().Dot( p2.Unit() ) ;
  return std::acos ( val ) ;
}
// ============================================================================
/*  trivial function to calculate the delta-angle 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return the angle between vectors 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::Kinematics::deltaAlpha
( const LoKi::LorentzVector& p1 , 
  const LoKi::LorentzVector& p2 )
{
  return deltaAlpha ( p1.Vect() , p2.Vect() ) ;
}



// ============================================================================
// The END 
// ============================================================================

