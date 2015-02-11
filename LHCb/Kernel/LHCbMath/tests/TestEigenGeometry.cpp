
// STL
#include <random>
#include <vector>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Eigen
//#include <Eigen/Geometry>
#include "LHCbMath/EigenTypes.h"

// VectorClass
//#include "VectorClass/vectorclass.h"
//#include "LHCbMath/VectorClassTypes.h"

// VDT
#include "vdt/sqrt.h"

// Eigen type to use for 3D Vector
typedef LHCb::Math::Eigen::XYZVector EigenXYZVector;

// Eigen type to use for 3D Point
typedef LHCb::Math::Eigen::XYZPoint EigenXYZPoint;

// VectorClass type to use for 3D Vector
//typedef LHCb::Math::VectorClass::XYZVector VClassXYZVector;

// VectorClass type to use for 3D Point
//typedef LHCb::Math::VectorClass::XYZPoint VClassXYZPoint;


class DataROOT
{
public:
  typedef std::vector<DataROOT> Vector;
public:
  Gaudi::XYZPoint  position;
  Gaudi::XYZVector direction;
  Gaudi::XYZPoint  CoC;
  float            radius;
public:
  DataROOT() 
  {
    // randomn generator
    static std::default_random_engine gen;
    // Distributions for each member
    static std::uniform_real_distribution<double> r_emiss_x(-800,800), r_emiss_y(-600,600), r_emiss_z(10000,10500);
    static std::uniform_real_distribution<double> r_coc_x(-3000,3000), r_coc_y(-20,20),     r_coc_z(3300,3400);
    static std::uniform_real_distribution<double> r_vdp_x(-3000,3000), r_vdp_y(-200,200),   r_vdp_z(8100,8200);
    static std::uniform_real_distribution<float>  r_rad(8500,8600);
    // setup data
    position     = Gaudi::XYZPoint ( r_emiss_x(gen), r_emiss_y(gen), r_emiss_z(gen) );
    CoC          = Gaudi::XYZPoint ( r_coc_x(gen),   r_coc_y(gen),   r_coc_z(gen)   );
    direction    = Gaudi::XYZVector( r_vdp_x(gen),   r_vdp_y(gen),   r_vdp_z(gen)   );
    radius       = r_rad(gen);
  }
};

class DataEigen 
{
public:
  typedef std::vector<DataEigen> Vector;
public:
  EigenXYZPoint  position;
  EigenXYZVector direction;
  EigenXYZPoint  CoC;
  float          radius;
  DataEigen( const DataROOT& d ) 
    : position(d.position), 
      direction(d.direction), 
      CoC(d.CoC),
      radius(d.radius) { }
};

// class DataVClass
// {
// public:
//   typedef std::vector<DataVClass> Vector;
// public:
//   VClassXYZPoint  position;
//   VClassXYZVector direction;
//   VClassXYZPoint  CoC;
//   float           radius;
//   DataVClass( const DataROOT& d ) 
//     : position(d.position), 
//       direction(d.direction), 
//       CoC(d.CoC),
//       radius(d.radius) { }
// };

bool reflectSpherical_ROOT ( Gaudi::XYZPoint& position,
                             Gaudi::XYZVector& direction,
                             const Gaudi::XYZPoint& CoC,
                             const float radius )
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/

  auto a = direction.Mag2();

  auto delta = position - CoC ;
  auto b = 2.0 * direction.Dot( delta );
  auto c = delta.Mag2() - radius*radius;
  auto discr = b*b - 4.0*a*c;

  auto distance1 = 0.5 * ( std::sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal

  auto normal = position - CoC ;
  direction -= ( 2.0 / normal.Mag2() ) * normal.Dot(direction) * normal;

  return true;
}

bool reflectSpherical_Eigen ( EigenXYZPoint& position,
                              EigenXYZVector& direction,
                              const EigenXYZPoint& CoC,
                              const float radius )
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/

  const auto a = direction.dot(direction);

  const auto delta = position - CoC;
  const auto b = 2.0f * direction.dot(delta);
  const auto c = delta.dot(delta) - radius*radius;
  const auto discr = b*b - 4.0f*a*c;

  //const auto sd = vdt::fast_isqrt( discr );
  //const auto distance1 = (1.0f - (b*sd) ) / ( 2.f * a * sd );
   const auto distance1 = 0.5f * ( std::sqrt(discr) - b ) / a;

  // change position to the intersection point
  position += distance1 * direction;

  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  const auto  normal = position - CoC;
  direction -= ( 2.0f * normal.dot(direction) / normal.dot(normal) )  * normal ;

  //std::cout << "Eigen " << position << " " << direction << std::endl;

  return true;
}

