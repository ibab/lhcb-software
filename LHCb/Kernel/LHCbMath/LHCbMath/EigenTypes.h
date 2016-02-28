#ifndef LHCBMATH_EIGENTYPES_H 
#define LHCBMATH_EIGENTYPES_H 1

// STL
#include <cmath>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

// Eigen
#include <Eigen/Geometry>

// VDT
#include "vdt/sqrt.h"

namespace LHCb
{
  namespace Math
  {

    /** @namespace Eigen EigenTypes.h
     *
     *  Namespace for classes implementing geometrical entities using the
     *  Eigen library. Internal implementation choosen to utilise the
     *  vectorisation capabilities of the library. See
     *
     *  http://eigen.tuxfamily.org
     *
     *  For more details.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     */
    namespace Eigen
    {

      /// floating point precision
      using FloatType = double;

      /// The Eigen 4D Vector type
      using E4Vector = ::Eigen::Matrix< FloatType , 4 , 1 >;

      /// The Eigen 3D Vector type
      using E3Vector = ::Eigen::Matrix< FloatType , 3 , 1 >;
      
      /** @class XYZVector EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D vector as an Eigen Matrix.
       *  Uses internally a 4D float Matrix in order to utilise
       *  automatic vectorisation of the classes and operations.
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      class XYZVector : public E4Vector
      {

      public:

        /// Standard constructor
        XYZVector( const FloatType x = 0.0, 
                   const FloatType y = 0.0,
                   const FloatType z = 0.0 ) : E4Vector(x,y,z,0) { }

        /// Constructor from a Gaudi Point
        XYZVector( const Gaudi::XYZVector& p ) : E4Vector( p.x(), p.y(), p.z(), 0 ) { }

        /// Constructor from an Eigen 3D Vector
        XYZVector( const E3Vector& v ) : E4Vector( v.x(), v.y(), v.z(), 0 ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZVector( const TYPE& v ) : E4Vector(v) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZVector() const { return { x(), y(), z() }; }

      public:

        /// Dot product against a Gaudi vector
        inline FloatType Dot( const Gaudi::XYZVector& v ) const { return dot(XYZVector(v)); }

        /// Implement 'Dot' method ( alias for dot)
        template < class TYPE >
        inline FloatType Dot( const TYPE& v ) const { return dot(v); }

        /// Magnitude squared
        inline FloatType Mag2() const { return dot(*this); }

        /// Magnitude squared
        inline FloatType mag2() const { return Mag2(); }

        /// Get the polar R Coordinate
        inline FloatType R() const { return std::sqrt(Mag2()); }

        /// 3D Cross product
        template < class TYPE >
        inline XYZVector Cross( const TYPE& v ) const { return cross3(v); }

        /// Returns a normalised unit vector
        inline XYZVector Unit() const { return (*this) * vdt::fast_isqrt(Mag2()); }

        /// Returns a normalised unit vector
        inline XYZVector unit() const { return Unit(); }

      public: /// Alias methods for accessors and setters

        /// Get X Coordinate
        inline FloatType X() const noexcept { return x(); }

        /// Get Y Coordinate
        inline FloatType Y() const noexcept { return y(); }

        /// Get Z Coordinate
        inline FloatType Z() const noexcept { return z(); }

        /// Set X Coordinate
        inline void SetX( const FloatType x ) noexcept { (*this)[0] = x; }

        /// Set Y Coordinate
        inline void SetY( const FloatType y ) noexcept { (*this)[1] = y; }

        /// Set Z Coordinate
        inline void SetZ( const FloatType z ) noexcept { (*this)[2] = z; }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZVector & v )
        { 
          return os << "(" << v.x() << "," << v.y() << "," << v.z() << ")"; 
        }

      };

      /** @class XYZPoint EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D point as an Eigen Matrix.
       *  Uses internally a 4D FloatType Matrix in order to utilise
       *  automatic vectorisation of the classes and operations.
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      class XYZPoint : public E4Vector
      {

      public:

        /// Standard constructor
        XYZPoint( const FloatType x = 0.0, 
                  const FloatType y = 0.0,
                  const FloatType z = 0.0 ) : E4Vector(x,y,z,0) { }

        /// Constructor from a Gaudi Point
        XYZPoint( const Gaudi::XYZPoint& p ) : E4Vector( p.x(), p.y(), p.z(), 0 ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZPoint( const TYPE& p ) : E4Vector(p) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZPoint() const 
        {
          return Gaudi::XYZPoint( x(), y(), z() );
        }

      public:

        // /// Operator -
        // inline XYZVector operator-( const Gaudi::XYZPoint& p ) const 
        // { 
        //   return (*this) - E4Vector( p.x(), p.y(), p.z(), 0 );
        // }

        // /// Operator -
        // inline XYZPoint operator-( const Gaudi::XYZVector& v ) const 
        // { 
        //   return (*this) - E4Vector( v.x(), v.y(), v.z(), 0 ); 
        // }

      public: /// Alias methods for accessors and setters

        /// Get the polar R Coordinate
        inline FloatType R() const { return std::sqrt( dot(*this) ); }

        /// Get X Coordinate
        inline FloatType X() const noexcept { return x(); }

        /// Get Y Coordinate
        inline FloatType Y() const noexcept { return y(); }

        /// Get Z Coordinate
        inline FloatType Z() const noexcept { return z(); }

        /// Set X Coordinate
        inline void SetX( const FloatType x ) noexcept { (*this)[0] = x; }

        /// Set Y Coordinate
        inline void SetY( const FloatType y ) noexcept { (*this)[1] = y; }

        /// Set Z Coordinate
        inline void SetZ( const FloatType z ) noexcept { (*this)[2] = z; }

        /// Set all three coordinates
        inline void SetCoordinates( const FloatType x,
                                    const FloatType y,
                                    const FloatType z ) noexcept { *this = E4Vector(x,y,z,0); }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZPoint & p )
        { 
          return os << "(" << p.x() << "," << p.y() << "," << p.z() << ")"; 
        }

      };

      /** @class Rotation3D EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D Rotation
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      class Rotation3D
      {
      };

      /// The internal Eigen type to use for 3D Planes
      typedef ::Eigen::Hyperplane< FloatType, 3 > EPlane;

      /** @class Plane3D EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D plane as an Eigen Hyperplane
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      class Plane3D : public EPlane
      {
      private:

        typedef ::Eigen::Matrix<FloatType,3,1> Vec3f;

      private:

        /// Stupid method to deal with fact Gaudi::XYZPlane's accessors are not const..
        template < typename TYPE >
        inline static TYPE& nonc( const TYPE& t ) 
        {
          return const_cast<TYPE&>(t);
        } 
        
      public :   

        /// Default constructor
        Plane3D() = default;
      
        /** Constructor from parameters from the general form
         *  according to the equation ax + by + cz + d = 0 */
        Plane3D( const FloatType a, const FloatType b,
                 const FloatType c, const FloatType d ) 
          : EPlane( Vec3f(a,b,c), d ) { }

