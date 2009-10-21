// $Id: PackedEventChecks.h,v 1.1 2009-10-21 16:40:16 jonrob Exp $
#ifndef EVENT_PACKEDEVENTCHECKS_H
#define EVENT_PACKEDEVENTCHECKS_H 1

// STL
#include <cmath>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiAlg/GaudiAlgorithm.h"

namespace DataPacking
{

  /** @class DataChecks Event/PackedEventChecks.h
   *
   *  basic utilities to check the packed data
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-15
   */
  class DataChecks
  {

  private:

    /// Standard constructor disallowed
    DataChecks( ) : parent(NULL)  { }

  public:

    /// Constructor
    explicit DataChecks( GaudiAlgorithm & _parent ) : parent(&_parent) { }

    /// Destructor
    ~DataChecks( ) { }

  public:

    /// Compare two points to within the given tolerance
    inline bool comparePoints(  const std::string & name,
                                const Gaudi::XYZPoint & a,
                                const Gaudi::XYZPoint & b,
                                const double tol = 1.0e-4 ) const
    {
      const bool ok = ( std::fabs( a.x() - b.x() ) < tol &&
                        std::fabs( a.y() - b.y() ) < tol &&
                        std::fabs( a.z() - b.z() ) < tol );
      if (!ok)
      {
        parent->warning() << name << " comparison failed :-" << endmsg
                          << " Original " << a.x() << " " << a.y() << " " << a.z() << endmsg
                          << " Unpacked " << b.x() << " " << b.y() << " " << b.z() << endmsg
                          << " Diff.    "
                          << std::fabs( a.x() - b.x() ) << " "
                          << std::fabs( a.y() - b.y() ) << " "
                          << std::fabs( a.z() - b.z() ) << endmsg;
      }
      parent->counter("Original - "+name+" x") += a.x();
      parent->counter("Original - "+name+" y") += a.y();
      parent->counter("Original - "+name+" z") += a.z();
      parent->counter("Unpacked - "+name+" x") += b.x();
      parent->counter("Unpacked - "+name+" y") += b.y();
      parent->counter("Unpacked - "+name+" z") += b.z();
      parent->counter("Diff.    - "+name+" x") += a.x()-b.x();
      parent->counter("Diff.    - "+name+" y") += a.y()-b.y();
      parent->counter("Diff.    - "+name+" z") += a.z()-b.z();
      return ok;
    }

    /// Compare two vectors to within the given tolerance
    inline bool compareVectors( const std::string & name,
                                const Gaudi::XYZVector & a,
                                const Gaudi::XYZVector & b,
                                const double tol = 1.0e-4 ) const
    {
      const bool ok = ( std::fabs( a.x() - b.x() ) < tol &&
                        std::fabs( a.y() - b.y() ) < tol &&
                        std::fabs( a.z() - b.z() ) < tol );
      if (!ok)
      {
        parent->warning() << name << " comparison failed :-" << endmsg
                          << " Original " << a.x() << " " << a.y() << " " << a.z() << endmsg
                          << " Unpacked " << b.x() << " " << b.y() << " " << b.z() << endmsg
                          << "  Diff    "
                          << std::fabs( a.x() - b.x() ) << " "
                          << std::fabs( a.y() - b.y() ) << " "
                          << std::fabs( a.z() - b.z() ) << endmsg;
      }
      parent->counter("Original - "+name+" x") += a.x();
      parent->counter("Original - "+name+" y") += a.y();
      parent->counter("Original - "+name+" z") += a.z();
      parent->counter("Unpacked - "+name+" x") += b.x();
      parent->counter("Unpacked - "+name+" y") += b.y();
      parent->counter("Unpacked - "+name+" z") += b.z();
      parent->counter("Diff.    - "+name+" x") += a.x()-b.x();
      parent->counter("Diff.    - "+name+" y") += a.y()-b.y();
      parent->counter("Diff.    - "+name+" z") += a.z()-b.z();
      return ok;
    }

    /// Compare two double values
    inline bool compareDoubles( const std::string & name,
                                const double a,
                                const double b,
                                const double tol = 1.0e-4 ) const
    {
      const bool ok = ( std::fabs( a - b ) < tol );
      if (!ok)
      {
        parent->warning() << name << " comparison failed :-" << endmsg
                          << " Original = " << a << endmsg
                          << " Unpacked = " << b << endmsg
                          << "  Diff = " << std::fabs(a-b) << " > " << tol
                          << endmsg;
      }
      parent->counter("Original - "+name) += a;
      parent->counter("Unpacked - "+name) += b;
      parent->counter("Diff.    - "+name) += a-b;
      return ok;
    }

    /// Compare two int values
    inline bool compareInts( const int a,
                             const int b ) const
    {
      const bool ok = ( a == b );
      if (!ok)
      {
        parent->warning() << "compare(signed)Ints failed :-" << endmsg
                          << "  " << a << " " << b << endmsg;
      }
      return ok;
    }

    /// Compare two unsigned int values
    inline bool compareInts( const unsigned int a,
                             const unsigned int b ) const
    {
      const bool ok = ( a == b );
      if (!ok)
      {
        parent->warning() << "compare(unsigned)Ints failed" << endmsg
                          << "  " << a << " " << b << endmsg;
      }
      return ok;
    }

    /// Compare two double 'energy' values
    inline bool compareEnergies( const std::string & name,
                                 const double a,
                                 const double b,
                                 const double tol = 5.0e-3 ) const
    {
      return compareDoubles( name, a, b, tol );
    }

    /// Compare two double 'energy' vectors (e.g. Momentum vectors)
    inline bool compareEnergies( const std::string & name,
                                 const Gaudi::XYZVector & a,
                                 const Gaudi::XYZVector & b,
                                 const double tol = 5.0e-3 ) const
    {
      return compareVectors( name, a, b, tol );
    }

  private:

    /// MessageStream
    mutable GaudiAlgorithm * parent;

  };

}
#endif // EVENT_PACKEDEVENTCHECKS_H