// bool reflectSpherical_VClass ( VClassXYZPoint& position,
//                                VClassXYZVector& direction,
//                                const VClassXYZPoint& CoC,
//                                const float radius )
// {
//   // find intersection point
//   // for line sphere intersection look at http://www.realtimerendering.com/int/

//   const auto a = horizontal_add(square(direction));

//   const auto delta = position - CoC;
//   const auto b = 2.0f * horizontal_add(direction*delta);;
//   const auto c = horizontal_add(square(delta)) - radius*radius;
//   const auto discr = b*b - 4.0f*a*c;

//   //const auto sd = vdt::fast_isqrt( discr );
//   //const auto distance1 = (1.0f - (b*sd) ) / ( 2.f * a * sd );
//   const auto distance1 = 0.5f * ( std::sqrt(discr) - b ) / a;

//   // change position to the intersection point
//   position += distance1 * direction;

//   // reflect the vector
//   // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
//   const auto  normal = position - CoC;
//   direction -= ( 2.0f * horizontal_add(normal*direction) / horizontal_add(square(normal)) )  * normal ;

//   //std::cout << "VClass " << position << " " << direction << std::endl;

//   return true;
// }

float testEigenDotProduct( const EigenXYZVector& v )
{
  return v.dot(v);
}

float testROOTDotProduct( const Gaudi::XYZVector& v )
{
  return v.Dot(v);
}

// float testVClassDotProduct( const VClassXYZVector& v )
// {
//   return horizontal_add(square(v));
// }

int main ( int /*argc*/, char** /*argv*/ )
{

  const unsigned int nPhotons = 1e7;
  
  DataROOT::Vector dataV_ROOT;
  dataV_ROOT.reserve( nPhotons );
  // Construct the data to work on
  std::cout << "Creating Random Data ..." << std::endl;
  for ( unsigned int i = 0; i < nPhotons; ++i ) { dataV_ROOT.emplace_back(  ); }
  DataEigen::Vector dataV_Eigen; 
  dataV_Eigen.reserve( dataV_ROOT.size() );
  std::transform( std::begin( dataV_ROOT ), std::end( dataV_ROOT ), std::back_inserter( dataV_Eigen ),
                  [](const DataROOT& d) { return DataEigen{d} ; } );
  // DataVClass::Vector dataV_VClass; 
  // dataV_VClass.reserve( dataV_ROOT.size() );
  // std::transform( std::begin( dataV_ROOT ), std::end( dataV_ROOT ), std::back_inserter( dataV_VClass ),
  //                 [](const DataROOT& d) { return DataVClass{d} ; } );

  // Loop over the data
  std::cout << "Looping over the data to test various ray tracing methods" << std::endl;

  unsigned int ok_ROOT(0), ok_Eigen(0);// ok_VClass(0);

  // ROOT Ray Tracing
  for ( auto& data : dataV_ROOT )
  {
    if ( reflectSpherical_ROOT( data.position, data.direction, data.CoC, data.radius ) )  { ++ok_ROOT; }
  }

  // Eigen Ray Tracing
  for ( auto& data : dataV_Eigen ) 
  {
    if ( reflectSpherical_Eigen( data.position, data.direction, data.CoC, data.radius ) ) { ++ok_Eigen; }
  }

  // VectorClass
  // for ( auto& data : dataV_VClass ) 
  // {
  //   if ( reflectSpherical_VClass( data.position, data.direction, data.CoC, data.radius ) ) { ++ok_VClass; }
  // }

  // test dot product
  for ( const auto& data : dataV_ROOT ) 
  {
    testROOTDotProduct(data.direction);
  }
  for ( const auto& data : dataV_Eigen ) 
  {
    testEigenDotProduct(data.direction);
  }
  // for ( const auto& data : dataV_VClass ) 
  // {
  //   testVClassDotProduct(data.direction);
  // }

  std::cout << "Successfully solved : "
            << "ROOT "   << ( 100.0 * ok_ROOT   ) / dataV_ROOT.size()   << "% " 
            << "Eigen "  << ( 100.0 * ok_Eigen  ) / dataV_Eigen.size()  << "% " 
    //<< "VClass " << ( 100.0 * ok_VClass ) / dataV_VClass.size() << "%" 
            << std::endl;

  return 0;
}
