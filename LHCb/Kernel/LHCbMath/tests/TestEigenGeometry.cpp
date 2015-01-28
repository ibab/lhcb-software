
// STL
#include <random>
#include <vector>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Eigen
#include <Eigen/Geometry>

// Eigen type to use for 3D Vector
typedef ::Eigen::Matrix< float , 4 , 1 > EigenXYZVector;
// Eigen type to use for 3D Point
typedef ::Eigen::Matrix< float , 4 , 1 > EigenXYZPoint;

EigenXYZVector evector( const Gaudi::XYZVector& v )
{
  return EigenXYZVector( v.x(), v.y(), v.z(), 0 );
}

EigenXYZPoint epoint( const Gaudi::XYZPoint& v )
{
  return EigenXYZPoint( v.x(), v.y(), v.z(), 0 );
}

class Data
{
public:
  typedef std::vector<Data> Vector;
public:
  Gaudi::XYZPoint  position;
  Gaudi::XYZVector direction;
  Gaudi::XYZPoint  CoC;
  double           radius;
public:
  Data() 
  {
    // randomn generator
    static std::default_random_engine gen;
    // Distributions for each member
    static std::uniform_real_distribution<double> r_emiss_x(-800,800), r_emiss_y(-600,600), r_emiss_z(10000,10500);
    static std::uniform_real_distribution<double> r_coc_x(-3000,3000), r_coc_y(-20,20),     r_coc_z(3300,3400);
    static std::uniform_real_distribution<double> r_vdp_x(-3000,3000), r_vdp_y(-200,200),   r_vdp_z(8100,8200);
    static std::uniform_real_distribution<double> r_rad(8500,8600);
    // setup data
    position     = Gaudi::XYZPoint ( r_emiss_x(gen), r_emiss_y(gen), r_emiss_z(gen) );
    CoC          = Gaudi::XYZPoint ( r_coc_x(gen),   r_coc_y(gen),   r_coc_z(gen)   );
    direction    = Gaudi::XYZVector( r_vdp_x(gen),   r_vdp_y(gen),   r_vdp_z(gen)   );
    radius       = r_rad(gen);
  }
};

bool reflectSpherical_ROOT ( Gaudi::XYZPoint& position,
                             Gaudi::XYZVector& direction,
                             const Gaudi::XYZPoint& CoC,
                             const double radius )
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/

  const double a = direction.Mag2();
  if ( 0 == a )  
  {
    std::cerr << "reflectSpherical: Direction vector has zero length..." << std::endl;
    return false;
  }
  const Gaudi::XYZVector delta( position - CoC );
  const double b = 2.0 * direction.Dot( delta );
  const double c = delta.Mag2() - radius*radius;
  const double discr = b*b - 4.0*a*c;
  if ( discr < 0 ) return false;

  const double distance1 = 0.5 * ( std::sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal

  const Gaudi::XYZVector normal( position - CoC );
  direction -= ( 2.0 / normal.Mag2() ) * normal.Dot(direction) * normal;

  return true;
}

bool reflectSpherical_Eigen ( EigenXYZPoint& position,
                              EigenXYZVector& direction,
                              const Gaudi::XYZPoint& CoC,
                              const double radius )
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/

  const float a = direction.dot(direction);
  if ( 0 == a )
  {
    std::cerr << "reflectSpherical: Direction vector has zero length..." << std::endl;
    return false;
  }
  const EigenXYZVector delta( position.x() - CoC.x(),
                              position.y() - CoC.y(),
                              position.z() - CoC.z(), 0 );
  const float b = 2.0f * direction.dot(delta);
  const float c = delta.dot(delta) - radius*radius;
  const float discr = b*b - 4.0f*a*c;
  if ( discr < 0 ) return false;

  const float distance1 = 0.5f * ( std::sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal

  const EigenXYZVector normal( position.x() - CoC.x(),
                               position.y() - CoC.y(),
                               position.z() - CoC.z(), 0 );
  direction -= ( 2.0f / normal.dot(normal) ) * normal.dot(direction) * normal;

  return true;
}

float testEigenDotProduct( const EigenXYZVector& v )
{
  return v.dot(v);
}

float testROOTDotProduct( const Gaudi::XYZVector& v )
{
  return v.Dot(v);
}

int main ( int /*argc*/, char** /*argv*/ )
{

  const unsigned int nPhotons = 10000000;
  
  Data::Vector dataV;
  dataV.reserve( nPhotons );
  // Construct the data to work on
  std::cout << "Creating Random Data ..." << std::endl;
  for ( unsigned int i = 0; i < nPhotons; ++i ) { dataV.push_back( Data() ); }

  // Loop over the data
  std::cout << "Looping over the data to test various ray tracing methods" << std::endl;
  unsigned int ok_ROOT(0), ok_Eigen(0);
  for ( const auto& data : dataV )
  {

    // Eigen Ray Tracing
    EigenXYZPoint eposition   = epoint(data.position);
    EigenXYZVector edirection = evector(data.direction);
    if ( reflectSpherical_Eigen( eposition, edirection, data.CoC, data.radius ) ) { ++ok_Eigen; }

    // ROOT Ray Tracing
    Gaudi::XYZPoint rposition   = data.position;
    Gaudi::XYZVector rdirection = data.direction;
    if ( reflectSpherical_ROOT( rposition, rdirection, data.CoC, data.radius ) )  { ++ok_ROOT; }

    // test dot products
    const float a = testROOTDotProduct(rdirection);
    const float b = testEigenDotProduct(edirection);

  }

  std::cout << "Successfully solved : "
            << "ROOT "  << ( 100.0 * ok_ROOT  ) / dataV.size() << "% " 
            << "Eigen " << ( 100.0 * ok_Eigen ) / dataV.size() << "%" 
            << std::endl;

  return 0;
}
