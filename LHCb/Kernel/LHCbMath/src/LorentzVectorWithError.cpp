// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LorentzVectorWithError.h"
#include "LHCbMath/MatrixTransforms.h"
#include "LHCbMath/SymPosDefMatrixInverter.h"
#include "LHCbMath/Kinematics.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
/** @file
 *  Implementation file for class Gaudi::Math::LorentVectorWithError
 *  @date 2009-06-12 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor for lorent vector and convariance
// ============================================================================
Gaudi::Math::LorentzVectorWithError::LorentzVectorWithError 
( const Gaudi::LorentzVector&                            value , 
  const Gaudi::Math::LorentzVectorWithError::Covariance& cov2  ) 
  : Gaudi::LorentzVector ( value ) 
  , m_cov2 ( cov2 ) 
{}
// ============================================================================
// constructor for lorent vector and convariance
// ============================================================================
Gaudi::Math::LorentzVectorWithError::LorentzVectorWithError 
( const Gaudi::Math::LorentzVectorWithError::Covariance& cov2  , 
  const Gaudi::LorentzVector&                            value )
  : Gaudi::LorentzVector ( value ) 
    , m_cov2 ( cov2 ) 
{}
// ============================================================================
// constructor for lorent vector and convariance
// ============================================================================
Gaudi::Math::LorentzVectorWithError::LorentzVectorWithError 
( const Gaudi::Math::LorentzVectorWithError::Vector&     value , 
  const Gaudi::Math::LorentzVectorWithError::Covariance& cov2  ) 
  : Gaudi::LorentzVector () 
  , m_cov2 ( cov2 ) 
{
  SetPx ( value[0] ) ;
  SetPy ( value[1] ) ;
  SetPz ( value[2] ) ;
  SetE  ( value[3] ) ;
}
// ============================================================================
// constructor for lorent vector and convariance
// ============================================================================
Gaudi::Math::LorentzVectorWithError::LorentzVectorWithError 
( const Gaudi::Math::LorentzVectorWithError::VectorE& value )
  : Gaudi::LorentzVector () 
  , m_cov2 ( value.cov2 () ) 
{
  SetPx ( value[0] ) ;
  SetPy ( value[1] ) ;
  SetPz ( value[2] ) ;
  SetE  ( value[3] ) ;
}
// ============================================================================/
void Gaudi::Math::LorentzVectorWithError::setValue 
( const Gaudi::Math::LorentzVectorWithError::Vector& v ) 
{
  SetPx ( v [0] ) ;
  SetPy ( v [1] ) ;
  SetPz ( v [2] ) ;
  SetE  ( v [3] ) ;
}
// ============================================================================/
void Gaudi::Math::LorentzVectorWithError::setValue 
( const Gaudi::Math::LorentzVectorWithError::VectorE& v ) 
{
  setValue ( v.value() ) ;
  m_cov2 = v.cov2() ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator+= 
( const Gaudi::Math::LorentzVectorWithError& right ) 
{
  vector4d() += right.vector4d() ;
  m_cov2     += right.m_cov2 ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator-= 
( const Gaudi::Math::LorentzVectorWithError& right ) 
{
  vector4d() -= right.vector4d() ;
  m_cov2     += right.m_cov2 ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator+= 
( const Gaudi::Math::LorentzVectorWithError::VectorE& right ) 
{
  Gaudi::Math::add ( vector4d() , right.value () ) ;
  m_cov2     += right.cov2() ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator-= 
( const Gaudi::Math::LorentzVectorWithError::VectorE& right ) 
{
  Gaudi::Math::sub ( vector4d() , right.value () ) ;
  m_cov2     += right.cov2() ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator+= 
( const Gaudi::Math::LorentzVectorWithError::Vector& right ) 
{
  Gaudi::Math::add ( vector4d() , right ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator-= 
( const Gaudi::Math::LorentzVectorWithError::Vector& right ) 
{
  Gaudi::Math::sub ( vector4d() , right ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator+= 
( const Gaudi::LorentzVector& right ) 
{
  vector4d() += right ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator-= 
( const Gaudi::LorentzVector& right ) 
{
  vector4d() -= right ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator*= ( const double v ) 
{
  vector4d() *= v ;
  m_cov2     *= (v*v) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::operator/= ( const double v ) 
{
  vector4d() /= v ;
  m_cov2     /= (v*v) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__add__
( const Gaudi::Math::LorentzVectorWithError& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__sub__
( const Gaudi::Math::LorentzVectorWithError& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__add__
( const Gaudi::LorentzVector& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__add__
( const Gaudi::Math::LorentzVectorWithError::VectorE& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__add__
( const Gaudi::Math::LorentzVectorWithError::Vector& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__sub__
( const Gaudi::LorentzVector& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__sub__
( const Gaudi::Math::LorentzVectorWithError::VectorE& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__sub__
( const Gaudi::Math::LorentzVectorWithError::Vector& right ) const 
{
  LorentzVectorWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError
Gaudi::Math::LorentzVectorWithError::__rsub__
( const Gaudi::LorentzVector& right )  const 
{
  return LorentzVectorWithError( right - vector4d() , cov2() ) ;
}
// ============================================================================
// printout 
// ============================================================================
namespace 
{
  inline double err ( double cov ) 
  { return 0 <= cov ? std::sqrt ( cov ) : -std::sqrt(-cov) ; } 
}
// ============================================================================
std::ostream& 
Gaudi::Math::LorentzVectorWithError::fillStream ( std::ostream& s ) const // printout 
{
  return s << "[" << "( " 
           << X () << " +- " << err ( m_cov2(0,0) ) << " , "
           << Y () << " +- " << err ( m_cov2(1,1) ) << " , "
           << Z () << " +- " << err ( m_cov2(2,2) ) << " ), "
           << E () << " +- " << err ( m_cov2(3,3) ) << " ]";
}  
// ============================================================================
// conversion to the string 
// ============================================================================
std::string
Gaudi::Math::LorentzVectorWithError::toString   () const // conversion to the string 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2 
( const Gaudi::Math::LorentzVectorWithError& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  s_cov2 += right.cov2() ;
  // use Manuel's inverter:
  const bool ok = s_cov2.InvertChol() ;
  if  ( !ok ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( vector4d() - right.vector4d() , s_cov2 ) ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2
( const Gaudi::LorentzVector& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  // use Manuel's inverter:
  const bool ok = s_cov2.InvertChol() ;
  if  ( !ok ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( vector4d() - right , s_cov2 ) ;
}

// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2 
( const Gaudi::Math::LorentzVectorWithError::VectorE& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  s_cov2 += right.cov2() ;
  // use Manuel's inverter:
  const bool ok = s_cov2.InvertChol() ;
  if  ( !ok ) { return -1 ; }                                 // RETURN  
  /// calculate chi2
  Vector vct ;
  Gaudi::Math::geo2LA ( vector4d() , vct ) ;
  vct -= right.value() ;
  //
  return ROOT::Math::Similarity ( vct , s_cov2 ) ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2
( const Gaudi::Math::LorentzVectorWithError::Vector& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  // use Manuel's inverter:
  const bool ok = s_cov2.InvertChol() ;
  if  ( !ok ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  Vector vct ;
  Gaudi::Math::geo2LA ( vector4d() , vct ) ;
  vct -= right ;
  //
  return ROOT::Math::Similarity ( vct , s_cov2 ) ;
}
// ============================================================================
void Gaudi::Math::LorentzVectorWithError::asVector 
( Gaudi::Math::LorentzVectorWithError::Vector& data ) const 
{ Gaudi::Math::geo2LA ( vector4d() , data ) ; }
// ============================================================================
void Gaudi::Math::LorentzVectorWithError::asVector 
( Gaudi::Math::LorentzVectorWithError::VectorE& data ) const 
{
  Gaudi::Math::geo2LA ( vector4d() , data.value() ) ; 
  data.setCov2( cov2() ) ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError::VectorE 
Gaudi::Math::LorentzVectorWithError::asVector () const 
{
  Gaudi::Math::LorentzVectorWithError::VectorE data ;
  Gaudi::Math::geo2LA ( vector4d() , data.value() ) ;
  data.setCov2( cov2() ) ;
  return data ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError::Vector 
Gaudi::Math::LorentzVectorWithError::asVector4 () const 
{
  Gaudi::Math::LorentzVectorWithError::Vector data ;
  Gaudi::Math::geo2LA ( vector4d() , data ) ;
  return data ;
}
// ============================================================================

// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::__imul__ ( const double v ) 
{
  vector4d() *= v ;
  m_cov2     *= (v*v) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::LorentzVectorWithError&
Gaudi::Math::LorentzVectorWithError::__idiv__ ( const double v ) 
{
  vector4d() /= v ;
  m_cov2     /= (v*v) ;
  return *this ;
}
// ============================================================================

// ============================================================================
Gaudi::Math::LorentzVectorWithError 
Gaudi::Math::LorentzVectorWithError::__mul__ ( const double v ) const
{
  Gaudi::Math::LorentzVectorWithError tmp (*this) ;
  return ( tmp *= v ) ;
}
// ============================================================================

// ============================================================================
Gaudi::Math::LorentzVectorWithError 
Gaudi::Math::LorentzVectorWithError::__div__ ( const double v ) const
{
  Gaudi::Math::LorentzVectorWithError tmp (*this) ;
  return ( tmp /= v ) ;
}
// ============================================================================

// ============================================================================
// set of helper functions 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// almost zero  ? 
  const LHCb::Math::Zero<double>      s_zero  ;
  /// almost equal ? 
  const LHCb::Math::Equal_To<double>  s_equal ;
  // ==========================================================================
}
// ============================================================================
/*  calculate the mass with uncertainty 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return mass with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::mass 
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double m = mom.M() ;
  return 
    m <= 0 || s_zero ( m ) ? 
    Gaudi::Math::ValueWithError ( m ) : 
    Gaudi::Math::ValueWithError ( m , Gaudi::Math::sigma2mass ( mom , cov ) ) ;
}
// ============================================================================
/*  calculate the scalar momentum (p) with uncertainty 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return p with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::momentum 
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double p = mom.P() ;
  return 
    p <= 0 || s_zero ( p ) ?  
    Gaudi::Math::ValueWithError ( p ) : 
    Gaudi::Math::ValueWithError ( p , Gaudi::Math::sigma2p ( mom , cov ) ) ;
}
// ============================================================================
/*  calculate the rapidity (y) with uncertainty 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return y with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::rapidity 
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double y  = mom.Rapidity() ;
  const double e  = std::abs ( mom.E  ()  ) ;
  const double pz = std::abs ( mom.Pz ()  ) ;
  return 
    e <= pz || s_equal ( e , pz  ) ?  
    Gaudi::Math::ValueWithError ( y ) : 
    Gaudi::Math::ValueWithError ( y , Gaudi::Math::sigma2y ( mom , cov ) ) ;  
}
// ============================================================================
/*  calculate the pseudo-rapidity (eta) with uncertainty 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return eta with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::pseudorapidity 
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double eta = mom.Eta   () ;
  const double pt2 = mom.Perp2 () ;
  if ( 0 >= pt2 || s_zero ( pt2 ) ) { return Gaudi::Math::ValueWithError ( eta ) ; }
  const double p   = mom.P     () ;
  if ( 0 >= p   || s_zero ( p   ) ) { return Gaudi::Math::ValueWithError ( eta ) ; }
  //
  const double pz  = mom.Pz    () ;
  const double c   = - pz / ( pt2 * p ) ;
  //
  // get the vector d(Eta)/dp_i :
  ROOT::Math::SVector<double,4> dEtadP_i;
  dEtadP_i [0] =  c * mom.Px() ;
  dEtadP_i [1] =  c * mom.Py() ;
  dEtadP_i [2] =  1 / p ;
  dEtadP_i [3] =  0.0 ;
  //
  const double s2eta = ROOT::Math::Similarity ( cov , dEtadP_i ) ;
  return 
    s2eta <= 0 || s_zero ( s2eta )  ? 
    Gaudi::Math::ValueWithError ( eta         ) : 
    Gaudi::Math::ValueWithError ( eta , s2eta ) ;
}
// ============================================================================
/*  calculate the transverse momentum (pt) with uncertainty 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return pt with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::transverseMomentum  
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double pt = mom.Pt () ;
  return 
    pt <= 0 || s_zero ( pt  ) ?  
    Gaudi::Math::ValueWithError ( pt ) : 
    Gaudi::Math::ValueWithError ( pt , Gaudi::Math::sigma2pt ( mom , cov ) ) ;  
}
// ============================================================================
/*  calculate the squared transverse mass  (mT2) with uncertainty 
 *  \f$ m_T^2 = e^2 - p_z^2  = m^2 + p_T^2 \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return mT2 with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::transverseMass2
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double mt2 = mom.Mt2() ;
  //
  // get the vector d(Mt2)/dp_i :
  ROOT::Math::SVector<double,4> dMt2dP_i;
  dMt2dP_i [0] =  0.0 ;
  dMt2dP_i [1] =  0.0 ;
  dMt2dP_i [2] = -2 * mom.Pz() ;
  dMt2dP_i [3] =  2 * mom.E () ;
  //
  const double s2mt2 = ROOT::Math::Similarity ( cov , dMt2dP_i ) ;
  return 
    s2mt2 <= 0 || s_zero ( s2mt2 )  ? 
    Gaudi::Math::ValueWithError ( mt2         ) : 
    Gaudi::Math::ValueWithError ( mt2 , s2mt2 ) ;
}
// ============================================================================
/*  calculate the transverse mass  (mT) with uncertainty 
 *  \f$ m_T  = \sqrt{e^2 - p_z^2}  = \sqrt{m^2 + p_T^2} \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return mT2 with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::transverseMass
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const Gaudi::Math::ValueWithError mt2 = transverseMass2 ( mom , cov ) ;
  return 
    !s_zero ( mt2.value() ) ?    Gaudi::Math::signed_sqrt ( mt2 ) :
    Gaudi::Math::ValueWithError ( LHCb::Math::signed_sqrt ( mt2.value() ) ) ;
}
// ============================================================================
/*  calculate the squared transverse energy (eT2) with uncertainty 
 *  \f$ e_T^2 = \frac{e^2p_t^2}{p^2} \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return eT2 with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::transverseEnergy2
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double et2 = mom.Et2() ;
  //
  const double p2  = mom.P2 () ;
  if ( 0 >= p2 || s_zero ( p2 ) ) { return Gaudi::Math::ValueWithError ( et2 ) ; }
  //
  const double pt2 = mom.Perp2() ;
  const double e   = mom.E    () ;
  const double e2  = e * e       ;
  const double c1  = 2 * e2 / p2 ;
  const double c2  = pt2    / p2 ;
  //
  // get the vector d(Et2)/dp_i :
  ROOT::Math::SVector<double,4> dEt2dP_i;
  dEt2dP_i [0] =  c1 * ( 1 - c2 ) * mom.Px () ;
  dEt2dP_i [1] =  c1 * ( 1 - c2 ) * mom.Py () ;
  dEt2dP_i [2] =  c1 * ( 0 - c2 ) * mom.Pz () ;
  dEt2dP_i [3] =  2 * e * c2 ;
  //
  const double s2et2 = ROOT::Math::Similarity ( cov , dEt2dP_i ) ;
  return 
    s2et2 <= 0 || s_zero ( s2et2 )  ? 
    Gaudi::Math::ValueWithError ( et2         ) : 
    Gaudi::Math::ValueWithError ( et2 , s2et2 ) ;
}
// ============================================================================
/** calculate the transverse energy  (eT) with uncertainty 
 *  \f$ e_T^2 = \frac{e p_t}{p} \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return eT with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::transverseEnergy
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const Gaudi::Math::ValueWithError et2 = transverseEnergy2 ( mom , cov ) ;
  return 
    !s_zero ( et2.value() ) ?    Gaudi::Math::signed_sqrt ( et2 ) :
    Gaudi::Math::ValueWithError ( LHCb::Math::signed_sqrt ( et2.value() ) ) ;
}
// ============================================================================
/*  calculate the kinetic energy  (eK) with uncertainty 
 *  \f$ e_K = e - m \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return eK with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::kineticEnergy
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double m2 = mom.M2() ;
  if ( 0 >= m2 || s_zero ( m2 ) ) 
  {
    const double c2ee = cov( 3,3 ) ;
    return 
      0 >= c2ee || s_zero ( c2ee ) ?  
      Gaudi::Math::ValueWithError ( mom.E()        ) : 
      Gaudi::Math::ValueWithError ( mom.E() , c2ee ) ;
  }
  const double m   = std::sqrt  ( m2 ) ;
  const double eK  = mom.E() - m       ;
  //
  // get the vector d(Ek)/dp_i :
  ROOT::Math::SVector<double,4> dEkdP_i;
  dEkdP_i [0] =      mom.Px() / m ;
  dEkdP_i [1] =      mom.Py() / m ;
  dEkdP_i [2] =      mom.Pz() / m ;
  dEkdP_i [3] =  1 - mom.E () / m ;
  //
  const double s2ek = ROOT::Math::Similarity ( cov , dEkdP_i ) ;
  return 
    s2ek <= 0 || s_zero ( s2ek )  ? 
    Gaudi::Math::ValueWithError ( eK        ) : 
    Gaudi::Math::ValueWithError ( eK , s2ek ) ;
}
// ============================================================================
/* calculate the phi (asymuthal angle) with uncertainy 
 *  \f$ \tan \phi = \frac{p_y}{p_x} \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return phi with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::phi
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double phi = mom.Phi  () ;
  const double pt2 = mom.Perp2() ;
  //
  if ( 0 >= pt2 || s_zero ( pt2 ) ) { return Gaudi::Math::ValueWithError ( phi ) ; }
  //
  // get the vector d(Phi)/dp_i :
  ROOT::Math::SVector<double,4> dPhidP_i;
  dPhidP_i [0] = - mom.Py() / pt2 ;
  dPhidP_i [1] =   mom.Px() / pt2 ;
  dPhidP_i [2] =   0.0 ;
  dPhidP_i [3] =   0.0 ;
  //
  const double s2phi = ROOT::Math::Similarity ( cov , dPhidP_i ) ;
  return 
    s2phi <= 0 || s_zero ( s2phi )  ? 
    Gaudi::Math::ValueWithError ( phi         ) : 
    Gaudi::Math::ValueWithError ( phi , s2phi ) ;
}
// ============================================================================
/*  calculate the theta (polar angle) with uncertainy 
 *  \f$ \tan \theta = \frac{p_T}{p_z} \f$ 
 *  @param mom 4-momentum 
 *  @param cov covariance 
 *  @return theta with uncertainty 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::Kinematics::theta
( const Gaudi::LorentzVector& mom , 
  const Gaudi::SymMatrix4x4&  cov ) 
{
  const double theta = mom.Theta  () ;
  const double pt2   = mom.Perp2  () ;
  if ( 0 >= pt2 || s_zero ( pt2 ) ) 
  { return Gaudi::Math::ValueWithError ( theta ) ; }
  const double p2    = mom.P2     () ;
  if ( 0 >= p2  || s_zero ( p2  ) ) 
  { return Gaudi::Math::ValueWithError ( theta ) ; }
  //
  const double pt    = mom.Pt     () ;
  const double pz    = mom.Pz     () ;
  const double c     = pz / ( p2 * pt ) ;
  //
  // get the vector d(Theta)/dp_i :
  ROOT::Math::SVector<double,4> dThetadP_i;
  dThetadP_i [0] =  c * mom.Px() ;
  dThetadP_i [1] =  c * mom.Py() ;
  dThetadP_i [2] =  - pt / p2    ;
  dThetadP_i [3] =  0.0 ;
  //
  const double s2theta = ROOT::Math::Similarity ( cov , dThetadP_i ) ;
  return 
    s2theta <= 0 || s_zero ( s2theta )  ? 
    Gaudi::Math::ValueWithError ( theta           ) : 
    Gaudi::Math::ValueWithError ( theta , s2theta ) ;
}


// ============================================================================
// The END 
// ============================================================================

