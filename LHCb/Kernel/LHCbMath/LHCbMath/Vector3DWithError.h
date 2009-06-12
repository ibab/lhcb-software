// $Id: Vector3DWithError.h,v 1.1 2009-06-12 14:57:17 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_VECTOR3DWITHERROR_H 
#define LHCBMATH_VECTOR3DWITHERROR_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Point3DWithError.h"
// ============================================================================
/** @file 
 *  Collection of useful objects with associated "covarinaces".
 *  The concept has been stollen from Wouter Hulsbergen's lines 
 *  @author Vanya BELYAEV Ivane.BElyaev@nikhef.nl
 *  @date 20090603
 */
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    /// forward declarations 
    class Point3DWithError  ;
    class Vector3DWithError ;
    // ========================================================================
    class Vector3DWithError : public Gaudi::XYZVector 
    {
    public:
      // ======================================================================
      /// the actual type of the Vector 
      typedef Gaudi::XYZVector                                     Vector3D   ;
      typedef Gaudi::XYZVector                                     Value      ;
      /// the actual type of covariance 
      typedef Gaudi::SymMatrix3x3                                  Covariance ;
      // ======================================================================
    public:
      /// the actual type of the point 
      typedef Gaudi::XYZPoint                                      Point3D    ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor form the point and covariance matrix 
      Vector3DWithError 
      ( const Vector3D&   vct    = Vector3D   () , 
        const Covariance& matrix = Covariance () ) ;
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      const  Vector3D&   vector3d   ( ) const { return *this       ; }
      const  Vector3D&   vector3D   ( ) const { return vector3d () ; }
      const  Covariance& covariance ( ) const { return cov2  ()    ; }      
      inline Vector3D&   vector3d   ( )       { return *this       ; }
      // ======================================================================      
      const  Vector3D&   value      ( ) const { return vector3d () ; }
      const  Covariance& cov2       ( ) const { return m_cov2      ; }
      // ======================================================================      
    public: // setters 
      // ======================================================================
      void setVector3d   ( const Vector3D&   v ) { vector3d() =  v   ; }
      void setVector3D   ( const Vector3D&   v ) { setVector3d ( v ) ; }
      void setVector     ( const Vector3D&   v ) { setVector3d ( v ) ; }
      // ======================================================================
      void setValue      ( const Vector3D&   v ) { setVector3d ( v ) ; }
      void setCovariance ( const Covariance& c ) { m_cov2      = c   ; }      
      // ======================================================================
    public: // finally it is just a point + covariance 
      // ======================================================================
      operator const Covariance& () const { return cov2  () ; }        
      operator       Covariance& ()       { return m_cov2   ; }        
      // ======================================================================
    public: // operators 
      // ======================================================================
      Vector3DWithError& operator+= ( const Vector3DWithError& right ) ;
      Vector3DWithError& operator-= ( const Vector3DWithError& right ) ;
      Vector3DWithError& operator+= ( const Vector3D&          right ) ;
      Vector3DWithError& operator-= ( const Vector3D&          right ) ;
      // ======================================================================
    public: // scaling
      // ======================================================================
      /// *= 
      Vector3DWithError& operator*= ( const double v ) ;                  // *= 
      /// /= 
      Vector3DWithError& operator/= ( const double v ) ;                  // /= 
      // ======================================================================
    public: // unary -
      // ======================================================================
      Vector3DWithError operator-() const ;                           // unary-
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate chi2 distance 
      double chi2 ( const Vector3DWithError& right )  const ;
      /// evaluate chi2 distance 
      double chi2 ( const Vector3D&          right )  const ;      
      // ======================================================================
    public: // helper operators for Python
      // ======================================================================
      Vector3DWithError __add__  ( const Vector3DWithError& right ) const ;
      Vector3DWithError __add__  ( const Vector3D&          right ) const ;
      Point3DWithError  __add__  ( const Point3DWithError&  right ) const ;
      Vector3DWithError __sub__  ( const Vector3DWithError& right ) const ;
      Vector3DWithError __sub__  ( const Vector3D&          right ) const ;
      // ======================================================================
      Vector3DWithError __radd__ ( const Vector3D&          right ) const 
      { return __add__ ( right ) ; }
      Vector3DWithError __rsub__ ( const Vector3D&          right ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// printout 
      std::ostream& fillStream ( std::ostream& s ) const ;          // printout 
      /// conversion to the string 
      std::string   toString   () const ;           // conversion to the string 
      // ======================================================================
    private:
      // ======================================================================
      /// the covariance 
      Covariance m_cov2     ;                                 // the covariance 
      // ======================================================================
    } ;
    // ========================================================================
    inline Vector3DWithError operator+ 
    ( const Vector3DWithError& a , 
      const Vector3DWithError& b ) { return a.__add__  ( b ) ; }
    inline Vector3DWithError operator+ 
    ( const Vector3DWithError& a , 
      const Gaudi::XYZVector&  b ) { return a.__add__  ( b ) ; }
    inline Vector3DWithError operator-
    ( const Vector3DWithError& a , 
      const Vector3DWithError& b ) { return a.__sub__  ( b ) ; }
    inline Vector3DWithError operator-
    ( const Vector3DWithError& a , 
      const Gaudi::XYZVector&  b ) { return a.__sub__  ( b ) ; }
    inline Vector3DWithError operator-
    ( const Gaudi::XYZVector&  b , 
      const Vector3DWithError& a ) { return a.__rsub__ ( b ) ; }   
    // ========================================================================
    Vector3DWithError operator- 
    ( const Gaudi::XYZPoint&  b ,
      const Point3DWithError& a ) ;
    Vector3DWithError operator- 
    ( const Point3DWithError& a , 
      const Point3DWithError& b ) ;
    Vector3DWithError operator- 
    ( const Point3DWithError& a , 
      const Gaudi::XYZPoint&  b ) ;
    // ========================================================================
    inline double chi2 
    ( const Vector3DWithError& a , 
      const Vector3DWithError& b ) { return a.chi2 ( b ) ; }  
    inline double chi2 
    ( const Vector3DWithError& a , 
      const Gaudi::XYZVector&  b ) { return a.chi2 ( b ) ; }  
    inline double chi2 
    ( const Gaudi::XYZVector&  b ,
      const Vector3DWithError& a ) { return a.chi2 ( b ) ; }  
    // ========================================================================
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                    end of namespace  Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_POINTWITHERROR_H
// ============================================================================
