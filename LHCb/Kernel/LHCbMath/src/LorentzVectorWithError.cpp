// $Id: LorentzVectorWithError.cpp,v 1.5 2010-06-08 17:51:53 ibelyaev Exp $
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
#include "LHCbMath/MatrixInversion.h"
#include "LHCbMath/Kinematics.h"
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
Gaudi::Math::LorentzVectorWithError::__sub__
( const Gaudi::LorentzVector& right ) const 
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
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
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
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
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
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
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
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
  if  ( !ok ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  Vector vct ;
  Gaudi::Math::geo2LA ( vector4d() , vct ) ;
  vct -= right ;
  //
  return ROOT::Math::Similarity ( vct , s_cov2 ) ;
}
// ============================================================================
// evaluate  sigma mass 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigmaMass () const 
{ return Gaudi::Math::sigmamass ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma2 mass 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigma2Mass () const 
{ return Gaudi::Math::sigma2mass ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma2 mass2 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigma2Mass2 () const 
{ return Gaudi::Math::sigma2mass2 ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  chi2-mass
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2mass ( const double m0 ) const 
{ return Gaudi::Math::chi2mass( m0 , vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma^2 p 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigma2p () const 
{ return Gaudi::Math::sigma2p ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma p 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigmap () const 
{ return Gaudi::Math::sigmap  ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma^2 pt 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigma2pt () const 
{ return Gaudi::Math::sigma2pt ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  sigma pt 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigmapt  () const 
{ return Gaudi::Math::sigmapt ( vector4d() , cov2() ) ; }
// ============================================================================
// get the mass with erorr 
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::LorentzVectorWithError::massWithError() const 
{ return Gaudi::Math::ValueWithError ( M() , sigma2Mass() ) ; }
// ============================================================================
// get the energy with erorr 
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::LorentzVectorWithError::energyWithError() const
{ return Gaudi::Math::ValueWithError ( E() , cov2()(3,3) ) ; }
// ============================================================================
// get the momentum with erorr 
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::LorentzVectorWithError::momentumWithError() const
{ return Gaudi::Math::ValueWithError ( P() , sigma2p() ) ; }
// ============================================================================
// get the transverse momentum with error 
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::LorentzVectorWithError::ptWithError       () const 
{ return Gaudi::Math::ValueWithError ( Pt() , sigma2pt() ) ; }
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
// The END 
// ============================================================================
