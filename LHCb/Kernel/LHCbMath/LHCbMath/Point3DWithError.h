// $Id: Point3DWithError.h,v 1.3 2009-06-13 18:30:12 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_POINT3DWITHERROR_H 
#define LHCBMATH_POINT3DWITHERROR_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Vector3DWithError.h"
// ============================================================================
/** @file 
 *  Collection of useful objects with associated "covarinaces".
 *  The concept has been stollen from Wouter Hulsbergen's lines 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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
    class Point3DWithError : public Gaudi::XYZPoint 
    {
    public:
      // ======================================================================
      /// the actual type of the point 
      typedef Gaudi::XYZPoint                                      Point3D    ;
      /// the actual type of covariance 
      typedef Gaudi::SymMatrix3x3                                  Covariance ;
      /// the actual type of the point 
      typedef Gaudi::XYZPoint                                      Value      ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual type of the Vector 
      typedef Gaudi::XYZVector                                     Vector3D   ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the point and covariance matrix 
      Point3DWithError 
      ( const Point3D&    point  = Point3D    () , 
        const Covariance& matrix = Covariance () ) ;
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      const  Point3D&    point      ( ) const { return *this    ; }
      const  Point3D&    point3d    ( ) const { return point () ; }
      const  Point3D&    point3D    ( ) const { return point () ; }
      const  Covariance& covariance ( ) const { return cov2  () ; }      
      // ======================================================================      
      const  Point3D&    value      ( ) const { return point () ; }
      const  Covariance& cov2       ( ) const { return m_cov2   ; }      
      // ======================================================================
      inline Point3D&    point      ( )       { return *this    ; }
      // ======================================================================      
    public: // setters 
      // ======================================================================
      void setPoint3D    ( const Point3D&    v ) { point ()   = v   ; }
      void setPoint3d    ( const Point3D&    v ) { setPoint3D ( v ) ; }
      void setPoint      ( const Point3D&    v ) { setPoint3D ( v ) ; }
      // ======================================================================
      void setValue      ( const Point3D&    v ) { setPoint3D ( v ) ; }
      void setCovariance ( const Covariance& c ) { m_cov2     = c   ; }      
      // ======================================================================
    public: // finally it is just a point + covariance 
      // ======================================================================
      operator const Covariance& () const { return cov2  () ; }        
      operator       Covariance& ()       { return m_cov2   ; }        
      // ======================================================================
    public: // operators 
      // ======================================================================
      Point3DWithError& operator+= ( const Vector3DWithError& right ) ;
      Point3DWithError& operator-= ( const Vector3DWithError& right ) ;
      Point3DWithError& operator+= ( const Vector3D&          right ) ;
      Point3DWithError& operator-= ( const Vector3D&          right ) ;
      // ======================================================================
    public: // scaling
      // ======================================================================
      /// *= 
      Point3DWithError& operator*= ( const double v ) ;                   // *= 
      /// /= 
      Point3DWithError& operator/= ( const double v ) ;                   // /= 
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate chi2 distance 
      double chi2 ( const Point3DWithError& right )  const ;
      /// evaluate chi2 distance 
      double chi2 ( const Point3D&          right )  const ;      
      // ======================================================================
    public: // helper functions for Python
      // ======================================================================
      Point3DWithError  __add__   ( const Vector3DWithError& right ) const ;
      Point3DWithError  __add__   ( const Gaudi::XYZVector&  right ) const ;
      Point3DWithError  __sub__   ( const Vector3DWithError& right ) const ;
      Point3DWithError  __sub__   ( const Gaudi::XYZVector&  right ) const ;
      Vector3DWithError __sub__   ( const Point3DWithError&  right ) const ;
      Vector3DWithError __sub__   ( const Gaudi::XYZPoint&   right ) const ;
      // ======================================================================
      Point3DWithError  __radd__  ( const Gaudi::XYZVector&  right ) const 
      { return __add__ ( right ) ; }
      Vector3DWithError __rsub__  ( const Gaudi::XYZPoint&   right ) const ;
      // =====================================================================
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
      Covariance m_cov2  ;                                    // the covariance 
      // ======================================================================
    } ;
    // ========================================================================
    inline Point3DWithError operator+
    ( const Point3DWithError&  a , 
      const Vector3DWithError& b ) { return a.__add__ ( b ) ; }
    inline Point3DWithError operator- 
    ( const Point3DWithError&  a , 
      const Vector3DWithError& b ) { return a.__sub__ ( b ) ; }
    inline Point3DWithError operator+
    ( const Point3DWithError&  a , 
      const Gaudi::XYZVector&  b ) { return a.__add__ ( b ) ; }
    inline Point3DWithError operator- 
    ( const Point3DWithError&  a , 
      const Gaudi::XYZVector&  b ) { return a.__sub__ ( b ) ; }
    inline Point3DWithError operator+
    ( const Vector3DWithError& b , 
      const Point3DWithError&  a ) { return a + b  ; }
    inline Point3DWithError operator+
    ( const Gaudi::XYZVector&  b , 
      const Point3DWithError&  a ) { return a + b  ; }
    // ========================================================================
    inline double chi2 
    ( const Point3DWithError& a , 
      const Point3DWithError& b ) { return a.chi2 ( b ) ; }  
    inline double chi2 
    ( const Point3DWithError& a , 
      const Gaudi::XYZPoint&  b ) { return a.chi2 ( b ) ; }  
    inline double chi2 
    ( const Gaudi::XYZPoint&  b ,
      const Point3DWithError& a ) { return a.chi2 ( b ) ; }  
    // ========================================================================
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                    end of namespace  Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_POINTWITHERROR_H
// ============================================================================
