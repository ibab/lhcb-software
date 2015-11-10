
//----------------------------------------------------------------------
/** @file QuarticSolver.h
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2015-01-27
 */
//----------------------------------------------------------------------

#ifndef RICHRECPHOTONTOOLS_QuarticSolver_H
#define RICHRECPHOTONTOOLS_QuarticSolver_H 1

// Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// STL
#include <math.h>
#include <type_traits>

// Eigen
#include "LHCbMath/EigenTypes.h"
#include <Eigen/Geometry>

// VDT
#include "vdt/asin.h"

// VectorClass
#include "VectorClass/vectorclass.h"

// For complex classes. Use vector class with gcc, STL with clang
#ifdef __clang__
#include <complex>
#else
#include "VectorClass/complexvec.h"
#endif

// LHCb Maths
#include "LHCbMath/FastRoots.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class QuarticSolver
     *
     *  Utility class that implements the solving of the Quartic equation for the RICH
     *
     *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
     *  @date   2015-01-27
     */
    //-----------------------------------------------------------------------------
    class QuarticSolver
    {

    public:

      /** Solves the characteristic quartic equation for the RICH optical system.
       *
       *  See note LHCB/98-040 RICH section 3 for more details
       *
       *  @param emissionPoint Assumed photon emission point on track
       *  @param CoC           Spherical mirror centre of curvature
       *  @param virtDetPoint  Virtual detection point
       *  @param radius        Spherical mirror radius of curvature
       *  @param sphReflPoint  The reconstructed reflection pont on the spherical mirror
       *
       *  @return boolean indicating status of the quartic solution
       *  @retval true  Calculation was successful. sphReflPoint is valid.
       *  @retval false Calculation failed. sphReflPoint is not valid.
       */
      template< class TYPE >
      inline bool solve( const Gaudi::XYZPoint& emissionPoint,
                         const Gaudi::XYZPoint& CoC,
                         const Gaudi::XYZPoint& virtDetPoint,
                         const TYPE radius,
                         Gaudi::XYZPoint& sphReflPoint ) const
      {

        // typedefs vectorised types
        typedef Eigen::Matrix< TYPE , 3 , 1 > Eigen3Vector;
        typedef LHCb::Math::Eigen::XYZVector  Eigen4Vector;
        using Vec4x = typename std::conditional<std::is_same<TYPE,float>::value,Vec4f,Vec4d>::type;

        bool OK = true;

        // vector from mirror centre of curvature to assumed emission point
        const Eigen4Vector evec( emissionPoint - CoC );
        const auto e2 = evec.dot(evec);

        // vector from mirror centre of curvature to virtual detection point
        const Eigen4Vector dvec( virtDetPoint - CoC );
        const auto d2 = dvec.dot(dvec);

        if ( UNLIKELY( (e2 < 1e-99) || (d2 < 1e-99) ) )
        {
          OK = false;
        }
        else
        {

          // various quantities needed to create quartic equation
          // see LHCB/98-040 section 3, equation 3
          const auto cosgamma2 = std::pow( evec.dot(dvec), 2 ) / ( e2 * d2 );
          // vectorise 4 square roots into 1
          const auto tmp_sqrt = sqrt( Vec4x( e2, 
                                             d2, 
                                             cosgamma2 < 1.0f ? 1.0f-cosgamma2 : 0.0f, 
                                             cosgamma2 ) );
          const auto e         = tmp_sqrt[0];
          const auto d         = tmp_sqrt[1];
          const auto singamma  = tmp_sqrt[2];
          const auto cosgamma  = tmp_sqrt[3];
          const auto dx        = d * cosgamma;
          const auto dy        = d * singamma;
          const auto r2        = radius * radius;
          const auto dy2       = dy * dy;
          const auto edx       = e + dx;

          // Fill array for quartic equation
          const auto a0     =     4.0f * e2 * d2;
          const auto inv_a0 =   ( 1.0f / a0 );
          const auto a1     = - ( 4.0f * e2 * dy * radius ) * inv_a0;
          const auto a2     =   ( (dy2 * r2) + ( edx * edx * r2 ) - a0 ) * inv_a0;
          const auto a3     =   ( 2.0f * e * dy * (e-dx) * radius ) * inv_a0;
          const auto a4     =   ( ( e2 - r2 ) * dy2 ) * inv_a0;

          // use simplified RICH version of quartic solver
          const auto sinbeta = solve_quartic_RICH<TYPE>( a1, // a
                                                         a2, // b
                                                         a3, // c
                                                         a4  // d
                                                         );

          // (normalised) normal vector to reflection plane
          auto n = evec.cross3(dvec);
          n /= std::sqrt( n.dot(n) );

          // construct rotation transformation
          // Set vector magnitude to radius
          // rotate vector and update reflection point
          const Eigen::AngleAxis<TYPE> angleaxis( vdt::fast_asinf(sinbeta),
                                                  Eigen3Vector(n[0],n[1],n[2]) );
          sphReflPoint = CoC +
            Gaudi::XYZVector( angleaxis *
                              Eigen3Vector(evec[0],evec[1],evec[2]) *
                              ( radius / e ) );
          
        }

        return OK;
      }

    private:

      /// The cube root implementaton to use
      template < class TYPE >
      inline TYPE my_cbrt( const TYPE& x ) const
      {
        // STL
        //return std::cbrt(x);
        // LHCbMath FastRoots
        return FastRoots::cbrt(x);
      }

      //----------------------------------------------------------------------
      /** Solves the quartic equation x^4 + a x^3 + b x^2 + c x + d = 0
       *
       *  Optimised to give only solutions needed by RICH optical system
       *
       *  Implemented using STL Complex numbers
       *
       *  @return The solution needed by the RICH
       */
      //----------------------------------------------------------------------
      template < class TYPE >
      inline TYPE solve_quartic_RICH ( const TYPE& a,
                                       const TYPE& b,
                                       const TYPE& c,
                                       const TYPE& d ) const
      {

        const auto r4 = 1.0f / 4.0f;
        const auto q2 = 1.0f / 2.0f;
        const auto q8 = 1.0f / 8.0f;
        const auto q1 = 3.0f / 8.0f;
        const auto q3 = 3.0f / 16.0f;

        const auto UU = -( std::sqrt(3.0) / 2.0 );

        const auto aa = a * a;
        const auto pp = b - q1 * aa;
        const auto qq = c - q2 * a * (b - r4 * aa);
        const auto rr = d - r4 * (a * c - r4 * aa * (b - q3 * aa));
        const auto rc = q2 * pp;
        const auto sc = r4 * (r4 * pp * pp - rr);
        const auto tc = -(q8 * qq * q8 * qq);

        const auto qcub = (rc * rc - 3 * sc);
        const auto rcub = (2.0f * rc * rc * rc - 9 * rc * sc + 27.0f * tc);

        const auto Q = qcub / 9.0f;
        const auto R = rcub / 54.0f;

        const auto Q3 = Q * Q * Q;
        const auto R2 = R * R;

        const auto sgnR = ( R >= 0 ? -1 : 1 );

        const auto A = sgnR * my_cbrt( (TYPE)( fabs(R) + std::sqrt( fabs(R2-Q3) ) ) );
        
        const auto B = Q / A;

        const auto u1 = -0.5f * (A + B) - rc / 3.0f;
        const auto u2 = UU * fabs(A-B);

#ifdef __clang__

        // Implementation using STL classes

        const auto w1 = std::sqrt( std::complex<TYPE>(u1, u2) );
        const auto w2 = std::sqrt( std::complex<TYPE>(u1,-u2) );
        const auto  V = w1 * w2;
        const std::complex<TYPE> w3 = ( std::abs(V) != 0.0 ? ( qq * -0.125f ) / V : 
                                        std::complex<TYPE>(0,0) );
        const TYPE res = std::real(w1) + std::real(w2) + std::real(w3) - (r4*a);

#else

        // Vectorised implementation using VectorClass
        // (Much) faster, but currently causes problems with clang

        using Complex4x = typename std::conditional<std::is_same<TYPE,float>::value,Complex4f,Complex4d>::type;
        using Complex2x = typename std::conditional<std::is_same<TYPE,float>::value,Complex2f,Complex2d>::type;

        const Complex4x  W = sqrt( Complex4x(u1,u2,u1,-u2) );
        const auto       V = W.get_low() * W.get_high();
        const auto      w3 = ( abs(V) != 0.0 ? ( qq * -0.125f ) / V : Complex2x(0,0) );
        const TYPE     res = W.extract(0) + W.extract(2) + w3.extract(0) - (r4*a);

#endif

        // return the final result
        return ( res >  1.0 ?  1.0 :
                 res < -1.0 ? -1.0 :
                 res );
      }     
      
    };
    
  }
}

#endif // RICHRECPHOTONTOOLS_QuarticSolver_H