        /// Constructor from a normal vector and a point in the plane
        template < class VECTOR, class POINT >
        Plane3D( const VECTOR& v, const POINT& p ) 
          : EPlane( Vec3f(v.x(),v.y(),v.z()), 
                    Vec3f(p.x(),p.y(),p.z()) ) { }

        /// Constructor from three points in the plane
        template < class POINT >
        Plane3D( const POINT& p1, const POINT& p2, const POINT& p3 )
          : EPlane( EPlane::Through( Vec3f(p1.x(),p1.y(),p1.z()),
                                     Vec3f(p2.x(),p2.y(),p2.z()),
                                     Vec3f(p3.x(),p3.y(),p3.z()) ) ) { }       

        /** Constructor from a Gaudi Plane3D
         *  Why on Earth are the Plane acccessors not const... */
        Plane3D( const Gaudi::Plane3D& p )
          : EPlane( Vec3f(nonc(p).A(),nonc(p).B(),nonc(p).C()), nonc(p).D() ) { }

        /// Implicit conversion back to a Gaudi Plane3D
        inline operator Gaudi::Plane3D() const 
        {
          const auto n = normal();
          return Gaudi::Plane3D( n.x(), n.y(), n.z(), offset() );
        }

      public:

        /// Access the plane normal 
        inline XYZVector Normal() const 
        { 
          const auto n = normal();
          return XYZVector( n.x(), n.y(), n.z() );
        }

        /// Distance to a point
        template < class POINT >
        inline FloatType Distance( const POINT& p ) const 
        { 
          return signedDistance( Vec3f(p.x(),p.y(),p.z()) ); 
        }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const Plane3D & p )
        { 
          return os << "(" << p.coeffs() << ")"; 
        }

      };

    }
  }
}

#endif // LHCBMATH_EIGENTYPES_H
