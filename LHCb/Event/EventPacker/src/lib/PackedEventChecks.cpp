
// STL
#include <cmath>
#include <sstream>

#include "Event/PackedEventChecks.h"

using namespace DataPacking;

//-----------------------------------------------------------------------------
// Implementation file for class : PackedEventChecks
//
// 2012-04-01 : Chris Jones
//-----------------------------------------------------------------------------

bool
DataChecks::compareLorentzVectors( const std::string & name,
                                   const Gaudi::LorentzVector & a,
                                   const Gaudi::LorentzVector & b,
                                   const double tolV,
                                   const double tolE ) const
{
  const Gaudi::XYZVector av(a.px(),a.py(),a.pz());
  const Gaudi::XYZVector bv(b.px(),b.py(),b.pz());
  const bool vOK = compareVectors ( name+":Vect",   av,    bv,    tolV );
  const bool eOK = compareEnergies( name+":Energy", a.E(), b.E(), tolE );
  return vOK && eOK;
}

bool
DataChecks::comparePoints( const std::string & name,
                           const Gaudi::XYZPoint & a,
                           const Gaudi::XYZPoint & b,
                           const double tol ) const
{
  const bool ok = ( std::fabs( a.x() - b.x() ) < tol &&
                    std::fabs( a.y() - b.y() ) < tol &&
                    std::fabs( a.z() - b.z() ) < tol );
  if ( UNLIKELY(!ok) )
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

bool
DataChecks::compareVectors( const std::string & name,
                            const Gaudi::XYZVector & a,
                            const Gaudi::XYZVector & b,
                            const double tol ) const
{
  const bool ok = ( std::fabs( a.x() - b.x() ) < tol &&
                    std::fabs( a.y() - b.y() ) < tol &&
                    std::fabs( a.z() - b.z() ) < tol );
  if ( UNLIKELY(!ok) )
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

bool
DataChecks::compareVectors( const std::string & name,
                            const Gaudi::Vector3 & a,
                            const Gaudi::Vector3 & b,
                            const double tol ) const
{
  const bool ok = ( std::fabs( a[0] - b[0] ) < tol &&
                    std::fabs( a[1] - b[1] ) < tol &&
                    std::fabs( a[2] - b[2] ) < tol );
  if ( UNLIKELY(!ok) )
  {
    parent->warning() << name << " comparison failed :-" << endmsg
                      << " Original " << a[0] << " " << a[1] << " " << a[2] << endmsg
                      << " Unpacked " << b[0] << " " << b[1] << " " << b[2] << endmsg
                      << "  Diff    "
                      << std::fabs( a[0] - b[0] ) << " "
                      << std::fabs( a[1] - b[1] ) << " "
                      << std::fabs( a[2] - b[2] ) << endmsg;
  }
  parent->counter("Original - "+name+" [0]") += a[0];
  parent->counter("Original - "+name+" [1]") += a[1];
  parent->counter("Original - "+name+" [2]") += a[2];
  parent->counter("Unpacked - "+name+" [0]") += b[0];
  parent->counter("Unpacked - "+name+" [1]") += b[1];
  parent->counter("Unpacked - "+name+" [2]") += b[2];
  parent->counter("Diff.    - "+name+" [0]") += a[0]-b[0];
  parent->counter("Diff.    - "+name+" [1]") += a[1]-b[1];
  parent->counter("Diff.    - "+name+" [2]") += a[2]-b[2];
  return ok;
}

bool
DataChecks::compareVectors( const std::string & name,
                            const Gaudi::Vector2 & a,
                            const Gaudi::Vector2 & b,
                            const double tol ) const
{
  const bool ok = ( std::fabs( a[0] - b[0] ) < tol &&
                    std::fabs( a[1] - b[1] ) < tol );
  if ( UNLIKELY(!ok) )
  {
    parent->warning() << name << " comparison failed :-" << endmsg
                      << " Original " << a[0] << " " << a[1] << endmsg
                      << " Unpacked " << b[0] << " " << b[1] << endmsg
                      << "  Diff    "
                      << std::fabs( a[0] - b[0] ) << " "
                      << std::fabs( a[1] - b[1] ) << endmsg;
  }
  parent->counter("Original - "+name+" [0]") += a[0];
  parent->counter("Original - "+name+" [1]") += a[1];
  parent->counter("Unpacked - "+name+" [0]") += b[0];
  parent->counter("Unpacked - "+name+" [1]") += b[1];
  parent->counter("Diff.    - "+name+" [0]") += a[0]-b[0];
  parent->counter("Diff.    - "+name+" [1]") += a[1]-b[1];
  return ok;
}

bool
DataChecks::compareDoubles( const std::string & name,
                            const double& a,
                            const double& b,
                            const double tol ) const
{
  const bool ok = ( std::fabs( a - b ) < tol );
  if ( UNLIKELY(!ok) )
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

bool
DataChecks::compareFloats( const std::string & name,
                           const float& a,
                           const float& b,
                           const float tol ) const
{
  const bool ok = ( std::fabs( a - b ) < tol );
  if ( UNLIKELY(!ok) )
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
