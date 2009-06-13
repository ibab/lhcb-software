// $Id: LorentzVectorWithError.cpp,v 1.2 2009-06-13 18:30:12 ibelyaev Exp $
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
  return s << "{" << "(" 
           << X () << "+-" << err ( m_cov2(0,0) ) << ","
           << Y () << "+-" << err ( m_cov2(1,1) ) << ","
           << Z () << "+-" << err ( m_cov2(2,2) ) << "),"
           << E () << "+-" << err ( m_cov2(3,3) ) << "}";
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
// evaluate  sigma mass 
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::sigmaMass () const 
{ return Gaudi::Math::sigmamass ( vector4d() , cov2() ) ; }
// ============================================================================
// evaluate  chi2-mass
// ============================================================================
double Gaudi::Math::LorentzVectorWithError::chi2mass ( const double m0 ) const 
{ return Gaudi::Math::chi2mass( m0 , vector4d() , cov2() ) ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
