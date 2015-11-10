

// Quartic Solver
#include "RichRecPhotonTools/QuarticSolver.h"

// STL
#include <random>
#include <vector>
#include <iostream>
#include <string>

// Make an instance of the quartic solver
Rich::Rec::QuarticSolver qSolver;

Gaudi::XYZPoint sphReflPoint;

class Data
{
public:
  typedef std::vector<Data> Vector;
public:
  Gaudi::XYZPoint emissPnt;
  Gaudi::XYZPoint centOfCurv;
  Gaudi::XYZPoint virtDetPoint;
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
    static std::uniform_real_distribution<float>  r_rad(8500,8600);
    // setup data
    emissPnt     = Gaudi::XYZPoint( r_emiss_x(gen), r_emiss_y(gen), r_emiss_z(gen) );
    centOfCurv   = Gaudi::XYZPoint( r_coc_x(gen),   r_coc_y(gen),   r_coc_z(gen)   );
    virtDetPoint = Gaudi::XYZPoint( r_vdp_x(gen),   r_vdp_y(gen),   r_vdp_z(gen)   );
    radius       = r_rad(gen);
  }
};

bool solve( const Data& data )
{
  return qSolver.solve<double>( data.emissPnt, 
                                data.centOfCurv, 
                                data.virtDetPoint,
                                data.radius, 
                                sphReflPoint );
}

void solve( const Data::Vector & dataV )
{

  std::cout << "Solving Quartic Equation for " << dataV.size() << " Photons ..." << std::endl;

  // iterate over the data and solve it...
  unsigned int ok(0);
  for ( const auto& data : dataV )
  {
    if ( solve(data) ) 
    {
      ++ok;
      //std::cout << sphReflPoint << std::endl;
    }
  }

  std::cout << "Successfully solved " << ( 100.0 * ok ) / dataV.size() << "% of photons" << std::endl;

}

int main ( int /*argc*/, char** /*argv*/ )
{

  const unsigned int nPhotons = 1e7;
  
  Data::Vector dataV;
  dataV.reserve( nPhotons );
  // Construct the data to work on
  std::cout << "Creating Random Photon Data ..." << std::endl;
  for ( unsigned int i = 0; i < nPhotons; ++i ) { dataV.push_back( Data() ); }

  // run the solver
  solve( dataV );

  return 0;
}
