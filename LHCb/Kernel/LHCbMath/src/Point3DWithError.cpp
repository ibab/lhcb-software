// $Id: Point3DWithError.cpp,v 1.2 2009-06-13 18:30:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD& STL 
// ============================================================================
#include <sstream>
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/Point3DWithError.h"
#include "LHCbMath/Vector3DWithError.h"
#include "LHCbMath/MatrixTransforms.h"
#include "LHCbMath/SymPosDefMatrixInverter.h"
#include "LHCbMath/MatrixInversion.h"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Math::PointWithError
 *  @author Vanya BELYAEV Ivane.BElyaev@nikhef.nl
 *  @date 20090603
 */
// ============================================================================
// constructor form the point and covariance matrix
// ============================================================================
Gaudi::Math::Point3DWithError::Point3DWithError 
( const Gaudi::Math::Point3DWithError::Point3D&    point  ,
  const Gaudi::Math::Point3DWithError::Covariance& matrix ) 
  : Gaudi::XYZPoint ( point ) 
  , m_cov2  ( matrix ) 
{}
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator+= 
( const Gaudi::Math::Vector3DWithError& right ) 
{
  point () += right.value() ;
  m_cov2   += right.cov2 () ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator-= 
( const Gaudi::Math::Vector3DWithError& right ) 
{
  point() -= right.value() ;
  m_cov2  += right.cov2 () ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator+= 
( const Gaudi::Math::Point3DWithError::Vector3D& right ) 
{
  point () += right ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator-= 
( const Gaudi::Math::Point3DWithError::Vector3D& right ) 
{
  point () -= right;
  return *this ;
}
// ============================================================================
// scaling
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator*= ( const double v ) 
{
  point () *=  v    ;
  m_cov2   *= (v*v) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Point3DWithError& 
Gaudi::Math::Point3DWithError::operator/= ( const double v ) 
{
  point () /=  v    ;
  m_cov2   /= (v*v) ;
  return *this ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::Point3DWithError::chi2 
( const Gaudi::Math::Point3DWithError& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  s_cov2 += right.cov2() ;
  // use Manuel's inverter:
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
  if  ( !ok ) { return -1 ; }                                       // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( point() - right.point() , s_cov2 ) ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::Point3DWithError::chi2 
( const Gaudi::XYZPoint& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  // use Manuel's inverter:
  const bool ok = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
  if  ( !ok ) { return -1 ; }                                        // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( point() - right , s_cov2 ) ;
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
Gaudi::Math::Point3DWithError::fillStream ( std::ostream& s ) const // printout 
{
  return s << "(" 
           << X () << "+-" << err ( m_cov2(0,0) ) << ","
           << Y () << "+-" << err ( m_cov2(1,1) ) << ","
           << Z () << "+-" << err ( m_cov2(2,2) ) << ")";
}  
// ============================================================================
// conversion to the string 
// ============================================================================
std::string 
Gaudi::Math::Point3DWithError::toString   () const // conversion to the string 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================


// ============================================================================
Gaudi::Math::Point3DWithError
Gaudi::Math::Point3DWithError::__add__ 
( const Gaudi::Math::Vector3DWithError& right ) const 
{ 
  Gaudi::Math::Point3DWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::Point3DWithError
Gaudi::Math::Point3DWithError::__sub__ 
( const Gaudi::Math::Vector3DWithError& right ) const 
{ 
  Gaudi::Math::Point3DWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::Point3DWithError
Gaudi::Math::Point3DWithError::__add__ ( const Gaudi::XYZVector& right ) const 
{ 
  Gaudi::Math::Point3DWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::Point3DWithError
Gaudi::Math::Point3DWithError::__sub__ ( const Gaudi::XYZVector& right ) const 
{ 
  Gaudi::Math::Point3DWithError tmp ( *this ) ;
  return tmp -= right ;
}


// ============================================================================
Gaudi::Math::Vector3DWithError
Gaudi::Math::Point3DWithError::__sub__ 
( const Gaudi::Math::Point3DWithError& right ) const 
{ return Gaudi::Math::Vector3DWithError ( point() -  right.point () , 
                                          cov2 () +  right.cov2  () ) ; }
// ============================================================================
Gaudi::Math::Vector3DWithError
Gaudi::Math::Point3DWithError::__sub__ 
( const Gaudi::XYZPoint& right ) const 
{ return Gaudi::Math::Vector3DWithError ( point() -  right , cov2() ) ; }
// ============================================================================


// ============================================================================
Gaudi::Math::Vector3DWithError
Gaudi::Math::Point3DWithError::__rsub__ 
( const Gaudi::XYZPoint& right ) const 
{ return Gaudi::Math::Vector3DWithError ( right - point() , cov2() ) ; }
// ============================================================================





// ============================================================================
// The END 
// ============================================================================

