
#ifndef EVENT_PACKEDEVENTCHECKS_H
#define EVENT_PACKEDEVENTCHECKS_H 1

// Gaudi
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// Boost
#include "boost/array.hpp"

namespace DataPacking
{

  /** @class DataChecks Event/PackedEventChecks.h
   *
   *  Basic utilities to check the packed data
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-15
   */
  class DataChecks
  {

  private:

    /// Standard constructor disallowed
    DataChecks( ) { }

  public:

    /// Constructor
    explicit DataChecks( const GaudiAlgorithm & p ) : parent(&p) { }

  public:

    /// Compare two Matrices
    template < class TYPE, unsigned int N, unsigned int M >
    inline bool compareMatrices( const std::string & name,
                                 const TYPE & a,
                                 const TYPE & b,
                                 const double tol = 5.0e-3 ) const
    {
      bool ok = true;
      for ( unsigned int n = 0; n<N; ++n )
      {
        for ( unsigned int m = 0; m<M; ++m )
        {
          std::ostringstream text;
          text << name << ":" << n << m;
          double tolRel = tol * fabs( a(n,m) );
          if ( tolRel < tol ) tolRel = tol;
          ok &= compareDoubles( text.str(), a(n,m), b(n,m), tolRel );
        }
      }
      return ok;
    }

    /// Compare two 'Covariance' Matrices
    template < class TYPE, unsigned int N >
    inline bool compareCovMatrices( const std::string & name,
                                    const TYPE & a,
                                    const TYPE & b,
                                    const boost::array<double,N> tolOnDiag,
                                    const double tolOffDiag ) const
    {
      bool ok = true;
      for ( unsigned int n = 0; n<N; ++n )
      {
        for ( unsigned int m = n; m<N; ++m )
        {
          std::ostringstream text;
          text << name << ":" << n << m;
          if ( m == n )
          { // On diagonal
            ok &= compareDoubles( text.str(),
                                  std::sqrt(a(n,m)),
                                  std::sqrt(b(n,m)),
                                  tolOnDiag[n] );
          }
          else
          { // Off diagonal
            const auto testA = ( fabs(a(n,n)) > 0 && fabs(a(m,m)) > 0 ?
                                 a(n,m) / std::sqrt( a(n,n) * a(m,m) ) : 0 );
            const auto testB = ( fabs(b(n,n)) > 0 && fabs(b(m,m)) > 0 ?
                                 b(n,m) / std::sqrt( b(n,n) * b(m,m) ) : 0 );
            ok &= compareDoubles( text.str(), testA, testB, tolOffDiag );
          }
        }
      }
      return ok;
    }

    /// Compare two Lorentz vector
    bool compareLorentzVectors( const std::string & name,
                                const Gaudi::LorentzVector & a,
                                const Gaudi::LorentzVector & b,
                                const double tolV = 5.0e-3,
                                const double tolE = 1.0e-2 ) const;

    /// Compare two points to within the given tolerance
    bool comparePoints( const std::string & name,
                        const Gaudi::XYZPoint & a,
                        const Gaudi::XYZPoint & b,
                        const double tol = 1.0e-4 ) const;

    /// Compare two XYZ vectors to within the given tolerance
    bool compareVectors( const std::string & name,
                         const Gaudi::XYZVector & a,
                         const Gaudi::XYZVector & b,
                         const double tol = 1.0e-4 ) const;

    /// Compare two 3D vectors to within the given tolerance
    bool compareVectors( const std::string & name,
                         const Gaudi::Vector3 & a,
                         const Gaudi::Vector3 & b,
                         const double tol = 1.0e-4 ) const;

    /// Compare two 2D vectors to within the given tolerance
    bool compareVectors( const std::string & name,
                         const Gaudi::Vector2 & a,
                         const Gaudi::Vector2 & b,
                         const double tol = 1.0e-4 ) const;

    /// Compare two double values
    bool compareDoubles( const std::string & name,
                         const double& a,
                         const double& b,
                         const double tol = 1.0e-4 ) const;

    /// Compare two float values
    bool compareFloats( const std::string & name,
                        const float& a,
                        const float& b,
                        const float tol = 1.0e-4 ) const;

    /// Compare two unsigned int values
    template < class TYPE >
    inline bool compareInts( const std::string & name,
                             const TYPE& a,
                             const TYPE& b ) const
    {
      const bool ok = ( a == b );
      if ( UNLIKELY(!ok && parent) )
      {
        parent->warning() << name << " comparison failed :-" << endmsg
                          << " Original = " << a << endmsg
                          << " Unpacked = " << b << endmsg;
      }
      return ok;
    }

    /// Compare two double 'energy' values
    inline bool compareEnergies( const std::string & name,
                                 const double& a,
                                 const double& b,
                                 const double tol = 5.0e-3 ) const
    {
      return compareDoubles( name, a, b, tol );
    }

    /// Compare two double 'energy' vectors (e.g. Momentum vectors)
    inline bool compareEnergies( const std::string & name,
                                 const Gaudi::XYZVector& a,
                                 const Gaudi::XYZVector& b,
                                 const double tol = 5.0e-3 ) const
    {
      return compareVectors( name, a, b, tol );
    }

    template < class TYPE >
    inline bool comparePointers( const std::string & name,
                                 const TYPE * a,
                                 const TYPE * b ) const
    {
      const bool ok = ( a == b );
      if ( UNLIKELY(!ok && parent) )
      {
        parent->warning() << name << " comparison failed :-" << endmsg
                          << " Original = " << a << endmsg
                          << " Unpacked = " << b << endmsg;
      }
      return ok;
    }

  private:

    /// MessageStream
    mutable const GaudiAlgorithm * parent = nullptr;

  };

}
#endif // EVENT_PACKEDEVENTCHECKS_H
