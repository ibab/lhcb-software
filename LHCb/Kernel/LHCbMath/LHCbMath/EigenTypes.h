#ifndef LHCBMATH_EIGENTYPES_H 
#define LHCBMATH_EIGENTYPES_H 1

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

// Eigen
#include <Eigen/Geometry>

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

      /// The internal Eigen type to use for 3D vectors and points
      typedef ::Eigen::Matrix< float , 4 , 1 > EVector;
      
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

      class XYZVector : public EVector
      {

      public:

        /// Standard constructor
        XYZVector( const float x = 0.0f, 
                   const float y = 0.0f,
                   const float z = 0.0f ) : EVector(x,y,z,0) { }

        /// Constructor from a Gaudi Point
        XYZVector( const Gaudi::XYZVector& p ) : EVector( p.x(), p.y(), p.z(), 0 ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZVector( const TYPE& v ) : EVector(v) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZVector() const 
        {
          return Gaudi::XYZVector( x(), y(), z() );
        }

        ///< Destructor
        ~XYZVector( ) { }

      public:

        /// Implement 'Dot' method ( alias for dot)
        template < class TYPE >
        inline float Dot( const TYPE& v ) const { return dot(v); }

        /// Magnitude squared
        inline float Mag2() const { return dot(*this); }

        /// 3D Cross product
        template < class TYPE >
        inline XYZVector Cross( const TYPE& v ) const { return cross3(v); }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZVector & v )
        { return os << "(" << v.x() << "," << v.y() << "," << v.z() << ")"; }

      };

      /** @class XYZPoint EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D point as an Eigen Matrix.
       *  Uses internally a 4D float Matrix in order to utilise
       *  automatic vectorisation of the classes and operations.
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */

      class XYZPoint : public EVector
      {

      public:

        /// Standard constructor
        XYZPoint( const float x = 0.0f, 
                  const float y = 0.0f,
                  const float z = 0.0f ) : EVector(x,y,z,0) { }

        /// Constructor from a Gaudi Point
        XYZPoint( const Gaudi::XYZPoint& p ) : EVector( p.x(), p.y(), p.z(), 0 ) { }

        /// Templated copy constructor
        template < class TYPE >
        XYZPoint( const TYPE& p ) : EVector(p) { }
        
        /// Implicit conversion back to a Gaudi Vector
        inline operator Gaudi::XYZPoint() const 
        {
          return Gaudi::XYZPoint( x(), y(), z() );
        }

        ///< Destructor
        ~XYZPoint( ) { }

      public:

        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os, const XYZPoint & p )
        { return os << "(" << p.x() << "," << p.y() << "," << p.z() << ")"; }

      };

      /// The internal Eigen type to use for 3D Planes
      //typedef ::Eigen::Hyperplane< float, 3 >  EPlane;

      /** @class XYZPlane EigenTypes.h LHCbMath/EigenTypes.h
       *
       *  Implementation of a 3D plane as an Eigen Hyperplane
       *
       *  This class is designed to 'smooth' the interaction
       *  and conversions from and to the Gaudi classes.
       *
       *  @author Chris Jones
       *  @date   2015-01-28
       */
      //class XYZPlane : public EPlane
      // {
      //public :         
      //};

    }
  }
}

#endif // LHCBMATH_EIGENTYPES_H
