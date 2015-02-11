#ifndef LHCBMATH_VectorClassTypes_H 
#define LHCBMATH_VectorClassTypes_H 1

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

// VectorClass
#include "VectorClass/vector3d.h"

namespace LHCb
{
  namespace Math
  {

    /** @namespace VectorClass VectorClassTypes.h
     *
     *  Namespace for classes implementing geometrical entities using the
     *  VectorClass library. Internal implementation choosen to best utilise 
     *  the vectorisation capabilities of the library. See
     *
     *  http://www.agner.org/optimize/
     *
     *  For more details.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     */
    namespace VectorClass
    {

      /// The floating point type in use
      typedef float FloatType;
      
      /// The internal type to use for 3D vectors and points
      typedef Vec3f VCVec;
      
      /** @class XYZVector VectorClassTypes.h LHCbMath/VectorClassTypes.h
       *
       *  Implementation of a 3D vector.
       *  Uses internally a 4D vector in order to utilise
       *  automatic vectorisation of the classes and operations.
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      class XYZVector : public VCVec
      {

      public:

        /// Standard constructor
        XYZVector( const FloatType x = 0.0f, 
                   const FloatType y = 0.0f,
                   const FloatType z = 0.0f ) : VCVec(x,y,z) { }

        /// Constructor from a Gaudi Point
        XYZVector( const Gaudi::XYZVector& p ) : VCVec( p.x(), p.y(), p.z() ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZVector( const TYPE& v ) : VCVec(v) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZVector() const 
        {
          return Gaudi::XYZVector( x(), y(), z() );
        }

        ///< Destructor
        ~XYZVector( ) { }

        /// Implement dot method
        inline FloatType dot( const XYZVector& v ) const { return dot_product(*this,v); }

        /// Implement Dot method (alias to dot, to match ROOT...)
        inline FloatType Dot( const XYZVector& v ) const { return dot(v); }
        
        /// Magnitude squared
        inline FloatType Mag2() const { return horizontal_add(square(*this)); }

        /// Magnitude squared
        inline FloatType mag2() const { return Mag2(); }

        /// Get the polar R Coordinate
        inline FloatType R() const { return std::sqrt(Mag2()); }

        /// 3D Cross product
        inline XYZVector Cross( const XYZVector& v ) const { return cross_product(*this,v); }

        /// Returns a normalised unit vector
        inline XYZVector Unit() const { return normalize_vector(*this); }

        /// Returns a normalised unit vector
        inline XYZVector unit() const { return Unit(); }

      public:

        /// X accessor
        inline FloatType x() const { return get_x(); }

        /// Y accessor
        inline FloatType y() const { return get_y(); }

        /// Z accessor
        inline FloatType z() const { return get_z(); }

        /// Get X Coordinate
        inline FloatType X() const { return x(); }

        /// Get Y Coordinate
        inline FloatType Y() const { return y(); }

        /// Get Z Coordinate
        inline FloatType Z() const { return z(); }

        /// Set X Coordinate
        inline void SetX( const FloatType x ) { insert(0,x); }

        /// Set Y Coordinate
        inline void SetY( const FloatType y ) { insert(1,y); }

        /// Set Z Coordinate
        inline void SetZ( const FloatType z ) { insert(2,z); }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZVector & v )
        { return os << "(" << v.x() << "," << v.y() << "," << v.z() << ")"; }
        
      };

      /** @class XYZPoint VectorClassTypes.h LHCbMath/VectorClassTypes.h
       *
       *  Implementation of a 3D point as an Eigen Matrix.
       *  Uses internally a 4D vector in order to utilise
       *  automatic vectorisation of the classes and operations.
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */

      class XYZPoint : public VCVec
      {

      public:

        /// Standard constructor
        XYZPoint( const FloatType x = 0.0f, 
                  const FloatType y = 0.0f,
                  const FloatType z = 0.0f ) : VCVec(x,y,z) { }

        /// Constructor from a Gaudi Point
        XYZPoint( const Gaudi::XYZPoint& p ) : VCVec( p.x(), p.y(), p.z() ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZPoint( const TYPE& p ) : VCVec(p) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZPoint() const 
        {
          return Gaudi::XYZPoint( x(), y(), z() );
        }

        ///< Destructor
        ~XYZPoint( ) { }

      public:
        
        /// X accessor
        inline FloatType x() const { return get_x(); }
        
        /// Y accessor
        inline FloatType y() const { return get_y(); }
        
        /// Z accessor
        inline FloatType z() const { return get_z(); }

        /// Get X Coordinate
        inline FloatType X() const { return x(); }

        /// Get Y Coordinate
        inline FloatType Y() const { return y(); }

        /// Get Z Coordinate
        inline FloatType Z() const { return z(); }

        /// Set X Coordinate
        inline void SetX( const FloatType x ) { insert(0,x); }

        /// Set Y Coordinate
        inline void SetY( const FloatType y ) { insert(1,y); }

        /// Set Z Coordinate
        inline void SetZ( const FloatType z ) { insert(2,z); }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZPoint & p )
        { return os << "(" << p.x() << "," << p.y() << "," << p.z() << ")"; }

      };

    }
  }
}

#endif // LHCBMATH_VectorClassTypes_H
